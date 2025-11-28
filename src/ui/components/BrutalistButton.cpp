#include "BrutalistButton.hpp"

BrutalistButton::BrutalistButton(const QString& text, const QString& color, QWidget* parent)
    : QWidget(parent), isPressed(false), isHovered(false), buttonColor(color) {
    
    setFixedHeight(76);  
    
    shadow = new QFrame(this);
    shadow->setStyleSheet("QFrame { background-color: black; border: none; }");
    shadow->setGeometry(6, 6, width() - 6, 70);
    
    button = new QPushButton(text, this);
    button->setFixedHeight(70); 
    button->setStyleSheet(
        QString("QPushButton { "
        "   font-family: 'Arial', sans-serif; "
        "   font-size: 16px; "
        "   font-weight: bold; "
        "   color: black; "
        "   border: 3px solid black; "
        "   padding: 10px; "
        "   text-align: center; "
        "   background-color: %1; "
        "}").arg(color)
    );
    button->setGeometry(0, 0, width() - 6, 60);
    
    connect(button, &QPushButton::pressed, this, [this]() {
        isPressed = true;
        updatePosition();
    });
    
    connect(button, &QPushButton::released, this, [this]() {
        isPressed = false;
        updatePosition();
    });
    
    connect(button, &QPushButton::clicked, this, &BrutalistButton::clicked);
    
    button->installEventFilter(this);
}

void BrutalistButton::updatePosition() {
    int offsetX, offsetY;
    
    if (isPressed) {
        offsetX = pressedOffsetX;
        offsetY = pressedOffsetY;
    } else if (isHovered) {
        offsetX = hoverOffsetX;
        offsetY = hoverOffsetY;
    } else {
        offsetX = normalOffsetX;
        offsetY = normalOffsetY;
    }
    
    button->setGeometry(offsetX, offsetY, width() - 6, 70);
}

bool BrutalistButton::event(QEvent* e) {
    if (e->type() == QEvent::Resize) {
        shadow->setGeometry(6, 6, width() - 6, 70);
        updatePosition();
    }
    return QWidget::event(e);
}

bool BrutalistButton::eventFilter(QObject* obj, QEvent* event) {
    if (obj == button) {
        if (event->type() == QEvent::Enter) {
            isHovered = true;
            updatePosition();
            return false;
        } else if (event->type() == QEvent::Leave) {
            isHovered = false;
            updatePosition();
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}
