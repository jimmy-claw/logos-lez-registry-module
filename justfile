default: build

# Configure the CMake build directory
configure:
    cmake -S . -B build -G Ninja \
      ${LOGOS_CORE_ROOT:+-DLOGOS_CORE_ROOT="$LOGOS_CORE_ROOT"} \
      ${LEZ_REGISTRY_LIB:+-DLEZ_REGISTRY_LIB="$LEZ_REGISTRY_LIB"} \
      ${LEZ_REGISTRY_INCLUDE:+-DLEZ_REGISTRY_INCLUDE="$LEZ_REGISTRY_INCLUDE"}

# Build the plugin
build: configure
    cmake --build build --parallel --target lez_registry_module

# Remove build artefacts (keeps nix result)
clean:
    rm -rf build

# Full clean including nix result
distclean: clean
    rm -rf result

# Rebuild from scratch
rebuild: distclean build

# Enter nix dev shell
dev:
    nix develop

# Inspect the compiled Qt plugin metadata
inspect:
    nix run '.#inspect-module'

# Run clang-format on all sources
fmt:
    nix shell nixpkgs#clang-tools -c clang-format -i src/*.cpp src/*.h

# Decode unicode escapes in log files (useful for Qt debug output)
unicode-logs file:
    perl -pe 's/\\u([0-9A-Fa-f]{4})/chr(hex($1))/ge' {{file}} | less -R
