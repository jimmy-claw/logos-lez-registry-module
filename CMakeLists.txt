cmake_minimum_required(VERSION 3.20)
project(lez-registry-module VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── logos-cpp-sdk ─────────────────────────────────────────────────────────────
set(LOGOS_CORE_ROOT "" CACHE PATH "Path to logos-cpp-sdk install (contains include/ and lib/)")
if("${LOGOS_CORE_ROOT}" STREQUAL "")
    message(FATAL_ERROR "LOGOS_CORE_ROOT must be set to the logos-cpp-sdk install prefix")
endif()

# ── Qt6 ───────────────────────────────────────────────────────────────────────
# When Qt6 components live in separate Nix store paths we must find each
# component independently to avoid the prefix-mismatch check in Qt6Config.cmake.
find_package(Qt6Core    REQUIRED)
find_package(Qt6RemoteObjects REQUIRED)
find_package(Qt6Concurrent    REQUIRED)
find_package(Qt6Qml           REQUIRED)
find_package(Qt6Quick         REQUIRED)
find_package(Qt6QmlModels     REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ── lez-registry-ffi ─────────────────────────────────────────────────────────
set(LEZ_REGISTRY_LIB     "" CACHE PATH "Path to directory containing liblez_registry_ffi.so")
set(LEZ_REGISTRY_INCLUDE "" CACHE PATH "Path to directory containing lez_registry.h")

if("${LEZ_REGISTRY_LIB}" STREQUAL "")
    message(FATAL_ERROR "LEZ_REGISTRY_LIB must be set to the lez-registry-ffi lib directory")
endif()
if("${LEZ_REGISTRY_INCLUDE}" STREQUAL "")
    set(LEZ_REGISTRY_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    message(STATUS "LEZ_REGISTRY_INCLUDE not set — using bundled header: ${LEZ_REGISTRY_INCLUDE}")
endif()

# ── SDK interface targets ─────────────────────────────────────────────────────
add_library(logos_cpp_sdk INTERFACE)
target_include_directories(logos_cpp_sdk INTERFACE
    "${LOGOS_CORE_ROOT}/include"
    "${LOGOS_CORE_ROOT}/include/cpp"
)

add_library(logos_core SHARED IMPORTED)
set_target_properties(logos_core PROPERTIES
    IMPORTED_LOCATION "${LOGOS_CORE_ROOT}/lib/liblogos_core.so"
)

# ── Plugin target ─────────────────────────────────────────────────────────────
set(PLUGIN_TARGET lez_registry_module)

qt_add_plugin(${PLUGIN_TARGET} CLASS_NAME LezRegistryModule)

target_sources(${PLUGIN_TARGET} PRIVATE
    src/lez_registry_module.cpp
    src/program_list_model.cpp
    src/plugin.cpp
    qml/qml.qrc
)

set_property(TARGET ${PLUGIN_TARGET} PROPERTY PUBLIC_HEADER
    src/i_lez_registry_module.h
)

target_include_directories(${PLUGIN_TARGET} PRIVATE
    src
    "${LEZ_REGISTRY_INCLUDE}"
)

# ── lez-registry-ffi linkage ──────────────────────────────────────────────────
add_library(lez_registry_ffi SHARED IMPORTED)
set_target_properties(lez_registry_ffi PROPERTIES
    IMPORTED_LOCATION "${LEZ_REGISTRY_LIB}/liblez_registry_ffi.so"
    IMPORTED_SONAME   "liblez_registry_ffi.dylib"
)

target_link_libraries(${PLUGIN_TARGET} PRIVATE
    Qt6::Core
    Qt6::RemoteObjects
    Qt6::Quick
    Qt6::Qml
    Qt6::Concurrent
    logos_cpp_sdk
    logos_core
    lez_registry_ffi
)

# ── Compile definitions ───────────────────────────────────────────────────────
target_compile_definitions(${PLUGIN_TARGET} PRIVATE
    LEZ_REGISTRY_MODULE_METADATA_FILE="${CMAKE_CURRENT_SOURCE_DIR}/metadata.json"
)

# ── RPATH ─────────────────────────────────────────────────────────────────────
if(APPLE)
    set_target_properties(${PLUGIN_TARGET} PROPERTIES
        BUILD_RPATH   "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
elseif(UNIX)
    set_target_properties(${PLUGIN_TARGET} PROPERTIES
        BUILD_RPATH   "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )
endif()

# ── Install ───────────────────────────────────────────────────────────────────
install(TARGETS ${PLUGIN_TARGET}
    LIBRARY         DESTINATION lib
    PUBLIC_HEADER   DESTINATION include
)
install(FILES include/lez_registry.h DESTINATION include)
