#include "MainWindow.hpp"
#include <QDebug>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFrame>
#include <QMovie>
#include <QResizeEvent>
#include <QScrollArea>
#include <QGridLayout>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QRegularExpression>
#include <QScrollBar>
#include <cstdio>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), colorManager(new WindowColorManager()) {
    clearScreenshotsDirectory(); 
    
    QFile::remove("clipboard.txt");
    QFile emptyClip("clipboard.txt");
    emptyClip.open(QIODevice::WriteOnly);
    emptyClip.close();
    
    fileWatcher = new QFileSystemWatcher(this);
    fileWatcher->addPath("screenshots");
    
    QString currentPath = QDir::currentPath();
    fileWatcher->addPath(currentPath);
    fileWatcher->addPath("clipboard.txt");

    connect(fileWatcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &path) {
        if (path.contains("screenshots")) {
            QTimer::singleShot(500, this, &MainWindow::loadScreenshots);
        } else {
            QTimer::singleShot(100, this, &MainWindow::loadClipboardEntries);
        }
    });
    
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
        if (path.contains("clipboard")) {
            QTimer::singleShot(100, this, &MainWindow::loadClipboardEntries);

            QTimer::singleShot(200, this, [this]() {
                if (!fileWatcher->files().contains("clipboard.txt")) {
                    fileWatcher->addPath("clipboard.txt");
                }
            });
        }
    });
    
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
    connect(workerThread, &WorkerThread::mouseClicked, this, &MainWindow::onMouseClicked);
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
    // Page 2: Screenshots
    QWidget *screenshotsPage = new QWidget(this);
    QVBoxLayout *screenshotsLayout = new QVBoxLayout(screenshotsPage);
    screenshotsLayout->setContentsMargins(20, 20, 20, 20);
    screenshotsLayout->setAlignment(Qt::AlignTop);
    
    QLabel *screenshotsTitle = new QLabel("SCREENSHOTS", this);
    screenshotsTitle->setStyleSheet("font-size: 48px; font-weight: bold; color: #666; margin-bottom: 20px;");
    screenshotsTitle->setAlignment(Qt::AlignCenter);
    screenshotsLayout->addWidget(screenshotsTitle);
    
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical {"
        "    background: #1a1a1a;"
        "    width: 12px;"
        "    border: 1px solid #333;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: #666;"
        "    min-height: 20px;"
        "    border-radius: 3px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background: #888;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: none;"
        "}"
    );
    
    screenshotsContainer = new QWidget();
    screenshotsContainer->setStyleSheet("background: transparent;");
    screenshotsGrid = new QGridLayout(screenshotsContainer);
    screenshotsGrid->setSpacing(20);
    screenshotsGrid->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    
    scrollArea->setWidget(screenshotsContainer);
    screenshotsLayout->addWidget(scrollArea);
    
    loadScreenshots();
    
    std::remove("mouse_log.txt");
    
    clicksPage = new ClicksPageWidget(this);
    clicksPage->loadClicks();
    
    // Page 4: Clipboard
    QWidget *clipboardPage = new QWidget(this);
    QVBoxLayout *clipboardLayout = new QVBoxLayout(clipboardPage);
    clipboardLayout->setContentsMargins(20, 20, 20, 20);
    clipboardLayout->setAlignment(Qt::AlignTop);
    
    QLabel *clipboardTitle = new QLabel("CLIPBOARD", this);
    clipboardTitle->setStyleSheet("font-size: 48px; font-weight: bold; color: #666; margin-bottom: 20px;");
    clipboardTitle->setAlignment(Qt::AlignCenter);
    clipboardLayout->addWidget(clipboardTitle);
    
    QScrollArea *clipboardScrollArea = new QScrollArea(this);
    clipboardScrollArea->setWidgetResizable(true);
    clipboardScrollArea->setFrameShape(QFrame::NoFrame);
    clipboardScrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical {"
        "    background: #1a1a1a;"
        "    width: 12px;"
        "    border: 1px solid #333;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: #666;"
        "    min-height: 20px;"
        "    border-radius: 3px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background: #888;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: none;"
        "}"
    );
    
    clipboardContainer = new QWidget();
    clipboardContainer->setStyleSheet("background: transparent;");
    clipboardEntriesLayout = new QVBoxLayout(clipboardContainer);
    clipboardEntriesLayout->setSpacing(15);
    clipboardEntriesLayout->setAlignment(Qt::AlignTop);
    
    clipboardScrollArea->setWidget(clipboardContainer);
    clipboardLayout->addWidget(clipboardScrollArea);
    
    loadClipboardEntries();
    
    contentStack->addWidget(homePage);
    contentStack->addWidget(logsPage);
    contentStack->addWidget(screenshotsPage);
    contentStack->addWidget(clicksPage);
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
    connect(btnClicks, &BrutalistButton::clicked, this, [this]() { switchToPage(3); });
    connect(btnClipboard, &BrutalistButton::clicked, this, [this]() { switchToPage(4); });

    mainHLayout->addWidget(contentStack, 1);
    
    notificationWidget = new NotificationWidget(this);
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
    
    btnLogs = new BrutalistButton("KEY LOGS", "#FF0090", this);
    
    btnScreenshots = new BrutalistButton("SCREENSHOTS", "#00D9FF", this);
    
    btnClicks = new BrutalistButton("MOUSE CLICKS", "#FFD600", this);
    
    btnClipboard = new BrutalistButton("CLIPBOARD", "#00FF85", this);

    sidebarLayout->addWidget(btnLogs);
    sidebarLayout->addWidget(btnScreenshots);
    sidebarLayout->addWidget(btnClicks);
    sidebarLayout->addWidget(btnClipboard);
    
    sidebarLayout->addStretch();
    
    btnSaveData = new BrutalistButton("SAVE DATA", "#4CAF50", this);
    btnSaveData->setStyleSheet(
        "QPushButton { font-size: 14px; }"
    );
    connect(btnSaveData, &BrutalistButton::clicked, this, &MainWindow::saveAllData);
    sidebarLayout->addWidget(btnSaveData);
    
    sidebarLayout->addSpacing(10);
    
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
    btnClicks->setActive(false);
    btnClipboard->setActive(false);
    
    switch (index) {
        case 1: btnLogs->setActive(true); break;
        case 2: 
            btnScreenshots->setActive(true); 
            loadScreenshots(); 
            break;
        case 3: 
            btnClicks->setActive(true); 
            break;
        case 4: 
            btnClipboard->setActive(true); 
            loadClipboardEntries();
            break;
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

void MainWindow::onMouseClicked(int x, int y) {
    if (clicksPage) {
        clicksPage->addClick(x, y);
    }
}

void MainWindow::saveAllData() {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString basePath = "saved_data/" + timestamp;
    
    QDir dir;
    if (!dir.mkpath(basePath)) {
        qDebug() << "Failed to create directory:" << basePath;
        return;
    }
    
    QFile logFile("log.txt");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&logFile);
        
        if (logsPage) {
            QWidget* logContainer = logsPage->getLogContainer();
            QList<KeyLogBlock*> blocks = logContainer->findChildren<KeyLogBlock*>();
            
            for (KeyLogBlock* block : blocks) {
                if (block && !block->isEmpty()) {
                    out << block->getHeader() << "\n";
                    out << block->getLogText() << "\n\n";
                }
            }
        }
        
        logFile.close();
    }
    
    if (clicksPage) {
        QFile clickFile("mouse_log.txt");
        if (clickFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&clickFile);
            for (const auto& point : clicksPage->getClicks()) {
                out << "X: " << point.x << ", Y: " << point.y << " - Count: " << point.count << "\n";
            }
            clickFile.close();
        }
    }
    
    auto copyFile = [](const QString& src, const QString& dest) -> bool {
        if (!QFile::exists(src)) {
            qDebug() << "Source file does not exist:" << src;
            return false;
        }
        QFile::copy(src, dest);
        return true;
    };
    
    auto copyDir = [](const QString& srcPath, const QString& destPath) -> bool {
        QDir srcDir(srcPath);
        if (!srcDir.exists()) return false;
        
        QDir destDir;
        if (!destDir.mkpath(destPath)) return false;
        
        QFileInfoList files = srcDir.entryInfoList(QDir::Files);
        for (const QFileInfo& file : files) {
            QString destFile = destPath + "/" + file.fileName();
            QFile::copy(file.absoluteFilePath(), destFile);
        }
        return true;
    };
    
    copyFile("log.txt", basePath + "/log.txt");
    copyFile("mouse_log.txt", basePath + "/mouse_log.txt");
    copyFile("clipboard.txt", basePath + "/clipboard.txt");
    
    copyDir("screenshots", basePath + "/screenshots");
    
    qDebug() << "Data saved to:" << basePath;
    
    showSaveNotification();
}

