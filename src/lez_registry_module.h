#pragma once

#include "i_lez_registry_module.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <lez_registry.h>
#ifdef __cplusplus
}
#endif

#include <QObject>
#include <QString>
#include <QVariantList>

/**
 * LezRegistryModule — Qt6 plugin implementation of ILezRegistryModule.
 *
 * Wraps the lez-registry-ffi C library (liblez_registry_ffi.so) and
 * exposes on-chain program registry operations as Q_INVOKABLE methods
 * so that Logos Core can discover and call them via Qt Remote Objects.
 *
 * Follow the logos-blockchain-module / logos-execution-zone-module pattern:
 *   - Inherit QObject, PluginInterface, ILezRegistryModule
 *   - Declare Q_PLUGIN_METADATA with the embedded metadata.json
 *   - All public API methods are Q_INVOKABLE
 *   - Emit eventResponse(QString, QVariantList) for async notifications
 */
class LezRegistryModule final : public QObject, public PluginInterface, public ILezRegistryModule {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ILezRegistryModule_iid FILE LEZ_REGISTRY_MODULE_METADATA_FILE)
    Q_INTERFACES(PluginInterface)

public:
    LezRegistryModule();
    ~LezRegistryModule() override;

    // ── PluginInterface ──────────────────────────────────────────────────────

    [[nodiscard]] QString name() const override { return QStringLiteral("liblez_registry_module"); }
    [[nodiscard]] QString version() const override;

    // ── ILezRegistryModule lifecycle ─────────────────────────────────────────

    Q_INVOKABLE void initLogos(LogosAPI* logosAPIInstance) override;

    // ── Registry Operations ──────────────────────────────────────────────────

    Q_INVOKABLE QString registerProgram(const QString& argsJson) override;
    Q_INVOKABLE QString updateProgram(const QString& argsJson) override;
    Q_INVOKABLE QString listPrograms(const QString& argsJson) override;
    Q_INVOKABLE QString getProgramByName(const QString& name) override;
    Q_INVOKABLE QString getProgramById(const QString& programId) override;

    // ── Logos Storage (IDL) Operations ───────────────────────────────────────

    Q_INVOKABLE QString uploadIdl(const QString& argsJson) override;
    Q_INVOKABLE QString downloadIdl(const QString& cid) override;

signals:
    /**
     * Emitted for async registry events (e.g. "programRegistered", "programUpdated").
     * eventName: one of the event name constants below
     * data:      JSON-encoded payload as QVariantList[0] = QString
     */
    void eventResponse(const QString& eventName, const QVariantList& data);

private:
    LogosAPIClient* m_client = nullptr;

    /**
     * Default Logos Storage URL. Can be overridden via module config or
     * by embedding "logos_storage_url" in the args JSON.
     */
    QString m_defaultStorageUrl;

    /**
     * Default sequencer URL. Can be overridden per-call via args JSON.
     */
    QString m_defaultSequencerUrl;

    /**
     * Convenience: call a lez_registry_ffi function that takes a JSON string
     * and returns a heap-allocated JSON string.  Converts the result to
     * QString and frees the C string automatically.
     */
    using FfiJsonFn = char* (*)(const char*);
    static QString callFfi(FfiJsonFn fn, const QString& argsJson);

    /**
     * Build a minimal args JSON that includes the default sequencer URL
     * merged with any caller-supplied fields.
     */
    QString mergeWithDefaults(const QString& argsJson) const;

    /**
     * Emit a registry event to the Logos Core event bus.
     */
    void emitEvent(const QString& eventName, const QVariantList& data);
};
