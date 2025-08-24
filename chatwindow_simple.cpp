#include "chatwindow_simple.h"
#include "ui_chatwindow_simple.h"

// ChatServer Implementation
ChatServer::ChatServer(QObject *parent) : QObject(parent), server(new QTcpServer(this))
{
    connect(server, &QTcpServer::newConnection, this, &ChatServer::onNewConnection);
}

void ChatServer::startServer(quint16 port)
{
    if (server->listen(QHostAddress::Any, port)) {
        qDebug() << "Server started on port" << port;
    }
}

void ChatServer::stopServer()
{
    server->close();
    for (auto client : clients) {
        client->disconnectFromHost();
    }
    clients.clear();
}

bool ChatServer::isListening() const
{
    return server->isListening();
}

void ChatServer::onNewConnection()
{
    QTcpSocket *client = server->nextPendingConnection();
    clients.append(client);
    
    connect(client, &QTcpSocket::disconnected, this, &ChatServer::onClientDisconnected);
    connect(client, &QTcpSocket::readyRead, this, &ChatServer::onDataReceived);
    
    emit clientConnected(client->peerAddress().toString());
}

void ChatServer::onClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        clients.removeAll(client);
        emit clientDisconnected(client->peerAddress().toString());
        client->deleteLater();
    }
}

void ChatServer::onDataReceived()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        QByteArray data = client->readAll();
        QString message = QString::fromUtf8(data);
        
        int colonIndex = message.indexOf(':');
        if (colonIndex > 0) {
            QString sender = message.left(colonIndex);
            QString content = message.mid(colonIndex + 1);
            
            emit messageReceived(sender, content);
            broadcastMessage(message, client);
        }
    }
}

void ChatServer::broadcastMessage(const QString &message, QTcpSocket *excludeClient)
{
    for (auto client : clients) {
        if (client != excludeClient) {
            sendToClient(client, message);
        }
    }
}

void ChatServer::sendToClient(QTcpSocket *client, const QString &message)
{
    if (client && client->state() == QTcpSocket::ConnectedState) {
        client->write(message.toUtf8());
        client->flush();
    }
}

// ChatClient Implementation
ChatClient::ChatClient(QObject *parent) : QObject(parent), socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::connected, this, &ChatClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &ChatClient::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &ChatClient::onDataReceived);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &ChatClient::onError);
}

void ChatClient::connectToHost(const QString &host, quint16 port)
{
    socket->connectToHost(host, port);
}

void ChatClient::disconnectFromHost()
{
    socket->disconnectFromHost();
}

void ChatClient::sendMessage(const QString &message)
{
    if (socket->state() == QTcpSocket::ConnectedState) {
        socket->write(message.toUtf8());
        socket->flush();
    }
}

bool ChatClient::isConnected() const
{
    return socket->state() == QTcpSocket::ConnectedState;
}

void ChatClient::onConnected()
{
    emit connected();
}

void ChatClient::onDisconnected()
{
    emit disconnected();
}

void ChatClient::onDataReceived()
{
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    emit messageReceived(message);
}

void ChatClient::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    emit errorOccurred(socket->errorString());
}

// ChatWindow Implementation
ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
    , server(new ChatServer(this))
    , client(new ChatClient(this))
    , statusTimer(new QTimer(this))
    , isServerMode(true)
{
    ui->setupUi(this);
    
    // Get username
    bool ok;
    username = QInputDialog::getText(this, "设置用户名", "请输入您的用户名:", QLineEdit::Normal, "User", &ok);
    if (!ok || username.isEmpty()) {
        username = "Anonymous";
    }
    
    setWindowTitle(QString("网络聊天程序 - %1").arg(username));
    
    // Set up chat log file
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataDir);
    chatLogFile = appDataDir + "/chat_history.txt";
    
    setupConnections();
    
    // Initially disable host input in server mode
    ui->hostInput->setEnabled(false);
    
    // Connect timer
    connect(statusTimer, &QTimer::timeout, this, &ChatWindow::updateConnectionStatus);
    statusTimer->start(1000);
    
    addSystemMessage(QString("欢迎使用网络聊天程序！当前用户：%1").arg(username));
    addSystemMessage(QString("本机IP地址：%1").arg(getLocalIP()));
    
    ui->messageInput->setFocus();
}

ChatWindow::~ChatWindow()
{
    delete ui;
    if (server->isListening()) {
        server->stopServer();
    }
    if (client->isConnected()) {
        client->disconnectFromHost();
    }
}

