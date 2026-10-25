#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QString>
#include <QVector>

struct FavoriteSong
{
    int id = 0;
    QString title;
    QString artist;
    QString duration;
    QString filePath;
};

struct UserPlaylist
{
    int id = 0;
    QString name;
};

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static DatabaseManager &instance();

    bool init(const QString &dbPath);

    bool ensureUser(const QString &username, const QString &password);
    bool login(const QString &username, const QString &password, int &userIdOut);

    bool addFavoriteSong(int userId, const FavoriteSong &song);
    bool removeFavoriteSong(int userId, const FavoriteSong &song);
    QVector<FavoriteSong> favoritesForUser(int userId);

    int createPlaylist(int userId, const QString &name);
    bool deletePlaylist(int playlistId);
    bool addSongToPlaylist(int playlistId, const FavoriteSong &song);
    bool removeSongFromPlaylist(int playlistId, int songId);
    QVector<FavoriteSong> playlistSongs(int playlistId);
    QVector<UserPlaylist> userPlaylists(int userId);
    
    // 特殊歌单ID：我喜欢的音乐
    static constexpr int FAVORITE_PLAYLIST_ID = -1;

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    QSqlDatabase m_db;

    bool initSchema();
};

#endif // DATABASEMANAGER_H


