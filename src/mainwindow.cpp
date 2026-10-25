#include "mainwindow.h"
#include "playerpanel.h"
#include "databasemanager.h"
#include "logindialog.h"
#include "registerdialog.h"
#include "heartbutton.h"

#include <QAudioOutput>
#include <QBoxLayout>
#include <QEvent>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QInputDialog>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMediaPlayer>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QSlider>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QUrl>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("简易音乐播放器 - Qt6"));
    setMinimumSize(1024, 640);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet(QStringLiteral(
        "QMainWindow { background-color: transparent; }"
        "QToolTip { color: #ffffff; background-color: #607d8b; border: none; }"
    ));

    setupUi();
    setupConnections();
    setupPlayer();
}

void MainWindow::setupUi()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_centralWidget->setStyleSheet(QStringLiteral("background: transparent;"));

    auto *mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(0);

    m_backgroundFrame = new QFrame(m_centralWidget);
    m_backgroundFrame->setObjectName(QStringLiteral("backgroundFrame"));
    m_backgroundFrame->setStyleSheet(QStringLiteral(
        "#backgroundFrame {"
        "  background: rgba(255, 255, 255, 0.9);"
        "  border-radius: 28px;"
        "  border: 1px solid rgba(255, 255, 255, 0.35);"
        "  backdrop-filter: blur(20px);"
        "}"
    ));
    auto *shadow = new QGraphicsDropShadowEffect(m_backgroundFrame);
    shadow->setBlurRadius(40.0);
    shadow->setOffset(0, 20);
    shadow->setColor(QColor(0, 0, 0, 80));
    m_backgroundFrame->setGraphicsEffect(shadow);
    m_backgroundFrame->installEventFilter(this);

    auto *cardLayout = new QVBoxLayout(m_backgroundFrame);
    cardLayout->setContentsMargins(24, 24, 24, 24);
    cardLayout->setSpacing(0);

    setupTopBar();
    setupCenter();
    setupBottomBar();

    cardLayout->addWidget(m_topBar);
    cardLayout->addWidget(m_centerComposite, 1);
    cardLayout->addWidget(m_bottomBar);

    mainLayout->addWidget(m_backgroundFrame);
}

void MainWindow::setupTopBar()
{
    m_topBar = new QWidget(m_backgroundFrame);
    m_topBar->setFixedHeight(56);
    m_topBar->setAttribute(Qt::WA_StyledBackground, true);
    m_topBar->setStyleSheet(QStringLiteral("background: transparent;"));

    auto *layout = new QHBoxLayout(m_topBar);
    layout->setContentsMargins(8, 0, 8, 0);
    layout->setSpacing(12);

    // 搜索栏
    m_searchEdit = new QLineEdit(m_topBar);
    m_searchEdit->setPlaceholderText(QStringLiteral("搜索歌曲 / 歌手 / 专辑"));
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setFixedHeight(36);
    m_searchEdit->setStyleSheet(QStringLiteral(
        "QLineEdit {"
        "  background: rgba(255,255,255,0.95);"
        "  border-radius: 18px;"
        "  padding-left: 12px;"
        "  color: #37474f;"
        "  border: 1px solid #dde3f0;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #7c9cff;"
        "}"
    ));

    m_userButton = new QPushButton(QStringLiteral("未登录（点击登录/注册）"), m_topBar);
    m_userButton->setCursor(Qt::PointingHandCursor);
    m_userButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: #455a64; font-size: 13px;"
        "  background: transparent;"
        "  border-radius: 18px;"
        "  border: 1px solid #dde3f0;"
        "  padding: 6px 16px;"
        "}"
        "QPushButton:hover {"
        "  color: #3f51b5;"
        "  background: rgba(243,246,255,0.6);"
        "}"
        "QPushButton:pressed {"
        "  background: rgba(255,255,255,0.6);"
        "}"
    ));

    layout->addWidget(m_searchEdit, 3);
    layout->addStretch(1);
    layout->addWidget(m_userButton);
    m_settingsButton = new QPushButton(QStringLiteral("设置"), m_topBar);
    m_settingsButton->setCursor(Qt::PointingHandCursor);
    m_settingsButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: #455a64;"
        "  background: transparent;"
        "  border: 1px solid transparent;"
        "  padding: 6px 16px;"
        "  border-radius: 18px;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(255,255,255,0.3);"
        "}"
        "QPushButton:pressed {"
        "  background: rgba(255,255,255,0.15);"
        "}"
    ));
    layout->addWidget(m_settingsButton);

    const QString windowBtnStyle = QStringLiteral(
        "QPushButton {"
        "  color: #607d8b;"
        "  background: transparent;"
        "  border: 1px solid rgba(96,125,139,0.3);"
        "  width: 34px;"
        "  height: 24px;"
        "  font-size: 16px;"
        "  border-radius: 12px;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(255,255,255,0.4);"
        "}"
        "QPushButton:pressed {"
        "  background: rgba(255,255,255,0.2);"
        "}"
    );
    layout->addSpacing(8);
    m_minButton = new QPushButton(QStringLiteral("—"), m_topBar);
    m_minButton->setStyleSheet(windowBtnStyle);
    layout->addWidget(m_minButton);

    m_maxButton = new QPushButton(QStringLiteral("□"), m_topBar);
    m_maxButton->setStyleSheet(windowBtnStyle);
    layout->addWidget(m_maxButton);

    m_closeButton = new QPushButton(QStringLiteral("×"), m_topBar);
    m_closeButton->setStyleSheet(windowBtnStyle + QStringLiteral(
        "QPushButton:hover { background: #ff5252; color: white; }"
        "QPushButton:pressed { background: #ff1744; }"
    ));
    layout->addWidget(m_closeButton);
}

