{
  description = "logos-lez-registry-module — Qt6 Logos Core plugin and standalone app for LEZ Program Registry";

  inputs = {
    nixpkgs.follows = "logos-liblogos/nixpkgs";

    logos-liblogos.url = "github:logos-co/logos-liblogos";
    logos-cpp-sdk.url = "github:logos-co/logos-cpp-sdk";
    logos-capability-module.url = "github:logos-co/logos-capability-module";

    lez-registry-ffi.url = "github:jimmy-claw/lez-registry?dir=lez-registry-ffi";
  };

  outputs =
    {
      self,
      nixpkgs,
      logos-cpp-sdk,
      logos-liblogos,
      logos-capability-module,
      lez-registry-ffi,
      ...
    }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f {
        pkgs = import nixpkgs { inherit system; };
        logosSdk = logos-cpp-sdk.packages.${system}.default;
        logosLiblogos = logos-liblogos.packages.${system}.default;
        logosCapabilityModule = logos-capability-module.packages.${system}.default;
        lezRegistryFfi = lez-registry-ffi.packages.${system}.default;
      });
    in
    {
      packages = forAllSystems ({ pkgs, logosSdk, logosLiblogos, logosCapabilityModule, lezRegistryFfi }:
        let
          common = import ./nix/default.nix {
            inherit pkgs logosSdk logosLiblogos;
          };
          src = ./.;

          lib = import ./nix/lib.nix {
            inherit pkgs common src logosSdk logosLiblogos lezRegistryFfi;
          };

          app = import ./nix/app.nix {
            inherit pkgs common src logosLiblogos logosSdk logosCapabilityModule lezRegistryFfi;
            lezRegistryModule = lib;
          };
        in
        {
          lib = lib;
          app = app;
          default = lib;
        }
      );

      devShells = forAllSystems ({ pkgs, logosSdk, logosLiblogos, lezRegistryFfi, ... }: {
        default = pkgs.mkShell {
          nativeBuildInputs = [
            pkgs.cmake
            pkgs.ninja
            pkgs.pkg-config
          ];
          buildInputs = [
            pkgs.qt6.qtbase
            pkgs.qt6.qtremoteobjects
            pkgs.qt6.qtdeclarative
            pkgs.zstd
            pkgs.krb5
            pkgs.abseil-cpp
          ];

          shellHook = ''
            export LOGOS_CPP_SDK_ROOT="${logosSdk}"
            export LOGOS_LIBLOGOS_ROOT="${logosLiblogos}"
            export LEZ_REGISTRY_LIB="${lezRegistryFfi}/lib"
            export LEZ_REGISTRY_INCLUDE="${lezRegistryFfi}/include"
            echo "LEZ Registry Module development environment"
          '';
        };
      });
    };
}
