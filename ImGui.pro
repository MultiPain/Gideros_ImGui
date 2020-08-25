QT -= core ui

TARGET = imgui
TEMPLATE = lib

DESTDIR = ../../../plugins/

INCLUDEPATH += ../../../Sdk/include
INCLUDEPATH += ../../../Sdk/include/gideros
INCLUDEPATH += ../../../Src/2dsg
INCLUDEPATH += ../../../Src/2dsg/gfxbackends
INCLUDEPATH += ../../../Src/libgideros
INCLUDEPATH += ../../../Src/libgid/include
INCLUDEPATH += ../../../Src/luabinding
INCLUDEPATH += ../../../Src/lua/src

SOURCES += \
        imgui_src/imgui.cpp \
        imgui_src/imgui_demo.cpp \
        imgui_src/imgui_draw.cpp \
        imgui_src/imgui_user.cpp \
        imgui_src/imgui_widgets.cpp \
        main.cpp \
        binder.cpp

HEADERS += \
        binder.h \
    imgui_src/imconfig.h \
    imgui_src/imgui.h \
    imgui_src/imgui_internal.h \
    imgui_src/imgui_user.h \
    imgui_src/imstb_rectpack.h \
    imgui_src/imstb_textedit.h \
    imgui_src/imstb_truetype.h

LIBS += -L"../../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs

DEFINES += GID_LIBRARY