void MainWindow::setupCenter()
{
    m_centerComposite = new QWidget(m_backgroundFrame);
    auto *hLayout = new QHBoxLayout(m_centerComposite);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);

    setupLeftSidebar();

    m_centerStack = new QStackedWidget(m_centerComposite);
    m_centerStack->setObjectName(QStringLiteral("centerStack"));
    m_centerStack->setStyleSheet(QStringLiteral(
        "#centerStack {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #ffffff, stop:1 #f3f6ff);"
        "  border-radius: 16px;"
        "}"
    ));

    QWidget *playlistPage = new QWidget(this);
    auto *layout = new QVBoxLayout(playlistPage);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(12);

    m_playlistTitleLabel = new QLabel(QStringLiteral("本地歌单"), playlistPage);
    m_playlistTitleLabel->setStyleSheet(QStringLiteral(
        "color: #37474f; font-size: 20px; font-weight: 600;"
    ));

    // 批量操作按钮
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(8);
    buttonLayout->addStretch();
    
    m_batchAddButton = new QPushButton(QStringLiteral("批量添加"), playlistPage);
    m_batchAddButton->setCursor(Qt::PointingHandCursor);
    m_batchAddButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: #ffffff;"
        "  background: #7c9cff;"
        "  border-radius: 16px;"
        "  border: none;"
        "  padding: 8px 20px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background: #6a89ff;"
        "}"
    ));
    
    m_batchDeleteButton = new QPushButton(QStringLiteral("批量删除"), playlistPage);
    m_batchDeleteButton->setCursor(Qt::PointingHandCursor);
    m_batchDeleteButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: #ffffff;"
        "  background: #ff5252;"
        "  border-radius: 16px;"
        "  border: none;"
        "  padding: 8px 20px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "  background: #ff1744;"
        "}"
    ));
    
    buttonLayout->addWidget(m_batchAddButton);
    buttonLayout->addWidget(m_batchDeleteButton);

    m_playlistTable = new QTableWidget(0, 3, playlistPage);
    QStringList headers;
    headers << QStringLiteral("歌曲") << QStringLiteral("歌手") << QStringLiteral("时长");
    m_playlistTable->setHorizontalHeaderLabels(headers);
    m_playlistTable->horizontalHeader()->setStretchLastSection(true);
    m_playlistTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_playlistTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_playlistTable->horizontalHeader()->setStyleSheet(QStringLiteral(
        "QHeaderView::section {"
        "  background: transparent; color: #78909c; border: none; padding: 6px 4px;"
        "  font-weight: 500;"
        "}"
    ));
    m_playlistTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_playlistTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_playlistTable->setShowGrid(false);
    m_playlistTable->setAlternatingRowColors(true);
    m_playlistTable->setContextMenuPolicy(Qt::CustomContextMenu);
    m_playlistTable->setFocusPolicy(Qt::StrongFocus); // 确保可以接收键盘事件
    m_playlistTable->setStyleSheet(QStringLiteral(
        "QTableWidget {"
        "  background: transparent;"
        "  color: #37474f;"
        "  gridline-color: transparent;"
        "  selection-background-color: rgba(124,156,255,0.18);"
        "  selection-color: #1a237e;"
        "  alternate-background-color: rgba(255,255,255,0.7);"
        "}"
        "QTableCornerButton::section { background: transparent; border: none; }"
    ));

    layout->addWidget(m_playlistTitleLabel);
    layout->addLayout(buttonLayout);
    layout->addWidget(m_playlistTable, 1);

    m_playerPanel = new PlayerPanel(this);

    m_centerStack->addWidget(playlistPage);
    m_centerStack->addWidget(m_playerPanel);

    hLayout->addWidget(m_leftSidebar);
    hLayout->addWidget(m_centerStack, 1);

    loadDemoPlaylist();
}

void MainWindow::setupLeftSidebar()
{
    m_leftSidebar = new QWidget(m_centerComposite);
    m_leftSidebar->setFixedWidth(220);
    m_leftSidebar->setAttribute(Qt::WA_StyledBackground, true);
    m_leftSidebar->setStyleSheet(QStringLiteral(
        "QWidget {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #fdfbff, stop:1 #f1f5ff);"
        "  border-radius: 16px;"
        "}"
        "QListWidget {"
        "  background: transparent;"
        "  border: none;"
        "  color: #455a64;"
        "}"
        "QListWidget::item { padding: 4px 2px; }"
        "QListWidget::item:selected { background: rgba(124,156,255,0.18); }"
    ));

    auto *layout = new QVBoxLayout(m_leftSidebar);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    // 在线音乐
    const QString badgeStyle = QStringLiteral(
        "color: #5c6f82;"
        "background: rgba(255,255,255,0.85);"
        "border: 1px solid #d0d8e7;"
        "border-radius: 14px;"
        "padding: 4px 12px;"
        "font-size: 12px;"
        "letter-spacing: 1px;"
    );

    auto *onlineTitle = new QLabel(QStringLiteral("在线音乐"), m_leftSidebar);
    onlineTitle->setAlignment(Qt::AlignCenter);
    onlineTitle->setStyleSheet(badgeStyle);
    m_onlineList = new QListWidget(m_leftSidebar);
    m_onlineList->addItems(QStringList()
                           << QStringLiteral("推荐歌曲")
                           << QStringLiteral("新歌速递")
                           << QStringLiteral("排行榜"));

    // 我喜欢（作为特殊歌单）
    auto *favTitle = new QLabel(QStringLiteral("我喜欢"), m_leftSidebar);
    favTitle->setAlignment(Qt::AlignCenter);
    favTitle->setStyleSheet(badgeStyle);
    m_favoriteList = new QListWidget(m_leftSidebar);
    // 添加一个特殊项表示"我喜欢"歌单
    auto *favoriteItem = new QListWidgetItem(QStringLiteral("我喜欢"), m_favoriteList);
    favoriteItem->setData(Qt::UserRole, DatabaseManager::FAVORITE_PLAYLIST_ID);

    // 用户歌单
    auto *userTitle = new QLabel(QStringLiteral("我的歌单"), m_leftSidebar);
    userTitle->setAlignment(Qt::AlignCenter);
    userTitle->setStyleSheet(badgeStyle);
    m_userPlaylistList = new QListWidget(m_leftSidebar);
    m_newPlaylistButton = new QPushButton(QStringLiteral("新建歌单"), m_leftSidebar);
    m_newPlaylistButton->setCursor(Qt::PointingHandCursor);
    m_newPlaylistButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: #455a64;"
        "  background: rgba(255,255,255,0.9);"
        "  border-radius: 18px;"
        "  border: 1px solid #cfd8dc;"
        "  padding: 6px 16px;"
        "}"
        "QPushButton:hover {"
        "  background: #f3f6ff;"
        "}"
    ));
    
    m_deletePlaylistButton = new QPushButton(QStringLiteral("删除歌单"), m_leftSidebar);
    m_deletePlaylistButton->setCursor(Qt::PointingHandCursor);
    m_deletePlaylistButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: #ffffff;"
        "  background: #ff5252;"
        "  border-radius: 18px;"
        "  border: none;"
        "  padding: 6px 16px;"
        "}"
        "QPushButton:hover {"
        "  background: #ff1744;"
        "}"
    ));

    layout->addWidget(onlineTitle);
    layout->addWidget(m_onlineList);
    layout->addSpacing(6);
    layout->addWidget(favTitle);
    layout->addWidget(m_favoriteList);
    layout->addSpacing(6);
    layout->addWidget(userTitle);
    layout->addWidget(m_userPlaylistList, 1);
    layout->addWidget(m_newPlaylistButton);
    layout->addWidget(m_deletePlaylistButton);
}

