#include "MainWindow.hpp"
#include <QDebug>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFrame>
#include <QMovie>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUi();
    applyStyles();
    
    resize(1200, 800);
    setMinimumSize(900, 650);
    
    dotOpacity = new QGraphicsOpacityEffect();
    dotLabel->setGraphicsEffect(dotOpacity);
    
    fadeAnimation = new QPropertyAnimation(dotOpacity, "opacity", this);
    fadeAnimation->setDuration(2400);  
    fadeAnimation->setStartValue(0.3);
    
    fadeAnimation->setKeyValueAt(0.5, 1.0);  
    fadeAnimation->setEndValue(0.3);         
    
    fadeAnimation->setEasingCurve(QEasingCurve::InOutSine);
    fadeAnimation->setLoopCount(-1); 
    fadeAnimation->start();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUi() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    bgMovie = new QMovie("../resources/background.gif");
    if (bgMovie->isValid()) {
        connect(bgMovie, &QMovie::frameChanged, this, [this](){
            this->update(); 
        });
        bgMovie->start();
    } else {
        qDebug() << "Failed to load background GIF from ../resources/background.gif";
    }

    QHBoxLayout *mainHLayout = new QHBoxLayout(centralWidget);
    mainHLayout->setSpacing(0);
    mainHLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *sidebar = new QWidget(this);
    sidebar->setObjectName("sidebar");
    sidebar->setMinimumWidth(350);  
    sidebar->setMaximumWidth(500);  
    
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setSpacing(25);  
    sidebarLayout->setContentsMargins(30, 50, 30, 50);

    titleLabel = new QLabel("KEYLOG: ACTIVE", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(titleLabel);
    
    sidebarLayout->addSpacing(2);
    
    QWidget *recordingWidget = new QWidget(this);
    recordingWidget->setObjectName("recordingWidget");
    QHBoxLayout *recordingLayout = new QHBoxLayout(recordingWidget);
    recordingLayout->setContentsMargins(15, 8, 15, 8);
    
    recordingLayout->addStretch();
    
    dotLabel = new QLabel("●", this);
    dotLabel->setObjectName("dotLabel");
    
    recordingLabel = new QLabel("RECORDING", this);
    recordingLabel->setObjectName("recordingLabel");
    
    recordingLayout->addWidget(dotLabel);
    recordingLayout->addWidget(recordingLabel);
    recordingLayout->addStretch();
    
    sidebarLayout->addWidget(recordingWidget);
    
    sidebarLayout->addSpacing(100);

    btnLogs = new BrutalistButton("KEY LOGS", "#FF0090", this);
    btnScreenshots = new BrutalistButton("SCREENSHOTS", "#00D9FF", this);
    btnTrajectory = new BrutalistButton("MOUSE TRAJECTORY", "#FFD600", this);
    btnClipboard = new BrutalistButton("CLIPBOARD", "#00FF85", this);

    sidebarLayout->addWidget(btnLogs);
    sidebarLayout->addWidget(btnScreenshots);
    sidebarLayout->addWidget(btnTrajectory);
    sidebarLayout->addWidget(btnClipboard);
    
    sidebarLayout->addStretch();
    
    sidebarLayout->addSpacing(40);

    contentStack = new QStackedWidget(this);
    contentStack->setObjectName("contentStack");
    
    QWidget *homePage = new QWidget(this);
    QVBoxLayout *homeLayout = new QVBoxLayout(homePage);
    homeLayout->setContentsMargins(40, 40, 40, 40);
    homeLayout->setAlignment(Qt::AlignCenter);
    
    logoLabel = new QLabel(this);
    logoLabel->setAlignment(Qt::AlignCenter);
    QPixmap logo("../resources/logo.png");
    if (!logo.isNull()) {
        int logoSize = qMin(500, qMax(300, width() / 3));
        logoLabel->setPixmap(logo.scaled(logoSize, logoSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logoLabel->setText("🔐");
        logoLabel->setStyleSheet("font-size: 120px;");
        qDebug() << "Failed to load logo from ../resources/logo.png";
    }
    
    homeLayout->addWidget(logoLabel);
    
    // Page 1: Key Logs
    QWidget *logsPage = new QWidget(this);
    QVBoxLayout *logsLayout = new QVBoxLayout(logsPage);
    logsLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *logsTitle = new QLabel("KEY LOGS", this);
    logsTitle->setStyleSheet("font-size: 48px; font-weight: bold; color: #FF0090;");
    logsTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *logsDesc = new QLabel("Keystroke logs will appear here", this);
    logsDesc->setStyleSheet("font-size: 18px; color: #666;");
    logsDesc->setAlignment(Qt::AlignCenter);
    
    logsLayout->addWidget(logsTitle);
    logsLayout->addSpacing(20);
    logsLayout->addWidget(logsDesc);
    
    // Page 2: Screenshots
    QWidget *screenshotsPage = new QWidget(this);
    QVBoxLayout *screenshotsLayout = new QVBoxLayout(screenshotsPage);
    screenshotsLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *screenshotsTitle = new QLabel("SCREENSHOTS", this);
    screenshotsTitle->setStyleSheet("font-size: 48px; font-weight: bold; color: #00D9FF;");
    screenshotsTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *screenshotsDesc = new QLabel("Screenshot gallery will appear here", this);
    screenshotsDesc->setStyleSheet("font-size: 18px; color: #666;");
    screenshotsDesc->setAlignment(Qt::AlignCenter);
    
    screenshotsLayout->addWidget(screenshotsTitle);
    screenshotsLayout->addSpacing(20);
    screenshotsLayout->addWidget(screenshotsDesc);
    
    // Page 3: Mouse Trajectory
    QWidget *trajectoryPage = new QWidget(this);
    QVBoxLayout *trajectoryLayout = new QVBoxLayout(trajectoryPage);
    trajectoryLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *trajectoryTitle = new QLabel("MOUSE TRAJECTORY", this);
    trajectoryTitle->setStyleSheet("font-size: 48px; font-weight: bold; color: #FFD600;");
    trajectoryTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *trajectoryDesc = new QLabel("Mouse movement visualization will appear here", this);
    trajectoryDesc->setStyleSheet("font-size: 18px; color: #666;");
    trajectoryDesc->setAlignment(Qt::AlignCenter);
    
    trajectoryLayout->addWidget(trajectoryTitle);
    trajectoryLayout->addSpacing(20);
    trajectoryLayout->addWidget(trajectoryDesc);
    
    // Page 4: Clipboard
    QWidget *clipboardPage = new QWidget(this);
    QVBoxLayout *clipboardLayout = new QVBoxLayout(clipboardPage);
    clipboardLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *clipboardTitle = new QLabel("CLIPBOARD", this);
    clipboardTitle->setStyleSheet("font-size: 48px; font-weight: bold; color: #00FF85;");
    clipboardTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *clipboardDesc = new QLabel("Clipboard history will appear here", this);
    clipboardDesc->setStyleSheet("font-size: 18px; color: #666;");
    clipboardDesc->setAlignment(Qt::AlignCenter);
    
    clipboardLayout->addWidget(clipboardTitle);
    clipboardLayout->addSpacing(20);
    clipboardLayout->addWidget(clipboardDesc);
    
    // Add all pages to stack
    contentStack->addWidget(homePage);         
    contentStack->addWidget(logsPage);         
    contentStack->addWidget(screenshotsPage);    
    contentStack->addWidget(trajectoryPage);   
    contentStack->addWidget(clipboardPage);    
    
    contentStack->setCurrentIndex(0);
    
    connect(btnLogs, &BrutalistButton::clicked, this, [this]() { switchToPage(1); });
    connect(btnScreenshots, &BrutalistButton::clicked, this, [this]() { switchToPage(2); });
    connect(btnTrajectory, &BrutalistButton::clicked, this, [this]() { switchToPage(3); });
    connect(btnClipboard, &BrutalistButton::clicked, this, [this]() { switchToPage(4); });

    mainHLayout->addWidget(sidebar);
    mainHLayout->addWidget(contentStack, 1);
}

void MainWindow::switchToPage(int index) {
    contentStack->setCurrentIndex(index);
}

void MainWindow::applyStyles() {
    QString bgLight = "transparent"; 
    QString sidebarBg = "rgba(255, 255, 255, 50)"; 
    QString textDark = "#000000"; 
    QString borderDark = "#000000"; 

    this->setStyleSheet(QString("QMainWindow { background-color: #000000; }")); 
    this->setStyleSheet(QString("QMainWindow { background-color: #000000; }")); 

    QString sidebarStyle = QString(
        "#sidebar { "
        "   background-color: %1; "
        "   border-right: 4px solid %2; "
        "}"
    ).arg(sidebarBg).arg(borderDark);
    
    centralWidget->findChild<QWidget*>("sidebar")->setStyleSheet(sidebarStyle);

    QString titleStyle = QString(
        "QLabel { "
        "   color: white; "
        "   background-color: #000000; "
        "   font-family: 'Arial Black', sans-serif; "
        "   font-size: 22px; "
        "   font-weight: bold; "
        "   padding: 18px 15px; "
        "   border: 4px solid %1; "
        "}"
    ).arg(borderDark);
    titleLabel->setStyleSheet(titleStyle);
    
    QString recordingWidgetStyle = QString(
        "#recordingWidget { "
        "   background-color: #1a1a1a; "
        "   border: 2px solid #00FF85; "
        "   border-radius: 4px; "
        "}"
    );
    centralWidget->findChild<QWidget*>("recordingWidget")->setStyleSheet(recordingWidgetStyle);
    
    QString dotStyle = QString(
        "#dotLabel { "
        "   color: #00FF85; "
        "   font-size: 20px; "
        "   background: transparent; "
        "}"
    );
    dotLabel->setStyleSheet(dotStyle);
    
    QString recordingStyle = QString(
        "#recordingLabel { "
        "   color: #00FF85; "
        "   background: transparent; "
        "   font-family: 'Arial', sans-serif; "
        "   font-size: 13px; "
        "   font-weight: bold; "
        "}"
    );
    recordingLabel->setStyleSheet(recordingStyle);
    
    QString contentStyle = QString(
        "#contentStack { "
        "   background-color: %1; "
        "}"
    ).arg(bgLight);
    contentStack->setStyleSheet(contentStyle);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QMainWindow::paintEvent(event);
    
    if (bgMovie && bgMovie->isValid() && bgMovie->state() == QMovie::Running) {
        QPainter painter(this);
        QPixmap currentFrame = bgMovie->currentPixmap();
        if (!currentFrame.isNull()) {
            painter.drawTiledPixmap(rect(), currentFrame);
        }
    }
}
