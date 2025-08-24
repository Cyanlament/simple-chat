/********************************************************************************
** Form generated from reading UI file 'chatwindow_simple.ui'
**
** Created by: Qt User Interface Compiler version 5.15.16
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATWINDOW_SIMPLE_H
#define UI_CHATWINDOW_SIMPLE_H

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
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
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
    QTextEdit *chatDisplay;
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
"    background-color: #f8f9fa;\n"
"}\n"
"\n"
"QTextEdit#chatDisplay {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e9ecef;\n"
"    border-radius: 12px;\n"
"    padding: 15px;\n"
"    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;\n"
"    font-size: 14px;\n"
"    selection-background-color: #667eea;\n"
"}\n"
"\n"
"QLineEdit#messageInput {\n"
"    border: 2px solid #e9ecef;\n"
"    border-radius: 20px;\n"
"    padding: 12px 20px;\n"
"    font-size: 14px;\n"
"    background-color: white;\n"
"    color: #333;\n"
"}\n"
"\n"
"QLineEdit#messageInput:focus {\n"
"    border-color: #667eea;\n"
"    outline: none;\n"
"}\n"
"\n"
"QPushButton#sendButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 #667eea, stop:1 #764ba2);\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 20px;\n"
"    padding: 12px 20px;\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    min-width: 60px;\n"
"}\n"
"\n"
"QPushButton#send"
                        "Button:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 #5a67d8, stop:1 #6b46c1);\n"
"}\n"
"\n"
"QPushButton#emojiButton {\n"
"    background-color: #f8f9fa;\n"
"    border: 2px solid #e9ecef;\n"
"    border-radius: 20px;\n"
"    padding: 8px;\n"
"    font-size: 16px;\n"
"    min-width: 40px;\n"
"    max-width: 40px;\n"
"}\n"
"\n"
"QPushButton#emojiButton:hover {\n"
"    background-color: #e9ecef;\n"
"}\n"
"\n"
"QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 1px solid #dee2e6;\n"
"    border-radius: 8px;\n"
"    margin: 5px;\n"
"    padding-top: 15px;\n"
"    background-color: white;\n"
"    color: #495057;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 8px;\n"
"    color: #495057;\n"
"}\n"
"\n"
"QComboBox {\n"
"    background-color: white;\n"
"    border: 1px solid #dee2e6;\n"
"    border-radius: 6px;\n"
"    padding: 8px 12px;\n"
"    font-size: 14px;\n"
"    color: #495057;\n"
"    min-width: 120px;\n"
""
                        "}\n"
"\n"
"QLineEdit#hostInput {\n"
"    border: 1px solid #dee2e6;\n"
"    border-radius: 6px;\n"
"    padding: 8px 12px;\n"
"    font-size: 14px;\n"
"    background-color: white;\n"
"    color: #495057;\n"
"}\n"
"\n"
"QPushButton#actionButton {\n"
"    background-color: #28a745;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 8px 16px;\n"
"    font-weight: bold;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QPushButton#actionButton:hover {\n"
"    background-color: #218838;\n"
"}\n"
"\n"
"QLabel#connectionLabel {\n"
"    font-weight: bold;\n"
"    padding: 6px 12px;\n"
"    border-radius: 4px;\n"
"    background-color: #f8d7da;\n"
"    color: #721c24;\n"
"}\n"
"\n"
"QStatusBar {\n"
"    background-color: #f8f9fa;\n"
"    color: #6c757d;\n"
"    border-top: 1px solid #dee2e6;\n"
"}"));
        centralWidget = new QWidget(ChatWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(15, 10, 15, 10);
        connectionGroupBox = new QGroupBox(centralWidget);
        connectionGroupBox->setObjectName(QString::fromUtf8("connectionGroupBox"));
        connectionGroupBox->setMaximumSize(QSize(16777215, 70));
        connectionLayout = new QHBoxLayout(connectionGroupBox);
        connectionLayout->setSpacing(10);
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

        connectionLayout->addWidget(connectionLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        connectionLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(connectionGroupBox);

        chatDisplay = new QTextEdit(centralWidget);
        chatDisplay->setObjectName(QString::fromUtf8("chatDisplay"));
        chatDisplay->setReadOnly(true);
        chatDisplay->setFrameShape(QFrame::NoFrame);

        verticalLayout->addWidget(chatDisplay);

        inputContainer = new QWidget(centralWidget);
        inputContainer->setObjectName(QString::fromUtf8("inputContainer"));
        inputContainer->setMaximumSize(QSize(16777215, 70));
        inputContainer->setStyleSheet(QString::fromUtf8("QWidget#inputContainer {\n"
"    background-color: white;\n"
"    border-top: 1px solid #dee2e6;\n"
"    border-radius: 0 0 12px 12px;\n"
"}"));
        inputLayout = new QHBoxLayout(inputContainer);
        inputLayout->setSpacing(10);
        inputLayout->setObjectName(QString::fromUtf8("inputLayout"));
        inputLayout->setContentsMargins(15, 15, 15, 15);
        emojiButton = new QPushButton(inputContainer);
        emojiButton->setObjectName(QString::fromUtf8("emojiButton"));

        inputLayout->addWidget(emojiButton);

        messageInput = new QLineEdit(inputContainer);
        messageInput->setObjectName(QString::fromUtf8("messageInput"));

        inputLayout->addWidget(messageInput);

        sendButton = new QPushButton(inputContainer);
        sendButton->setObjectName(QString::fromUtf8("sendButton"));

        inputLayout->addWidget(sendButton);


        verticalLayout->addWidget(inputContainer);

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

#endif // UI_CHATWINDOW_SIMPLE_H