void ChatWindow::setupConnections()
{
    // UI connections
    connect(ui->sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
    connect(ui->messageInput, &QLineEdit::returnPressed, this, &ChatWindow::onReturnPressed);
    connect(ui->modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ChatWindow::onModeChanged);
    connect(ui->actionButton, &QPushButton::clicked, [this]() {
        if (isServerMode) {
            startServer();
        } else {
            connectToServer();
        }
    });
    
    // Network connections
    connect(server, &ChatServer::messageReceived, this, &ChatWindow::onServerMessage);
    connect(server, &ChatServer::clientConnected, this, &ChatWindow::onClientConnected);
    connect(server, &ChatServer::clientDisconnected, this, &ChatWindow::onClientDisconnected);
    
    connect(client, &ChatClient::connected, this, &ChatWindow::onClientConnected);
    connect(client, &ChatClient::disconnected, this, &ChatWindow::onClientDisconnected);
    connect(client, &ChatClient::messageReceived, this, &ChatWindow::onClientMessage);
    connect(client, &ChatClient::errorOccurred, this, &ChatWindow::onConnectionError);
}

void ChatWindow::onModeChanged(int index)
{
    isServerMode = (index == 0);
    ui->hostInput->setEnabled(!isServerMode);
    ui->actionButton->setText(isServerMode ? "启动服务器" : "连接服务器");
}

void ChatWindow::sendMessage()
{
    QString message = ui->messageInput->text().trimmed();
    if (message.isEmpty()) {
        return;
    }
    
    // Create message object
    ChatMessage msg(username, message, true); // true = my message
    
    if (isServerMode && server->isListening()) {
        // Server mode: add message locally and broadcast
        addMessage(msg);
        saveMessageToFile(msg);
        
        // Broadcast to all clients
        QString broadcastMsg = QString("%1:%2").arg(username).arg(message);
        server->broadcastMessage(broadcastMsg);
        
    } else if (!isServerMode && client->isConnected()) {
        // Client mode: send to server
        QString clientMsg = QString("%1:%2").arg(username).arg(message);
        client->sendMessage(clientMsg);
        
        // Add message locally
        addMessage(msg);
        saveMessageToFile(msg);
    } else {
        addSystemMessage("错误：未连接到网络");
        return;
    }
    
    ui->messageInput->clear();
    ui->messageInput->setFocus();
}

void ChatWindow::onReturnPressed()
{
    sendMessage();
}

void ChatWindow::startServer()
{
    if (server->isListening()) {
        server->stopServer();
        ui->actionButton->setText("启动服务器");
        ui->connectionLabel->setText("未连接");
        ui->connectionLabel->setStyleSheet("color: #dc3545; background-color: #f8d7da;");
        addSystemMessage("服务器已停止");
    } else {
        server->startServer(12345);
        if (server->isListening()) {
            ui->actionButton->setText("停止服务器");
            ui->connectionLabel->setText("服务器运行中");
            ui->connectionLabel->setStyleSheet("color: #155724; background-color: #d4edda;");
            addSystemMessage(QString("服务器启动成功，监听端口：12345\n本机IP：%1").arg(getLocalIP()));
        }
    }
}

void ChatWindow::connectToServer()
{
    if (client->isConnected()) {
        client->disconnectFromHost();
        ui->actionButton->setText("连接服务器");
        ui->connectionLabel->setText("未连接");
        ui->connectionLabel->setStyleSheet("color: #dc3545; background-color: #f8d7da;");
    } else {
        QString host = ui->hostInput->text().trimmed();
        if (host.isEmpty()) {
            QMessageBox::warning(this, "错误", "请输入服务器IP地址");
            return;
        }
        
        client->connectToHost(host, 12345);
        addSystemMessage(QString("正在连接到服务器：%1:12345").arg(host));
    }
}

void ChatWindow::onServerMessage(const QString &sender, const QString &message)
{
    ChatMessage msg(sender, message, false);
    addMessage(msg);
    saveMessageToFile(msg);
}

void ChatWindow::onClientMessage(const QString &message)
{
    int colonIndex = message.indexOf(':');
    if (colonIndex > 0) {
        QString sender = message.left(colonIndex);
        QString content = message.mid(colonIndex + 1);
        
        ChatMessage msg(sender, content, false);
        addMessage(msg);
        saveMessageToFile(msg);
    }
}

void ChatWindow::onClientConnected()
{
    if (isServerMode) {
        addSystemMessage("新客户端已连接");
        ui->connectionLabel->setText("服务器运行中");
        ui->connectionLabel->setStyleSheet("color: #155724; background-color: #d4edda;");
    } else {
        ui->actionButton->setText("断开连接");
        ui->connectionLabel->setText("已连接");
        ui->connectionLabel->setStyleSheet("color: #155724; background-color: #d4edda;");
        addSystemMessage("成功连接到服务器");
    }
}

void ChatWindow::onClientDisconnected()
{
    if (isServerMode) {
        addSystemMessage("客户端已断开连接");
    } else {
        ui->actionButton->setText("连接服务器");
        ui->connectionLabel->setText("未连接");
        ui->connectionLabel->setStyleSheet("color: #dc3545; background-color: #f8d7da;");
        addSystemMessage("与服务器的连接已断开");
    }
}

void ChatWindow::onConnectionError(const QString &error)
{
    addSystemMessage(QString("连接错误：%1").arg(error));
    ui->connectionLabel->setText("连接失败");
    ui->connectionLabel->setStyleSheet("color: #dc3545; background-color: #f8d7da;");
}

void ChatWindow::updateConnectionStatus()
{
    QString status;
    if (isServerMode && server->isListening()) {
        status = QString("服务器模式 - 端口：12345 - 用户：%1").arg(username);
    } else if (!isServerMode && client->isConnected()) {
        status = QString("客户端模式 - 连接到：%1 - 用户：%2").arg(ui->hostInput->text()).arg(username);
    } else {
        status = QString("离线模式 - 用户：%1").arg(username);
    }
    
    statusBar()->showMessage(status);
}

void ChatWindow::addMessage(const ChatMessage &msg)
{
    messages.append(msg);
    
    QString formattedMsg = formatMessage(msg);
    ui->chatDisplay->append(formattedMsg);
    scrollToBottom();
}

void ChatWindow::addSystemMessage(const QString &message)
{
    ChatMessage msg("系统", message);
    
    QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString html = QString(
        "<div style='text-align: center; margin: 10px 0;'>"
        "<span style='background-color: #f8f9fa; color: #6c757d; "
        "padding: 6px 12px; border-radius: 12px; font-size: 12px;'>"
        "[%1] %2</span></div>"
    ).arg(timeStr).arg(message);
    
    ui->chatDisplay->append(html);
    scrollToBottom();
}

QString ChatWindow::formatMessage(const ChatMessage &msg)
{
    QString timeStr = msg.timestamp.toString("hh:mm");
    
    if (msg.isMyMessage) {
        // My message - right aligned with purple gradient
        return QString(
            "<div style='text-align: right; margin: 8px 0;'>"
            "<div style='display: inline-block; max-width: 70%; text-align: left;'>"
            "<div style='background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
            "stop:0 #667eea, stop:1 #764ba2); color: white; "
            "padding: 12px 16px; border-radius: 18px 18px 4px 18px; "
            "margin-bottom: 4px; word-wrap: break-word; font-size: 14px;'>"
            "%1</div>"
            "<div style='text-align: right; color: #999; font-size: 11px; margin-right: 8px;'>%2</div>"
            "</div></div>"
        ).arg(msg.message.toHtmlEscaped()).arg(timeStr);
    } else {
        // Other's message - left aligned with light blue
        return QString(
            "<div style='text-align: left; margin: 8px 0;'>"
            "<div style='color: #667eea; font-size: 12px; font-weight: bold; margin-left: 8px; margin-bottom: 2px;'>%1</div>"
            "<div style='display: inline-block; max-width: 70%;'>"
            "<div style='background-color: #f1f3f4; color: #333; "
            "padding: 12px 16px; border-radius: 18px 18px 18px 4px; "
            "margin-bottom: 4px; word-wrap: break-word; font-size: 14px;'>"
            "%2</div>"
            "<div style='text-align: left; color: #999; font-size: 11px; margin-left: 8px;'>%3</div>"
            "</div></div>"
        ).arg(msg.sender).arg(msg.message.toHtmlEscaped()).arg(timeStr);
    }
}

void ChatWindow::scrollToBottom()
{
    QScrollBar *scrollBar = ui->chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWindow::saveMessageToFile(const ChatMessage &msg)
{
    QFile file(chatLogFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QString("[%1] %2: %3\n")
            .arg(msg.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(msg.sender)
            .arg(msg.message);
    }
}

QString ChatWindow::getLocalIP()
{
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
            return address.toString();
        }
    }
    return "127.0.0.1";
}