#pragma once

#include <QWidget>
#include <vector>
#include <utility>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

struct ClickPoint {
    int x;
    int y;
    int count;
};

class ClicksPageWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal offset READ offset WRITE setOffset)

public:
    explicit ClicksPageWidget(QWidget *parent = nullptr);
    void loadClicks();
    void addClick(int x, int y);
    
    const std::vector<ClickPoint>& getClicks() const { return clicks; }
    
    qreal offset() const { return m_offset; }
    void setOffset(qreal offset);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void startRandomMove();

private:
    std::vector<ClickPoint> clicks;
    int maxCount = 0;
    int maxX = 0;
    int maxY = 0;
    
    QLabel *titleLabel;
    qreal m_offset;
    QPropertyAnimation *offsetAnimation;
    QTimer *moveTimer;
    
    QColor getColorForCount(int count);
};
