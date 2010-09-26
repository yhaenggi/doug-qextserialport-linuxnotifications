include(../common.pri)
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

qextserialport-uselib:!qextserialport-buildlib {
    LIBS += -L$$QEXTSERIALPORT_LIBDIR -l$$QEXTSERIALPORT_LIBNAME
} else {
    HEADERS                += $$PWD/qextserialport.h \
                              $$PWD/qextserialenumerator.h \
                              $$PWD/qextserialport_global.h
    SOURCES                += $$PWD/qextserialport.cpp
    unix:SOURCES           += $$PWD/posix_qextserialport.cpp
    unix:!macx:SOURCES     += $$PWD/qextserialenumerator_unix.cpp
    macx:SOURCES           += $$PWD/qextserialenumerator_osx.cpp
    win32:SOURCES          += $$PWD/win_qextserialport.cpp \
                              $$PWD/qextserialenumerator_win.cpp

    macx:LIBS              += -framework IOKit -framework CoreFoundation
    win32:LIBS             += -lsetupapi -ladvapi32 -luser32
}

win32 {
    contains(TEMPLATE, lib):contains(CONFIG, shared):DEFINES += QEXTSERIALPORT_EXPORT
    else:qextserialport-uselib:DEFINES += QEXTSERIALPORT_IMPORT
}
