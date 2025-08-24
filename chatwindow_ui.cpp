#include "chatwindow_ui.h"
#include "ui_chatwindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QScrollBar>
#include <QStandardPaths>

// ChatServer Implementation (same as before)
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

// ChatClient Implementation (same as before)
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
    
    // Get the messages layout from UI
    messagesLayout = ui->scrollAreaWidgetContents->findChild<QVBoxLayout*>("messagesLayout");
    
    setupConnections();
    createEmojiPanel();
    
    // Initially disable host input in server mode
    ui->hostInput->setEnabled(false);
    
    // Connect timer
    connect(statusTimer, &QTimer::timeout, this, &ChatWindow::updateConnectionStatus);
    statusTimer->start(1000);
    
    // Load previous messages
    loadMessagesFromFile();
    
    addSystemMessage(QString("欢迎使用网络聊天程序！当前用户：%1").arg(username));
    addSystemMessage(QString("本机IP地址：%1").arg(getLocalIP()));
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
    connect(ui->emojiButton, &QPushButton::clicked, this, &ChatWindow::toggleEmojiPanel);
    
    // Network connections
    connect(server, &ChatServer::messageReceived, this, &ChatWindow::onServerMessage);
    connect(server, &ChatServer::clientConnected, this, &ChatWindow::onClientConnected);
    connect(server, &ChatServer::clientDisconnected, this, &ChatWindow::onClientDisconnected);
    
    connect(client, &ChatClient::connected, this, &ChatWindow::onClientConnected);
    connect(client, &ChatClient::disconnected, this, &ChatWindow::onClientDisconnected);
    connect(client, &ChatClient::messageReceived, this, &ChatWindow::onClientMessage);
    connect(client, &ChatClient::errorOccurred, this, &ChatWindow::onConnectionError);
    
    ui->messageInput->setFocus();
}

void ChatWindow::onModeChanged(int index)
{
    isServerMode = (index == 0);
    ui->hostInput->setEnabled(!isServerMode);
    ui->actionButton->setText(isServerMode ? "启动服务器" : "连接服务器");
}

void ChatWindow::createMessageWidget(const ChatMessage &msg)
{
    // Create message container
    QWidget *messageWidget = new QWidget();
    messageWidget->setObjectName("messageItem");
    
    QHBoxLayout *messageLayout = new QHBoxLayout(messageWidget);
    messageLayout->setContentsMargins(0, 5, 0, 5);
    
    // Create message bubble
    QLabel *messageLabel = new QLabel();
    messageLabel->setText(msg.message);
    messageLabel->setWordWrap(true);
    messageLabel->setMaximumWidth(400);
    messageLabel->setMinimumHeight(40);
    
    // Create timestamp
    QLabel *timeLabel = new QLabel(msg.timestamp.toString("hh:mm"));
    timeLabel->setObjectName("timestampLabel");
    
    if (msg.isMyMessage) {
        // My message - align right, blue bubble
        messageLabel->setObjectName("myMessageText");
        messageLabel->setAlignment(Qt::AlignRight);
        
        messageLayout->addStretch();
        messageLayout->addWidget(timeLabel);
        messageLayout->addWidget(messageLabel);
        
        // Add subtle shadow effect
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(8);
        shadow->setColor(QColor(0, 0, 0, 30));
        shadow->setOffset(0, 2);
        messageLabel->setGraphicsEffect(shadow);
        
    } else {
        // Other user's message - align left, light blue bubble
        messageLabel->setObjectName("messageText");
        messageLabel->setAlignment(Qt::AlignLeft);
        
        // Add sender name for other users
        QLabel *senderLabel = new QLabel(msg.sender);
        senderLabel->setStyleSheet("color: #666; font-size: 12px; font-weight: bold; margin-bottom: 3px;");
        
        QVBoxLayout *leftMessageLayout = new QVBoxLayout();
        leftMessageLayout->setSpacing(2);
        leftMessageLayout->addWidget(senderLabel);
        
        QHBoxLayout *bubbleLayout = new QHBoxLayout();
        bubbleLayout->addWidget(messageLabel);
        bubbleLayout->addWidget(timeLabel);
        bubbleLayout->addStretch();
        
        leftMessageLayout->addLayout(bubbleLayout);
        
        QWidget *leftContainer = new QWidget();
        leftContainer->setLayout(leftMessageLayout);
        
        messageLayout->addWidget(leftContainer);
        messageLayout->addStretch();
        
        // Add subtle shadow effect
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(8);
        shadow->setColor(QColor(0, 0, 0, 20));
        shadow->setOffset(0, 1);
        messageLabel->setGraphicsEffect(shadow);
    }
    
    // Insert before the spacer (last item)
    int index = messagesLayout->count() - 1;
    messagesLayout->insertWidget(index, messageWidget);
    
    // Animate message appearance
    QPropertyAnimation *animation = new QPropertyAnimation(messageWidget, "geometry");
    animation->setDuration(200);
    animation->setStartValue(QRect(messageWidget->x(), messageWidget->y() - 20, messageWidget->width(), 0));
    animation->setEndValue(messageWidget->geometry());
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    
    scrollToBottom();
}

