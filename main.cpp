#include "mainwindow.hpp"
#include "screenshotutil.hpp"
#include <QApplication>
#include <QCommandLineParser>
#include <QtGlobal>
#include <iostream>
#include <notifications.hpp>
#include <screenareaselector/screenareaselector.hpp>
#include <stdio.h>
#include <worker/worker.hpp>

bool verbose = false;

void handler(QtMsgType type, const QMessageLogContext &, const QString &msg) {
    std::string stdMsg = msg.toStdString();
    switch (type) {
    case QtDebugMsg:
        if (verbose) std::cout << "DEBUG: " << stdMsg << "\n";
        break;
    case QtInfoMsg:
        std::cout << "INFO: " << stdMsg << "\n";
        break;
    case QtWarningMsg:
        std::cerr << "WARN: " << stdMsg << "\n";
        break;
    case QtCriticalMsg:
        std::cerr << "CRIT: " << stdMsg << "\n";
        break;
    case QtFatalMsg:
        std::cerr << "FATAL: " << stdMsg << "\n";
        notifications::notify("KShare Fatal Error", msg, QSystemTrayIcon::Critical);
        break;
    }
}

int main(int argc, char *argv[]) {
    qInstallMessageHandler(handler);
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.setApplicationName("KShare");
    a.setOrganizationName("ArsenArsen");
    a.setApplicationVersion("3.0");

    QCommandLineParser parser;
    parser.addHelpOption();

    QCommandLineOption h({ "b", "background" }, "Does not show the main window, starts in tray.");
    QCommandLineOption v({ "v", "verbose" }, "Enables QtDebugMsg outputs");
    QCommandLineOption ver({ "ver", "version" }, "Prints KShare version");
    parser.addOption(h);
    parser.addOption(v);
    parser.addOption(ver);
    parser.process(a);

    if (parser.isSet(ver)) {
        printf("%s %s\n", a.applicationName().toLocal8Bit().constData(), a.applicationVersion().toLocal8Bit().constData());
        return 0;
    }

    verbose = parser.isSet(v);

    MainWindow w;
    Worker::init();
    a.connect(&a, &QApplication::aboutToQuit, Worker::end);
    if (!parser.isSet(h)) w.show();
    (new ScreenAreaSelector())->show();
    return a.exec();
}
