TEMPLATE=lib
CONFIG+=staticlib
QT-=gui

message(Touch mimeTeX qmake)

QMAKE_CFLAGS += -DAA
QMAKE_CXXFLAGS += -DAA

SOURCES=mimetex.c

HEADERS=mimetex.h \
        texfonts.h

        
OBJECTS_DIR = $${_PRO_FILE_PWD_}/build/obj
DESTDIR = $${_PRO_FILE_PWD_}/build/lib

