#include "KeyLogBlock.hpp"
#include <QHBoxLayout>
#include <QAbstractTextDocumentLayout>
#include <QResizeEvent>

KeyLogBlock::KeyLogBlock(const QString& windowName, const QString& time, const QString& headerColor, QWidget* parent)
    : QFrame(parent) {
    setupUi(windowName, time);
    applyStyles(headerColor);
    updateHeight(); 
}

void KeyLogBlock::setupUi(const QString& windowName, const QString& time) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Header container
    QWidget* headerWidget = new QWidget(this);
    headerWidget->setObjectName("headerWidget");
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(15, 10, 15, 10);

    headerLabel = new QLabel(windowName, this);
    headerLabel->setObjectName("headerLabel");
    
    timeLabel = new QLabel(time, this);
    timeLabel->setObjectName("timeLabel");
    
    headerLayout->addWidget(headerLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(timeLabel);

    // Content area
    contentArea = new QTextEdit(this);
    contentArea->setObjectName("contentArea");
    contentArea->setReadOnly(true);
    contentArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    contentArea->setMinimumHeight(0);
    contentArea->setMaximumHeight(16777215); 

    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(contentArea);
}

void KeyLogBlock::appendLog(const QString& text) {
    contentArea->moveCursor(QTextCursor::End);
    contentArea->insertPlainText(text);
    contentArea->moveCursor(QTextCursor::End);
    
    updateHeight();
}

QString KeyLogBlock::getLogText() const {
    return contentArea->toPlainText();
}

QString KeyLogBlock::getHeader() const {
    return QString("[%1] [%2]").arg(headerLabel->text()).arg(timeLabel->text());
}

void KeyLogBlock::updateHeight() {
    contentArea->document()->setTextWidth(contentArea->viewport()->width());

    int height = contentArea->document()->documentLayout()->documentSize().height();
    
    height += 10; 
    height = qMax(30, height);

    contentArea->setFixedHeight(height);
}

void KeyLogBlock::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    updateHeight();
}

bool KeyLogBlock::isEmpty() const {
    QString text = contentArea->toPlainText().trimmed();
    return text.isEmpty();
}

void KeyLogBlock::markNoInput() {
    contentArea->setHtml("<i style='color: #888;'>&lt;No Input&gt;</i>");
    updateHeight();
}

void KeyLogBlock::applyStyles(const QString& headerColor) {
    this->setStyleSheet(
        "KeyLogBlock { "
        "   background-color: #FFFFFF; "
        "   border: 3px solid #000000; "
        "   margin-bottom: 20px; "
        "}"
    );

    headerLabel->setStyleSheet(
        "font-family: 'Arial Black', sans-serif; "
        "font-size: 16px; "
        "font-weight: bold; "
        "color: #000000;"
    );

    timeLabel->setStyleSheet(
        "font-family: 'Courier New', monospace; "
        "font-size: 14px; "
        "font-weight: 900; "
        "color: #000000;"
    );

    QWidget* headerWidget = this->findChild<QWidget*>("headerWidget");
    if (headerWidget) {
        headerWidget->setStyleSheet(
            QString("#headerWidget { "
            "   background-color: %1; "
            "   border-bottom: 3px solid #000000; "
            "}").arg(headerColor)
        );
    }

    contentArea->setStyleSheet(
        "#contentArea { "
        "   background-color: #FFFFFF; "
        "   color: #000000; "
        "   font-family: 'Verdana', sans-serif; "
        "   font-size: 15px; "
        "   border: none; "
        "   padding: 5px; "
        "}"
    );
}
