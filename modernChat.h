#ifndef MODERNCHAT_H
#define MODERNCHAT_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QAbstractSocket>
#include <QDir>
#include <QStandardPaths>
#include <QScrollBar>
#include <QFrame>
#include "chatbubble.h"

class ChatMessage {
public:
    QString sender;
    QString message;
    QDateTime timestamp;
    bool isMyMessage;
    
    ChatMessage(const QString &s, const QString &m, bool mine = false)
        : sender(s), message(m), timestamp(QDateTime::currentDateTime()), isMyMessage(mine) {}
};

class ChatServer : public QObject
{
    Q_OBJECT
    
public:
    ChatServer(QObject *parent = nullptr);
    void startServer(quint16 port = 12345);
    void stopServer();
    bool isListening() const;
    
    QTcpServer *server;
    QList<QTcpSocket*> clients;
    void broadcastMessage(const QString &message, QTcpSocket *excludeClient = nullptr);
    void sendToClient(QTcpSocket *client, const QString &message);
    
signals:
    void messageReceived(const QString &sender, const QString &message);
    void clientConnected(const QString &address);
    void clientDisconnected(const QString &address);
    
private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onDataReceived();
};

class ChatClient : public QObject
{
    Q_OBJECT
    
public:
    ChatClient(QObject *parent = nullptr);
    void connectToHost(const QString &host, quint16 port = 12345);
    void disconnectFromHost();
    void sendMessage(const QString &message);
    bool isConnected() const;
    
signals:
    void connected();
    void disconnected();
    void messageReceived(const QString &message);
    void errorOccurred(const QString &error);
    
private slots:
    void onConnected();
    void onDisconnected();
    void onDataReceived();
    void onError(QAbstractSocket::SocketError error);
    
private:
    QTcpSocket *socket;
};

class ModernChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    ModernChatWindow(QWidget *parent = nullptr);
    ~ModernChatWindow();

private slots:
    void sendMessage();
    void startServer();
    void connectToServer();
    void onServerMessage(const QString &sender, const QString &message);
    void onClientMessage(const QString &message);
    void onClientConnected();
    void onClientDisconnected();
    void onConnectionError(const QString &error);
    void updateConnectionStatus();
    void onModeChanged(int index);
    void toggleEmojiPanel();
    void insertEmoji(const QString &emoji);

private:
    void setupUI();
    void setupConnections();
    void addMessageBubble(const ChatMessage &msg);
    void addSystemMessage(const QString &message);
    void scrollToBottom();
    void saveMessageToFile(const ChatMessage &msg);
    QString getLocalIP();
    void createEmojiPanel();
    
    // UI Components
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    
    // Connection panel
    QGroupBox *connectionGroup;
    QComboBox *modeCombo;
    QLineEdit *hostInput;
    QPushButton *actionButton;
    QLabel *connectionLabel;
    
    // Chat area
    QScrollArea *scrollArea;
    QWidget *scrollContent;
    QVBoxLayout *messagesLayout;
    
    // Input area
    QWidget *inputContainer;
    QPushButton *emojiButton;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    
    // Emoji panel
    QWidget *emojiPanel;
    
    // Network components
    ChatServer *server;
    ChatClient *client;
    
    // Data
    QList<ChatMessage> messages;
    QString username;
    QTimer *statusTimer;
    bool isServerMode;
    
    // File operations
    QString chatLogFile;
};

#endif // MODERNCHAT_H