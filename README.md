作业2：网络聊天程序设计与实现
1.1 功能需求分析
网络聊天程序是基于TCP协议的实时通信应用，需要实现多用户之间的消息传输和交互功能。系统的核心需求包括网络通信、用户界面、消息管理等多个方面。
网络通信功能是系统的基础，需要实现稳定可靠的客户端-服务器通信架构。服务器端需要能够监听指定端口，接受多个客户端的连接请求，并负责消息的接收和转发。客户端需要能够连接到服务器，发送消息给服务器，并接收来自其他客户端的消息。系统需要处理网络连接的建立、维护和断开，确保通信的稳定性。
用户界面需要提供直观友好的聊天体验，类似于现代即时通讯软件的界面设计。聊天消息需要以气泡形式显示，自己发送的消息显示在右侧，其他用户的消息显示在左侧。每条消息需要显示发送者姓名、消息内容、发送时间等信息。界面需要支持消息的滚动查看，新消息自动滚动到可视区域。
消息管理功能包括消息的发送、接收、存储和加载。系统需要支持文本消息的实时传输，确保消息的及时性和准确性。消息需要支持本地存储，用户可以查看历史聊天记录。系统还需要提供表情符号支持，增强聊天的趣味性和表达能力。
系统功能模块图
功能模块	主要组件	实现技术
网络通信	ChatServer, ChatClient	QTcpServer, QTcpSocket
消息显示	ChatBubble, ChatWindow	QWidget, QScrollArea
用户界面	连接面板, 输入区域	Qt UI文件, QGroupBox
数据存储	消息记录, 文件操作	QFile, QTextStream
1.2 系统设计步骤
系统架构采用经典的客户端-服务器模式，使用TCP协议进行网络通信。服务器端使用QTcpServer类监听端口，管理客户端连接列表，负责消息的广播转发。客户端使用QTcpSocket类连接服务器，发送和接收消息。这种架构确保了通信的可靠性和扩展性。
界面设计采用现代化的聊天应用风格，重点突出消息气泡的视觉效果。主界面分为三个区域：顶部的连接控制面板、中间的消息显示区域、底部的消息输入区域。消息显示使用自定义的ChatBubble组件，每条消息都是独立的圆角气泡，支持左右对齐和不同的颜色样式。
数据流设计确保消息传输的效率和准确性。用户发送消息时，客户端将消息发送给服务器，服务器接收后广播给所有连接的客户端。接收端获取消息后解析发送者和内容，在界面上创建相应的消息气泡。所有消息都会保存到本地文件，支持历史记录的查看和恢复。
多线程设计保证了用户界面的响应性，网络I/O操作不会阻塞界面更新。Qt的信号-槽机制提供了优雅的事件处理方式，确保网络事件能够及时响应并更新界面。错误处理机制能够捕获网络异常，向用户提供友好的错误提示。
时序图
服务器启动 -> 监听端口12345
客户端启动 -> 连接服务器
服务器接受连接 -> 添加到客户端列表
用户A发送消息 -> 客户端发送到服务器
服务器接收消息 -> 广播给所有客户端
其他客户端接收 -> 界面显示消息气泡
消息保存到本地文件 -> 完成消息传输
1.3 系统运行界面
聊天程序主界面采用现代化的设计风格，整体背景为淡灰色，营造舒适的视觉环境。窗口尺寸为900x700像素，提供充足的聊天显示空间。
顶部连接设置面板采用白色背景的分组框设计，包含模式选择下拉框、IP地址输入框、连接按钮和状态显示标签。模式选择支持服务器模式和客户端模式切换，IP地址输入框在客户端模式下可用，连接按钮根据当前状态显示不同的文字和功能。
中间聊天显示区域是程序的核心部分，使用白色背景的滚动区域显示聊天内容。消息以气泡形式展现，自己发送的消息显示在右侧，使用紫色渐变的圆角气泡；其他用户的消息显示在左侧，使用浅灰色的圆角气泡。每个气泡都包含消息内容、发送时间，左侧消息还显示发送者姓名。
底部消息输入区域采用白色背景，包含表情按钮、消息输入框和发送按钮。输入框使用圆角设计，支持多行文本输入。表情按钮点击后弹出表情选择面板，提供常用的emoji表情。发送按钮使用与登录界面相同的紫色渐变设计，保持视觉一致性。
界面还包含状态栏显示当前连接状态和用户信息，菜单栏提供文件操作和帮助功能。整体界面布局合理，操作直观，符合现代聊天应用的使用习惯。
1.4 系统源代码
主程序文件
main.cpp负责程序的初始化和主窗口的创建。
#include <QApplication>
#include "modernChat.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    ModernChatWindow window;
    window.show();
    
    return app.exec();
}
聊天气泡组件
ChatBubble类实现了自定义的消息气泡组件，支持不同样式的消息显示。
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
网络服务器实现
ChatServer类实现了TCP服务器功能，支持多客户端连接和消息广播。
void ChatServer::startServer(quint16 port)
{
    if (server->listen(QHostAddress::Any, port)) {
        qDebug() << "Server started on port" << port;
    }
}

