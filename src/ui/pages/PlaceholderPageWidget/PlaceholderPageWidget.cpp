#include "PlaceholderPageWidget.hpp"
#include <QVBoxLayout>
#include <QLabel>

PlaceholderPageWidget::PlaceholderPageWidget(const QString& title, const QString& titleColor,
                                             const QString& description, QWidget* parent)
    : QWidget(parent) {
    setupUi(title, titleColor, description);
}

void PlaceholderPageWidget::setupUi(const QString& title, const QString& titleColor, 
                                    const QString& description) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    
    QLabel* titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet(QString("font-size: 48px; font-weight: bold; color: %1;").arg(titleColor));
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QLabel* descLabel = new QLabel(description, this);
    descLabel->setStyleSheet("font-size: 18px; color: #666;");
    descLabel->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addWidget(descLabel);
}
