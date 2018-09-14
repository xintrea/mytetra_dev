TEMPLATE=app
QT-=gui

message(Touch mimeTeX qmake)

QMAKE_CFLAGS += -DAA -D_GNU_SOURCE
QMAKE_CXXFLAGS += -DAA -D_GNU_SOURCE

SOURCES=mimetex.c \
        gifsave.c \

HEADERS=mimetex.h \
        texfonts.h

        
OBJECTS_DIR = $${_PRO_FILE_PWD_}/build/obj
DESTDIR = $${_PRO_FILE_PWD_}/build/bin

