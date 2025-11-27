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
#include "BrutalistButton.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};
