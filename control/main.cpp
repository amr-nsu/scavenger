#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <iostream>

void messageHandler(QtMsgType /* type */, const QMessageLogContext& /* mlc */, const QString& msg)
{
    std::cout << msg.toStdString() << std::endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qInstallMessageHandler(messageHandler);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return QGuiApplication::exec();
}
