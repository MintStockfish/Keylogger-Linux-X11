#include "LogsPageWidget.hpp"
#include <QLabel>

LogsPageWidget::LogsPageWidget(QWidget* parent)
    : QWidget(parent) {
    setupUi();
}

void LogsPageWidget::setupUi() {
    QVBoxLayout* logsLayout = new QVBoxLayout(this);
    logsLayout->setContentsMargins(30, 0, 30, 0);
    
    QLabel* logsTitle = new QLabel("KEY LOGS", this);
    logsTitle->setStyleSheet("font-size: 48px; font-weight: bold; color: #333333; padding-top: 15px;");
    logsTitle->setAlignment(Qt::AlignCenter);
    
    logsLayout->addWidget(logsTitle);
    logsLayout->addSpacing(20);
    
    logScrollArea_ = new QScrollArea(this);
    logScrollArea_->setWidgetResizable(true);
    logScrollArea_->setStyleSheet("background: transparent; border: none;");
    
    logContainer_ = new QWidget();
    logContainer_->setStyleSheet("background: transparent;");
    logContainerLayout_ = new QVBoxLayout(logContainer_);
    logContainerLayout_->setAlignment(Qt::AlignTop);
    logContainerLayout_->setSpacing(20);
    
    logScrollArea_->setWidget(logContainer_);
    logsLayout->addWidget(logScrollArea_);
}

void LogsPageWidget::addLogBlock(KeyLogBlock* block) {
    logContainerLayout_->insertWidget(0, block);
}
