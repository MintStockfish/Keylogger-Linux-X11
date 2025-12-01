#include "NotificationWidget.hpp"
#include <QVBoxLayout>

NotificationWidget::NotificationWidget(QWidget *parent) : QWidget(parent) {
    setAutoFillBackground(true);
    setAttribute(Qt::WA_StyledBackground, true);
    
    setupUi();
    
    hideTimer = new QTimer(this);
    hideTimer->setSingleShot(true);
    connect(hideTimer, &QTimer::timeout, this, &NotificationWidget::hideNotification);
    
    opacityEffect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(opacityEffect);
    
    fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeAnimation->setDuration(300);
    
    hide();
}

void NotificationWidget::setupUi() {
    this->setFixedSize(400, 80);
    
    this->setStyleSheet(
        "QWidget { "
        "   background-color: #FFD600; "
        "   border: 4px solid #000000; "
        "}"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    
    messageLabel = new QLabel(this);
    messageLabel->setStyleSheet(
        "QLabel { "
        "   color: #000000; "
        "   font-family: 'Arial Black', sans-serif; "
        "   font-size: 18px; "
        "   font-weight: bold; "
        "   border: none; "
        "   background: transparent; "
        "}"
    );
    messageLabel->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(messageLabel);
}

void NotificationWidget::showNotification(const QString &message, int durationMs) {
    messageLabel->setText(message);
    
    fadeAnimation->stop();
    hideTimer->stop();
    
    disconnect(fadeAnimation, &QPropertyAnimation::finished, this, &QWidget::hide);
    
    opacityEffect->setOpacity(0.0);
    show();
    raise();
    
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start();
    
    hideTimer->start(durationMs);
}

void NotificationWidget::hideNotification() {
    hideTimer->stop();
    
    fadeAnimation->stop();
    fadeAnimation->setStartValue(opacityEffect->opacity());
    fadeAnimation->setEndValue(0.0);
    
    disconnect(fadeAnimation, &QPropertyAnimation::finished, this, &QWidget::hide);
    connect(fadeAnimation, &QPropertyAnimation::finished, this, &QWidget::hide);
    
    fadeAnimation->start();
}
