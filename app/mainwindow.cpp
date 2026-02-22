#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QPluginLoader>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QDir>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    // Determine platform plugin extension
    QString pluginExtension;
    #if defined(Q_OS_WIN)
        pluginExtension = ".dll";
    #elif defined(Q_OS_MAC)
        pluginExtension = ".dylib";
    #else
        pluginExtension = ".so";
    #endif

    // Load the lez_registry_module plugin
    QString pluginPath = QCoreApplication::applicationDirPath() + "/../modules/liblez_registry_module" + pluginExtension;
    QPluginLoader loader(pluginPath);

    bool pluginLoaded = false;

    if (loader.load()) {
        QObject* plugin = loader.instance();
        if (plugin) {
            qInfo() << "LEZ Registry module plugin loaded successfully";
            pluginLoaded = true;
        }
    }

    if (!pluginLoaded) {
        qWarning() << "================================================";
        qWarning() << "Failed to load LEZ Registry module from:" << pluginPath;
        qWarning() << "Error:" << loader.errorString();
        qWarning() << "================================================";
    }

    // Create QQuickWidget to host our QML
    m_quickWidget = new QQuickWidget(this);
    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // Add QML import paths — look next to the binary and in the module dir
    QString qmlDir = QCoreApplication::applicationDirPath() + "/../qml";
    m_quickWidget->engine()->addImportPath(qmlDir);

    // Load the main QML view
    QUrl qmlUrl;
    QString localQml = qmlDir + "/LezRegistryView.qml";
    if (QFile::exists(localQml)) {
        qmlUrl = QUrl::fromLocalFile(localQml);
    } else {
        // Fallback: try loading from Qt resources (compiled into plugin)
        qmlUrl = QUrl("qrc:/qml/LezRegistryView.qml");
    }

    m_quickWidget->setSource(qmlUrl);

    if (m_quickWidget->status() == QQuickWidget::Error) {
        qWarning() << "QML loading errors:";
        for (const auto &error : m_quickWidget->errors()) {
            qWarning() << "  " << error.toString();
        }
        // Fallback widget
        QWidget* fallbackWidget = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(fallbackWidget);
        QLabel* messageLabel = new QLabel("LEZ Registry QML view failed to load", fallbackWidget);
        QFont font = messageLabel->font();
        font.setPointSize(14);
        messageLabel->setFont(font);
        messageLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(messageLabel);
        setCentralWidget(fallbackWidget);
    } else {
        setCentralWidget(m_quickWidget);
    }

    setWindowTitle("LEZ Registry App");
    resize(800, 600);
}
