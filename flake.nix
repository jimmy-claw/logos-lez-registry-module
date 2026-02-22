{
  description = "logos-lez-registry-module — Qt6 Logos Core plugin for the LEZ Program Registry";

  inputs = {
    # Pin nixpkgs to match logos-liblogos (avoids Qt version mismatches)
    nixpkgs.follows = "logos-liblogos/nixpkgs";

    logos-liblogos.url = "github:logos-co/logos-liblogos";
    logos-core.url     = "github:logos-co/logos-cpp-sdk";

    # logos-module-viewer — for the inspect-module app
    logos-module-viewer.url = "github:logos-co/logos-module-viewer";

    # lez-registry-ffi Nix input is intentionally omitted for now.
    # Once lez-registry-ffi has its own flake.nix, add:
    #
    #   lez-registry-ffi.url = "github:jimmy-claw/lez-registry?dir=lez-registry-ffi";
    #
    # and wire it into cmakeFlags below.
  };

  outputs =
    {
      self,
      nixpkgs,
      logos-core,
      logos-module-viewer,
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

          logosCore = logos-core.packages.${system}.default;

          # ── TODO: wire in lez-registry-ffi once it has a flake ─────────────
          # lezRegistryFfi = lez-registry-ffi.packages.${system}.default;
          #
          # For now, LEZ_REGISTRY_LIB / LEZ_REGISTRY_INCLUDE must be supplied
          # as environment variables or via `nix develop` overrides.
          # ───────────────────────────────────────────────────────────────────

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
              pkgs.qt6.qttools
              llvmPkgs.clang
              llvmPkgs.libclang
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
              # When lez-registry-ffi flake is ready, replace with:
              # "-DLEZ_REGISTRY_LIB=${lezRegistryFfi}/lib"
              # "-DLEZ_REGISTRY_INCLUDE=${lezRegistryFfi}/include"
              #
              # Until then, the build requires LEZ_REGISTRY_LIB to be set
              # in the environment (see devShell below).
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

      # ── inspect-module app ─────────────────────────────────────────────────
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

      # ── dev shell ──────────────────────────────────────────────────────────
      devShells = forAll (
        system:
        let
          pkgs      = mkPkgs system;
          pkg       = self.packages.${system}.default;
          logosCore = logos-core.packages.${system}.default;
        in
        {
          default = pkgs.mkShell {
            inputsFrom = [ pkg ];

            inherit (pkg) LIBCLANG_PATH CLANG_PATH;

            LOGOS_CORE_ROOT = "${logosCore}";
            # Set these in your shell once lez-registry-ffi is built:
            # LEZ_REGISTRY_LIB     = "/path/to/lez-registry-ffi/target/release";
            # LEZ_REGISTRY_INCLUDE = "/path/to/lez-registry-ffi/include";

            shellHook = ''
              BLUE='\e[1;34m'
              GREEN='\e[1;32m'
              YELLOW='\e[1;33m'
              RESET='\e[0m'

              echo -e "\n''${BLUE}=== logos-lez-registry-module Dev Shell ===''${RESET}"
              echo -e "''${GREEN}LOGOS_CORE_ROOT:''${RESET}      $LOGOS_CORE_ROOT"
              echo -e "''${YELLOW}LEZ_REGISTRY_LIB:''${RESET}    ''${LEZ_REGISTRY_LIB:-<not set — required for build>}"
              echo -e "''${YELLOW}LEZ_REGISTRY_INCLUDE:''${RESET} ''${LEZ_REGISTRY_INCLUDE:-<not set — using bundled header>}"
              echo -e "''${BLUE}------------------------------------------''${RESET}"
              echo -e "Run: just build"
            '';
          };
        }
      );
    };
}
