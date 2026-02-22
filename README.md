# logos-lez-registry-module

A **Logos Core Qt6 plugin** that wraps the [`lez-registry-ffi`](https://github.com/jimmy-claw/lez-registry) C library and exposes on-chain LEZ Program Registry operations as `Q_INVOKABLE` methods callable by the Logos Core module system.

> **Logos Storage** (previously known as Codex) is used for IDL file storage. The module handles both registry queries and IDL upload/download transparently.

---

## Features

| Method | Description |
|---|---|
| `registerProgram(argsJson)` | Register a new program in the LEZ on-chain registry |
| `updateProgram(argsJson)` | Update metadata for an existing entry (author-only) |
| `listPrograms(argsJson)` | List all registered programs |
| `getProgramByName(name)` | Query a program by its human-readable name |
| `getProgramById(programId)` | Query a program by its on-chain ID |
| `uploadIdl(argsJson)` | Upload an IDL file to Logos Storage, returns CID |
| `downloadIdl(cid)` | Fetch and parse an IDL JSON from Logos Storage |
| `version()` | Returns the version of the underlying FFI library |

All methods accept/return JSON strings for seamless interoperability with
[`LogosAPIClient::invokeRemoteMethod`](https://github.com/logos-co/logos-cpp-sdk).

### Events emitted

| Event | Payload |
|---|---|
| `programRegistered` | JSON with `tx_hash` |
| `programUpdated` | JSON with `tx_hash` |
| `idlUploaded` | JSON with `cid` |

---

## Architecture

```
Qt Plugin (C++)           C FFI (C ABI)             Rust
LezRegistryModule  →  liblez_registry_ffi.so  →  lez-registry crate
                                                   ↓
                                           LEZ Sequencer (HTTP)
                                           Logos Storage  (HTTP)
```

Follows the [`logos-execution-zone-module`](https://github.com/logos-blockchain/logos-execution-zone-module) pattern:
- Inherits `QObject`, `PluginInterface`, `ILezRegistryModule`
- `Q_PLUGIN_METADATA` embeds `metadata.json` for Logos Core discovery
- All public methods are `Q_INVOKABLE`
- Events emitted via `eventResponse(QString, QVariantList)` signal

---

## Repository Structure

```
logos-lez-registry-module/
├── CMakeLists.txt              # CMake build (C++20, Qt6, logos-cpp-sdk)
├── flake.nix                   # Nix flake for reproducible builds
├── justfile                    # Common tasks (build, fmt, inspect)
├── metadata.json               # Logos Core plugin descriptor
├── README.md
├── include/
│   └── lez_registry.h          # C FFI header (copied from lez-registry-ffi)
└── src/
    ├── i_lez_registry_module.h # Pure virtual public interface
    ├── lez_registry_module.h   # Implementation class header
    ├── lez_registry_module.cpp # Implementation
    └── plugin.cpp              # Qt plugin entry point (minimal)
```

---

## Prerequisites

| Dependency | Notes |
|---|---|
| Qt 6.x | `qtbase`, `qtremoteobjects` |
| [logos-cpp-sdk](https://github.com/logos-co/logos-cpp-sdk) | Provides `PluginInterface`, `LogosAPI`, `LogosAPIClient` |
| [lez-registry-ffi](https://github.com/jimmy-claw/lez-registry) | Rust C FFI — `liblez_registry_ffi.so` |
| CMake ≥ 3.20, Ninja | Build toolchain |

---

## Build

### With Nix (recommended)

```bash
# Once lez-registry-ffi has a flake input wired in:
nix build

# Inspect plugin metadata
nix run '.#inspect-module'

# Dev shell
nix develop
```

> **Note:** The `lez-registry-ffi` Nix input is not yet wired into `flake.nix`.
> Once the `lez-registry` crate exposes a `flake.nix`, add it as an input and
> fill in the `cmakeFlags` for `LEZ_REGISTRY_LIB` / `LEZ_REGISTRY_INCLUDE`.

### Without Nix (manual)

```bash
# 1. Build lez-registry-ffi
cd /path/to/lez-registry/lez-registry-ffi
cargo build --release

# 2. Build this module
export LOGOS_CORE_ROOT=/path/to/logos-cpp-sdk/result
export LEZ_REGISTRY_LIB=/path/to/lez-registry/lez-registry-ffi/target/release
export LEZ_REGISTRY_INCLUDE=/path/to/lez-registry/lez-registry-ffi/include

just build
# or:
cmake -S . -B build -G Ninja \
  -DLOGOS_CORE_ROOT="$LOGOS_CORE_ROOT" \
  -DLEZ_REGISTRY_LIB="$LEZ_REGISTRY_LIB" \
  -DLEZ_REGISTRY_INCLUDE="$LEZ_REGISTRY_INCLUDE"
cmake --build build --parallel
```

---

## Usage

### Headless / logoscore

```bash
# Copy module and FFI lib into a modules directory
mkdir -p modules
cp build/liblez_registry_module.so modules/
cp /path/to/lez-registry-ffi/target/release/liblez_registry_ffi.so modules/

# List all registered programs
logoscore -m ./modules \
  --load-modules liblez_registry_module \
  --call 'liblez_registry_module.listPrograms({"sequencer_url":"http://localhost:9000"})'

# Register a program
logoscore -m ./modules \
  --load-modules liblez_registry_module \
  --call 'liblez_registry_module.registerProgram({
    "sequencer_url": "http://localhost:9000",
    "wallet_path": "/path/to/wallet",
    "program_id": "abc123",
    "name": "my-program",
    "version": "0.1.0"
  })'

# Upload IDL to Logos Storage and get CID
logoscore -m ./modules \
  --load-modules liblez_registry_module \
  --call 'liblez_registry_module.uploadIdl({
    "logos_storage_url": "http://localhost:8080",
    "file_path": "/path/to/program.idl.json"
  })'

# Download IDL by CID
logoscore -m ./modules \
  --load-modules liblez_registry_module \
  --call 'liblez_registry_module.downloadIdl("bafybeiabc...")'
```

### From another Logos Core module

```cpp
// In YourModule::initLogos():
m_registry = logosAPI->getClient("liblez_registry_module");

// List programs
QVariant programs = m_registry->invokeRemoteMethod(
    "liblez_registry_module",
    "listPrograms",
    R"({"sequencer_url":"http://localhost:9000"})"
);

// Subscribe to registration events
m_registry->onEvent(remoteObject, this, "programRegistered",
    [this](const QString& event, const QVariantList& data) {
        qInfo() << "New program registered:" << data;
    });
```

---

## JSON API Reference

### `registerProgram(argsJson)`

```json
{
  "sequencer_url": "http://localhost:9000",
  "wallet_path": "/path/to/wallet",
  "program_id": "hex_or_comma_u32s",
  "name": "my-program",
  "version": "0.1.0",
  "idl_cid": "bafy...",
  "description": "Optional description",
  "tags": ["governance", "token"]
}
```

Returns: `{ "success": true, "tx_hash": "0x..." }`

### `listPrograms(argsJson)`

```json
{ "sequencer_url": "http://localhost:9000" }
```

Returns:
```json
{
  "success": true,
  "programs": [
    {
      "program_id": "...",
      "name": "lez-multisig",
      "version": "0.1.0",
      "author": "0x...",
      "idl_cid": "bafy...",
      "description": "...",
      "tags": ["governance"]
    }
  ]
}
```

### Error responses

All methods return `{ "success": false, "error": "<message>" }` on failure.

---

## Development

```bash
# Format code
just fmt

# Clean and rebuild
just rebuild

# Inspect Qt plugin metadata
just inspect
```

---

## License

MIT — see [LICENSE](LICENSE).
