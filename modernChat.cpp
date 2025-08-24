#include "modernChat.h"

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

// ModernChatWindow Implementation
ModernChatWindow::ModernChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , server(new ChatServer(this))
    , client(new ChatClient(this))
    , statusTimer(new QTimer(this))
    , isServerMode(true)
{
    // Get username
    bool ok;
    username = QInputDialog::getText(this, "设置用户名", "请输入您的用户名:", QLineEdit::Normal, "User", &ok);
    if (!ok || username.isEmpty()) {
        username = "Anonymous";
    }
    
    setWindowTitle(QString("网络聊天程序 - %1").arg(username));
    setMinimumSize(500, 600);
    
    // Set up chat log file
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataDir);
    chatLogFile = appDataDir + "/chat_history.txt";
    
    setupUI();
    setupConnections();
    createEmojiPanel();
    
    // Connect timer
    connect(statusTimer, &QTimer::timeout, this, &ModernChatWindow::updateConnectionStatus);
    statusTimer->start(1000);
    
    addSystemMessage(QString("欢迎使用网络聊天程序！当前用户：%1").arg(username));
    addSystemMessage(QString("本机IP地址：%1").arg(getLocalIP()));
    
    messageInput->setFocus();
}

ModernChatWindow::~ModernChatWindow()
{
    if (server->isListening()) {
        server->stopServer();
    }
    if (client->isConnected()) {
        client->disconnectFromHost();
    }
}

void ModernChatWindow::setupUI()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 10, 15, 10);
    mainLayout->setSpacing(10);
    
    // Connection group
    connectionGroup = new QGroupBox("连接设置");
    connectionGroup->setMaximumHeight(80);
    connectionGroup->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 1px solid #dee2e6;
            border-radius: 8px;
            margin: 5px;
            padding-top: 15px;
            background-color: white;
            color: #495057;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 8px;
            color: #495057;
        }
    )");
    
    QHBoxLayout *connectionLayout = new QHBoxLayout(connectionGroup);
    connectionLayout->setSpacing(10);
    
    connectionLayout->addWidget(new QLabel("模式:"));
    
    modeCombo = new QComboBox;
    modeCombo->addItem("服务器模式");
    modeCombo->addItem("客户端模式");
    modeCombo->setStyleSheet(R"(
        QComboBox {
            background-color: white;
            border: 1px solid #dee2e6;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 14px;
            color: #495057;
            min-width: 120px;
        }
    )");
    connectionLayout->addWidget(modeCombo);
    
    connectionLayout->addWidget(new QLabel("主机:"));
    
    hostInput = new QLineEdit("127.0.0.1");
    hostInput->setPlaceholderText("输入服务器IP地址");
    hostInput->setEnabled(false);
    hostInput->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #dee2e6;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 14px;
            background-color: white;
            color: #495057;
        }
        QLineEdit:focus {
            border-color: #667eea;
        }
    )");
    connectionLayout->addWidget(hostInput);
    
    actionButton = new QPushButton("启动服务器");
    actionButton->setStyleSheet(R"(
        QPushButton {
            background-color: #28a745;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #218838;
        }
    )");
    connectionLayout->addWidget(actionButton);
    
    connectionLabel = new QLabel("未连接");
    connectionLabel->setStyleSheet(R"(
        QLabel {
            font-weight: bold;
            padding: 6px 12px;
            border-radius: 4px;
            background-color: #f8d7da;
            color: #721c24;
        }
    )");
    connectionLayout->addWidget(connectionLabel);
    
    connectionLayout->addStretch();
    
    mainLayout->addWidget(connectionGroup);
    
    // Chat area
    scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background-color: #f8f9fa;
            border: 1px solid #dee2e6;
            border-radius: 12px;
        }
    )");
    
    scrollContent = new QWidget;
    scrollContent->setStyleSheet("background-color: #f8f9fa;");
    
    messagesLayout = new QVBoxLayout(scrollContent);
    messagesLayout->setContentsMargins(0, 10, 0, 10);
    messagesLayout->setSpacing(5);
    messagesLayout->addStretch(); // Keep messages at bottom initially
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
    
    // Input container
    inputContainer = new QWidget;
    inputContainer->setMaximumHeight(70);
    inputContainer->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border: 1px solid #dee2e6;
            border-radius: 12px;
        }
    )");
    
    QHBoxLayout *inputLayout = new QHBoxLayout(inputContainer);
    inputLayout->setContentsMargins(15, 15, 15, 15);
    inputLayout->setSpacing(10);
    
    emojiButton = new QPushButton("😀");
    emojiButton->setFixedSize(40, 40);
    emojiButton->setStyleSheet(R"(
        QPushButton {
            background-color: #f8f9fa;
            border: 2px solid #dee2e6;
            border-radius: 20px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #e9ecef;
        }
    )");
    inputLayout->addWidget(emojiButton);
    
    messageInput = new QLineEdit;
    messageInput->setPlaceholderText("输入消息...");
    messageInput->setStyleSheet(R"(
        QLineEdit {
            border: 2px solid #e9ecef;
            border-radius: 20px;
            padding: 12px 20px;
            font-size: 14px;
            background-color: white;
            color: #333;
        }
        QLineEdit:focus {
            border-color: #667eea;
            outline: none;
        }
    )");
    inputLayout->addWidget(messageInput);
    
    sendButton = new QPushButton("发送");
    sendButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #667eea, stop:1 #764ba2);
            color: white;
            border: none;
            border-radius: 20px;
            padding: 12px 20px;
            font-size: 14px;
            font-weight: bold;
            min-width: 60px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #5a67d8, stop:1 #6b46c1);
        }
    )");
    inputLayout->addWidget(sendButton);
    
    mainLayout->addWidget(inputContainer);
    
    // Set overall style
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f8f9fa;
        }
        QLabel {
            color: #495057;
        }
    )");
}

