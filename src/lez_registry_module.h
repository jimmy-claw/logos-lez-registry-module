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
#include <QQmlEngine>
#include <QFutureWatcher>

class ProgramListModel;

/**
 * LezRegistryModule — Qt6 plugin implementation of ILezRegistryModule.
 *
 * Wraps the lez-registry-ffi C library (liblez_registry_ffi.so) and
 * exposes on-chain program registry operations as Q_INVOKABLE methods
 * so that Logos Core can discover and call them via Qt Remote Objects.
 *
 * Also owns the ProgramListModel exposed to QML as a context property,
 * and provides async helper invokables used by LezRegistryView.qml.
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

    // ── QML-facing async helpers ──────────────────────────────────────────────

    /**
     * Async: fetch IDL by CID from Logos Storage.
     * Emits idlFetched(content, error) when done.
     */
    Q_INVOKABLE void fetchIdlAsync(const QString& cid);

    /**
     * Async: register a program.
     * Emits programRegistered() on success, or registerFailed(error) on failure.
     */
    Q_INVOKABLE void registerAsync(const QString& argsJson);

signals:
    /**
     * Emitted for async registry events (e.g. "programRegistered", "programUpdated").
     * eventName: one of the event name constants below
     * data:      JSON-encoded payload as QVariantList[0] = QString
     */
    void eventResponse(const QString& eventName, const QVariantList& data);

    /** Emitted when fetchIdlAsync() completes. */
    void idlFetched(const QString& content, const QString& error);

    /** Emitted by registerAsync() on success. */
    void programRegistered();

    /** Emitted by registerAsync() on failure. */
    void registerFailed(const QString& error);

private:
    LogosAPIClient* m_client = nullptr;
    ProgramListModel* m_model = nullptr;
    QQmlEngine* m_qmlEngine = nullptr;

    QFutureWatcher<QString>* m_idlWatcher      = nullptr;
    QFutureWatcher<QString>* m_registerWatcher = nullptr;

    QString m_defaultStorageUrl;
    QString m_defaultSequencerUrl;

    using FfiJsonFn = char* (*)(const char*);
    static QString callFfi(FfiJsonFn fn, const QString& argsJson);

    QString mergeWithDefaults(const QString& argsJson) const;
    void emitEvent(const QString& eventName, const QVariantList& data);
};
