#pragma once

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>

class AnimatedBorder : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal offset READ offset WRITE setOffset)

public:
    explicit AnimatedBorder(QWidget *parent = nullptr);
    ~AnimatedBorder();

    qreal offset() const { return m_offset; }
    void setOffset(qreal offset);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void startRandomMove();

private:
    qreal m_offset;
    QPropertyAnimation *offsetAnimation;
    QTimer *moveTimer;
};
