# Builds the lez-registry-module plugin .so
{ pkgs, common, src, logosSdk, logosLiblogos, lezRegistryFfi }:

let
  llvmPkgs = pkgs.llvmPackages;

  # Create a merged LOGOS_CORE_ROOT with headers from SDK and libs from liblogos
  mergedLogosCore = pkgs.symlinkJoin {
    name = "logos-core-merged";
    paths = [ logosSdk logosLiblogos ];
  };
in
pkgs.stdenv.mkDerivation {
  pname = "${common.pname}-lib";
  version = common.version;

  inherit src;
  inherit (common) meta;

  nativeBuildInputs = common.nativeBuildInputs;

  buildInputs = common.buildInputs ++ [
    llvmPkgs.clang
    llvmPkgs.libclang
    lezRegistryFfi
  ];

  LIBCLANG_PATH = "${llvmPkgs.libclang.lib}/lib";
  CLANG_PATH = "${llvmPkgs.clang}/bin/clang";

  cmakeFlags = [
    "-GNinja"
    "-DLOGOS_CORE_ROOT=${mergedLogosCore}"
    "-DLEZ_REGISTRY_LIB=${lezRegistryFfi}/lib"
    "-DLEZ_REGISTRY_INCLUDE=${lezRegistryFfi}/include"
  ];

  installPhase = ''
    runHook preInstall

    mkdir -p $out/lib
    found=""
    for f in $(find . -name "liblez_registry_module.so" -type f); do
      cp "$f" $out/lib/
      found=1
    done
    if [ -z "$found" ]; then
      echo "Error: liblez_registry_module.so not found"
      find . -name "liblez_registry*" -type f
      exit 1
    fi

    runHook postInstall
  '';
}
