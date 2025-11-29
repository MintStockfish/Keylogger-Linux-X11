#include "MainWindow.hpp"
#include <QDebug>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFrame>
#include <QMovie>
#include <QResizeEvent>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), colorManager(new WindowColorManager()) {
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

    workerThread = new WorkerThread();
    connect(workerThread, &WorkerThread::windowChanged, this, &MainWindow::onWindowChanged);
    connect(workerThread, &WorkerThread::keyPressed, this, &MainWindow::onKeyPressed);
    workerThread->start();
}

MainWindow::~MainWindow() {
    if (workerThread->isRunning()) {
        workerThread->terminate(); 
        workerThread->wait();
    }
    delete workerThread;
    delete colorManager;
}

void MainWindow::setupUi() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    setupBackground();
    
    QHBoxLayout *mainHLayout = new QHBoxLayout(centralWidget);
    mainHLayout->setSpacing(0);
    mainHLayout->setContentsMargins(0, 0, 0, 0);
    
    setupSidebar(mainHLayout);
    
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
    
    logsPage = new LogsPageWidget(this);
    PlaceholderPageWidget *screenshotsPage = new PlaceholderPageWidget("SCREENSHOTS", "#00D9FF", "Screenshot gallery will appear here", this);
    PlaceholderPageWidget *trajectoryPage = new PlaceholderPageWidget("MOUSE TRAJECTORY", "#FFD600", "Mouse trajectory visualization will appear here", this);
    PlaceholderPageWidget *clipboardPage = new PlaceholderPageWidget("CLIPBOARD", "#00FF85", "Clipboard logs will appear here", this);
    
    contentStack->addWidget(homePage);
    contentStack->addWidget(logsPage);
    contentStack->addWidget(screenshotsPage);
    contentStack->addWidget(trajectoryPage);
    contentStack->addWidget(clipboardPage);    
    
    contentStack->setCurrentIndex(0);
    
    pageOpacity = new QGraphicsOpacityEffect(contentStack);
    contentStack->setGraphicsEffect(pageOpacity);
    pageOpacity->setOpacity(1.0);
    
    pageTransition = new QPropertyAnimation(pageOpacity, "opacity", this);
    pageTransition->setDuration(200);
    pageTransition->setEasingCurve(QEasingCurve::InOutQuad);
    
    connect(pageTransition, &QPropertyAnimation::finished, this, [this]() {
        if (pageOpacity->opacity() == 0.0) {
            contentStack->setCurrentIndex(targetPageIndex);
            pageTransition->setStartValue(0.0);
            pageTransition->setEndValue(1.0);
            pageTransition->start();
        }
    });
    
    connect(btnLogs, &BrutalistButton::clicked, this, [this]() { switchToPage(1); });
    connect(btnScreenshots, &BrutalistButton::clicked, this, [this]() { switchToPage(2); });
    connect(btnTrajectory, &BrutalistButton::clicked, this, [this]() { switchToPage(3); });
    connect(btnClipboard, &BrutalistButton::clicked, this, [this]() { switchToPage(4); });

    mainHLayout->addWidget(contentStack, 1);
}

void MainWindow::setupBackground() {
    bgMovie = new QMovie("../resources/background.gif");
    if (bgMovie->isValid()) {
        connect(bgMovie, &QMovie::frameChanged, this, [this](){
            this->update(); 
        });
        bgMovie->start();
    } else {
        qDebug() << "Failed to load background GIF from ../resources/background.gif";
    }
}

void MainWindow::setupSidebar(QHBoxLayout* mainHLayout) {
    QWidget *sidebar = new QWidget(this);
    sidebar->setObjectName("sidebar");
    sidebar->setMinimumWidth(350);  
    sidebar->setMaximumWidth(500);  
    
    sidebarBorder = new AnimatedBorder(centralWidget);
    
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
    
    mainHLayout->addWidget(sidebar);
    
    sidebarBorder->setGeometry(sidebar->width() - 3, 0, 6, centralWidget->height());
    sidebarBorder->raise();
}

void MainWindow::switchToPage(int index) {
    if (contentStack->currentIndex() == index) {
        return;
    }
    
    if (pageTransition->state() == QPropertyAnimation::Running) {
        pageTransition->stop();
    }
    
    btnLogs->setActive(false);
    btnScreenshots->setActive(false);
    btnTrajectory->setActive(false);
    btnClipboard->setActive(false);
    
    switch (index) {
        case 1: btnLogs->setActive(true); break;
        case 2: btnScreenshots->setActive(true); break;
        case 3: btnTrajectory->setActive(true); break;
        case 4: btnClipboard->setActive(true); break;
    }
    
    targetPageIndex = index;
    pageTransition->setStartValue(1.0);
    pageTransition->setEndValue(0.0);
    pageTransition->start();

}

void MainWindow::onWindowChanged(const QString& name, const QString& time) {
    if (currentBlock && currentBlock->isEmpty()) {
        currentBlock->markNoInput();
    }

    QString color = colorManager->getColorForWindow(name);
    currentBlock = new KeyLogBlock(name, time, color, logsPage->getLogContainer());
    logsPage->addLogBlock(currentBlock);
    
    QScrollArea* scrollArea = logsPage->getScrollArea();
    if (scrollArea) {
        QScrollBar *sb = scrollArea->verticalScrollBar();
        if (sb) {
            sb->setValue(sb->minimum());
        }
    }
}

void MainWindow::onKeyPressed(const QString& text) {
    if (!currentBlock) {
        onWindowChanged("Unknown Window", "00:00:00");
    }
    currentBlock->appendLog(text);
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
        "}"
    ).arg(sidebarBg);
    
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
    
    if (sidebarBorder) {
        QWidget *sidebar = centralWidget->findChild<QWidget*>("sidebar");
        if (sidebar) {
            sidebarBorder->setGeometry(sidebar->width() - 3, 0, 6, height());
        }
    }
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
