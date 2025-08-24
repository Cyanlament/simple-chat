#include <QApplication>
#include "chatwindow_ui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    ChatWindow window;
    window.show();
    
    return app.exec();
}