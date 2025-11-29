#pragma once

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>

class KeyLogBlock : public QFrame {
    Q_OBJECT

public:
    explicit KeyLogBlock(const QString& windowName, const QString& time, QWidget* parent = nullptr);
    void appendLog(const QString& text);
    bool isEmpty() const;
    void markNoInput();

private:
    void setupUi(const QString& windowName, const QString& time);
    void updateHeight();
    void applyStyles();

protected:
    void resizeEvent(QResizeEvent* event) override;

    QLabel* headerLabel;
    QLabel* timeLabel;
    QTextEdit* contentArea;
};