void ChatWindow::addSystemMessage(const QString &message)
{
    QWidget *systemWidget = new QWidget();
    QHBoxLayout *systemLayout = new QHBoxLayout(systemWidget);
    systemLayout->setContentsMargins(0, 10, 0, 10);
    
    QLabel *systemLabel = new QLabel(message);
    systemLabel->setStyleSheet(R"(
        color: #999;
        font-size: 12px;
        font-style: italic;
        background-color: #f0f0f0;
        padding: 8px 12px;
        border-radius: 12px;
    )");
    systemLabel->setAlignment(Qt::AlignCenter);
    systemLabel->setWordWrap(true);
    
    systemLayout->addStretch();
    systemLayout->addWidget(systemLabel);
    systemLayout->addStretch();
    
    int index = messagesLayout->count() - 1;
    messagesLayout->insertWidget(index, systemWidget);
    
    scrollToBottom();
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
        createMessageWidget(msg);
        saveMessageToFile(msg);
        
        // Broadcast to all clients (server doesn't send to itself via network)
        QString broadcastMsg = QString("%1:%2").arg(username).arg(message);
        for (auto client : server->clients) {
            server->sendToClient(client, broadcastMsg);
        }
        
    } else if (!isServerMode && client->isConnected()) {
        // Client mode: send to server
        QString clientMsg = QString("%1:%2").arg(username).arg(message);
        client->sendMessage(clientMsg);
        
        // Add message locally
        createMessageWidget(msg);
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
    if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
        // Shift+Enter: new line
        ui->messageInput->insert("\n");
    } else {
        // Enter: send message
        sendMessage();
    }
}

