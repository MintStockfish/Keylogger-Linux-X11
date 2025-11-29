#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include "../../components/KeyLogBlock.hpp"

class LogsPageWidget : public QWidget {
    Q_OBJECT

public:
    explicit LogsPageWidget(QWidget* parent = nullptr);
    
    void addLogBlock(KeyLogBlock* block);
    QVBoxLayout* getLogContainerLayout() { return logContainerLayout_; }
    QWidget* getLogContainer() { return logContainer_; }
    QScrollArea* getScrollArea() { return logScrollArea_; }

private:
    void setupUi();
    
    QScrollArea* logScrollArea_;
    QWidget* logContainer_;
    QVBoxLayout* logContainerLayout_;
};