void MainWindow::setupBottomBar()
{
    m_bottomBar = new QWidget(m_backgroundFrame);
    m_bottomBar->setFixedHeight(80);
    m_bottomBar->setAttribute(Qt::WA_StyledBackground, true);
    m_bottomBar->setStyleSheet(QStringLiteral(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #f6f0ff, stop:1 #e8f1ff);"
        "border-radius: 16px;"
    ));

    auto *layout = new QHBoxLayout(m_bottomBar);
    layout->setContentsMargins(16, 10, 16, 10);
    layout->setSpacing(12);

    // 左侧封面缩略图按钮
    m_coverButton = new QPushButton(m_bottomBar);
    m_coverButton->setFixedSize(56, 56);
    m_coverButton->setCursor(Qt::PointingHandCursor);
    m_coverButton->setToolTip(QStringLiteral("点击查看歌词和旋转封面"));
    m_coverButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  border-radius: 8px;"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "      stop:0 #ff9a9e, stop:1 #fad0c4);"
        "  border: 1px solid rgba(255,255,255,0.6);"
        "}"
        "QPushButton:hover {"
        "  opacity: 0.9;"
        "}"
    ));

    auto *songInfoLayout = new QVBoxLayout;
    songInfoLayout->setContentsMargins(0, 0, 0, 0);
    songInfoLayout->setSpacing(2);

    m_songTitleLabel = new QLabel(QStringLiteral("当前歌曲：--"), m_bottomBar);
    m_songTitleLabel->setStyleSheet(QStringLiteral(
        "color: #37474f; font-size: 14px;"
    ));
    m_artistLabel = new QLabel(QStringLiteral("歌手：--"), m_bottomBar);
    m_artistLabel->setStyleSheet(QStringLiteral(
        "color: #90a4ae; font-size: 12px;"
    ));

    songInfoLayout->addWidget(m_songTitleLabel);
    songInfoLayout->addWidget(m_artistLabel);

    m_prevButton = new QPushButton(QStringLiteral("上一首"), m_bottomBar);
    m_playPauseButton = new QPushButton(QStringLiteral("播放"), m_bottomBar);
    m_nextButton = new QPushButton(QStringLiteral("下一首"), m_bottomBar);

    const auto ctrlBtnStyle = QStringLiteral(
        "QPushButton {"
        "  min-width: 60px;"
        "  color: #455a64;"
        "  background: #ffffff;"
        "  border-radius: 16px;"
        "  border: 1px solid #cfd8dc;"
        "  padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "  background: #f3f6ff;"
        "}"
    );

    // 加入我喜欢 按钮（爱心图标，无圆形背景）
    m_favoriteButton = new HeartButton(m_bottomBar);
    m_favoriteButton->setToolTip(QStringLiteral("加入我喜欢"));
    m_favoriteButton->setFavorite(false); // 初始为空心
    updateFavoriteButtonStyle(false); // 设置样式（透明背景，无边框）

    m_prevButton->setStyleSheet(ctrlBtnStyle);
    m_playPauseButton->setStyleSheet(ctrlBtnStyle);
    m_nextButton->setStyleSheet(ctrlBtnStyle);

    layout->addWidget(m_coverButton);
    layout->addLayout(songInfoLayout);
    layout->addWidget(m_favoriteButton);
    layout->addStretch(1);

    // 中间加入进度条
    m_positionSlider = new QSlider(Qt::Horizontal, m_bottomBar);
    m_positionSlider->setRange(0, 0);
    m_positionSlider->setStyleSheet(QStringLiteral(
        "QSlider::groove:horizontal {"
        "  height: 4px;"
        "  background: #404040;"
        "  border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: #1db954;"
        "  width: 12px;"
        "  margin: -4px 0;"
        "  border-radius: 6px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: #1db954;"
        "  border-radius: 2px;"
        "}"
    ));
    layout->addWidget(m_prevButton);
    layout->addWidget(m_playPauseButton);
    layout->addWidget(m_nextButton);
    layout->addWidget(m_positionSlider, 2);

    connect(m_favoriteButton, &QPushButton::clicked, this, &MainWindow::onAddToFavorite);
}

