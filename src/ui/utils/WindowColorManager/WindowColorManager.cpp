#include "WindowColorManager.hpp"
#include <QColor>
#include <QRandomGenerator>

WindowColorManager::WindowColorManager() {
}

QString WindowColorManager::getColorForWindow(const QString& windowName) {
    if (windowColorMap_.contains(windowName)) {
        return windowColorMap_[windowName];
    }
    
    QString color = generateUniqueColor();
    windowColorMap_[windowName] = color;
    return color;
}

QString WindowColorManager::generateUniqueColor() {
    QStringList baseColors = {
        "#FF0090", "#00D9FF", "#FFD600", "#00FF85",
        "#FF6B35", "#FF1744", "#D500F9", "#651FFF",
        "#2979FF", "#00B0FF", "#00E5FF", "#1DE9B6",
        "#00E676", "#76FF03", "#C6FF00", "#FFEA00",
        "#FFC400", "#FF9100", "#FF3D00", "#F50057",
        "#E91E63", "#9C27B0", "#673AB7", "#3F51B5",
        "#2196F3", "#03A9F4", "#00BCD4", "#009688",
        "#4CAF50", "#8BC34A", "#CDDC39", "#FFC107",
        "#FF9800", "#FF5722"
    };
    
    if (usedColors_.size() >= baseColors.size()) {
        usedColors_.clear();
    }
    
    QStringList availableColors;
    for (const QString& bc : baseColors) {
        if (!usedColors_.contains(bc)) {
            availableColors.append(bc);
        }
    }
    
    QString baseColor = availableColors[QRandomGenerator::global()->bounded(availableColors.size())];
    
    int variation = QRandomGenerator::global()->bounded(41) - 20;
    QColor qc(baseColor);
    int h, s, v;
    qc.getHsv(&h, &s, &v);
    v = qBound(0, v + variation, 255);
    qc.setHsv(h, s, v);
    
    usedColors_.insert(baseColor);
    return qc.name();
}
