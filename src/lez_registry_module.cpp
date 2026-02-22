#include <QQmlContext>
#include "lez_registry_module.h"
#include "program_list_model.h"

#include <logos_api_provider.h>
#include <logos_api_client.h>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QtConcurrent/QtConcurrent>

// ── Constructor / Destructor ──────────────────────────────────────────────────

LezRegistryModule::LezRegistryModule()
    : m_defaultStorageUrl(QStringLiteral("http://localhost:8080"))
    , m_defaultSequencerUrl(QStringLiteral("http://localhost:9000"))
    , m_idlWatcher(new QFutureWatcher<QString>(this))
    , m_registerWatcher(new QFutureWatcher<QString>(this))
{
    // IDL fetch completion
    connect(m_idlWatcher, &QFutureWatcher<QString>::finished, this, [this]() {
        const QString resultJson = m_idlWatcher->result();
        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(resultJson.toUtf8(), &err);
        if (err.error != QJsonParseError::NoError) {
            emit idlFetched(QString(), QStringLiteral("JSON parse error: ") + err.errorString());
            return;
        }
        const QJsonObject root = doc.object();
        if (!root.value(QLatin1String("success")).toBool()) {
            emit idlFetched(QString(), root.value(QLatin1String("error")).toString(QStringLiteral("Unknown error")));
            return;
        }
        // IDL is the "idl" key as a pretty-printed JSON object
        const QJsonValue idlVal = root.value(QLatin1String("idl"));
        QString content;
        if (idlVal.isObject()) {
            content = QString::fromUtf8(QJsonDocument(idlVal.toObject()).toJson(QJsonDocument::Indented));
        } else if (idlVal.isString()) {
            content = idlVal.toString();
        } else {
            content = resultJson; // fallback: show full response
        }
        emit idlFetched(content, QString());
    });

    // Register completion
    connect(m_registerWatcher, &QFutureWatcher<QString>::finished, this, [this]() {
        const QString resultJson = m_registerWatcher->result();
        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(resultJson.toUtf8(), &err);
        if (err.error != QJsonParseError::NoError) {
            emit registerFailed(QStringLiteral("JSON parse error: ") + err.errorString());
            return;
        }
        const QJsonObject root = doc.object();
        if (!root.value(QLatin1String("success")).toBool()) {
            emit registerFailed(root.value(QLatin1String("error")).toString(QStringLiteral("Unknown error")));
            return;
        }
        QVariantList eventData;
        eventData.append(resultJson);
        emitEvent(QStringLiteral("programRegistered"), eventData);
        emit programRegistered();
    });
}

LezRegistryModule::~LezRegistryModule() = default;

// ── PluginInterface ───────────────────────────────────────────────────────────

