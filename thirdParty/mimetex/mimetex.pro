TEMPLATE=app
QT-=gui

message(Touch mimeTeX qmake)

QMAKE_CFLAGS += -DAA
QMAKE_CXXFLAGS += -DAA

SOURCES=mimetex.c \
        gifsave.c \

HEADERS=mimetex.h \
        texfonts.h

        
OBJECTS_DIR = $${_PRO_FILE_PWD_}/build/obj
DESTDIR = $${_PRO_FILE_PWD_}/build/bin

win32-msvc {
    DEFINES += _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS
}
