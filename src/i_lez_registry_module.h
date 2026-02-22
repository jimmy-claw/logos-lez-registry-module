#ifndef I_LEZ_REGISTRY_MODULE_H
#define I_LEZ_REGISTRY_MODULE_H

#include <QString>
#include <core/interface.h>

/**
 * ILezRegistryModule — Public interface for the LEZ Program Registry Logos Core module.
 *
 * Exposes on-chain program registry operations (register, update, list, query)
 * and Logos Storage IDL management (upload, download) as Q_INVOKABLE Qt methods.
 *
 * All methods accept and return JSON strings for maximum interoperability with
 * the Logos Core inter-module RPC system (LogosAPIClient::invokeRemoteMethod).
 */
class ILezRegistryModule {
public:
    virtual ~ILezRegistryModule() = default;

    // ── Logos Core lifecycle ─────────────────────────────────────────────────

    /**
     * Called by Logos Core after the module is loaded.
     * The module should store the LogosAPI pointer and register itself as a provider.
     */
    virtual void initLogos(LogosAPI* logosAPIInstance) = 0;

    // ── Registry Operations ──────────────────────────────────────────────────

    /**
     * Register a new program in the LEZ on-chain registry.
     *
     * argsJson: JSON object with fields:
     *   sequencer_url, wallet_path, program_id, name, version,
     *   idl_cid (optional), description (optional), tags (optional array)
     *
     * Returns: JSON { "success": true, "tx_hash": "0x..." }
     *       or JSON { "success": false, "error": "..." }
     */
    virtual QString registerProgram(const QString& argsJson) = 0;

    /**
     * Update metadata for an existing registry entry (original author only).
     *
     * argsJson: JSON object with fields:
     *   sequencer_url, wallet_path, program_id,
     *   version (optional), idl_cid (optional),
     *   description (optional), tags (optional array)
     *
     * Returns: JSON { "success": true, "tx_hash": "0x..." }
     *       or JSON { "success": false, "error": "..." }
     */
    virtual QString updateProgram(const QString& argsJson) = 0;

    /**
     * List all programs registered in the LEZ registry.
     *
     * argsJson: JSON object with field: sequencer_url
     *
     * Returns: JSON { "success": true, "programs": [ { ... }, ... ] }
     *       or JSON { "success": false, "error": "..." }
     */
    virtual QString listPrograms(const QString& argsJson) = 0;

    /**
     * Look up a single program entry by its human-readable name.
     *
     * name: Program name (e.g. "lez-multisig")
     *
     * Returns: JSON { "success": true, "program": { ... } }
     *       or JSON { "success": false, "error": "..." }
     */
    virtual QString getProgramByName(const QString& name) = 0;

    /**
     * Look up a single program entry by its on-chain program ID.
     *
     * programId: hex string or comma-separated u32 array
     *
     * Returns: JSON { "success": true, "program": { ... } }
     *       or JSON { "success": false, "error": "..." }
     */
    virtual QString getProgramById(const QString& programId) = 0;

    // ── Logos Storage (IDL) Operations ──────────────────────────────────────

    /**
     * Upload an IDL file to Logos Storage, returning its CID.
     *
     * argsJson: JSON object with fields:
     *   logos_storage_url, file_path
     *
     * Returns: JSON { "success": true, "cid": "bafy..." }
     *       or JSON { "success": false, "error": "..." }
     */
    virtual QString uploadIdl(const QString& argsJson) = 0;

    /**
     * Fetch and parse an IDL JSON document from Logos Storage by CID.
     *
     * cid: Logos Storage content identifier (e.g. "bafy...")
     *
     * Returns: JSON { "success": true, "idl": { ... } }
     *       or JSON { "success": false, "error": "..." }
     *
     * Note: Caller must also supply logos_storage_url via module config or
     *       pass it embedded in the cid argument as "url|cid".
     */
    virtual QString downloadIdl(const QString& cid) = 0;

    // ── Version ──────────────────────────────────────────────────────────────

    /**
     * Returns the version string of the underlying lez-registry-ffi library.
     */
    virtual QString version() = 0;
};

#define ILezRegistryModule_iid "org.logos.ilezregistrymodule"
Q_DECLARE_INTERFACE(ILezRegistryModule, ILezRegistryModule_iid)

#endif // I_LEZ_REGISTRY_MODULE_H