void ChatWindow::startServer()
{
    if (server->isListening()) {
        server->stopServer();
        ui->actionButton->setText("启动服务器");
        ui->connectionLabel->setText("未连接");
        ui->connectionLabel->setStyleSheet("color: red; font-weight: bold;");
        addSystemMessage("服务器已停止");
    } else {
        server->startServer(12345);
        if (server->isListening()) {
            ui->actionButton->setText("停止服务器");
            ui->connectionLabel->setText("服务器运行中");
            ui->connectionLabel->setStyleSheet("color: green; font-weight: bold;");
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
        ui->connectionLabel->setStyleSheet("color: red; font-weight: bold;");
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

void ChatWindow::disconnectFromServer()
{
    if (isServerMode && server->isListening()) {
        server->stopServer();
        ui->actionButton->setText("启动服务器");
        addSystemMessage("服务器已停止");
    } else if (!isServerMode && client->isConnected()) {
        client->disconnectFromHost();
        ui->actionButton->setText("连接服务器");
        addSystemMessage("已断开与服务器的连接");
    }
    
    ui->connectionLabel->setText("未连接");
    ui->connectionLabel->setStyleSheet("color: red; font-weight: bold;");
}

void ChatWindow::onServerMessage(const QString &sender, const QString &message)
{
    ChatMessage msg(sender, message, false); // false = not my message
    createMessageWidget(msg);
    saveMessageToFile(msg);
}

void ChatWindow::onClientMessage(const QString &message)
{
    int colonIndex = message.indexOf(':');
    if (colonIndex > 0) {
        QString sender = message.left(colonIndex);
        QString content = message.mid(colonIndex + 1);
        
        ChatMessage msg(sender, content, false); // false = not my message
        createMessageWidget(msg);
        saveMessageToFile(msg);
    }
}

void ChatWindow::onClientConnected()
{
    if (isServerMode) {
        addSystemMessage("新客户端已连接");
        ui->connectionLabel->setText("服务器运行中");
        ui->connectionLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->actionButton->setText("断开连接");
        ui->connectionLabel->setText("已连接");
        ui->connectionLabel->setStyleSheet("color: green; font-weight: bold;");
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
        ui->connectionLabel->setStyleSheet("color: red; font-weight: bold;");
        addSystemMessage("与服务器的连接已断开");
    }
}

void ChatWindow::onConnectionError(const QString &error)
{
    addSystemMessage(QString("连接错误：%1").arg(error));
    ui->connectionLabel->setText("连接失败");
    ui->connectionLabel->setStyleSheet("color: red; font-weight: bold;");
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

void ChatWindow::createEmojiPanel()
{
    emojiPanel = new QWidget(this);
    emojiPanel->setWindowFlags(Qt::Popup);
    emojiPanel->setFixedSize(320, 200);
    emojiPanel->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border: 1px solid #e0e0e0;
            border-radius: 8px;
        }
        QPushButton {
            border: none;
            border-radius: 4px;
            font-size: 18px;
            background-color: transparent;
        }
        QPushButton:hover {
            background-color: #f0f0f0;
        }
    )");
    
    QGridLayout *emojiLayout = new QGridLayout(emojiPanel);
    emojiLayout->setSpacing(5);
    
    QStringList emojis = {"😀", "😊", "😂", "😍", "😎", "😭", "😱", "😴",
                          "👍", "👎", "👌", "✌️", "🤝", "👏", "🙏", "💪",
                          "❤️", "💔", "💯", "🔥", "⭐", "✨", "🎉", "🎊"};
    
    int row = 0, col = 0;
    for (const QString &emoji : emojis) {
        QPushButton *btn = new QPushButton(emoji);
        btn->setFixedSize(35, 35);
        connect(btn, &QPushButton::clicked, [this, emoji]() {
            insertEmoji(emoji);
        });
        
        emojiLayout->addWidget(btn, row, col);
        col++;
        if (col >= 8) {
            col = 0;
            row++;
        }
    }
    
    emojiPanel->hide();
}

void ChatWindow::toggleEmojiPanel()
{
    if (emojiPanel->isVisible()) {
        emojiPanel->hide();
    } else {
        QPoint pos = ui->emojiButton->mapToGlobal(QPoint(0, 0));
        pos.setY(pos.y() - emojiPanel->height() - 10);
        emojiPanel->move(pos);
        emojiPanel->show();
    }
}

void ChatWindow::insertEmoji(const QString &emoji)
{
    ui->messageInput->insert(emoji);
    emojiPanel->hide();
    ui->messageInput->setFocus();
}

void ChatWindow::scrollToBottom()
{
    QScrollBar *scrollBar = ui->scrollArea->verticalScrollBar();
    QTimer::singleShot(50, [scrollBar]() {
        scrollBar->setValue(scrollBar->maximum());
    });
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

void ChatWindow::loadMessagesFromFile()
{
    // Implementation for loading previous messages
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

