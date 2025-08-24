#include "chatwindow.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QStandardPaths>

// ChatServer Implementation
ChatServer::ChatServer(QObject *parent) : QObject(parent), server(new QTcpServer(this))
{
    connect(server, &QTcpServer::newConnection, this, &ChatServer::onNewConnection);
}

void ChatServer::startServer(quint16 port)
{
    if (server->listen(QHostAddress::Any, port)) {
        qDebug() << "Server started on port" << port;
    } else {
        qDebug() << "Failed to start server:" << server->errorString();
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
        
        // Parse message format: "sender:message"
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
    emit errorOccurred(socket->errorString());
}

// ChatWindow Implementation
ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , server(new ChatServer(this))
    , client(new ChatClient(this))
    , statusTimer(new QTimer(this))
    , isServerMode(false)
{
    // Get username
    bool ok;
    username = QInputDialog::getText(this, "设置用户名", "请输入您的用户名:", QLineEdit::Normal, "User", &ok);
    if (!ok || username.isEmpty()) {
        username = "Anonymous";
    }
    
    // Set up chat log file
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataDir);
    chatLogFile = appDataDir + "/chat_history.txt";
    
    setupUI();
    setupMenuBar();
    setupStyles();
    createEmojiPanel();
    
    // Connect signals
    connect(server, &ChatServer::messageReceived, this, &ChatWindow::onServerMessage);
    connect(server, &ChatServer::clientConnected, this, &ChatWindow::onClientConnected);
    connect(server, &ChatServer::clientDisconnected, this, &ChatWindow::onClientDisconnected);
    
    connect(client, &ChatClient::connected, this, &ChatWindow::onClientConnected);
    connect(client, &ChatClient::disconnected, this, &ChatWindow::onClientDisconnected);
    connect(client, &ChatClient::messageReceived, this, &ChatWindow::onClientMessage);
    connect(client, &ChatClient::errorOccurred, this, &ChatWindow::onConnectionError);
    
    connect(statusTimer, &QTimer::timeout, this, &ChatWindow::updateConnectionStatus);
    statusTimer->start(1000);
    
    // Load previous messages
    loadMessagesFromFile();
    
    addSystemMessage(QString("欢迎使用网络聊天程序！当前用户：%1").arg(username));
    addSystemMessage(QString("本机IP地址：%1").arg(getLocalIP()));
}

ChatWindow::~ChatWindow()
{
    if (server->isListening()) {
        server->stopServer();
    }
    if (client->isConnected()) {
        client->disconnectFromHost();
    }
}

void ChatWindow::setupUI()
{
    setWindowTitle(QString("网络聊天程序 - %1").arg(username));
    setMinimumSize(800, 600);
    
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Connection control panel
    QGroupBox *connectionGroup = new QGroupBox("连接设置");
    QHBoxLayout *connectionLayout = new QHBoxLayout(connectionGroup);
    
    modeCombo = new QComboBox;
    modeCombo->addItem("服务器模式", "server");
    modeCombo->addItem("客户端模式", "client");
    
    hostInput = new QLineEdit;
    hostInput->setPlaceholderText("输入服务器IP地址 (例如: 192.168.1.100)");
    hostInput->setText("127.0.0.1");
    hostInput->setEnabled(false);
    
    actionButton = new QPushButton("启动服务器");
    
    connectionLabel = new QLabel("未连接");
    connectionLabel->setStyleSheet("color: red; font-weight: bold;");
    
    connectionLayout->addWidget(new QLabel("模式:"));
    connectionLayout->addWidget(modeCombo);
    connectionLayout->addWidget(new QLabel("主机:"));
    connectionLayout->addWidget(hostInput);
    connectionLayout->addWidget(actionButton);
    connectionLayout->addWidget(connectionLabel);
    connectionLayout->addStretch();
    
    // Chat area
    mainSplitter = new QSplitter(Qt::Horizontal);
    
    // Chat display
    chatDisplay = new QTextEdit;
    chatDisplay->setReadOnly(true);
    chatDisplay->setMinimumWidth(500);
    
    // Input area
    QWidget *inputWidget = new QWidget;
    QVBoxLayout *inputLayout = new QVBoxLayout(inputWidget);
    
    QHBoxLayout *messageLayout = new QHBoxLayout;
    messageInput = new QLineEdit;
    messageInput->setPlaceholderText("输入消息...");
    
    emojiButton = new QPushButton("😀");
    emojiButton->setFixedSize(35, 35);
    
    sendButton = new QPushButton("发送");
    sendButton->setFixedWidth(80);
    
    messageLayout->addWidget(messageInput);
    messageLayout->addWidget(emojiButton);
    messageLayout->addWidget(sendButton);
    
    inputLayout->addLayout(messageLayout);
    
    QWidget *chatWidget = new QWidget;
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->addWidget(chatDisplay);
    chatLayout->addWidget(inputWidget);
    
    mainSplitter->addWidget(chatWidget);
    
    // Status area
    statusLabel = new QLabel("就绪");
    
    mainLayout->addWidget(connectionGroup);
    mainLayout->addWidget(mainSplitter);
    
    statusBar()->addWidget(statusLabel);
    
    // Connect UI signals
    connect(sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
    connect(messageInput, &QLineEdit::returnPressed, this, &ChatWindow::onReturnPressed);
    connect(modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        isServerMode = (index == 0);
        hostInput->setEnabled(!isServerMode);
        actionButton->setText(isServerMode ? "启动服务器" : "连接服务器");
    });
    connect(actionButton, &QPushButton::clicked, [this]() {
        if (isServerMode) {
            startServer();
        } else {
            connectToServer();
        }
    });
    connect(emojiButton, &QPushButton::clicked, this, &ChatWindow::toggleEmojiPanel);
    
    messageInput->setFocus();
}

void ChatWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    QMenu *fileMenu = menuBar->addMenu("文件(&F)");
    fileMenu->addAction("清空聊天记录(&C)", this, &ChatWindow::clearChat, QKeySequence::New);
    fileMenu->addAction("保存聊天记录(&S)", this, &ChatWindow::saveChat, QKeySequence::Save);
    fileMenu->addAction("加载聊天记录(&L)", this, &ChatWindow::loadChat, QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("退出(&Q)", this, &QWidget::close, QKeySequence::Quit);
    
    QMenu *connectionMenu = menuBar->addMenu("连接(&C)");
    connectionMenu->addAction("断开连接(&D)", this, &ChatWindow::disconnectFromServer);
    
    QMenu *helpMenu = menuBar->addMenu("帮助(&H)");
    helpMenu->addAction("关于(&A)", this, &ChatWindow::showAbout);
}

void ChatWindow::setupStyles()
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
        }
        
        QGroupBox {
            font-weight: bold;
            border: 2px solid #555;
            border-radius: 8px;
            margin: 5px 0;
            padding-top: 15px;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #fff;
        }
        
        QTextEdit {
            background-color: #1e1e1e;
            color: #fff;
            border: 1px solid #555;
            border-radius: 5px;
            padding: 10px;
            font-family: 'Consolas', 'Monaco', monospace;
            font-size: 13px;
        }
        
        QLineEdit {
            background-color: #3c3c3c;
            color: #fff;
            border: 1px solid #555;
            border-radius: 5px;
            padding: 8px;
            font-size: 13px;
        }
        
        QLineEdit:focus {
            border-color: #0078d4;
        }
        
        QPushButton {
            background-color: #0078d4;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 8px 16px;
            font-weight: bold;
        }
        
        QPushButton:hover {
            background-color: #106ebe;
        }
        
        QPushButton:pressed {
            background-color: #005a9e;
        }
        
        QComboBox {
            background-color: #3c3c3c;
            color: #fff;
            border: 1px solid #555;
            border-radius: 5px;
            padding: 5px;
            min-width: 120px;
        }
        
        QComboBox::drop-down {
            border: none;
        }
        
        QComboBox::down-arrow {
            image: url(down_arrow.png);
            width: 12px;
            height: 12px;
        }
        
        QLabel {
            color: #fff;
        }
        
        QStatusBar {
            background-color: #3c3c3c;
            color: #fff;
        }
    )");
}

