TEMPLATE = app

CONFIG -= qt debug_and_release debug_and_release_target app_bundle
CONFIG += c++17 no_include_pwd thread

DEFINES += SFML_STATIC
SFML_DIR = ../../SFML-3.0.0
INCLUDEPATH += $$SFML_DIR/include
LIBS += -L$$SFML_DIR/build/lib -lsfml-graphics-s -lsfml-window-s  -lsfml-audio-s -lsfml-system-s

win32 {
    LIBS += -lopengl32 -lfreetype -lgdi32 -lwinmm -lflac -lvorbisenc -lvorbisfile -lvorbis -logg
}
unix {
    LIBS += -lX11 -lfreetype -lXrandr -lXcursor -ludev -lXi -lvorbisenc -lvorbisfile -lvorbis -logg -lFLAC
}

SOURCES += \
    assets.cpp \
    cards.cpp \
    main.cpp \
    maingame.cpp

HEADERS += \
    assets.h \
    cards.h \
    maingame.h
