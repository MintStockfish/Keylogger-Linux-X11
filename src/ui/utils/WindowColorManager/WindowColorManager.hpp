#pragma once

#include <QString>
#include <QMap>
#include <QSet>

class WindowColorManager {
public:
    WindowColorManager();
    
    QString getColorForWindow(const QString& windowName);
    
private:
    QString generateUniqueColor();
    
    QMap<QString, QString> windowColorMap_;
    QSet<QString> usedColors_;
};
