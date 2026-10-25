#include "playerpanel.h"

#include <QBoxLayout>
#include <QByteArray>
#include <QFile>
#include <QFont>
#include <QIODevice>
#include <QLabel>
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QRegularExpression>
#include <QStringConverter>
#include <QTextStream>
#include <QTimer>
#include <algorithm>

PlayerPanel::PlayerPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();

    m_rotateTimer = new QTimer(this);
    m_rotateTimer->setInterval(50);
    connect(m_rotateTimer, &QTimer::timeout, this, &PlayerPanel::onRotateTick);
    m_rotateTimer->start();
}

void PlayerPanel::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);

    // 顶部返回按钮（左上角倒三角）
    m_backButton = new QPushButton(this);
    m_backButton->setFixedSize(40, 40);
    m_backButton->setCursor(Qt::PointingHandCursor);
    m_backButton->setToolTip(QStringLiteral("返回主界面"));
    m_backButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  background: rgba(255,255,255,0.15);"
        "  border-radius: 20px;"
        "  border: 1px solid rgba(255,255,255,0.3);"
        "}"
        "QPushButton:hover {"
        "  background: rgba(255,255,255,0.25);"
        "}"
    ));
    connect(m_backButton, &QPushButton::clicked, this, &PlayerPanel::backRequested);
    
    // 使用绝对定位放在左上角
    m_backButton->setParent(this);
    m_backButton->raise();
    m_backButton->move(20, 20); // 初始位置

    // 中间：左旋转封面 + 分割线 + 右歌词
    auto *centerLayout = new QHBoxLayout;
    centerLayout->setSpacing(24);

    m_coverLabel = new QLabel(this);
    m_coverLabel->setMinimumSize(240, 240);
    m_coverLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_coverLabel->setAlignment(Qt::AlignCenter);
    m_coverLabel->setStyleSheet(QStringLiteral("background: transparent;"));

    // 右侧歌词列表
    m_lyricsList = new QListWidget(this);
    m_lyricsList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_lyricsList->setStyleSheet(QStringLiteral(
        "QListWidget {"
        " background: transparent;"
        " color: #FFFFFF;"
        " border: none;"
        " font-size: 16px;"
        "}"
        "QListWidget::item {"
        " padding: 8px 0;"
        " text-align: center;"
        " color: rgba(255,255,255,0.6);"
        "}"
        "QListWidget::item:selected {"
        " color: #00c853;"
        " font-weight: bold;"
        "}"
    ));

    // 歌词列表初始为空，等待加载
    m_lyrics.clear();
    m_currentLyricIndex = -1;

    // 分割线（与音乐旋律相关：用带渐变的线来表现）
    auto *lineContainer = new QWidget(this);
    lineContainer->setFixedWidth(6);
    lineContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    lineContainer->setStyleSheet(QStringLiteral(
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        " stop:0 #00e676, stop:0.5 #1de9b6, stop:1 #00b0ff);"
    ));

    centerLayout->addWidget(m_coverLabel, 2);
    centerLayout->addWidget(lineContainer);
    centerLayout->addWidget(m_lyricsList, 3);

    mainLayout->addLayout(centerLayout, 1);
}

void PlayerPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // 确保返回按钮始终在左上角
    if (m_backButton) {
        m_backButton->move(20, 20);
        m_backButton->raise(); // 确保按钮在最上层
    }
}

void PlayerPanel::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    // 绘制整体背景
    QPainter bg(this);
    bg.fillRect(rect(), QLinearGradient(0, 0, 0, height()));
    QLinearGradient g(0, 0, 0, height());
    g.setColorAt(0.0, QColor(20, 20, 20));
    g.setColorAt(1.0, QColor(10, 10, 10));
    bg.fillRect(rect(), g);
    
    // 绘制返回按钮的倒三角图标
    if (m_backButton) {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        const QRect btnRect = m_backButton->geometry();
        const QPoint center = btnRect.center();
        const int size = 12;
        
        p.setPen(QPen(QColor(255, 255, 255), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        
        // 绘制倒三角（向下箭头）
        QPolygon triangle;
        triangle << QPoint(center.x() - size/2, center.y() - size/3)
                 << QPoint(center.x() + size/2, center.y() - size/3)
                 << QPoint(center.x(), center.y() + size/3);
        p.drawPolygon(triangle);
    }

    // 在封面区域绘制旋转唱片
    if (!m_coverLabel)
        return;

    const QRect coverRect = m_coverLabel->geometry();
    if (coverRect.isEmpty())
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    p.translate(coverRect.center());
    p.rotate(m_angle);

    const int radius = qMin(coverRect.width(), coverRect.height()) / 2 - 10;

    QRadialGradient grad(QPointF(0, 0), radius);
    grad.setColorAt(0.0, QColor(40, 40, 40));
    grad.setColorAt(0.6, QColor(80, 80, 80));
    grad.setColorAt(1.0, QColor(20, 20, 20));

    p.setBrush(grad);
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPointF(0, 0), radius, radius);

    // 中心小圆
    p.setBrush(QColor("#00c853"));
    p.drawEllipse(QPointF(0, 0), radius / 6.0, radius / 6.0);
}

