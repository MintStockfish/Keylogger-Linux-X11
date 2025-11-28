#include "AnimatedBorder.hpp"
#include <QPainter>
#include <QPen>
#include <QRandomGenerator>
#include <QDebug>

AnimatedBorder::AnimatedBorder(QWidget *parent)
    : QWidget(parent), m_offset(0.0) {
    
    setFixedWidth(6);
    setAttribute(Qt::WA_TranslucentBackground);
    
    offsetAnimation = new QPropertyAnimation(this, "offset", this);
    offsetAnimation->setDuration(600);
    offsetAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    
    connect(offsetAnimation, &QPropertyAnimation::valueChanged, this, [this]() {
        update();
    });
    
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &AnimatedBorder::startRandomMove);
    moveTimer->start(2000);
    
    startRandomMove();
}

AnimatedBorder::~AnimatedBorder() {
}

void AnimatedBorder::setOffset(qreal offset) {
    m_offset = offset;
    update();
}

void AnimatedBorder::startRandomMove() {
    bool moveUp = QRandomGenerator::global()->bounded(2) == 0;
    
    int randomDistance = QRandomGenerator::global()->bounded(15, 60);
    
    qreal currentOffset = m_offset;
    
    while (currentOffset < 0) currentOffset += 11;
    while (currentOffset >= 11) currentOffset -= 11;
    
    qreal newOffset;
    if (moveUp) {
        newOffset = currentOffset - randomDistance;
    } else {
        newOffset = currentOffset + randomDistance;
    }
        
    offsetAnimation->stop();
    offsetAnimation->setStartValue(currentOffset);
    offsetAnimation->setEndValue(newOffset);
    offsetAnimation->start();
}

void AnimatedBorder::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QPen pen;
    pen.setColor(QColor(0, 0, 0, 170));
    pen.setWidth(6);
    
    QVector<qreal> dashes;
    dashes << 5 << 6;
    pen.setDashPattern(dashes);
    pen.setDashOffset(m_offset);
    
    painter.setPen(pen);
    painter.drawLine(3, 0, 3, height());
}