void MainWindow::setupConnections()
{
    connect(m_coverButton, &QPushButton::clicked, this, &MainWindow::onCoverClicked);
    connect(m_playerPanel, &PlayerPanel::backRequested, this, &MainWindow::onBackToMain);

    connect(m_playPauseButton, &QPushButton::clicked, this, &MainWindow::onPlayPause);
    connect(m_prevButton, &QPushButton::clicked, this, &MainWindow::onPrev);
    connect(m_nextButton, &QPushButton::clicked, this, &MainWindow::onNext);
    connect(m_positionSlider, &QSlider::sliderMoved, this, &MainWindow::onSliderMoved);
    connect(m_newPlaylistButton, &QPushButton::clicked, this, &MainWindow::onCreatePlaylist);
    connect(m_deletePlaylistButton, &QPushButton::clicked, this, &MainWindow::onDeletePlaylist);
    connect(m_favoriteList, &QListWidget::itemClicked, this, &MainWindow::onFavoriteListClicked);
    connect(m_userButton, &QPushButton::clicked, this, &MainWindow::onUserButtonClicked);
    connect(m_playlistTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onSongDoubleClicked);
    connect(m_playlistTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::onPlaylistTableContextMenu);
    if (m_batchAddButton)
        connect(m_batchAddButton, &QPushButton::clicked, this, &MainWindow::onBatchAddSongs);
    if (m_batchDeleteButton)
        connect(m_batchDeleteButton, &QPushButton::clicked, this, &MainWindow::onBatchDeleteSongs);
    connect(m_userPlaylistList, &QListWidget::itemClicked, this, &MainWindow::onUserPlaylistSelected);
    connect(m_minButton, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(m_maxButton, &QPushButton::clicked, this, [this] {
        isMaximized() ? showNormal() : showMaximized();
    });
    connect(m_closeButton, &QPushButton::clicked, this, &QWidget::close);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
}

void MainWindow::onCoverClicked()
{
    if (!m_centerStack)
        return;
    
    // 向上拉动画：进入歌词界面
    if (m_slideAnimation) {
        m_slideAnimation->stop();
        m_slideAnimation->deleteLater();
        m_slideAnimation = nullptr;
    }
    
    // 先隐藏侧边栏
    if (m_leftSidebar)
        m_leftSidebar->setVisible(false);
    
    // 切换到歌词界面
    m_centerStack->setCurrentIndex(1);
    
    // 确保位置正确（重置可能被动画改变的位置）
    m_centerStack->move(0, 0);
    
    // 从底部向上滑入动画
    const QPoint startPos = QPoint(0, m_centerStack->height());
    const QPoint endPos = QPoint(0, 0);
    m_centerStack->move(startPos);
    
    m_slideAnimation = new QPropertyAnimation(m_centerStack, "pos", this);
    m_slideAnimation->setDuration(300);
    m_slideAnimation->setStartValue(startPos);
    m_slideAnimation->setEndValue(endPos);
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_slideAnimation->start();
}

void MainWindow::onBackToMain()
{
    if (!m_centerStack)
        return;
    
    // 向下拉动画：退出歌词界面
    if (m_slideAnimation) {
        m_slideAnimation->stop();
        m_slideAnimation->deleteLater();
        m_slideAnimation = nullptr;
    }
    
    // 向下滑出动画
    const QPoint startPos = m_centerStack->pos();
    const QPoint endPos = QPoint(0, m_centerStack->height());
    
    m_slideAnimation = new QPropertyAnimation(m_centerStack, "pos", this);
    m_slideAnimation->setDuration(300);
    m_slideAnimation->setStartValue(startPos);
    m_slideAnimation->setEndValue(endPos);
    m_slideAnimation->setEasingCurve(QEasingCurve::InCubic);
    
    connect(m_slideAnimation, &QPropertyAnimation::finished, this, [this]() {
        // 切换回主界面
        m_centerStack->setCurrentIndex(0);
        // 重置位置，确保布局正常
        m_centerStack->move(0, 0);
        // 显示侧边栏
        if (m_leftSidebar)
            m_leftSidebar->setVisible(true);
        // 清理动画对象
        if (m_slideAnimation) {
            m_slideAnimation->deleteLater();
            m_slideAnimation = nullptr;
        }
    });
    
    m_slideAnimation->start();
}

void MainWindow::setCurrentUser(int userId, const QString &username)
{
    m_currentUserId = userId;
    m_currentUsername = username;
    if (m_userButton) {
        if (userId > 0) {
            m_userButton->setText(QStringLiteral("已登录：%1").arg(username));
        } else {
            m_userButton->setText(QStringLiteral("未登录（点击登录/注册）"));
        }
    }
    refreshFavorites();
    refreshUserPlaylists();
}

void MainWindow::onUserButtonClicked()
{
    QMenu menu(this);
    QAction *loginAct = menu.addAction(QStringLiteral("登录"));
    QAction *registerAct = menu.addAction(QStringLiteral("注册"));
    QAction *logoutAct = nullptr;
    if (m_currentUserId > 0) {
        logoutAct = menu.addAction(QStringLiteral("退出登录"));
    }

    QAction *chosen = menu.exec(m_userButton->mapToGlobal(
        QPoint(m_userButton->width() / 2, m_userButton->height())));
    if (!chosen)
        return;

    if (chosen == loginAct) {
        LoginDialog dlg(this);
        dlg.setStyleSheet(QStringLiteral("QDialog { background-color: #f5f7fb; }"));
        if (dlg.exec() != QDialog::Accepted)
            return;
        int userId = 0;
        if (DatabaseManager::instance().login(dlg.username(), dlg.password(), userId)) {
            setCurrentUser(userId, dlg.username());
        } else {
            QMessageBox::warning(this, QStringLiteral("登录失败"), QStringLiteral("用户名或密码错误"));
        }
    } else if (chosen == registerAct) {
        RegisterDialog reg(this);
        reg.setStyleSheet(QStringLiteral("QDialog { background-color: #f5f7fb; }"));
        if (reg.exec() != QDialog::Accepted)
            return;
        int userId = 0;
        if (DatabaseManager::instance().login(reg.username(), reg.password(), userId)) {
            setCurrentUser(userId, reg.username());
        }
    } else if (logoutAct && chosen == logoutAct) {
        setCurrentUser(0, QString());
    }
}

void MainWindow::onSongDoubleClicked(int row, int)
{
    playSongAt(row);
}

void MainWindow::onUserPlaylistSelected(QListWidgetItem *item)
{
    if (!item)
        return;
    const int playlistId = item->data(Qt::UserRole).toInt();
    const auto songs = DatabaseManager::instance().playlistSongs(playlistId);
    m_currentPlaylistId = playlistId;
    setCurrentSongs(songs, item->text());
}

void MainWindow::onFavoriteListClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    const int playlistId = item->data(Qt::UserRole).toInt();
    if (playlistId == DatabaseManager::FAVORITE_PLAYLIST_ID) {
        // 加载"我喜欢"歌单（从favorite_songs表）
        const auto favs = DatabaseManager::instance().favoritesForUser(m_currentUserId);
        m_currentPlaylistId = DatabaseManager::FAVORITE_PLAYLIST_ID;
        setCurrentSongs(favs, QStringLiteral("我喜欢"));
    }
}

void MainWindow::onPlaylistTableContextMenu(const QPoint &pos)
{
    if (m_currentPlaylistId <= 0 || !m_playlistTable)
        return;
    
    const int row = m_playlistTable->rowAt(pos.y());
    if (row < 0 || row >= m_currentSongs.size())
        return;
    
    // 设置当前选中行
    m_playlistTable->selectRow(row);
    
    QMenu menu(this);
    QAction *addToOtherAct = menu.addAction(QStringLiteral("添加到其他歌单..."));
    QAction *removeAct = menu.addAction(QStringLiteral("删除歌曲"));
    removeAct->setShortcut(QKeySequence::Delete);
    menu.addSeparator();
    QAction *addSongAct = menu.addAction(QStringLiteral("添加歌曲到此歌单..."));
    
    QAction *selected = menu.exec(m_playlistTable->viewport()->mapToGlobal(pos));
    if (!selected)
        return;
    
    if (selected == addToOtherAct) {
        onAddSongToOtherPlaylist();
    } else if (selected == removeAct) {
        onRemoveSongFromPlaylist();
    } else if (selected == addSongAct) {
        onAddSongToPlaylist();
    }
}

