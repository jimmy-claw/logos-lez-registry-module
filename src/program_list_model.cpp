#include "program_list_model.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtConcurrent/QtConcurrent>

ProgramListModel::ProgramListModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_watcher(new QFutureWatcher<QString>(this))
{
    connect(m_watcher, &QFutureWatcher<QString>::finished, this, [this]() {
        const QString resultJson = m_watcher->result();
        setLoading(false);

        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(resultJson.toUtf8(), &err);
        if (err.error != QJsonParseError::NoError) {
            setError(QStringLiteral("JSON parse error: ") + err.errorString());
            return;
        }

        const QJsonObject root = doc.object();
        if (!root.value(QLatin1String("success")).toBool()) {
            setError(root.value(QLatin1String("error")).toString(QStringLiteral("Unknown FFI error")));
            return;
        }

        setError(QString()); // clear previous error

        const QJsonArray programs = root.value(QLatin1String("programs")).toArray();
        QList<Program> parsed;
        parsed.reserve(programs.size());
        for (const QJsonValue& v : programs) {
            if (!v.isObject()) continue;
            const QJsonObject p = v.toObject();
            Program prog;
            prog.programId   = p.value(QLatin1String("program_id")).toString();
            prog.name        = p.value(QLatin1String("name")).toString();
            prog.version     = p.value(QLatin1String("version")).toString();
            prog.author      = p.value(QLatin1String("author")).toString();
            prog.idlCid      = p.value(QLatin1String("idl_cid")).toString();
            prog.description = p.value(QLatin1String("description")).toString();
            const QJsonArray tags = p.value(QLatin1String("tags")).toArray();
            for (const QJsonValue& t : tags) {
                prog.tags << t.toString();
            }
            parsed.append(prog);
        }
        applyPrograms(parsed);
        emit refreshed();
    });
}

// ── QAbstractListModel ────────────────────────────────────────────────────────

int ProgramListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return m_programs.size();
}

QVariant ProgramListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_programs.size()) {
        return {};
    }
    const Program& p = m_programs.at(index.row());
    switch (static_cast<Roles>(role)) {
        case NameRole:        return p.name;
        case VersionRole:     return p.version;
        case DescriptionRole: return p.description;
        case ProgramIdRole:   return p.programId;
        case IdlCidRole:      return p.idlCid;
        case AuthorRole:      return p.author;
        case TagsRole:        return p.tags;
        default:              return {};
    }
}

QHash<int, QByteArray> ProgramListModel::roleNames() const {
    return {
        { NameRole,        "name"        },
        { VersionRole,     "version"     },
        { DescriptionRole, "description" },
        { ProgramIdRole,   "programId"   },
        { IdlCidRole,      "idlCid"      },
        { AuthorRole,      "author"      },
        { TagsRole,        "tags"        },
    };
}

// ── Public slots ──────────────────────────────────────────────────────────────

void ProgramListModel::refresh() {
    if (m_loading) return; // already in flight
    setLoading(true);
    setError(QString());

    // Capture for the worker lambda
    const QString seqUrl = m_sequencerUrl;

    QFuture<QString> future = QtConcurrent::run([seqUrl]() -> QString {
        QJsonObject obj;
        obj[QLatin1String("sequencer_url")] = seqUrl;
        const QByteArray argsUtf8 = QJsonDocument(obj).toJson(QJsonDocument::Compact);

        char* raw = lez_registry_list(argsUtf8.constData());
        if (!raw) {
            return QStringLiteral(R"({"success":false,"error":"FFI returned null"})");
        }
        QString result = QString::fromUtf8(raw);
        lez_registry_free_string(raw);
        return result;
    });

    m_watcher->setFuture(future);
}

void ProgramListModel::setSequencerUrl(const QString& url) {
    if (m_sequencerUrl != url) {
        m_sequencerUrl = url;
    }
}

// ── Private helpers ───────────────────────────────────────────────────────────

void ProgramListModel::setLoading(bool v) {
    if (m_loading != v) {
        m_loading = v;
        emit loadingChanged();
    }
}

void ProgramListModel::setError(const QString& e) {
    if (m_error != e) {
        m_error = e;
        emit errorChanged();
    }
}

void ProgramListModel::applyPrograms(const QList<Program>& programs) {
    beginResetModel();
    m_programs = programs;
    endResetModel();
    emit countChanged();
}