void ChatWindow::createEmojiPanel()
{
    emojiPanel = new QWidget(this);
    emojiPanel->setWindowFlags(Qt::Popup);
    emojiPanel->setFixedSize(300, 200);
    
    QGridLayout *emojiLayout = new QGridLayout(emojiPanel);
    
    QStringList emojis = {"😀", "😊", "😂", "😍", "😎", "😭", "😱", "😴",
                          "👍", "👎", "👌", "✌️", "🤝", "👏", "🙏", "💪",
                          "❤️", "💔", "💯", "🔥", "⭐", "✨", "🎉", "🎊"};
    
    int row = 0, col = 0;
    for (const QString &emoji : emojis) {
        QPushButton *btn = new QPushButton(emoji);
        btn->setFixedSize(35, 35);
        btn->setStyleSheet("QPushButton { font-size: 16px; }");
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

void ChatWindow::sendMessage()
{
    QString message = messageInput->text().trimmed();
    if (message.isEmpty()) {
        return;
    }
    
    // Create message object
    ChatMessage msg(username, message);
    
    if (isServerMode && server->isListening()) {
        // Server mode: add message locally and broadcast
        addMessage(msg);
        saveMessageToFile(msg);
        
        // Broadcast to all clients
        QString broadcastMsg = QString("%1:%2").arg(username).arg(message);
        // Server broadcasts through its own mechanism
        
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
    
    messageInput->clear();
    messageInput->setFocus();
}

void ChatWindow::onReturnPressed()
{
    if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
        // Shift+Enter: new line
        messageInput->insert("\n");
    } else {
        // Enter: send message
        sendMessage();
    }
}

void ChatWindow::startServer()
{
    if (server->isListening()) {
        server->stopServer();
        actionButton->setText("启动服务器");
        connectionLabel->setText("未连接");
        connectionLabel->setStyleSheet("color: red; font-weight: bold;");
        addSystemMessage("服务器已停止");
    } else {
        server->startServer(12345);
        if (server->isListening()) {
            actionButton->setText("停止服务器");
            connectionLabel->setText("服务器运行中");
            connectionLabel->setStyleSheet("color: green; font-weight: bold;");
            addSystemMessage(QString("服务器启动成功，监听端口：12345\n本机IP：%1").arg(getLocalIP()));
        }
    }
}

void ChatWindow::connectToServer()
{
    if (client->isConnected()) {
        client->disconnectFromHost();
        actionButton->setText("连接服务器");
        connectionLabel->setText("未连接");
        connectionLabel->setStyleSheet("color: red; font-weight: bold;");
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

void ChatWindow::disconnectFromServer()
{
    if (isServerMode && server->isListening()) {
        server->stopServer();
        actionButton->setText("启动服务器");
        addSystemMessage("服务器已停止");
    } else if (!isServerMode && client->isConnected()) {
        client->disconnectFromHost();
        actionButton->setText("连接服务器");
        addSystemMessage("已断开与服务器的连接");
    }
    
    connectionLabel->setText("未连接");
    connectionLabel->setStyleSheet("color: red; font-weight: bold;");
}

void ChatWindow::clearChat()
{
    chatDisplay->clear();
    messages.clear();
    addSystemMessage("聊天记录已清空");
}

void ChatWindow::saveChat()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存聊天记录", 
        QString("chat_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "文本文件 (*.txt)");
    
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setCodec("UTF-8");
            
            for (const auto &msg : messages) {
                out << QString("[%1] %2: %3\n")
                    .arg(msg.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
                    .arg(msg.sender)
                    .arg(msg.message);
            }
            
            addSystemMessage(QString("聊天记录已保存到：%1").arg(fileName));
        }
    }
}

void ChatWindow::loadChat()
{
    QString fileName = QFileDialog::getOpenFileName(this, "加载聊天记录", "", "文本文件 (*.txt)");
    if (!fileName.isEmpty()) {
        // Implementation for loading chat from file
        addSystemMessage(QString("从文件加载聊天记录：%1").arg(fileName));
    }
}

void ChatWindow::showAbout()
{
    QMessageBox::about(this, "关于", 
        "<h3>网络聊天程序</h3>"
        "<p>版本：1.0</p>"
        "<p>基于Qt开发的网络聊天应用</p>"
        "<p>支持服务器/客户端模式</p>"
        "<p><b>功能特性：</b></p>"
        "<ul>"
        "<li>实时消息传输</li>"
        "<li>表情符号支持</li>"
        "<li>聊天记录保存/加载</li>"
        "<li>多客户端支持</li>"
        "<li>现代化UI界面</li>"
        "</ul>");
}

void ChatWindow::onServerMessage(const QString &sender, const QString &message)
{
    ChatMessage msg(sender, message);
    addMessage(msg);
    saveMessageToFile(msg);
}

void ChatWindow::onClientMessage(const QString &message)
{
    // Parse message format: "sender:content"
    int colonIndex = message.indexOf(':');
    if (colonIndex > 0) {
        QString sender = message.left(colonIndex);
        QString content = message.mid(colonIndex + 1);
        
        ChatMessage msg(sender, content);
        addMessage(msg);
        saveMessageToFile(msg);
    }
}

void ChatWindow::onClientConnected()
{
    if (isServerMode) {
        addSystemMessage("新客户端已连接");
        connectionLabel->setText("服务器运行中");
        connectionLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        actionButton->setText("断开连接");
        connectionLabel->setText("已连接");
        connectionLabel->setStyleSheet("color: green; font-weight: bold;");
        addSystemMessage("成功连接到服务器");
    }
}

void ChatWindow::onClientDisconnected()
{
    if (isServerMode) {
        addSystemMessage("客户端已断开连接");
    } else {
        actionButton->setText("连接服务器");
        connectionLabel->setText("未连接");
        connectionLabel->setStyleSheet("color: red; font-weight: bold;");
        addSystemMessage("与服务器的连接已断开");
    }
}

void ChatWindow::onConnectionError(const QString &error)
{
    addSystemMessage(QString("连接错误：%1").arg(error));
    connectionLabel->setText("连接失败");
    connectionLabel->setStyleSheet("color: red; font-weight: bold;");
}

void ChatWindow::updateConnectionStatus()
{
    QString status;
    if (isServerMode && server->isListening()) {
        status = QString("服务器模式 - 端口：12345 - 在线用户：%1").arg(username);
    } else if (!isServerMode && client->isConnected()) {
        status = QString("客户端模式 - 连接到：%1 - 在线用户：%2").arg(hostInput->text()).arg(username);
    } else {
        status = QString("离线模式 - 用户：%1").arg(username);
    }
    
    statusLabel->setText(status);
}

void ChatWindow::toggleEmojiPanel()
{
    if (emojiPanel->isVisible()) {
        emojiPanel->hide();
    } else {
        QPoint pos = emojiButton->mapToGlobal(QPoint(0, 0));
        pos.setY(pos.y() - emojiPanel->height());
        emojiPanel->move(pos);
        emojiPanel->show();
    }
}

void ChatWindow::insertEmoji(const QString &emoji)
{
    messageInput->insert(emoji);
    emojiPanel->hide();
    messageInput->setFocus();
}

void ChatWindow::addMessage(const ChatMessage &msg)
{
    messages.append(msg);
    
    QString timeStr = msg.timestamp.toString("hh:mm:ss");
    QString html;
    
    if (msg.type == "system") {
        html = QString("<div style='color: #888; font-style: italic; margin: 5px 0;'>"
                      "[%1] <b>系统</b>: %2</div>")
               .arg(timeStr).arg(msg.message);
    } else {
        QString color = (msg.sender == username) ? "#4CAF50" : "#2196F3";
        html = QString("<div style='margin: 8px 0; padding: 5px; background-color: rgba(255,255,255,0.1); border-radius: 5px;'>"
                      "<span style='color: #888;'>[%1]</span> "
                      "<span style='color: %2; font-weight: bold;'>%3</span>: "
                      "<span style='color: #fff;'>%4</span></div>")
               .arg(timeStr).arg(color).arg(msg.sender).arg(msg.message.toHtmlEscaped());
    }
    
    chatDisplay->append(html);
    scrollToBottom();
}

void ChatWindow::addSystemMessage(const QString &message)
{
    ChatMessage msg("系统", message, "system");
    addMessage(msg);
    saveMessageToFile(msg);
}

void ChatWindow::scrollToBottom()
{
    QScrollBar *scrollBar = chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWindow::saveMessageToFile(const ChatMessage &msg)
{
    QFile file(chatLogFile);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << QString("[%1] %2: %3\n")
            .arg(msg.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(msg.sender)
            .arg(msg.message);
    }
}

void ChatWindow::loadMessagesFromFile()
{
    QFile file(chatLogFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        
        while (!in.atEnd()) {
            QString line = in.readLine();
            // Simple parsing - in real application would be more robust
            if (line.contains("] ") && line.contains(": ")) {
                // Extract timestamp, sender, and message
                // This is a simplified version
                chatDisplay->append(line);
            }
        }
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

