
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread

#CONFIG += c++17
*-g++* {
    QMAKE_CXXFLAGS += -std=c++17
    QMAKE_CXXFLAGS += -Wno-dangling-else
}

SOURCES += \
    ../../../../libqak/thread__test.cxx

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += $$_PRO_FILE_PWD_/../../../../include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qak/release/ -lqak
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qak/debug/ -lqak
else:unix: LIBS += -L$$OUT_PWD/../qak/ -lqak

INCLUDEPATH += $$PWD/../qak
DEPENDPATH += $$PWD/../qak
