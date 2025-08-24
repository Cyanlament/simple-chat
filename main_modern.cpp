#include <QApplication>
#include "modernChat.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    ModernChatWindow window;
    window.show();
    
    return app.exec();
}