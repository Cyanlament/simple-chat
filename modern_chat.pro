QT += core widgets network

CONFIG += c++17

TARGET = ModernChat

SOURCES += \
    main_modern.cpp \
    modernChat.cpp \
    chatbubble.cpp

HEADERS += \
    modernChat.h \
    chatbubble.h

# macOS specific settings
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
}