void ChatServer::onNewConnection()
{
    QTcpSocket *client = server->nextPendingConnection();
    clients.append(client);
    
    connect(client, &QTcpSocket::disconnected, this, &ChatServer::onClientDisconnected);
    connect(client, &QTcpSocket::readyRead, this, &ChatServer::onDataReceived);
    
    emit clientConnected(client->peerAddress().toString());
}

void ChatServer::broadcastMessage(const QString &message, QTcpSocket *excludeClient)
{
    for (auto client : clients) {
        if (client != excludeClient) {
            sendToClient(client, message);
        }
    }
}
消息处理逻辑
消息发送和接收的核心逻辑确保了通信的准确性和界面的正确更新。
void ModernChatWindow::sendMessage()
{
    QString message = messageInput->text().trimmed();
    if (message.isEmpty()) {
        return;
    }
    
    ChatMessage msg(username, message, true);
    
    if (isServerMode && server->isListening()) {
        addMessageBubble(msg);
        QString broadcastMsg = QString("%1:%2").arg(username).arg(message);
        server->broadcastMessage(broadcastMsg);
    } else if (!isServerMode && client->isConnected()) {
        QString clientMsg = QString("%1:%2").arg(username).arg(message);
        client->sendMessage(clientMsg);
        addMessageBubble(msg);
    }
    
    messageInput->clear();
    messageInput->setFocus();
}
聊天记录格式
聊天记录以文本格式保存，便于查看和处理。
[2026-03-15 10:30:25] Alice: 大家好！
[2026-03-15 10:30:35] Hanser: 你好Alice，欢迎加入聊天室
[2026-03-15 10:31:10] Alice: 这个聊天程序很不错
[2026-03-15 10:31:20] 系统: 用户Otto已加入聊天室
1.5 系统代码网址
网络聊天程序的完整源代码已上传至以下平台：
GitHub项目地址：https://github.com/Cyanlament/simple-chat
1.6 总结与展望
网络聊天程序成功实现了基于TCP协议的实时消息传输功能，采用现代化的聊天气泡界面设计，提供了良好的用户体验。系统支持服务器-客户端架构，能够处理多用户并发聊天，具备消息持久化存储功能。
技术实现方面，项目使用了Qt网络编程、自定义Widget开发、信号槽机制、多线程处理等多种技术。界面设计采用了现代化的扁平化风格，使用圆角元素、渐变色彩、阴影效果等视觉元素，营造了专业的应用体验。
开发环境配置包括Qt开发框架、C++编译器、qmake构建工具等。系统经过充分测试，在Windows、macOS、Linux等平台上都能稳定运行。代码结构清晰，遵循良好的编程规范，便于代码维护和功能扩展。
项目的创新点在于实现了真正的聊天气泡界面，区别于传统的文本显示方式。消息气泡支持左右对齐、不同颜色、动画效果等特性，提供了接近商业聊天软件的用户体验。
未来的改进方向可以考虑：增加文件传输功能，支持图片、文档等多媒体消息；实现用户在线状态显示，显示用户的登录、离线状态；添加聊天室功能，支持多个聊天频道；集成消息加密功能，保护用户隐私安全；实现消息撤回、@提醒等高级聊天功能；优化网络性能，支持更大规模的并发用户。
