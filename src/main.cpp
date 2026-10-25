#include "mainwindow.h"
#include "databasemanager.h"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName(QStringLiteral("MusicPlayerQt6"));
    QApplication::setOrganizationName(QStringLiteral("ExampleOrg"));

    // 初始化数据库（即使不登录也初始化，便于随时登录/注册）
    const QString dbPath = QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("musicplayer.db"));
    DatabaseManager::instance().init(dbPath);

    MainWindow w;
    w.setCurrentUser(0, QString());
    w.show();

    return a.exec();
}



