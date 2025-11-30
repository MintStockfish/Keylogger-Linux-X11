#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QMovie>
#include <QResizeEvent>
#include <QPainter>
#include <QDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QGridLayout>
#include <QDir>
#include <QFileSystemWatcher>
#include "../components/BrutalistButton.hpp"
#include "../components/AnimatedBorder.hpp"

#include <QThread>
#include "../../app/application.hpp"
#include "../utils/WindowColorManager/WindowColorManager.hpp"
#include "../pages/LogsPageWidget/LogsPageWidget.hpp"
#include "../pages/PlaceholderPageWidget/PlaceholderPageWidget.hpp"
#include "../pages/ClicksPageWidget/ClicksPageWidget.hpp"

class WorkerThread : public QThread {
    Q_OBJECT
public:
    void run() override {
        Application app(
            [this](const std::string& name, const std::string& time) {
                emit windowChanged(QString::fromStdString(name), QString::fromStdString(time));
            },
            [this](const std::string& text) {
                emit keyPressed(QString::fromStdString(text));
            },
            [this](int x, int y) {
                emit mouseClicked(x, y);
            }
        );
        app.run();
    }

signals:
    void windowChanged(const QString& name, const QString& time);
    void keyPressed(const QString& text);
    void mouseClicked(int x, int y);
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onWindowChanged(const QString& name, const QString& time);
    void onKeyPressed(const QString& text);
    void onMouseClicked(int x, int y);

private:
    void setupUi();
    void setupBackground();
    void setupSidebar(QHBoxLayout* mainLayout);
    void applyStyles();
    void switchToPage(int index);
    void loadScreenshots(); 
    void clearScreenshotsDirectory(); 
    void loadClipboardEntries();

    QWidget *centralWidget;
    QLabel *titleLabel;
    QLabel *logoLabel;
    QLabel *recordingLabel;
    QLabel *dotLabel;
    QGraphicsOpacityEffect *dotOpacity;
    QPropertyAnimation *fadeAnimation;
    
    QStackedWidget *contentStack;
    
    BrutalistButton *btnLogs;
    BrutalistButton *btnScreenshots;
    BrutalistButton *btnClicks;
    BrutalistButton *btnClipboard;

    QMovie *bgMovie;
    
    QGraphicsOpacityEffect *pageOpacity;
    QPropertyAnimation *pageTransition;
    int targetPageIndex;
    
    QWidget* screenshotsContainer;
    QGridLayout* screenshotsGrid;
    QFileSystemWatcher *fileWatcher;
    
    QWidget* clipboardContainer;
    QVBoxLayout* clipboardEntriesLayout;
    
    AnimatedBorder *sidebarBorder;

    LogsPageWidget *logsPage;
    KeyLogBlock *currentBlock = nullptr;
    WindowColorManager *colorManager;
    
    ClicksPageWidget *clicksPage;
    
    WorkerThread *workerThread;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override; 

private:
    void showFullImage(const QString& filePath);
};
