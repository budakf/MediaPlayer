#-------------------------------------------------
#
# Project created by QtCreator 2019-09-22T13:42:09
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MediaPlayer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        livevideopipelinebuilder.cpp \
        main.cpp \
        player.cpp \
        recordedvideopipelinebuilder.cpp

HEADERS += \
        ipipelinebuilder.h \
        livevideopipelinebuilder.h \
        player.h \
        recordedvideopipelinebuilder.h

FORMS += \
        player.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


macos{

    INCLUDEPATH += /usr/local/Cellar/gstreamer/1.16.0/include/gstreamer-1.0/gst
    INCLUDEPATH += /usr/local/Cellar/libffi/3.2.1/lib/libffi-3.2.1/include \
                   /usr/local/Cellar/gst-plugins-base/1.16.0/include/gstreamer-1.0 \
                   /usr/local/Cellar/gst-plugins-bad/1.16.0/include/gstreamer-1.0 \
                   /usr/local/Cellar/orc/0.4.28/include/orc-0.4 \
                   /usr/local/Cellar/gstreamer/1.16.0/include/gstreamer-1.0 \
                   /usr/local/Cellar/glib/2.60.2/include \
                   /usr/local/Cellar/glib/2.60.2/include/glib-2.0  \
                   /usr/local/Cellar/glib/2.60.2/lib/glib-2.0/include \
                   /usr/local/opt/gettext/include \
                   /usr/local/Cellar/pcre/8.43/include

    LIBS += -L/usr/local/Cellar/gst-plugins-base/1.16.0/lib \
            -L/usr/local/Cellar/gst-plugins-bad/1.16.0/lib \
            -L/usr/local/Cellar/gstreamer/1.16.0/lib  \
            -L/usr/local/Cellar/glib/2.60.2/lib  \
            -L/usr/local/opt/gettext/lib \
            -lgstvideo-1.0 -lgstbase-1.0 -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lintl

}