void MainWindow::onAddSongToPlaylist()
{
    if (m_currentPlaylistId <= 0)
        return;
    
    const QStringList files = QFileDialog::getOpenFileNames(this,
        QStringLiteral("选择要添加的歌曲"), QString(),
        QStringLiteral("音频文件 (*.mp3 *.wav *.flac *.m4a *.aac);;所有文件 (*.*)"));
    if (files.isEmpty())
        return;
    
    // "我喜欢"歌单使用不同的添加逻辑
    if (m_currentPlaylistId == DatabaseManager::FAVORITE_PLAYLIST_ID) {
        for (const QString &filePath : files) {
            const qint64 durationMs = readDurationMs(filePath);
            FavoriteSong song = songFromFile(filePath, durationMs);
            DatabaseManager::instance().addFavoriteSong(m_currentUserId, song);
        }
        // 刷新当前歌单
        const auto songs = DatabaseManager::instance().favoritesForUser(m_currentUserId);
        setCurrentSongs(songs, QStringLiteral("我喜欢"));
    } else {
        for (const QString &filePath : files) {
            const qint64 durationMs = readDurationMs(filePath);
            FavoriteSong song = songFromFile(filePath, durationMs);
            DatabaseManager::instance().addSongToPlaylist(m_currentPlaylistId, song);
        }
        // 刷新当前歌单
        const auto songs = DatabaseManager::instance().playlistSongs(m_currentPlaylistId);
        setCurrentSongs(songs, m_currentPlaylistName);
    }
}

void MainWindow::onRemoveSongFromPlaylist()
{
    if (m_currentPlaylistId <= 0 || !m_playlistTable)
        return;
    
    const int row = m_playlistTable->currentRow();
    if (row < 0 || row >= m_currentSongs.size())
        return;
    
    const FavoriteSong &song = m_currentSongs.at(row);
    
    // 确认删除
    int ret = QMessageBox::question(this, QStringLiteral("确认删除"),
                                   QStringLiteral("确定要从歌单中删除《%1》吗？").arg(song.title),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;
    
    // "我喜欢"歌单使用不同的删除逻辑
    if (m_currentPlaylistId == DatabaseManager::FAVORITE_PLAYLIST_ID) {
        DatabaseManager::instance().removeFavoriteSong(m_currentUserId, song);
        // 刷新当前歌单
        const auto songs = DatabaseManager::instance().favoritesForUser(m_currentUserId);
        setCurrentSongs(songs, QStringLiteral("我喜欢"));
    } else if (song.id > 0) {
        DatabaseManager::instance().removeSongFromPlaylist(m_currentPlaylistId, song.id);
        // 刷新当前歌单
        const auto songs = DatabaseManager::instance().playlistSongs(m_currentPlaylistId);
        setCurrentSongs(songs, m_currentPlaylistName);
    }
    
    // 如果删除的是当前播放的歌曲，停止播放
    if (row == m_currentSongIndex && m_player) {
        m_player->stop();
        m_currentSongIndex = -1;
        if (m_playPauseButton)
            m_playPauseButton->setText(QStringLiteral("播放"));
        if (m_songTitleLabel)
            m_songTitleLabel->setText(QStringLiteral("当前歌曲：--"));
        if (m_artistLabel)
            m_artistLabel->setText(QStringLiteral("歌手：--"));
    }
}

void MainWindow::onDeletePlaylist()
{
    if (m_currentPlaylistId <= 0 || m_currentPlaylistId == DatabaseManager::FAVORITE_PLAYLIST_ID) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("无法删除\"我喜欢\"歌单"));
        return;
    }
    
    if (!m_userPlaylistList)
        return;
    
    QListWidgetItem *currentItem = m_userPlaylistList->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择要删除的歌单"));
        return;
    }
    
    const QString playlistName = currentItem->text();
    const int playlistId = currentItem->data(Qt::UserRole).toInt();
    
    // 确认删除
    int ret = QMessageBox::question(this, QStringLiteral("确认删除"),
                                   QStringLiteral("确定要删除歌单《%1》吗？\n此操作将删除歌单中的所有歌曲，且无法恢复。").arg(playlistName),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;
    
    if (DatabaseManager::instance().deletePlaylist(playlistId)) {
        refreshUserPlaylists();
        
        // 如果删除的是当前显示的歌单，清空显示
        if (playlistId == m_currentPlaylistId) {
            m_currentPlaylistId = -1;
            setCurrentSongs(QVector<FavoriteSong>(), QString());
        }
        
        QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("歌单已删除"));
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("删除歌单失败"));
    }
}

void MainWindow::onAddSongToOtherPlaylist()
{
    if (!m_playlistTable)
        return;
    
    const int row = m_playlistTable->currentRow();
    if (row < 0 || row >= m_currentSongs.size())
        return;
    
    const FavoriteSong &song = m_currentSongs.at(row);
    
    // 获取用户的所有歌单（包括"我喜欢"）
    QStringList playlistNames;
    playlistNames << QStringLiteral("我喜欢"); // 添加"我喜欢"选项
    
    const auto playlists = DatabaseManager::instance().userPlaylists(m_currentUserId);
    for (const auto &pl : playlists) {
        if (pl.id != m_currentPlaylistId) { // 排除当前歌单
            playlistNames << pl.name;
        }
    }
    
    if (playlistNames.size() == 1 && playlistNames.first() == QStringLiteral("我喜欢") && 
        m_currentPlaylistId == DatabaseManager::FAVORITE_PLAYLIST_ID) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("没有其他歌单可以添加"));
        return;
    }
    
    if (playlistNames.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先创建歌单"));
        return;
    }
    
    bool ok = false;
    const QString selectedName = QInputDialog::getItem(this, QStringLiteral("选择歌单"),
        QStringLiteral("选择要添加到的歌单："), playlistNames, 0, false, &ok);
    if (!ok)
        return;
    
    // 找到选中的歌单ID
    int targetPlaylistId = -1;
    if (selectedName == QStringLiteral("我喜欢")) {
        targetPlaylistId = DatabaseManager::FAVORITE_PLAYLIST_ID;
        DatabaseManager::instance().addFavoriteSong(m_currentUserId, song);
    } else {
        for (const auto &pl : playlists) {
            if (pl.name == selectedName) {
                targetPlaylistId = pl.id;
                break;
            }
        }
        if (targetPlaylistId > 0) {
            DatabaseManager::instance().addSongToPlaylist(targetPlaylistId, song);
        }
    }
    
    if (targetPlaylistId != 0) {
        QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("已添加到歌单：%1").arg(selectedName));
    }
}

