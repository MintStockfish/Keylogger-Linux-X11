#ifndef NOTIFICATIONWIDGET_HPP
#define NOTIFICATIONWIDGET_HPP

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class NotificationWidget : public QWidget {
    Q_OBJECT

public:
    explicit NotificationWidget(QWidget *parent = nullptr);
    void showNotification(const QString &message, int durationMs = 3000);

private slots:
    void hideNotification();

private:
    QLabel *messageLabel;
    QTimer *hideTimer;
    QPropertyAnimation *fadeAnimation;
    QGraphicsOpacityEffect *opacityEffect;
    
    void setupUi();
};

#endif 
