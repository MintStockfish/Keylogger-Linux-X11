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
#include <QPaintEvent>
#include "../components/BrutalistButton.hpp"
#include "../components/AnimatedBorder.hpp"

#include <QThread>
#include <QScrollArea>
#include <QVBoxLayout>
#include "../../app/application.hpp"
#include "../components/KeyLogBlock.hpp"

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
            }
        );
        app.run();
    }

signals:
    void windowChanged(const QString& name, const QString& time);
    void keyPressed(const QString& text);
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onWindowChanged(const QString& name, const QString& time);
    void onKeyPressed(const QString& text);

private:
    void setupUi();
    void applyStyles();
    void switchToPage(int index);

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
    BrutalistButton *btnTrajectory;
    BrutalistButton *btnClipboard;

    QMovie *bgMovie;
    
    QGraphicsOpacityEffect *pageOpacity;
    QPropertyAnimation *pageTransition;
    int targetPageIndex;
    
    AnimatedBorder *sidebarBorder;

    QScrollArea *logScrollArea;
    QWidget *logContainer;
    QVBoxLayout *logContainerLayout;
    KeyLogBlock *currentBlock = nullptr;
    
    WorkerThread *workerThread;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};
