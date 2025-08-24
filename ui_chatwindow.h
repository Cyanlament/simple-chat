/********************************************************************************
** Form generated from reading UI file 'chatwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.16
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_H
#define UI_CHATWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *connectionGroupBox;
    QHBoxLayout *connectionLayout;
    QLabel *modeLabel;
    QComboBox *modeCombo;
    QLabel *hostLabel;
    QLineEdit *hostInput;
    QPushButton *actionButton;
    QLabel *connectionLabel;
    QSpacerItem *horizontalSpacer;
    QWidget *chatContainer;
    QVBoxLayout *chatLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *messagesLayout;
    QSpacerItem *messagesSpacer;
    QWidget *inputContainer;
    QHBoxLayout *inputLayout;
    QPushButton *emojiButton;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ChatWindow)
    {
        if (ChatWindow->objectName().isEmpty())
            ChatWindow->setObjectName(QString::fromUtf8("ChatWindow"));
        ChatWindow->resize(900, 700);
        ChatWindow->setStyleSheet(QString::fromUtf8("QMainWindow {\n"
"    background-color: #f5f5f5;\n"
"}\n"
"\n"
"QWidget#centralWidget {\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QWidget#chatContainer {\n"
"    background-color: #ffffff;\n"
"    border-radius: 12px;\n"
"}\n"
"\n"
"QScrollArea {\n"
"    border: none;\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QScrollArea QWidget {\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QWidget#messageItem {\n"
"    background-color: transparent;\n"
"    margin: 5px 0;\n"
"}\n"
"\n"
"QLabel#messageText {\n"
"    background-color: #e3f2fd;\n"
"    color: #1976d2;\n"
"    padding: 12px 16px;\n"
"    border-radius: 18px;\n"
"    font-size: 14px;\n"
"    max-width: 300px;\n"
"    word-wrap: break-word;\n"
"}\n"
"\n"
"QLabel#myMessageText {\n"
"    background-color: #667eea;\n"
"    color: white;\n"
"    padding: 12px 16px;\n"
"    border-radius: 18px;\n"
"    font-size: 14px;\n"
"    max-width: 300px;\n"
"    word-wrap: break-word;\n"
"}\n"
"\n"
"QLabel#timestampLabel {\n"
"    color: #999999;\n"
"    fon"
                        "t-size: 11px;\n"
"    margin: 2px 8px;\n"
"}\n"
"\n"
"QLineEdit#messageInput {\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 20px;\n"
"    padding: 12px 20px;\n"
"    font-size: 14px;\n"
"    background-color: white;\n"
"    margin: 10px;\n"
"}\n"
"\n"
"QLineEdit#messageInput:focus {\n"
"    border-color: #667eea;\n"
"    outline: none;\n"
"}\n"
"\n"
"QPushButton#sendButton {\n"
"    background-color: #667eea;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 20px;\n"
"    padding: 12px 24px;\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    margin: 10px;\n"
"}\n"
"\n"
"QPushButton#sendButton:hover {\n"
"    background-color: #5a67d8;\n"
"}\n"
"\n"
"QPushButton#sendButton:pressed {\n"
"    background-color: #4c51bf;\n"
"}\n"
"\n"
"QPushButton#emojiButton {\n"
"    background-color: transparent;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 20px;\n"
"    padding: 8px;\n"
"    margin: 10px 5px;\n"
"    font-size: 16px;\n"
"}\n"
"\n"
"QPushButton#emojiButton:hove"
                        "r {\n"
"    background-color: #f5f5f5;\n"
"}\n"
"\n"
"QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 8px;\n"
"    margin: 5px;\n"
"    padding-top: 15px;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px 0 5px;\n"
"    color: #333;\n"
"}\n"
"\n"
"QComboBox {\n"
"    background-color: white;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 6px;\n"
"    padding: 8px;\n"
"    font-size: 14px;\n"
"    color: #333;\n"
"}\n"
"\n"
"QComboBox:focus {\n"
"    border-color: #667eea;\n"
"}\n"
"\n"
"QLineEdit#hostInput {\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 6px;\n"
"    padding: 8px;\n"
"    font-size: 14px;\n"
"    background-color: white;\n"
"    color: #333;\n"
"}\n"
"\n"
"QLineEdit#hostInput:focus {\n"
"    border-color: #667eea;\n"
"}\n"
"\n"
"QPushButton#actionButton {\n"
"    background-color: #4caf50;\n"
"    color: white;\n"
"    border: non"
                        "e;\n"
"    border-radius: 6px;\n"
"    padding: 8px 16px;\n"
"    font-weight: bold;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QPushButton#actionButton:hover {\n"
"    background-color: #45a049;\n"
"}\n"
"\n"
"QLabel#connectionLabel {\n"
"    font-weight: bold;\n"
"    padding: 8px;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QStatusBar {\n"
"    background-color: #f8f9fa;\n"
"    color: #666;\n"
"    border-top: 1px solid #e0e0e0;\n"
"}"));
        centralWidget = new QWidget(ChatWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(10, 10, 10, 0);
        connectionGroupBox = new QGroupBox(centralWidget);
        connectionGroupBox->setObjectName(QString::fromUtf8("connectionGroupBox"));
        connectionGroupBox->setMaximumSize(QSize(16777215, 80));
        connectionLayout = new QHBoxLayout(connectionGroupBox);
        connectionLayout->setObjectName(QString::fromUtf8("connectionLayout"));
        modeLabel = new QLabel(connectionGroupBox);
        modeLabel->setObjectName(QString::fromUtf8("modeLabel"));

        connectionLayout->addWidget(modeLabel);

        modeCombo = new QComboBox(connectionGroupBox);
        modeCombo->addItem(QString());
        modeCombo->addItem(QString());
        modeCombo->setObjectName(QString::fromUtf8("modeCombo"));

        connectionLayout->addWidget(modeCombo);

        hostLabel = new QLabel(connectionGroupBox);
        hostLabel->setObjectName(QString::fromUtf8("hostLabel"));

        connectionLayout->addWidget(hostLabel);

        hostInput = new QLineEdit(connectionGroupBox);
        hostInput->setObjectName(QString::fromUtf8("hostInput"));

        connectionLayout->addWidget(hostInput);

        actionButton = new QPushButton(connectionGroupBox);
        actionButton->setObjectName(QString::fromUtf8("actionButton"));

        connectionLayout->addWidget(actionButton);

        connectionLabel = new QLabel(connectionGroupBox);
        connectionLabel->setObjectName(QString::fromUtf8("connectionLabel"));
        connectionLabel->setStyleSheet(QString::fromUtf8("color: red;"));

        connectionLayout->addWidget(connectionLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        connectionLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(connectionGroupBox);

        chatContainer = new QWidget(centralWidget);
        chatContainer->setObjectName(QString::fromUtf8("chatContainer"));
        chatLayout = new QVBoxLayout(chatContainer);
        chatLayout->setSpacing(0);
        chatLayout->setObjectName(QString::fromUtf8("chatLayout"));
        chatLayout->setContentsMargins(0, 10, 0, 0);
        scrollArea = new QScrollArea(chatContainer);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 878, 69));
        messagesLayout = new QVBoxLayout(scrollAreaWidgetContents);
        messagesLayout->setSpacing(5);
        messagesLayout->setObjectName(QString::fromUtf8("messagesLayout"));
        messagesLayout->setContentsMargins(20, 10, 20, 10);
        messagesSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        messagesLayout->addItem(messagesSpacer);

        scrollArea->setWidget(scrollAreaWidgetContents);

        chatLayout->addWidget(scrollArea);

        inputContainer = new QWidget(chatContainer);
        inputContainer->setObjectName(QString::fromUtf8("inputContainer"));
        inputContainer->setMaximumSize(QSize(16777215, 80));
        inputContainer->setStyleSheet(QString::fromUtf8("QWidget#inputContainer {\n"
"    background-color: #fafafa;\n"
"    border-top: 1px solid #e0e0e0;\n"
"}"));
        inputLayout = new QHBoxLayout(inputContainer);
        inputLayout->setObjectName(QString::fromUtf8("inputLayout"));
        inputLayout->setContentsMargins(20, 15, 20, 15);
        emojiButton = new QPushButton(inputContainer);
        emojiButton->setObjectName(QString::fromUtf8("emojiButton"));
        emojiButton->setMaximumSize(QSize(40, 40));

        inputLayout->addWidget(emojiButton);

        messageInput = new QLineEdit(inputContainer);
        messageInput->setObjectName(QString::fromUtf8("messageInput"));

        inputLayout->addWidget(messageInput);

        sendButton = new QPushButton(inputContainer);
        sendButton->setObjectName(QString::fromUtf8("sendButton"));

        inputLayout->addWidget(sendButton);


        chatLayout->addWidget(inputContainer);


        verticalLayout->addWidget(chatContainer);

        ChatWindow->setCentralWidget(centralWidget);
        menubar = new QMenuBar(ChatWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 900, 22));
        ChatWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ChatWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ChatWindow->setStatusBar(statusbar);

        retranslateUi(ChatWindow);

        QMetaObject::connectSlotsByName(ChatWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ChatWindow)
    {
        ChatWindow->setWindowTitle(QCoreApplication::translate("ChatWindow", "\347\275\221\347\273\234\350\201\212\345\244\251\347\250\213\345\272\217", nullptr));
        connectionGroupBox->setTitle(QCoreApplication::translate("ChatWindow", "\350\277\236\346\216\245\350\256\276\347\275\256", nullptr));
        modeLabel->setText(QCoreApplication::translate("ChatWindow", "\346\250\241\345\274\217:", nullptr));
        modeCombo->setItemText(0, QCoreApplication::translate("ChatWindow", "\346\234\215\345\212\241\345\231\250\346\250\241\345\274\217", nullptr));
        modeCombo->setItemText(1, QCoreApplication::translate("ChatWindow", "\345\256\242\346\210\267\347\253\257\346\250\241\345\274\217", nullptr));

        hostLabel->setText(QCoreApplication::translate("ChatWindow", "\344\270\273\346\234\272:", nullptr));
        hostInput->setPlaceholderText(QCoreApplication::translate("ChatWindow", "\350\276\223\345\205\245\346\234\215\345\212\241\345\231\250IP\345\234\260\345\235\200", nullptr));
        hostInput->setText(QCoreApplication::translate("ChatWindow", "127.0.0.1", nullptr));
        actionButton->setText(QCoreApplication::translate("ChatWindow", "\345\220\257\345\212\250\346\234\215\345\212\241\345\231\250", nullptr));
        connectionLabel->setText(QCoreApplication::translate("ChatWindow", "\346\234\252\350\277\236\346\216\245", nullptr));
        emojiButton->setText(QCoreApplication::translate("ChatWindow", "\360\237\230\200", nullptr));
        messageInput->setPlaceholderText(QCoreApplication::translate("ChatWindow", "\350\276\223\345\205\245\346\266\210\346\201\257...", nullptr));
        sendButton->setText(QCoreApplication::translate("ChatWindow", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ChatWindow: public Ui_ChatWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATWINDOW_H