void MainWindow::onBatchAddSongs()
{
    if (m_currentPlaylistId <= 0)
        return;
    
    // "我喜欢"歌单使用不同的添加逻辑
    if (m_currentPlaylistId == DatabaseManager::FAVORITE_PLAYLIST_ID) {
        const QStringList files = QFileDialog::getOpenFileNames(this,
            QStringLiteral("选择要添加到\"我喜欢\"的歌曲（可多选）"), QString(),
            QStringLiteral("音频文件 (*.mp3 *.wav *.flac *.m4a *.aac);;所有文件 (*.*)"));
        if (files.isEmpty())
            return;
        
        int successCount = 0;
        for (const QString &filePath : files) {
            const qint64 durationMs = readDurationMs(filePath);
            FavoriteSong song = songFromFile(filePath, durationMs);
            if (DatabaseManager::instance().addFavoriteSong(m_currentUserId, song)) {
                successCount++;
            }
        }
        
        // 刷新当前歌单
        const auto songs = DatabaseManager::instance().favoritesForUser(m_currentUserId);
        setCurrentSongs(songs, QStringLiteral("我喜欢"));
        
        QMessageBox::information(this, QStringLiteral("完成"), 
                                QStringLiteral("成功添加 %1 首歌曲到\"我喜欢\"").arg(successCount));
        return;
    }
    
    // 普通歌单的添加逻辑
    
    const QStringList files = QFileDialog::getOpenFileNames(this,
        QStringLiteral("选择要添加的歌曲（可多选）"), QString(),
        QStringLiteral("音频文件 (*.mp3 *.wav *.flac *.m4a *.aac);;所有文件 (*.*)"));
    if (files.isEmpty())
        return;
    
    int successCount = 0;
    for (const QString &filePath : files) {
        const qint64 durationMs = readDurationMs(filePath);
        FavoriteSong song = songFromFile(filePath, durationMs);
        if (DatabaseManager::instance().addSongToPlaylist(m_currentPlaylistId, song)) {
            successCount++;
        }
    }
    
    // 刷新当前歌单
    const auto songs = DatabaseManager::instance().playlistSongs(m_currentPlaylistId);
    setCurrentSongs(songs, m_currentPlaylistName);
    
    QMessageBox::information(this, QStringLiteral("完成"), 
                            QStringLiteral("成功添加 %1 首歌曲").arg(successCount));
}

void MainWindow::onBatchDeleteSongs()
{
    if (m_currentPlaylistId <= 0 || !m_playlistTable)
        return;
    
    // 获取所有选中的行
    QList<int> selectedRows;
    QModelIndexList selectedIndexes = m_playlistTable->selectionModel()->selectedRows();
    for (const QModelIndex &index : selectedIndexes) {
        selectedRows.append(index.row());
    }
    
    if (selectedRows.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("请先选择要删除的歌曲"));
        return;
    }
    
    // "我喜欢"歌单使用不同的删除逻辑
    if (m_currentPlaylistId == DatabaseManager::FAVORITE_PLAYLIST_ID) {
        // 确认删除
        int ret = QMessageBox::question(this, QStringLiteral("确认批量删除"),
                                       QStringLiteral("确定要从\"我喜欢\"中删除选中的 %1 首歌曲吗？").arg(selectedRows.size()),
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
        if (ret != QMessageBox::Yes)
            return;
        
        // 按从大到小排序，避免删除时索引变化
        std::sort(selectedRows.begin(), selectedRows.end(), std::greater<int>());
        
        bool wasPlayingCurrent = false;
        int deletedCount = 0;
        
        for (int row : selectedRows) {
            if (row < 0 || row >= m_currentSongs.size())
                continue;
            
            const FavoriteSong &song = m_currentSongs.at(row);
            DatabaseManager::instance().removeFavoriteSong(m_currentUserId, song);
            deletedCount++;
            
            // 如果删除的是当前播放的歌曲，标记需要停止播放
            if (row == m_currentSongIndex) {
                wasPlayingCurrent = true;
            }
        }
        
        // 刷新当前歌单
        const auto songs = DatabaseManager::instance().favoritesForUser(m_currentUserId);
        setCurrentSongs(songs, QStringLiteral("我喜欢"));
        
        // 如果删除的是当前播放的歌曲，停止播放
        if (wasPlayingCurrent && m_player) {
            m_player->stop();
            m_currentSongIndex = -1;
            if (m_playPauseButton)
                m_playPauseButton->setText(QStringLiteral("播放"));
            if (m_songTitleLabel)
                m_songTitleLabel->setText(QStringLiteral("当前歌曲：--"));
            if (m_artistLabel)
                m_artistLabel->setText(QStringLiteral("歌手：--"));
        }
        
        QMessageBox::information(this, QStringLiteral("完成"), 
                                QStringLiteral("成功删除 %1 首歌曲").arg(deletedCount));
        return;
    }
    
    // 普通歌单的删除逻辑
    // 确认删除
    int ret = QMessageBox::question(this, QStringLiteral("确认批量删除"),
                                   QStringLiteral("确定要删除选中的 %1 首歌曲吗？").arg(selectedRows.size()),
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
    if (ret != QMessageBox::Yes)
        return;
    
    // 按从大到小排序，避免删除时索引变化
    std::sort(selectedRows.begin(), selectedRows.end(), std::greater<int>());
    
    bool wasPlayingCurrent = false;
    int deletedCount = 0;
    
    for (int row : selectedRows) {
        if (row < 0 || row >= m_currentSongs.size())
            continue;
        
        const FavoriteSong &song = m_currentSongs.at(row);
        if (song.id > 0) {
            DatabaseManager::instance().removeSongFromPlaylist(m_currentPlaylistId, song.id);
            deletedCount++;
            
            // 如果删除的是当前播放的歌曲，标记需要停止播放
            if (row == m_currentSongIndex) {
                wasPlayingCurrent = true;
            }
        }
    }
    
    // 刷新当前歌单
    const auto songs = DatabaseManager::instance().playlistSongs(m_currentPlaylistId);
    setCurrentSongs(songs, m_currentPlaylistName);
    
    // 如果删除的是当前播放的歌曲，停止播放
    if (wasPlayingCurrent && m_player) {
        m_player->stop();
        m_currentSongIndex = -1;
        if (m_playPauseButton)
            m_playPauseButton->setText(QStringLiteral("播放"));
        if (m_songTitleLabel)
            m_songTitleLabel->setText(QStringLiteral("当前歌曲：--"));
        if (m_artistLabel)
            m_artistLabel->setText(QStringLiteral("歌手：--"));
    }
    
    QMessageBox::information(this, QStringLiteral("完成"), 
                            QStringLiteral("成功删除 %1 首歌曲").arg(deletedCount));
}

void MainWindow::setupPlayer()
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.5);

    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);

    // 默认加载第 0 行示例歌曲（如果存在并且路径有效的话）
    // 这里留空：需要你运行时根据本地 mp3 路径修改

    m_currentSongIndex = -1;
}

