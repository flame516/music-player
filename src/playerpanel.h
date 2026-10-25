#ifndef PLAYERPANEL_H
#define PLAYERPANEL_H

#include <QWidget>

class QLabel;
class QPushButton;
class QListWidget;
class QTimer;

class PlayerPanel : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerPanel(QWidget *parent = nullptr);

signals:
    void backRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onRotateTick();

private:
    void setupUi();

    QLabel *m_coverLabel = nullptr;
    QListWidget *m_lyricsList = nullptr;
    QPushButton *m_backButton = nullptr;
    QTimer *m_rotateTimer = nullptr;
    qreal m_angle = 0.0;
    
    // 歌词数据
    struct LyricLine {
        qint64 timeMs = 0;
        QString text;
    };
    QVector<LyricLine> m_lyrics;
    int m_currentLyricIndex = -1;
    int m_previousLyricIndex = -1;
    
public:
    void loadLyrics(const QString &filePath);
    void updateLyricsPosition(qint64 positionMs);
};

#endif // PLAYERPANEL_H


