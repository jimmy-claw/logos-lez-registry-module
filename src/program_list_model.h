#pragma once

#include <QAbstractListModel>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QString>
#include <QList>

#ifdef __cplusplus
extern "C" {
#endif
#include <lez_registry.h>
#ifdef __cplusplus
}
#endif

/**
 * ProgramListModel — QAbstractListModel backed by the lez_registry_list() FFI.
 *
 * Exposes on-chain program registry entries to QML via standard list model
 * roles. FFI calls are made asynchronously via QtConcurrent to avoid blocking
 * the UI thread.
 */
class ProgramListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        VersionRole,
        DescriptionRole,
        ProgramIdRole,
        IdlCidRole,
        AuthorRole,
        TagsRole,
    };
    Q_ENUM(Roles)

    explicit ProgramListModel(QObject* parent = nullptr);
    ~ProgramListModel() override = default;

    // ── QAbstractListModel ───────────────────────────────────────────────────
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // ── Properties ──────────────────────────────────────────────────────────
    bool loading() const { return m_loading; }
    QString error() const { return m_error; }

    // ── Configuration ────────────────────────────────────────────────────────
    Q_INVOKABLE void setSequencerUrl(const QString& url);
    Q_INVOKABLE QString sequencerUrl() const { return m_sequencerUrl; }

public slots:
    /** Async: fetch the program list from the registry. */
    void refresh();

signals:
    void loadingChanged();
    void errorChanged();
    void countChanged();
    void refreshed();

private:
    struct Program {
        QString programId;
        QString name;
        QString version;
        QString author;
        QString idlCid;
        QString description;
        QStringList tags;
    };

    void setLoading(bool v);
    void setError(const QString& e);
    void applyPrograms(const QList<Program>& programs);

    QList<Program> m_programs;
    bool m_loading = false;
    QString m_error;
    QString m_sequencerUrl = QStringLiteral("http://localhost:9000");

    QFutureWatcher<QString>* m_watcher = nullptr;
};