void MainWindow::loadDemoPlaylist()
{
    setCurrentSongs({}, QStringLiteral("本地歌单"));
}

void MainWindow::refreshFavorites()
{
    // "我喜欢"歌单已经在setupLeftSidebar中创建，这里不需要刷新列表
    // 只需要在切换到这个歌单时刷新歌曲列表
}

void MainWindow::refreshUserPlaylists()
{
    if (!m_userPlaylistList)
        return;
    m_userPlaylistList->clear();
    const auto lists = DatabaseManager::instance().userPlaylists(m_currentUserId);
    for (const auto &pl : lists) {
        auto *item = new QListWidgetItem(pl.name, m_userPlaylistList);
        item->setData(Qt::UserRole, pl.id);
    }
}

void MainWindow::setCurrentSongs(const QVector<FavoriteSong> &songs, const QString &playlistName)
{
    if (!playlistName.isEmpty())
        m_currentPlaylistName = playlistName;
    if (m_playlistTitleLabel && !m_currentPlaylistName.isEmpty())
        m_playlistTitleLabel->setText(m_currentPlaylistName);

    m_currentSongs = songs;
    if (!m_playlistTable)
        return;

    m_playlistTable->setRowCount(m_currentSongs.size());
    for (int row = 0; row < m_currentSongs.size(); ++row) {
        const auto &song = m_currentSongs.at(row);
        m_playlistTable->setItem(row, 0, new QTableWidgetItem(song.title));
        m_playlistTable->setItem(row, 1, new QTableWidgetItem(song.artist.isEmpty() ? QStringLiteral("本地文件") : song.artist));
        m_playlistTable->setItem(row, 2, new QTableWidgetItem(song.duration.isEmpty() ? QStringLiteral("--:--") : song.duration));
    }

    m_currentSongIndex = -1;
    if (m_positionSlider)
        m_positionSlider->setValue(0);

    if (songs.isEmpty()) {
        if (m_songTitleLabel)
            m_songTitleLabel->setText(QStringLiteral("当前歌曲：--"));
        if (m_artistLabel)
            m_artistLabel->setText(QStringLiteral("歌手：--"));
    }
}

void MainWindow::onPlayPause()
{
    if (!m_player)
        return;

    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
        if (m_playPauseButton)
            m_playPauseButton->setText(QStringLiteral("播放"));
        return;
    }

    if (m_currentSongs.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("当前歌单为空，请先新建歌单并选择歌曲。"));
        return;
    }

    int targetIndex = m_currentSongIndex;
    if (targetIndex < 0) {
        const int selectedRow = m_playlistTable ? m_playlistTable->currentRow() : -1;
        targetIndex = selectedRow >= 0 ? selectedRow : 0;
    }
    playSongAt(targetIndex);
}

void MainWindow::onPrev()
{
    if (m_currentSongs.isEmpty())
        return;
    int previousIndex = m_currentSongIndex - 1;
    if (previousIndex < 0)
        previousIndex = m_currentSongs.size() - 1;
    playSongAt(previousIndex);
}

void MainWindow::onNext()
{
    if (m_currentSongs.isEmpty())
        return;
    int nextIndex = m_currentSongIndex + 1;
    if (nextIndex >= m_currentSongs.size() || nextIndex < 0)
        nextIndex = 0;
    playSongAt(nextIndex);
}

void MainWindow::onPositionChanged(qint64 pos)
{
    if (!m_positionSlider)
        return;
    if (!m_positionSlider->isSliderDown())
        m_positionSlider->setValue(static_cast<int>(pos));
    
    // 更新歌词位置
    if (m_playerPanel) {
        m_playerPanel->updateLyricsPosition(pos);
    }
}

void MainWindow::onDurationChanged(qint64 dur)
{
    if (m_positionSlider)
        m_positionSlider->setRange(0, static_cast<int>(dur));
}

void MainWindow::onSliderMoved(int value)
{
    if (!m_player)
        return;
    m_player->setPosition(static_cast<qint64>(value));
}

void MainWindow::onAddToFavorite()
{
    if (!m_playlistTable || m_currentSongIndex < 0 || m_currentSongIndex >= m_currentSongs.size())
        return;
    
    FavoriteSong song = m_currentSongs.at(m_currentSongIndex);
    
    // 检查是否已收藏
    const auto favs = DatabaseManager::instance().favoritesForUser(m_currentUserId);
    bool isFavorite = false;
    for (const auto &f : favs) {
        if (f.filePath == song.filePath || (f.title == song.title && f.artist == song.artist)) {
            isFavorite = true;
            break;
        }
    }
    
    if (isFavorite) {
        // 取消收藏：从数据库删除
        DatabaseManager::instance().removeFavoriteSong(m_currentUserId, song);
        // 如果当前显示的是"我喜欢"歌单，刷新列表
        if (m_currentPlaylistId == DatabaseManager::FAVORITE_PLAYLIST_ID) {
            const auto favs = DatabaseManager::instance().favoritesForUser(m_currentUserId);
            setCurrentSongs(favs, QStringLiteral("我喜欢"));
        }
        updateFavoriteButtonStyle(false);
    } else {
        // 加入收藏
        DatabaseManager::instance().addFavoriteSong(m_currentUserId, song);
        // 如果当前显示的是"我喜欢"歌单，刷新列表
        if (m_currentPlaylistId == DatabaseManager::FAVORITE_PLAYLIST_ID) {
            const auto favs = DatabaseManager::instance().favoritesForUser(m_currentUserId);
            setCurrentSongs(favs, QStringLiteral("我喜欢"));
        }
        updateFavoriteButtonStyle(true);
    }
}

