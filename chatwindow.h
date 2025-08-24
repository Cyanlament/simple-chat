#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSplitter>
#include <QListWidget>
#include <QComboBox>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QThread>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSystemTrayIcon>
#include <QDateTime>
#include <QStandardPaths>
#include <QInputDialog>
#include <QAbstractSocket>

class ChatMessage {
public:
    QString sender;
    QString message;
    QDateTime timestamp;
    QString type; // "text", "system", "file"
    
    ChatMessage(const QString &s, const QString &m, const QString &t = "text")
        : sender(s), message(m), timestamp(QDateTime::currentDateTime()), type(t) {}
};

class ChatServer : public QObject
{
    Q_OBJECT
    
public:
    ChatServer(QObject *parent = nullptr);
    void startServer(quint16 port = 12345);
    void stopServer();
    bool isListening() const;
    
signals:
    void messageReceived(const QString &sender, const QString &message);
    void clientConnected(const QString &address);
    void clientDisconnected(const QString &address);
    
private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onDataReceived();
    
private:
    QTcpServer *server;
    QList<QTcpSocket*> clients;
    
    void broadcastMessage(const QString &message, QTcpSocket *excludeClient = nullptr);
    void sendToClient(QTcpSocket *client, const QString &message);
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

class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

private slots:
    void sendMessage();
    void onReturnPressed();
    void startServer();
    void connectToServer();
    void disconnectFromServer();
    void clearChat();
    void saveChat();
    void loadChat();
    void showAbout();
    void onServerMessage(const QString &sender, const QString &message);
    void onClientMessage(const QString &message);
    void onClientConnected();
    void onClientDisconnected();
    void onConnectionError(const QString &error);
    void updateConnectionStatus();
    void toggleEmojiPanel();
    void insertEmoji(const QString &emoji);

private:
    void setupUI();
    void setupMenuBar();
    void setupStyles();
    void addMessage(const ChatMessage &msg);
    void addSystemMessage(const QString &message);
    void scrollToBottom();
    void saveMessageToFile(const ChatMessage &msg);
    void loadMessagesFromFile();
    QString getLocalIP();
    void createEmojiPanel();
    
    // UI Components
    QWidget *centralWidget;
    QSplitter *mainSplitter;
    QTextEdit *chatDisplay;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QPushButton *emojiButton;
    QWidget *emojiPanel;
    QLabel *statusLabel;
    QComboBox *modeCombo;
    QPushButton *actionButton;
    QLineEdit *hostInput;
    QLabel *connectionLabel;
    
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

#endif // CHATWINDOW_H