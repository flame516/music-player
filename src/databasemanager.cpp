#include "databasemanager.h"

#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QVariant>

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager mgr;
    return mgr;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

bool DatabaseManager::init(const QString &dbPath)
{
    if (m_db.isOpen())
        return true;

    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    m_db.setDatabaseName(dbPath);
    if (!m_db.open()) {
        return false;
    }
    return initSchema();
}

bool DatabaseManager::initSchema()
{
    QSqlQuery q;
    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE,"
            "password TEXT)"))) {
        return false;
    }

    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS favorite_songs ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER,"
            "title TEXT,"
            "artist TEXT,"
            "duration TEXT,"
            "file_path TEXT)"))) {
        return false;
    }

    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS playlists ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER,"
            "name TEXT)"))) {
        return false;
    }

    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS playlist_songs ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "playlist_id INTEGER,"
            "title TEXT,"
            "artist TEXT,"
            "duration TEXT,"
            "file_path TEXT)"))) {
        return false;
    }

    // 静默尝试补全旧表缺少的 file_path 列
    q.exec(QStringLiteral("ALTER TABLE favorite_songs ADD COLUMN file_path TEXT"));

    return true;
}

bool DatabaseManager::ensureUser(const QString &username, const QString &password)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("INSERT OR IGNORE INTO users(username, password) VALUES(:u, :p)"));
    q.bindValue(QStringLiteral(":u"), username);
    q.bindValue(QStringLiteral(":p"), password);
    if (!q.exec())
        return false;
    return true;
}

bool DatabaseManager::login(const QString &username, const QString &password, int &userIdOut)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT id FROM users WHERE username=:u AND password=:p"));
    q.bindValue(QStringLiteral(":u"), username);
    q.bindValue(QStringLiteral(":p"), password);
    if (!q.exec())
        return false;
    if (q.next()) {
        userIdOut = q.value(0).toInt();
        return true;
    }
    return false;
}

bool DatabaseManager::addFavoriteSong(int userId, const FavoriteSong &song)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("INSERT INTO favorite_songs(user_id, title, artist, duration, file_path) "
                             "VALUES(:uid, :t, :a, :d, :f)"));
    q.bindValue(QStringLiteral(":uid"), userId);
    q.bindValue(QStringLiteral(":t"), song.title);
    q.bindValue(QStringLiteral(":a"), song.artist);
    q.bindValue(QStringLiteral(":d"), song.duration);
    q.bindValue(QStringLiteral(":f"), song.filePath);
    return q.exec();
}

bool DatabaseManager::removeFavoriteSong(int userId, const FavoriteSong &song)
{
    QSqlQuery q;
    // 优先通过 filePath 匹配，如果没有则通过 title+artist 匹配
    if (!song.filePath.isEmpty()) {
        q.prepare(QStringLiteral("DELETE FROM favorite_songs WHERE user_id=:uid AND file_path=:f"));
        q.bindValue(QStringLiteral(":uid"), userId);
        q.bindValue(QStringLiteral(":f"), song.filePath);
    } else {
        q.prepare(QStringLiteral("DELETE FROM favorite_songs WHERE user_id=:uid AND title=:t AND artist=:a"));
        q.bindValue(QStringLiteral(":uid"), userId);
        q.bindValue(QStringLiteral(":t"), song.title);
        q.bindValue(QStringLiteral(":a"), song.artist);
    }
    return q.exec();
}

QVector<FavoriteSong> DatabaseManager::favoritesForUser(int userId)
{
    QVector<FavoriteSong> result;
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT id, title, artist, duration, file_path FROM favorite_songs WHERE user_id=:uid"));
    q.bindValue(QStringLiteral(":uid"), userId);
    if (!q.exec())
        return result;
    while (q.next()) {
        FavoriteSong s;
        s.id = q.value(0).toInt();
        s.title = q.value(1).toString();
        s.artist = q.value(2).toString();
        s.duration = q.value(3).toString();
        s.filePath = q.value(4).toString();
        result.push_back(s);
    }
    return result;
}

int DatabaseManager::createPlaylist(int userId, const QString &name)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("INSERT INTO playlists(user_id, name) VALUES(:uid, :n)"));
    q.bindValue(QStringLiteral(":uid"), userId);
    q.bindValue(QStringLiteral(":n"), name);
    if (!q.exec())
        return -1;
    return q.lastInsertId().toInt();
}

bool DatabaseManager::deletePlaylist(int playlistId)
{
    if (playlistId <= 0)
        return false;
    
    QSqlQuery q;
    // 先删除歌单中的所有歌曲
    q.prepare(QStringLiteral("DELETE FROM playlist_songs WHERE playlist_id=:pid"));
    q.bindValue(QStringLiteral(":pid"), playlistId);
    q.exec();
    
    // 再删除歌单
    q.prepare(QStringLiteral("DELETE FROM playlists WHERE id=:pid"));
    q.bindValue(QStringLiteral(":pid"), playlistId);
    return q.exec();
}

bool DatabaseManager::addSongToPlaylist(int playlistId, const FavoriteSong &song)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("INSERT INTO playlist_songs(playlist_id, title, artist, duration, file_path) "
                             "VALUES(:pid, :t, :a, :d, :f)"));
    q.bindValue(QStringLiteral(":pid"), playlistId);
    q.bindValue(QStringLiteral(":t"), song.title);
    q.bindValue(QStringLiteral(":a"), song.artist);
    q.bindValue(QStringLiteral(":d"), song.duration);
    q.bindValue(QStringLiteral(":f"), song.filePath);
    return q.exec();
}

bool DatabaseManager::removeSongFromPlaylist(int playlistId, int songId)
{
    QSqlQuery q;
    q.prepare(QStringLiteral("DELETE FROM playlist_songs WHERE playlist_id=:pid AND id=:sid"));
    q.bindValue(QStringLiteral(":pid"), playlistId);
    q.bindValue(QStringLiteral(":sid"), songId);
    return q.exec();
}

QVector<FavoriteSong> DatabaseManager::playlistSongs(int playlistId)
{
    QVector<FavoriteSong> result;
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT id, title, artist, duration, file_path FROM playlist_songs WHERE playlist_id=:pid"));
    q.bindValue(QStringLiteral(":pid"), playlistId);
    if (!q.exec())
        return result;
    while (q.next()) {
        FavoriteSong s;
        s.id = q.value(0).toInt();
        s.title = q.value(1).toString();
        s.artist = q.value(2).toString();
        s.duration = q.value(3).toString();
        s.filePath = q.value(4).toString();
        result.push_back(s);
    }
    return result;
}

QVector<UserPlaylist> DatabaseManager::userPlaylists(int userId)
{
    QVector<UserPlaylist> list;
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT id, name FROM playlists WHERE user_id=:uid ORDER BY id DESC"));
    q.bindValue(QStringLiteral(":uid"), userId);
    if (!q.exec())
        return list;
    while (q.next()) {
        UserPlaylist pl;
        pl.id = q.value(0).toInt();
        pl.name = q.value(1).toString();
        list.push_back(pl);
    }
    return list;
}


