#include "chatbubble.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>

ChatBubble::ChatBubble(const QString &message, const QString &sender, 
                       const QDateTime &timestamp, bool isMyMessage, QWidget *parent)
    : QWidget(parent)
    , m_message(message)
    , m_sender(sender) 
    , m_timestamp(timestamp)
    , m_isMyMessage(isMyMessage)
{
    setMinimumHeight(60);
    setMaximumWidth(600);
    
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 10, 20, 10);
    
    // Create bubble container
    QHBoxLayout *bubbleLayout = new QHBoxLayout();
    bubbleLayout->setSpacing(0);
    
    // Message content widget
    QWidget *bubbleWidget = new QWidget();
    bubbleWidget->setMaximumWidth(400);
    bubbleWidget->setStyleSheet(QString(
        "QWidget {"
        "background-color: %1;"
        "border-radius: 18px;"
        "padding: 0px;"
        "}"
    ).arg(isMyMessage ? "#667eea" : "#f1f3f4"));
    
    QVBoxLayout *contentLayout = new QVBoxLayout(bubbleWidget);
    contentLayout->setContentsMargins(16, 12, 16, 12);
    contentLayout->setSpacing(4);
    
    // Sender label (only for others' messages)
    if (!isMyMessage) {
        senderLabel = new QLabel(sender);
        senderLabel->setStyleSheet(QString(
            "color: #667eea; font-size: 12px; font-weight: bold; margin: 0;"
        ));
        contentLayout->addWidget(senderLabel);
    }
    
    // Message text
    messageLabel = new QLabel(message);
    messageLabel->setWordWrap(true);
    messageLabel->setStyleSheet(QString(
        "color: %1; font-size: 14px; line-height: 1.4; margin: 0; background: transparent;"
    ).arg(isMyMessage ? "white" : "#333333"));
    contentLayout->addWidget(messageLabel);
    
    // Time label
    timeLabel = new QLabel(timestamp.toString("hh:mm"));
    timeLabel->setStyleSheet(QString(
        "color: %1; font-size: 11px; margin: 0;"
    ).arg(isMyMessage ? "rgba(255,255,255,0.8)" : "#999999"));
    timeLabel->setAlignment(isMyMessage ? Qt::AlignRight : Qt::AlignLeft);
    contentLayout->addWidget(timeLabel);
    
    // Arrange bubble in layout
    if (isMyMessage) {
        // My message - right aligned
        bubbleLayout->addStretch();
        bubbleLayout->addWidget(bubbleWidget);
    } else {
        // Other's message - left aligned  
        bubbleLayout->addWidget(bubbleWidget);
        bubbleLayout->addStretch();
    }
    
    mainLayout->addLayout(bubbleLayout);
    
    // Calculate appropriate height
    QFontMetrics fm(messageLabel->font());
    int textHeight = fm.boundingRect(0, 0, 350, 0, Qt::TextWordWrap, message).height();
    int totalHeight = textHeight + (isMyMessage ? 40 : 60); // Extra space for sender name
    setMinimumHeight(qMax(60, totalHeight));
}

void ChatBubble::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    
    // Add subtle shadow effect
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // No additional painting needed - using stylesheets
}