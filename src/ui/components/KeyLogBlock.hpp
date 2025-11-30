#pragma once

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>

class KeyLogBlock : public QFrame {
    Q_OBJECT

public:
    explicit KeyLogBlock(const QString& windowName, const QString& time, const QString& headerColor = "#FFD600", QWidget* parent = nullptr);
    void appendLog(const QString& text);
    QString getLogText() const;
    QString getHeader() const;
    bool isEmpty() const;
    void markNoInput();

private:
    void setupUi(const QString& windowName, const QString& time);
    void updateHeight();
    void applyStyles(const QString& headerColor);

protected:
    void resizeEvent(QResizeEvent* event) override;

    QLabel* headerLabel;
    QLabel* timeLabel;
    QTextEdit* contentArea;
};