QString LezRegistryModule::version() const {
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

    // Create the program list model (parented to this module)
    m_model = new ProgramListModel(this);
    m_model->setSequencerUrl(m_defaultSequencerUrl);

    if (logosAPI) {
        logosAPI->getProvider()->registerObject(name(), this);

        m_client = logosAPI->getClient(name());
        if (!m_client) {
            qWarning() << "LezRegistryModule: failed to get client handle for" << name();
        }

        const QString storageProp = logosAPI->property("logosStorageUrl").toString();
        if (!storageProp.isEmpty()) {
            m_defaultStorageUrl = storageProp;
        }
        const QString seqProp = logosAPI->property("sequencerUrl").toString();
        if (!seqProp.isEmpty()) {
            m_defaultSequencerUrl = seqProp;
            m_model->setSequencerUrl(m_defaultSequencerUrl);
        }

        // Expose model and module to QML engine if available
        m_qmlEngine = logosAPI->property("qmlEngine").value<QQmlEngine*>();
        if (m_qmlEngine) {
            m_qmlEngine->rootContext()->setContextProperty(
                QStringLiteral("lezRegistryModel"), m_model);
            m_qmlEngine->rootContext()->setContextProperty(
                QStringLiteral("lezRegistryModule"), this);
            qInfo() << "LezRegistryModule: QML context properties registered";
        } else {
            qWarning() << "LezRegistryModule: no QML engine available — context properties not set";
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
    QJsonObject obj;
    if (!argsJson.isEmpty()) {
        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(argsJson.toUtf8(), &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            obj = doc.object();
        }
    }

    if (!obj.contains(QLatin1String("sequencer_url"))) {
        obj[QLatin1String("sequencer_url")] = m_defaultSequencerUrl;
    }
    if (!obj.contains(QLatin1String("logos_storage_url"))) {
        obj[QLatin1String("logos_storage_url")] = m_defaultStorageUrl;
    }

    return QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void LezRegistryModule::emitEvent(const QString& eventName, const QVariantList& data) {
    if (!logosAPI || !m_client) {
        qWarning() << "LezRegistryModule: cannot emit event — API/client not ready";
        return;
    }
    m_client->onEventResponse(this, eventName, data);
}

// ── QML-facing async helpers ──────────────────────────────────────────────────

void LezRegistryModule::fetchIdlAsync(const QString& cid) {
    const QString storageUrl = m_defaultStorageUrl;
    QString resolvedCid = cid;
    QString resolvedUrl = storageUrl;

    // Support "url|cid" inline override
    const int sep = cid.indexOf(QLatin1Char('|'));
    if (sep != -1) {
        resolvedUrl = cid.left(sep);
        resolvedCid = cid.mid(sep + 1);
    }

    QJsonObject obj;
    obj[QLatin1String("logos_storage_url")] = resolvedUrl;
    obj[QLatin1String("cid")] = resolvedCid;
    const QString argsJson = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));

    QFuture<QString> future = QtConcurrent::run([argsJson]() -> QString {
        const QByteArray utf8 = argsJson.toUtf8();
        char* raw = lez_storage_fetch_idl(utf8.constData());
        if (!raw) return QStringLiteral(R"({"success":false,"error":"FFI returned null"})");
        QString r = QString::fromUtf8(raw);
        lez_registry_free_string(raw);
        return r;
    });
    m_idlWatcher->setFuture(future);
}

void LezRegistryModule::registerAsync(const QString& argsJson) {
    const QString merged = mergeWithDefaults(argsJson);

    QFuture<QString> future = QtConcurrent::run([merged]() -> QString {
        const QByteArray utf8 = merged.toUtf8();
        char* raw = lez_registry_register(utf8.constData());
        if (!raw) return QStringLiteral(R"({"success":false,"error":"FFI returned null"})");
        QString r = QString::fromUtf8(raw);
        lez_registry_free_string(raw);
        return r;
    });
    m_registerWatcher->setFuture(future);
}

// ── Registry Operations (synchronous — for inter-module RPC) ─────────────────

QString LezRegistryModule::registerProgram(const QString& argsJson) {
    qDebug() << "LezRegistryModule::registerProgram";
    const QString merged = mergeWithDefaults(argsJson);
    const QString result = callFfi(&lez_registry_register, merged);

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8(), &err);
    if (err.error == QJsonParseError::NoError && doc.isObject()) {
        if (doc.object().value(QLatin1String("success")).toBool()) {
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
        if (doc.object().value(QLatin1String("success")).toBool()) {
            QVariantList eventData;
            eventData.append(result);
            emitEvent(QStringLiteral("programUpdated"), eventData);
        }
    }
    return result;
}

QString LezRegistryModule::listPrograms(const QString& argsJson) {
    qDebug() << "LezRegistryModule::listPrograms";
    return callFfi(&lez_registry_list, mergeWithDefaults(argsJson));
}

QString LezRegistryModule::getProgramByName(const QString& name) {
    qDebug() << "LezRegistryModule::getProgramByName name=" << name;
    QJsonObject obj;
    obj[QLatin1String("name")] = name;
    obj[QLatin1String("sequencer_url")] = m_defaultSequencerUrl;
    return callFfi(&lez_registry_get_by_name,
                   QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

QString LezRegistryModule::getProgramById(const QString& programId) {
    qDebug() << "LezRegistryModule::getProgramById programId=" << programId;
    QJsonObject obj;
    obj[QLatin1String("program_id")] = programId;
    obj[QLatin1String("sequencer_url")] = m_defaultSequencerUrl;
    return callFfi(&lez_registry_get_by_id,
                   QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}

// ── Logos Storage Operations ──────────────────────────────────────────────────

QString LezRegistryModule::uploadIdl(const QString& argsJson) {
    qDebug() << "LezRegistryModule::uploadIdl";
    const QString merged = mergeWithDefaults(argsJson);
    const QString result = callFfi(&lez_storage_upload, merged);

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(result.toUtf8(), &err);
    if (err.error == QJsonParseError::NoError && doc.isObject()) {
        if (doc.object().value(QLatin1String("success")).toBool()) {
            QVariantList eventData;
            eventData.append(result);
            emitEvent(QStringLiteral("idlUploaded"), eventData);
        }
    }
    return result;
}

QString LezRegistryModule::downloadIdl(const QString& cid) {
    qDebug() << "LezRegistryModule::downloadIdl cid=" << cid;
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
    return callFfi(&lez_storage_fetch_idl,
                   QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact)));
}