void PlayerPanel::onRotateTick()
{
    m_angle += 1.0;
    if (m_angle >= 360.0)
        m_angle -= 360.0;
    update();
}

void PlayerPanel::loadLyrics(const QString &filePath)
{
    m_lyrics.clear();
    m_currentLyricIndex = -1;
    m_previousLyricIndex = -1;
    m_lyricsList->clear();
    
    if (filePath.isEmpty() || !QFile::exists(filePath))
        return;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    
    // 读取文件内容到字节数组，用于编码检测
    QByteArray data = file.readAll();
    file.close();
    
    // 尝试多种编码
    QString content;
    QStringConverter::Encoding encoding = QStringConverter::Utf8;
    
    // 首先尝试UTF-8
    QStringDecoder utf8Decoder(QStringConverter::Utf8);
    QString utf8Content = utf8Decoder.decode(data);
    if (!utf8Decoder.hasError() && !utf8Content.isEmpty()) {
        content = utf8Content;
    } else {
        // UTF-8失败，尝试系统默认编码（Windows通常是GBK）
        QStringDecoder localDecoder(QStringConverter::System);
        QString localContent = localDecoder.decode(data);
        if (!localDecoder.hasError() && !localContent.isEmpty()) {
            content = localContent;
        } else {
            // 如果都失败，尝试Latin1作为最后手段
            QStringDecoder latin1Decoder(QStringConverter::Latin1);
            content = latin1Decoder.decode(data);
        }
    }
    
    // 使用QString创建QTextStream
    QTextStream in(&content, QIODevice::ReadOnly);
    
    // LRC格式：[mm:ss.xx]歌词内容
    QRegularExpression timeRegex(QStringLiteral(R"(\[(\d{2}):(\d{2})\.(\d{2})\])"));
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;
        
        // 匹配时间标签
        QRegularExpressionMatchIterator matches = timeRegex.globalMatch(line);
        if (!matches.hasNext()) {
            // 没有时间标签，可能是元数据或其他内容，跳过
            continue;
        }
        
        // 提取歌词文本（移除所有时间标签）
        QString lyricText = line;
        lyricText.remove(timeRegex);
        lyricText = lyricText.trimmed();
        
        if (lyricText.isEmpty())
            continue;
        
        // 处理多个时间标签（同一句歌词可能有多个时间点）
        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            int minutes = match.captured(1).toInt();
            int seconds = match.captured(2).toInt();
            int centiseconds = match.captured(3).toInt();
            qint64 timeMs = (minutes * 60 + seconds) * 1000 + centiseconds * 10;
            
            LyricLine lyric;
            lyric.timeMs = timeMs;
            lyric.text = lyricText;
            m_lyrics.append(lyric);
        }
    }
    
    // 按时间排序
    std::sort(m_lyrics.begin(), m_lyrics.end(), 
              [](const LyricLine &a, const LyricLine &b) { return a.timeMs < b.timeMs; });
    
    // 添加到列表显示
    for (const auto &lyric : m_lyrics) {
        m_lyricsList->addItem(lyric.text);
    }
}

void PlayerPanel::updateLyricsPosition(qint64 positionMs)
{
    if (m_lyrics.isEmpty())
        return;
    
    // 找到当前应该显示的歌词索引
    int newIndex = -1;
    for (int i = m_lyrics.size() - 1; i >= 0; --i) {
        if (m_lyrics.at(i).timeMs <= positionMs) {
            newIndex = i;
            break;
        }
    }
    
    // 如果索引改变，更新高亮和滚动
    if (newIndex != m_currentLyricIndex) {
        // 恢复之前歌词的字体大小（100%）
        if (m_previousLyricIndex >= 0 && m_previousLyricIndex < m_lyricsList->count()) {
            QListWidgetItem *prevItem = m_lyricsList->item(m_previousLyricIndex);
            QFont prevFont = prevItem->font();
            prevFont.setPointSizeF(16.0); // 恢复100%
            prevItem->setFont(prevFont);
        }
        
        // 清除所有高亮
        for (int i = 0; i < m_lyricsList->count(); ++i) {
            m_lyricsList->item(i)->setSelected(false);
        }
        
        if (newIndex >= 0 && newIndex < m_lyricsList->count()) {
            m_previousLyricIndex = m_currentLyricIndex;
            m_currentLyricIndex = newIndex;
            
            QListWidgetItem *currentItem = m_lyricsList->item(m_currentLyricIndex);
            currentItem->setSelected(true);
            
            // 放大当前歌词到150%
            QFont currentFont = currentItem->font();
            currentFont.setPointSizeF(24.0); // 150% of 16px = 24px
            currentItem->setFont(currentFont);
            
            // 平滑滚动到当前歌词（居中显示）
            m_lyricsList->scrollToItem(currentItem, QAbstractItemView::PositionAtCenter);
        } else {
            // 如果播放位置在第一个歌词之前，清除所有高亮
            m_previousLyricIndex = m_currentLyricIndex;
            m_currentLyricIndex = -1;
        }
    }
}


