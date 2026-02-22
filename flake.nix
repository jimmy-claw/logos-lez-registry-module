{
  description = "logos-lez-registry-module — Qt6 Logos Core plugin for the LEZ Program Registry";

  inputs = {
    nixpkgs.follows = "logos-liblogos/nixpkgs";

    logos-liblogos.url = "github:logos-co/logos-liblogos";
    logos-core.url     = "github:logos-co/logos-cpp-sdk";

    logos-module-viewer.url = "github:logos-co/logos-module-viewer";

    lez-registry-ffi.url = "github:jimmy-claw/lez-registry?dir=lez-registry-ffi";
  };

  outputs =
    {
      self,
      nixpkgs,
      logos-core,
      logos-liblogos,
      logos-module-viewer,
      lez-registry-ffi,
      ...
    }:
    let
      lib = nixpkgs.lib;

      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      forAll = lib.genAttrs systems;

      mkPkgs = system: import nixpkgs { inherit system; };
    in
    {
      packages = forAll (
        system:
        let
          pkgs     = mkPkgs system;
          llvmPkgs = pkgs.llvmPackages;

          logosCore      = logos-core.packages.${system}.default;
          lezRegistryFfi = lez-registry-ffi.packages.${system}.default;

          lezRegistryModule = pkgs.stdenv.mkDerivation {
            pname   = "lez-registry-module";
            version = "0.1.0";
            src     = ./.;

            nativeBuildInputs = [
              pkgs.cmake
              pkgs.ninja
              pkgs.pkg-config
              pkgs.qt6.wrapQtAppsHook
            ];

            buildInputs = [
              pkgs.qt6.qtbase
              pkgs.qt6.qtremoteobjects
              pkgs.qt6.qtdeclarative
              pkgs.qt6.qttools
              llvmPkgs.clang
              llvmPkgs.libclang
              lezRegistryFfi
            ]
            ++ lib.optionals pkgs.stdenv.isDarwin [
              pkgs.libiconv
              pkgs.cacert
            ];

            LIBCLANG_PATH = "${llvmPkgs.libclang.lib}/lib";
            CLANG_PATH    = "${llvmPkgs.clang}/bin/clang";
            SSL_CERT_FILE =
              lib.optionalString pkgs.stdenv.isDarwin
              "${pkgs.cacert}/etc/ssl/certs/ca-bundle.crt";

            cmakeFlags = [
              "-DLOGOS_CORE_ROOT=${logosCore}"
              "-DLEZ_REGISTRY_LIB=${lezRegistryFfi}/lib"
              "-DLEZ_REGISTRY_INCLUDE=${lezRegistryFfi}/include"
            ];

            meta = {
              description = "LEZ Program Registry — Logos Core Qt6 plugin";
              homepage    = "https://github.com/jimmy-claw/logos-lez-registry-module";
              license     = lib.licenses.mit;
            };
          };
        in
        {
          lib     = lezRegistryModule;
          default = lezRegistryModule;
        }
      );

      apps = forAll (
        system:
        let
          pkgs                  = mkPkgs system;
          lezRegistryModuleLib  = self.packages.${system}.lib;
          logosModuleViewer     = logos-module-viewer.packages.${system}.default;
          ext =
            if pkgs.stdenv.isDarwin then "dylib"
            else if pkgs.stdenv.hostPlatform.isWindows then "dll"
            else "so";
          inspectModule = {
            type    = "app";
            program =
              "${pkgs.writeShellScriptBin "inspect-module" ''
                exec ${logosModuleViewer}/bin/logos-module-viewer \
                  --module ${lezRegistryModuleLib}/lib/liblez_registry_module.${ext}
              ''}/bin/inspect-module";
          };
        in
        {
          inspect-module = inspectModule;
          default        = inspectModule;
        }
      );

      devShells = forAll (
        system:
        let
          pkgs           = mkPkgs system;
          pkg            = self.packages.${system}.default;
          logosCore      = logos-core.packages.${system}.default;
          lezRegistryFfi = lez-registry-ffi.packages.${system}.default;
        in
        {
          default = pkgs.mkShell {
            inputsFrom = [ pkg ];

            inherit (pkg) LIBCLANG_PATH CLANG_PATH;

            LOGOS_CORE_ROOT      = "${logosCore}";
            LEZ_REGISTRY_LIB     = "${lezRegistryFfi}/lib";
            LEZ_REGISTRY_INCLUDE = "${lezRegistryFfi}/include";

            shellHook = ''
              echo ""
              echo "=== logos-lez-registry-module Dev Shell ==="
              echo "LOGOS_CORE_ROOT:      $LOGOS_CORE_ROOT"
              echo "LEZ_REGISTRY_LIB:     $LEZ_REGISTRY_LIB"
              echo "LEZ_REGISTRY_INCLUDE: $LEZ_REGISTRY_INCLUDE"
              echo "-------------------------------------------"
              echo "Run: just build"
            '';
          };
        }
      );
    };
}
