#pragma once

#include <QWidget>
#include <QString>

class PlaceholderPageWidget : public QWidget {
    Q_OBJECT

public:
    explicit PlaceholderPageWidget(const QString& title, const QString& titleColor, 
                                   const QString& description, QWidget* parent = nullptr);

private:
    void setupUi(const QString& title, const QString& titleColor, const QString& description);
};
