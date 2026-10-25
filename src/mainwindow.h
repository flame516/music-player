#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QVector>

class QLineEdit;
class QLabel;
class QPushButton;
class QTableWidget;
class QStackedWidget;
class PlayerPanel;
class QListWidget;
class QListWidgetItem;
class QFrame;
class QSlider;
class QMediaPlayer;
class QAudioOutput;
class HeartButton;

struct FavoriteSong;
struct UserPlaylist;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onCoverClicked();
    void onBackToMain();

private:
    void setupUi();
    void setupTopBar();
    void setupCenter();
    void setupBottomBar();
    void setupConnections();

    QWidget *m_centralWidget = nullptr;
    QFrame *m_backgroundFrame = nullptr;
    QWidget *m_topBar = nullptr;
    QWidget *m_bottomBar = nullptr;
    QStackedWidget *m_centerStack = nullptr;
    QWidget *m_centerComposite = nullptr;
    QWidget *m_leftSidebar = nullptr;

    // 顶部
    QLineEdit *m_searchEdit = nullptr;
    QPushButton *m_userButton = nullptr;
    QPushButton *m_settingsButton = nullptr;
    QPushButton *m_minButton = nullptr;
    QPushButton *m_maxButton = nullptr;
    QPushButton *m_closeButton = nullptr;

    // 中间
    QLabel *m_playlistTitleLabel = nullptr;
    QPushButton *m_batchAddButton = nullptr;
    QPushButton *m_batchDeleteButton = nullptr;
    QTableWidget *m_playlistTable = nullptr;
    PlayerPanel *m_playerPanel = nullptr;

    // 左侧侧栏
    QListWidget *m_onlineList = nullptr;
    QListWidget *m_favoriteList = nullptr;
    QListWidget *m_userPlaylistList = nullptr;
    QPushButton *m_newPlaylistButton = nullptr;
    QPushButton *m_deletePlaylistButton = nullptr;

    // 底部
    QPushButton *m_coverButton = nullptr;
    QLabel *m_songTitleLabel = nullptr;
    QLabel *m_artistLabel = nullptr;
    QPushButton *m_prevButton = nullptr;
    QPushButton *m_playPauseButton = nullptr;
    QPushButton *m_nextButton = nullptr;
    HeartButton *m_favoriteButton = nullptr;
    QSlider *m_positionSlider = nullptr;

    // 播放
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;

    // 用户
    int m_currentUserId = 0;
    QString m_currentUsername;
    QVector<FavoriteSong> m_currentSongs;
    int m_currentSongIndex = -1;
    int m_currentPlaylistId = -1;
    QString m_currentPlaylistName;

    // 窗口拖动
    bool m_dragging = false;
    QPoint m_dragStartPos;
    
    // 动画
    class QPropertyAnimation *m_slideAnimation = nullptr;

public:
    void setCurrentUser(int userId, const QString &username);

    void setupPlayer();
    void loadDemoPlaylist();

    void setupLeftSidebar();
    void refreshFavorites();
    void refreshUserPlaylists();
    void setCurrentSongs(const QVector<FavoriteSong> &songs, const QString &playlistName = QString());
    void playSongAt(int index);
    void updateFavoriteButtonStyle(bool isFavorite);
    static QString formatDuration(qint64 ms);
    static qint64 readDurationMs(const QString &filePath);
    static FavoriteSong songFromFile(const QString &filePath, qint64 durationMs);

private slots:
    void onPlayPause();
    void onPrev();
    void onNext();
    void onPositionChanged(qint64 pos);
    void onDurationChanged(qint64 dur);
    void onSliderMoved(int value);
    void onAddToFavorite();
    void onCreatePlaylist();
    void onUserButtonClicked();
    void onSongDoubleClicked(int row, int column);
    void onUserPlaylistSelected(QListWidgetItem *item);
    void onPlaylistTableContextMenu(const QPoint &pos);
    void onAddSongToPlaylist();
    void onRemoveSongFromPlaylist();
    void onAddSongToOtherPlaylist();
    void onBatchAddSongs();
    void onBatchDeleteSongs();
    void onDeletePlaylist();
    void onFavoriteListClicked(QListWidgetItem *item);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // MAINWINDOW_H


