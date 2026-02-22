#include "lez_registry_module.h"
#include "logos_api_client.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

// ── Constructor / Destructor ──────────────────────────────────────────────────

LezRegistryModule::LezRegistryModule()
    : m_defaultStorageUrl(QStringLiteral("http://localhost:8080"))
    , m_defaultSequencerUrl(QStringLiteral("http://localhost:9000"))
{
}

LezRegistryModule::~LezRegistryModule() = default;

// ── PluginInterface ───────────────────────────────────────────────────────────

QString LezRegistryModule::version() const {
    // Delegate to the FFI library so the version is always in sync.
    char* raw = lez_registry_version();
    if (!raw) {
        return QStringLiteral("unknown");
    }
    QString v = QString::fromUtf8(raw);
    lez_registry_free_string(raw);
    return v;
}

// ── Logos Core lifecycle ──────────────────────────────────────────────────────

void LezRegistryModule::initLogos(LogosAPI* logosAPIInstance) {
    logosAPI = logosAPIInstance;

    if (logosAPI) {
        // Register this object so other modules can call our methods via LogosAPIClient.
        logosAPI->getProvider()->registerObject(name(), this);

        // Obtain a client handle so we can emit events back through the core bus.
        m_client = logosAPI->getClient(name());
        if (!m_client) {
            qWarning() << "LezRegistryModule: failed to get client handle for" << name();
        }

        // Read optional config overrides from the module property bag.
        const QString storageProp = logosAPI->property("logosStorageUrl").toString();
        if (!storageProp.isEmpty()) {
            m_defaultStorageUrl = storageProp;
        }
        const QString seqProp = logosAPI->property("sequencerUrl").toString();
        if (!seqProp.isEmpty()) {
            m_defaultSequencerUrl = seqProp;
        }
    }

    qInfo() << "LezRegistryModule: initialized. FFI version:" << version();
}

// ── Private helpers ───────────────────────────────────────────────────────────

/*static*/
QString LezRegistryModule::callFfi(FfiJsonFn fn, const QString& argsJson) {
    const QByteArray utf8 = argsJson.toUtf8();
    char* raw = fn(utf8.constData());
    if (!raw) {
        return QStringLiteral(R"({"success":false,"error":"FFI returned null"})");
    }
    QString result = QString::fromUtf8(raw);
    lez_registry_free_string(raw);
    return result;
}

QString LezRegistryModule::mergeWithDefaults(const QString& argsJson) const {
    // Parse caller-supplied JSON (may be empty / "{}").
    QJsonObject obj;
    if (!argsJson.isEmpty()) {
        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(argsJson.toUtf8(), &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            obj = doc.object();
        }
    }

    // Inject defaults only when the caller did not supply them.
    if (!obj.contains(QLatin1String("sequencer_url"))) {
        obj[QLatin1String("sequencer_url")] = m_defaultSequencerUrl;
    }
    if (!obj.contains(QLatin1String("logos_storage_url"))) {
        obj[QLatin1String("logos_storage_url")] = m_defaultStorageUrl;
    }

    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void LezRegistryModule::emitEvent(const QString& eventName, const QVariantList& data) {
    if (!logosAPI) {
        qWarning() << "LezRegistryModule: LogosAPI not available, cannot emit" << eventName;
        return;
    }
    if (!m_client) {
        qWarning() << "LezRegistryModule: no client handle, cannot emit" << eventName;
        return;
    }
    m_client->onEventResponse(this, eventName, data);
}

// ── Registry Operations ───────────────────────────────────────────────────────

QString LezRegistryModule::registerProgram(const QString& argsJson) {
    qDebug() << "LezRegistryModule::registerProgram";
    const QString merged = mergeWithDefaults(argsJson);
    const QString result = callFfi(&lez_registry_register, merged);

    // Emit async event so UI/other modules can react without polling.
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8(), &err);
    if (err.error == QJsonParseError::NoError && doc.isObject()) {
        const QJsonObject obj = doc.object();
        if (obj.value(QLatin1String("success")).toBool()) {
            QVariantList eventData;
            eventData.append(result);
            emitEvent(QStringLiteral("programRegistered"), eventData);
        }
    }

    return result;
}

QString LezRegistryModule::updateProgram(const QString& argsJson) {
    qDebug() << "LezRegistryModule::updateProgram";
    const QString merged = mergeWithDefaults(argsJson);
    const QString result = callFfi(&lez_registry_update, merged);

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8(), &err);
    if (err.error == QJsonParseError::NoError && doc.isObject()) {
        const QJsonObject obj = doc.object();
        if (obj.value(QLatin1String("success")).toBool()) {
            QVariantList eventData;
            eventData.append(result);
            emitEvent(QStringLiteral("programUpdated"), eventData);
        }
    }

    return result;
}

QString LezRegistryModule::listPrograms(const QString& argsJson) {
    qDebug() << "LezRegistryModule::listPrograms";
    const QString merged = mergeWithDefaults(argsJson);
    return callFfi(&lez_registry_list, merged);
}

QString LezRegistryModule::getProgramByName(const QString& name) {
    qDebug() << "LezRegistryModule::getProgramByName name=" << name;

    // Build a minimal args JSON with just the name field.
    QJsonObject obj;
    obj[QLatin1String("name")] = name;
    obj[QLatin1String("sequencer_url")] = m_defaultSequencerUrl;
    const QString argsJson = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));

    return callFfi(&lez_registry_get_by_name, argsJson);
}

QString LezRegistryModule::getProgramById(const QString& programId) {
    qDebug() << "LezRegistryModule::getProgramById programId=" << programId;

    QJsonObject obj;
    obj[QLatin1String("program_id")] = programId;
    obj[QLatin1String("sequencer_url")] = m_defaultSequencerUrl;
    const QString argsJson = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));

    return callFfi(&lez_registry_get_by_id, argsJson);
}

// ── Logos Storage (IDL) Operations ───────────────────────────────────────────

QString LezRegistryModule::uploadIdl(const QString& argsJson) {
    qDebug() << "LezRegistryModule::uploadIdl";
    const QString merged = mergeWithDefaults(argsJson);
    const QString result = callFfi(&lez_storage_upload, merged);

    // Emit event with the new CID so other modules can react.
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8(), &err);
    if (err.error == QJsonParseError::NoError && doc.isObject()) {
        const QJsonObject obj = doc.object();
        if (obj.value(QLatin1String("success")).toBool()) {
            QVariantList eventData;
            eventData.append(result);
            emitEvent(QStringLiteral("idlUploaded"), eventData);
        }
    }

    return result;
}

QString LezRegistryModule::downloadIdl(const QString& cid) {
    qDebug() << "LezRegistryModule::downloadIdl cid=" << cid;

    // Support optional "url|cid" format so callers can override the storage URL inline.
    QString storageUrl = m_defaultStorageUrl;
    QString resolvedCid = cid;
    const int sep = cid.indexOf(QLatin1Char('|'));
    if (sep != -1) {
        storageUrl = cid.left(sep);
        resolvedCid = cid.mid(sep + 1);
    }

    QJsonObject obj;
    obj[QLatin1String("logos_storage_url")] = storageUrl;
    obj[QLatin1String("cid")] = resolvedCid;
    const QString argsJson = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));

    return callFfi(&lez_storage_fetch_idl, argsJson);
}