void ModernChatWindow::setupConnections()
{
    // UI connections
    connect(sendButton, &QPushButton::clicked, this, &ModernChatWindow::sendMessage);
    connect(messageInput, &QLineEdit::returnPressed, this, &ModernChatWindow::sendMessage);
    connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModernChatWindow::onModeChanged);
    connect(actionButton, &QPushButton::clicked, [this]() {
        if (isServerMode) {
            startServer();
        } else {
            connectToServer();
        }
    });
    connect(emojiButton, &QPushButton::clicked, this, &ModernChatWindow::toggleEmojiPanel);
    
    // Network connections
    connect(server, &ChatServer::messageReceived, this, &ModernChatWindow::onServerMessage);
    connect(server, &ChatServer::clientConnected, this, &ModernChatWindow::onClientConnected);
    connect(server, &ChatServer::clientDisconnected, this, &ModernChatWindow::onClientDisconnected);
    
    connect(client, &ChatClient::connected, this, &ModernChatWindow::onClientConnected);
    connect(client, &ChatClient::disconnected, this, &ModernChatWindow::onClientDisconnected);
    connect(client, &ChatClient::messageReceived, this, &ModernChatWindow::onClientMessage);
    connect(client, &ChatClient::errorOccurred, this, &ModernChatWindow::onConnectionError);
}

void ModernChatWindow::onModeChanged(int index)
{
    isServerMode = (index == 0);
    hostInput->setEnabled(!isServerMode);
    actionButton->setText(isServerMode ? "启动服务器" : "连接服务器");
}

void ModernChatWindow::addMessageBubble(const ChatMessage &msg)
{
    messages.append(msg);
    
    // Create chat bubble widget
    ChatBubble *bubble = new ChatBubble(msg.message, msg.sender, msg.timestamp, msg.isMyMessage);
    
    // Insert before the stretch (last item)
    int index = messagesLayout->count() - 1;
    messagesLayout->insertWidget(index, bubble);
    
    scrollToBottom();
    saveMessageToFile(msg);
}

void ModernChatWindow::addSystemMessage(const QString &message)
{
    // Create system message widget
    QWidget *systemWidget = new QWidget;
    QHBoxLayout *systemLayout = new QHBoxLayout(systemWidget);
    systemLayout->setContentsMargins(20, 10, 20, 10);
    
    QLabel *systemLabel = new QLabel(message);
    systemLabel->setAlignment(Qt::AlignCenter);
    systemLabel->setWordWrap(true);
    systemLabel->setStyleSheet(R"(
        QLabel {
            background-color: rgba(108, 117, 125, 0.1);
            color: #6c757d;
            padding: 8px 16px;
            border-radius: 12px;
            font-size: 12px;
            font-style: italic;
        }
    )");
    
    systemLayout->addStretch();
    systemLayout->addWidget(systemLabel);
    systemLayout->addStretch();
    
    int index = messagesLayout->count() - 1;
    messagesLayout->insertWidget(index, systemWidget);
    
    scrollToBottom();
}

void ModernChatWindow::sendMessage()
{
    QString message = messageInput->text().trimmed();
    if (message.isEmpty()) {
        return;
    }
    
    // Create message object
    ChatMessage msg(username, message, true); // true = my message
    
    if (isServerMode && server->isListening()) {
        // Server mode: add message locally and broadcast
        addMessageBubble(msg);
        
        // Broadcast to all clients
        QString broadcastMsg = QString("%1:%2").arg(username).arg(message);
        server->broadcastMessage(broadcastMsg);
        
    } else if (!isServerMode && client->isConnected()) {
        // Client mode: send to server
        QString clientMsg = QString("%1:%2").arg(username).arg(message);
        client->sendMessage(clientMsg);
        
        // Add message locally
        addMessageBubble(msg);
    } else {
        addSystemMessage("错误：未连接到网络");
        return;
    }
    
    messageInput->clear();
    messageInput->setFocus();
}

