#ifndef CHATBUBBLE_H
#define CHATBUBBLE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QDateTime>

class ChatBubble : public QWidget
{
    Q_OBJECT

public:
    explicit ChatBubble(const QString &message, const QString &sender, 
                       const QDateTime &timestamp, bool isMyMessage, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_message;
    QString m_sender;
    QDateTime m_timestamp;
    bool m_isMyMessage;
    
    QLabel *messageLabel;
    QLabel *timeLabel;
    QLabel *senderLabel;
};

#endif // CHATBUBBLE_H