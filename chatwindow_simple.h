#ifndef CHATWINDOW_SIMPLE_H
#define CHATWINDOW_SIMPLE_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
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

QT_BEGIN_NAMESPACE
namespace Ui { class ChatWindow; }
QT_END_NAMESPACE

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
    
signals:
    void messageReceived(const QString &sender, const QString &message);
    void clientConnected(const QString &address);
    void clientDisconnected(const QString &address);
    
private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onDataReceived();
    
public:
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
    void onServerMessage(const QString &sender, const QString &message);
    void onClientMessage(const QString &message);
    void onClientConnected();
    void onClientDisconnected();
    void onConnectionError(const QString &error);
    void updateConnectionStatus();
    void onModeChanged(int index);

private:
    void setupConnections();
    void addMessage(const ChatMessage &msg);
    void addSystemMessage(const QString &message);
    void scrollToBottom();
    void saveMessageToFile(const ChatMessage &msg);
    QString getLocalIP();
    QString formatMessage(const ChatMessage &msg);
    
    Ui::ChatWindow *ui;
    
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

#endif // CHATWINDOW_SIMPLE_H