QT += core widgets network

CONFIG += c++17

TARGET = NetworkChat_UI

SOURCES += \
    main_ui.cpp \
    chatwindow_ui.cpp

HEADERS += \
    chatwindow_ui.h

FORMS += \
    chatwindow.ui

# macOS specific settings
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
}