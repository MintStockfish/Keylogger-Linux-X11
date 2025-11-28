#pragma once

#include <QPushButton>
#include <QFrame>
#include <QWidget>
#include <QVBoxLayout>
#include <QMouseEvent>

class BrutalistButton : public QWidget {
    Q_OBJECT

public:
    BrutalistButton(const QString& text, const QString& color, QWidget* parent = nullptr);
    
    void setActive(bool active);
    bool isButtonActive() const { return isActive; }
    
signals:
    void clicked();

protected:
    bool event(QEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void updatePosition();
    
    QPushButton* button;
    QFrame* shadow;
    bool isPressed;
    bool isHovered;
    bool isActive;
    QString buttonColor;
    
    int normalOffsetX = 0;
    int normalOffsetY = 0;
    int hoverOffsetX = 3;
    int hoverOffsetY = 3;
    int pressedOffsetX = 6;
    int pressedOffsetY = 6;
    int activeOffsetX = 5;
    int activeOffsetY = 5;
};