void MainWindow::updateFavoriteButtonStyle(bool isFavorite)
{
    if (!m_favoriteButton)
        return;
    
    // 去掉圆形背景，只保留透明背景，无边框
    m_favoriteButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(255,82,82,0.1);"
        "}"
    ));
    
    m_favoriteButton->setFavorite(isFavorite);
    
    if (isFavorite) {
        m_favoriteButton->setToolTip(QStringLiteral("取消喜欢"));
    } else {
        m_favoriteButton->setToolTip(QStringLiteral("加入我喜欢"));
    }
}

void MainWindow::onCreatePlaylist()
{
    bool ok = false;
    const QString name = QInputDialog::getText(this, QStringLiteral("新建歌单"),
                                               QStringLiteral("歌单名称："),
                                               QLineEdit::Normal, QString(), &ok);
    if (!ok || name.trimmed().isEmpty())
        return;

    const QStringList files = QFileDialog::getOpenFileNames(this,
        QStringLiteral("选择本地歌曲"), QString(),
        QStringLiteral("音频文件 (*.mp3 *.wav *.flac *.m4a *.aac);;所有文件 (*.*)"));
    if (files.isEmpty())
        return;

    const int playlistId = DatabaseManager::instance().createPlaylist(m_currentUserId, name.trimmed());
    if (playlistId <= 0) {
        QMessageBox::warning(this, QStringLiteral("创建失败"), QStringLiteral("创建歌单时出现错误"));
        return;
    }

    QVector<FavoriteSong> songs;
    songs.reserve(files.size());
    for (const QString &path : files) {
        const qint64 duration = readDurationMs(path);
        FavoriteSong song = songFromFile(path, duration);
        DatabaseManager::instance().addSongToPlaylist(playlistId, song);
        songs.push_back(song);
    }

    refreshUserPlaylists();
    m_currentPlaylistId = playlistId;
    setCurrentSongs(songs, name.trimmed());
}

void MainWindow::playSongAt(int index)
{
    if (!m_player || index < 0 || index >= m_currentSongs.size())
        return;

    const auto &song = m_currentSongs.at(index);
    if (song.filePath.isEmpty() || !QFile::exists(song.filePath)) {
        QMessageBox::warning(this, QStringLiteral("无法播放"), QStringLiteral("找不到该歌曲的本地文件，请重新添加。"));
        return;
    }

    m_player->setSource(QUrl::fromLocalFile(song.filePath));
    m_player->play();
    m_currentSongIndex = index;
    if (m_playPauseButton)
        m_playPauseButton->setText(QStringLiteral("暂停"));
    if (m_positionSlider)
        m_positionSlider->setValue(0);
    if (m_songTitleLabel)
        m_songTitleLabel->setText(QStringLiteral("当前歌曲：%1").arg(song.title));
    if (m_artistLabel)
        m_artistLabel->setText(QStringLiteral("歌手：%1").arg(song.artist.isEmpty() ? QStringLiteral("本地文件") : song.artist));
    
    // 加载歌词（尝试查找同名的.lrc文件）
    if (m_playerPanel) {
        QFileInfo fileInfo(song.filePath);
        QString lrcPath = fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".lrc";
        if (QFile::exists(lrcPath)) {
            m_playerPanel->loadLyrics(lrcPath);
        } else {
            m_playerPanel->loadLyrics(QString()); // 清空歌词
        }
    }
    
    // 更新爱心按钮状态
    if (m_currentUserId > 0) {
        const auto favs = DatabaseManager::instance().favoritesForUser(m_currentUserId);
        bool isFavorite = false;
        for (const auto &f : favs) {
            if (f.filePath == song.filePath || (f.title == song.title && f.artist == song.artist)) {
                isFavorite = true;
                break;
            }
        }
        updateFavoriteButtonStyle(isFavorite);
    } else {
        updateFavoriteButtonStyle(false);
    }
    m_currentSongIndex = index;
    if (m_playlistTable && m_playlistTable->currentRow() != index)
        m_playlistTable->selectRow(index);
}

QString MainWindow::formatDuration(qint64 ms)
{
    if (ms <= 0)
        return QStringLiteral("--:--");
    const qint64 totalSeconds = ms / 1000;
    const qint64 minutes = totalSeconds / 60;
    const qint64 seconds = totalSeconds % 60;
    return QStringLiteral("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

qint64 MainWindow::readDurationMs(const QString &filePath)
{
    if (filePath.isEmpty())
        return 0;
    QMediaPlayer tempPlayer;
    QAudioOutput tempOutput;
    tempPlayer.setAudioOutput(&tempOutput);

    QEventLoop loop;
    QObject::connect(&tempPlayer, &QMediaPlayer::durationChanged, &loop, [&loop](qint64 dur) {
        if (dur > 0)
            loop.quit();
    });
    QObject::connect(&tempPlayer, &QMediaPlayer::mediaStatusChanged, &loop, [&loop](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::InvalidMedia || status == QMediaPlayer::NoMedia)
            loop.quit();
    });
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(2000);

    tempPlayer.setSource(QUrl::fromLocalFile(filePath));
    loop.exec();
    return tempPlayer.duration();
}

FavoriteSong MainWindow::songFromFile(const QString &filePath, qint64 durationMs)
{
    FavoriteSong song;
    QFileInfo info(filePath);
    song.title = info.completeBaseName();
    song.artist = QStringLiteral("本地文件");
    song.duration = formatDuration(durationMs);
    song.filePath = filePath;
    return song;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->position().y() <= (m_topBar ? m_topBar->height() + 8 : 60)) {
        m_dragging = true;
        m_dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragStartPos);
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 支持Delete键或Backspace键删除当前选中的歌曲
    if ((event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) && 
        m_playlistTable && m_playlistTable->hasFocus() && m_currentPlaylistId > 0) {
        onRemoveSongFromPlaylist();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_backgroundFrame) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                m_dragging = true;
                m_dragStartPos = mouseEvent->globalPosition().toPoint() - frameGeometry().topLeft();
            }
        } else if (event->type() == QEvent::MouseMove) {
            auto *mouseEvent = static_cast<QMouseEvent *>(event);
            if (m_dragging && (mouseEvent->buttons() & Qt::LeftButton)) {
                move(mouseEvent->globalPosition().toPoint() - m_dragStartPos);
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            m_dragging = false;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}