void ModernChatWindow::startServer()
{
    if (server->isListening()) {
        server->stopServer();
        actionButton->setText("启动服务器");
        connectionLabel->setText("未连接");
        connectionLabel->setStyleSheet("background-color: #f8d7da; color: #721c24;");
        addSystemMessage("服务器已停止");
    } else {
        server->startServer(12345);
        if (server->isListening()) {
            actionButton->setText("停止服务器");
            connectionLabel->setText("服务器运行中");
            connectionLabel->setStyleSheet("background-color: #d4edda; color: #155724;");
            addSystemMessage(QString("服务器启动成功，监听端口：12345\n本机IP：%1").arg(getLocalIP()));
        }
    }
}

void ModernChatWindow::connectToServer()
{
    if (client->isConnected()) {
        client->disconnectFromHost();
        actionButton->setText("连接服务器");
        connectionLabel->setText("未连接");
        connectionLabel->setStyleSheet("background-color: #f8d7da; color: #721c24;");
    } else {
        QString host = hostInput->text().trimmed();
        if (host.isEmpty()) {
            QMessageBox::warning(this, "错误", "请输入服务器IP地址");
            return;
        }
        
        client->connectToHost(host, 12345);
        addSystemMessage(QString("正在连接到服务器：%1:12345").arg(host));
    }
}

void ModernChatWindow::onServerMessage(const QString &sender, const QString &message)
{
    // Don't display messages from myself if I'm the server
    if (sender != username) {
        ChatMessage msg(sender, message, false);
        addMessageBubble(msg);
    }
}

void ModernChatWindow::onClientMessage(const QString &message)
{
    int colonIndex = message.indexOf(':');
    if (colonIndex > 0) {
        QString sender = message.left(colonIndex);
        QString content = message.mid(colonIndex + 1);
        
        // Don't display messages from myself if I'm a client
        if (sender != username) {
            ChatMessage msg(sender, content, false);
            addMessageBubble(msg);
        }
    }
}

void ModernChatWindow::onClientConnected()
{
    if (isServerMode) {
        addSystemMessage("新客户端已连接");
        connectionLabel->setText("服务器运行中");
        connectionLabel->setStyleSheet("background-color: #d4edda; color: #155724;");
    } else {
        actionButton->setText("断开连接");
        connectionLabel->setText("已连接");
        connectionLabel->setStyleSheet("background-color: #d4edda; color: #155724;");
        addSystemMessage("成功连接到服务器");
    }
}

void ModernChatWindow::onClientDisconnected()
{
    if (isServerMode) {
        addSystemMessage("客户端已断开连接");
    } else {
        actionButton->setText("连接服务器");
        connectionLabel->setText("未连接");
        connectionLabel->setStyleSheet("background-color: #f8d7da; color: #721c24;");
        addSystemMessage("与服务器的连接已断开");
    }
}

void ModernChatWindow::onConnectionError(const QString &error)
{
    addSystemMessage(QString("连接错误：%1").arg(error));
    connectionLabel->setText("连接失败");
    connectionLabel->setStyleSheet("background-color: #f8d7da; color: #721c24;");
}

void ModernChatWindow::updateConnectionStatus()
{
    QString status;
    if (isServerMode && server->isListening()) {
        status = QString("服务器模式 - 端口：12345 - 用户：%1").arg(username);
    } else if (!isServerMode && client->isConnected()) {
        status = QString("客户端模式 - 连接到：%1 - 用户：%2").arg(hostInput->text()).arg(username);
    } else {
        status = QString("离线模式 - 用户：%1").arg(username);
    }
    
    statusBar()->showMessage(status);
}

void ModernChatWindow::scrollToBottom()
{
    QScrollBar *scrollBar = scrollArea->verticalScrollBar();
    QTimer::singleShot(50, [scrollBar]() {
        scrollBar->setValue(scrollBar->maximum());
    });
}

void ModernChatWindow::saveMessageToFile(const ChatMessage &msg)
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

void ModernChatWindow::createEmojiPanel()
{
    emojiPanel = new QWidget(this);
    emojiPanel->setWindowFlags(Qt::Popup);
    emojiPanel->setFixedSize(320, 200);
    emojiPanel->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border: 1px solid #dee2e6;
            border-radius: 8px;
        }
        QPushButton {
            border: none;
            border-radius: 4px;
            font-size: 18px;
            background-color: transparent;
        }
        QPushButton:hover {
            background-color: #f8f9fa;
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

void ModernChatWindow::toggleEmojiPanel()
{
    if (emojiPanel->isVisible()) {
        emojiPanel->hide();
    } else {
        QPoint pos = emojiButton->mapToGlobal(QPoint(0, 0));
        pos.setY(pos.y() - emojiPanel->height() - 10);
        emojiPanel->move(pos);
        emojiPanel->show();
    }
}

void ModernChatWindow::insertEmoji(const QString &emoji)
{
    messageInput->insert(emoji);
    emojiPanel->hide();
    messageInput->setFocus();
}

QString ModernChatWindow::getLocalIP()
{
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost) {
            return address.toString();
        }
    }
    return "127.0.0.1";
}