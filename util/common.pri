

INCLUDEPATH += $$PWD
HEADERS += util.h



LIBS += -lws2_32


CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../debug
}else {
    DESTDIR = $$PWD/../../release
}