void MainWindow::showSaveNotification() {
    if (notificationWidget) {
        int x = (width() - 400) / 2;
        int y = 50;  
        
        notificationWidget->move(x, y);
        notificationWidget->showNotification("DATA SAVED SUCCESSFULLY");
    }
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
    updateScreenshotsLayout();
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



void MainWindow::updateScreenshotsLayout() {
    if (screenshotWidgets.isEmpty()) return;

    QLayoutItem *child;
    while ((child = screenshotsGrid->takeAt(0)) != 0) {
        delete child;
    }

    int thumbnailWidth = 220;
    int spacing = 20;
    int containerWidth = screenshotsContainer->width();
    if (containerWidth < 100) containerWidth = width() - 100; 
    
    int maxCols = qMax(1, (containerWidth + spacing) / (thumbnailWidth + spacing));
    
    int row = 0;
    int col = 0;
    
    for (QWidget *widget : screenshotWidgets) {
        screenshotsGrid->addWidget(widget, row, col);
        widget->show(); 
        
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void MainWindow::loadScreenshots() {
    QDir dir("screenshots");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg";
    dir.setNameFilters(filters);
    dir.setSorting(QDir::Time); 
    
    QFileInfoList list = dir.entryInfoList();
    
    qDeleteAll(screenshotWidgets);
    screenshotWidgets.clear();
    
    QLayoutItem *child;
    while ((child = screenshotsGrid->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }
    
    if (list.isEmpty()) {
        QLabel *emptyLabel = new QLabel("NO SCREENSHOTS YET", this);
        emptyLabel->setStyleSheet(
            "QLabel { "
            "   color: #666; "
            "   font-size: 24px; "
            "   font-weight: bold; "
            "   font-family: 'Arial Black'; "
            "}"
        );
        emptyLabel->setAlignment(Qt::AlignCenter);
        screenshotsGrid->addWidget(emptyLabel, 0, 0, 1, 3, Qt::AlignCenter); 
        return;
    }
    
    int thumbnailWidth = 220;
    int spacing = 20;
    int containerWidth = screenshotsContainer->width();
    if (containerWidth < 100) containerWidth = width() - 100; 
    
    int maxCols = qMax(1, (containerWidth + spacing) / (thumbnailWidth + spacing));
    
    int row = 0;
    int col = 0;
    
    for (const QFileInfo &fileInfo : list) {
        QWidget *itemWidget = new QWidget();
        itemWidget->setFixedSize(220, 180);
        itemWidget->setProperty("filePath", fileInfo.absoluteFilePath()); 
        itemWidget->installEventFilter(this); 
        itemWidget->setCursor(Qt::PointingHandCursor); 
        
        QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(5);
        
        QLabel *thumbLabel = new QLabel();
        thumbLabel->setFixedSize(220, 140);
        thumbLabel->setAlignment(Qt::AlignCenter);
        thumbLabel->setStyleSheet(
            "QLabel { "
            "   background-color: black; "
            "   border: 3px solid #666; "
            "}"
        );
        
        QPixmap pixmap(fileInfo.absoluteFilePath());
        if (!pixmap.isNull()) {
            thumbLabel->setPixmap(pixmap.scaled(214, 134, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        
        // Timestamp/Name
        QLabel *nameLabel = new QLabel(fileInfo.baseName());
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setStyleSheet("color: #666; font-weight: bold; font-size: 12px;");
        
        itemLayout->addWidget(thumbLabel);
        itemLayout->addWidget(nameLabel);
        
        screenshotWidgets.append(itemWidget);
    }
    
    updateScreenshotsLayout();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonRelease) {
        QWidget *widget = qobject_cast<QWidget*>(watched);
        if (widget && widget->property("filePath").isValid()) {
            QString filePath = widget->property("filePath").toString();
            showFullImage(filePath);
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::showFullImage(const QString& filePath) {
    QDialog *viewer = new QDialog(this);
    viewer->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    viewer->setAttribute(Qt::WA_TranslucentBackground);
    viewer->resize(width() * 0.9, height() * 0.9);
    
    QVBoxLayout *layout = new QVBoxLayout(viewer);
    layout->setContentsMargins(0, 0, 0, 0);
    
    QFrame *container = new QFrame(viewer);
    container->setStyleSheet(
        "QFrame { "
        "   background-color: #000000; "
        "   border: 4px solid #00D9FF; "
        "}"
    );
    
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *imageLabel = new QLabel(container);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet("border: none;");
    
    QPixmap pixmap(filePath);
    if (!pixmap.isNull()) {
        int w = viewer->width() - 60;
        int h = viewer->height() - 100;
        imageLabel->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    QPushButton *closeBtn = new QPushButton("CLOSE", container);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { "
        "   background-color: #FF0090; "
        "   color: black; "
        "   font-weight: bold; "
        "   font-size: 16px; "
        "   border: none; "
        "   padding: 10px 30px; "
        "   font-family: 'Arial Black'; "
        "}"
        "QPushButton:hover { "
        "   background-color: #ff33a8; "
        "}"
    );
    connect(closeBtn, &QPushButton::clicked, viewer, &QDialog::accept);
    
    containerLayout->addWidget(imageLabel, 1);
    containerLayout->addSpacing(20);
    containerLayout->addWidget(closeBtn, 0, Qt::AlignCenter);
    
    layout->addWidget(container);
    
    viewer->exec();
    delete viewer;
}

void MainWindow::clearScreenshotsDirectory() {
    QDir dir("screenshots");
    if (dir.exists()) {
        dir.setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg");
        dir.setFilter(QDir::Files);
        for (QString dirFile : dir.entryList()) {
            dir.remove(dirFile);
        }
    }
}

void MainWindow::loadClipboardEntries() {

    QLayoutItem *child;
    while ((child = clipboardEntriesLayout->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }
    
    QFile file("clipboard.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QLabel *emptyLabel = new QLabel("NO CLIPBOARD ENTRIES YET", this);
        emptyLabel->setStyleSheet(
            "QLabel { "
            "   color: #666; "
            "   font-size: 24px; "
            "   font-weight: bold; "
            "   font-family: 'Arial Black'; "
            "}"
        );
        emptyLabel->setAlignment(Qt::AlignCenter);
        clipboardEntriesLayout->addWidget(emptyLabel);
        return;
    }
    
    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            lines.prepend(line); 
        }
    }
    file.close();
    
    if (lines.isEmpty()) {
        QLabel *emptyLabel = new QLabel("NO CLIPBOARD ENTRIES YET", this);
        emptyLabel->setStyleSheet(
            "QLabel { "
            "   color: #666; "
            "   font-size: 24px; "
            "   font-weight: bold; "
            "   font-family: 'Arial Black'; "
            "}"
        );
        emptyLabel->setAlignment(Qt::AlignCenter);
        clipboardEntriesLayout->addWidget(emptyLabel);
        return;
    }
    
    QStringList bgColors = {"#FFB3D9", "#B3E5FF", "#FFE680", "#B3FFD9"}; 
    int colorIndex = 0;
    
    for (const QString &line : lines) {
        QStringList parts = line.split(" | ", Qt::SkipEmptyParts);
        if (parts.size() < 2) continue;
        
        QString timestamp = parts[0];
        QString clipText = parts.mid(1).join(" | ");
        
        clipText.replace("\\n", "\n");
        
        QString bgColor = bgColors[colorIndex % bgColors.size()];
        colorIndex++;
        
        QWidget *entryWidget = new QWidget();
        entryWidget->setAutoFillBackground(true);
        entryWidget->setAttribute(Qt::WA_StyledBackground, true);
        entryWidget->setStyleSheet(QString(
            "QWidget { "
            "   background-color: %1; "
            "   border: 3px solid #000000; "
            "}"
        ).arg(bgColor));
        
        QVBoxLayout *entryLayout = new QVBoxLayout(entryWidget);
        entryLayout->setContentsMargins(20, 15, 20, 15);
        entryLayout->setSpacing(10);
        
        QLabel *timeLabel = new QLabel("🕐 " + timestamp);
        timeLabel->setStyleSheet(
            "QLabel { "
            "   color: #666666; "
            "   font-size: 12px; "
            "   font-weight: bold; "
            "   font-family: 'Arial'; "
            "   background: transparent; "
            "   border: none; "
            "}"
        );
        
        QLabel *textLabel = new QLabel(clipText);
        textLabel->setStyleSheet(
            "QLabel { "
            "   color: #000000; "
            "   font-size: 14px; "
            "   font-family: 'Arial'; "
            "   background: transparent; "
            "   border: none; "
            "   line-height: 1.4; "
            "}"
        );
        textLabel->setWordWrap(true);
        textLabel->setMaximumHeight(200);
        textLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        
        QFrame *separator = new QFrame();
        separator->setFrameShape(QFrame::HLine);
        separator->setStyleSheet(
            "QFrame { "
            "   background-color: #000000; "
            "   border: none; "
            "   max-height: 2px; "
            "}"
        );
        
        entryLayout->addWidget(timeLabel);
        entryLayout->addWidget(separator);
        entryLayout->addWidget(textLabel);
        
        entryWidget->setMinimumHeight(100);
        
        clipboardEntriesLayout->addWidget(entryWidget);
    }
    
    clipboardEntriesLayout->addStretch();
}
