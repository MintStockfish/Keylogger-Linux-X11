#include "ClicksPageWidget.hpp"
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <algorithm>
#include <cmath>
#include <QVBoxLayout>
#include <QRandomGenerator>

ClicksPageWidget::ClicksPageWidget(QWidget *parent) : QWidget(parent), m_offset(0.0) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setAlignment(Qt::AlignTop);

    titleLabel = new QLabel("MOUSE CLICKS HEATMAP", this);
    titleLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #666; margin-bottom: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    offsetAnimation = new QPropertyAnimation(this, "offset", this);
    offsetAnimation->setDuration(600);
    offsetAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    
    connect(offsetAnimation, &QPropertyAnimation::valueChanged, this, [this]() {
        update();
    });
    
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &ClicksPageWidget::startRandomMove);
    moveTimer->start(2000);
    
    startRandomMove();
}

void ClicksPageWidget::setOffset(qreal offset) {
    m_offset = offset;
    update();
}

void ClicksPageWidget::startRandomMove() {
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

void ClicksPageWidget::loadClicks() {
    clicks.clear();
    maxCount = 0;
    maxX = 0;
    maxY = 0;

    QFile file("mouse_log.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open mouse_log.txt";
        update();
        return;
    }

    QTextStream in(&file);
    QRegularExpression re("X: (\\d+), Y: (\\d+) - Count: (\\d+)");

    while (!in.atEnd()) {
        QString line = in.readLine();
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            int x = match.captured(1).toInt();
            int y = match.captured(2).toInt();
            int count = match.captured(3).toInt();

            clicks.push_back({x, y, count});
            
            if (count > maxCount) maxCount = count;
            if (x > maxX) maxX = x;
            if (y > maxY) maxY = y;
        }
    }
    file.close();
    
    if (maxX < 1920) maxX = 1920;
    if (maxY < 1080) maxY = 1080;

    update();
}

void ClicksPageWidget::addClick(int x, int y) {
    auto it = std::find_if(clicks.begin(), clicks.end(), [x, y](const ClickPoint& p) {
        return p.x == x && p.y == y;
    });

    if (it != clicks.end()) {
        it->count++;
        if (it->count > maxCount) maxCount = it->count;
    } else {
        clicks.push_back({x, y, 1});
        if (1 > maxCount) maxCount = 1;
    }

    if (x > maxX) maxX = x;
    if (y > maxY) maxY = y;


    if (maxX < 1920) maxX = 1920;
    if (maxY < 1080) maxY = 1080;

    update();
}

QColor ClicksPageWidget::getColorForCount(int count) {
    if (maxCount == 0) return QColor(0, 255, 255); 

    double ratio = (double)count / (double)maxCount;
    
    int hue = 240 - (int)(ratio * 240);
    return QColor::fromHsl(hue, 255, 128); 
}

void ClicksPageWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (clicks.empty()) {
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 14));
        painter.drawText(rect().adjusted(0, 100, 0, 0), Qt::AlignCenter, "No click data found. Start clicking!");
        return;
    }

    int titleHeight = 80;
    int margin = 40;
    
    int drawWidth = width() - 2 * margin;
    int drawHeight = height() - titleHeight - margin; 
    
    if (drawWidth <= 0 || drawHeight <= 0) return;

    double scaleX = (double)drawWidth / (double)maxX;
    double scaleY = (double)drawHeight / (double)maxY;
    
    double scale = std::min(scaleX, scaleY);

    int offsetX = margin + (drawWidth - (maxX * scale)) / 2;
    int offsetY = titleHeight + (drawHeight - (maxY * scale)) / 2;

    QPen pen;
    pen.setColor(QColor("#666"));
    pen.setWidth(4); 
    
    QVector<qreal> dashes;
    dashes << 5 << 6; 
    pen.setDashPattern(dashes);
    pen.setDashOffset(m_offset);
    
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(offsetX, offsetY, maxX * scale, maxY * scale);

    for (const auto& point : clicks) {
        int px = offsetX + (int)(point.x * scale);
        int py = offsetY + (int)(point.y * scale);
        
        int radius = 3 + std::min(point.count, 10); 
        
        QColor color = getColorForCount(point.count);
        color.setAlpha(220);
        
        painter.setBrush(color);
        painter.setPen(QPen(QColor(50, 50, 50), 2));
        painter.drawEllipse(QPoint(px, py), radius, radius);
    }
}
