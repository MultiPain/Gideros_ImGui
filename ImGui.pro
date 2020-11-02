QT -= core ui

TARGET = imgui_beta_nodes
TEMPLATE = lib

#DESTDIR = ../../plugins/

INCLUDEPATH += ../../Sdk/include
INCLUDEPATH += ../../Sdk/include/gideros
INCLUDEPATH += ../../Src/2dsg
INCLUDEPATH += ../../Src/2dsg/gfxbackends
INCLUDEPATH += ../../Src/libgideros
INCLUDEPATH += ../../Src/libgid/include
INCLUDEPATH += ../../Src/luabinding
INCLUDEPATH += ../../Src/lua/src

SOURCES += \
    imgui_src/imgui.cpp \
    imgui_src/imgui_demo.cpp \
    imgui_src/imgui_draw.cpp \
    imgui_src/imgui_widgets.cpp \
    imnodes/imnodes.cpp \
    imgui_user.cpp \
    ../../Src/2dsg/Matrices.cpp \
    ../../Src/2dsg/mouseevent.cpp \
    ../../Src/2dsg/keyboardevent.cpp \
    ../../Src/luabinding/binder.cpp \
    main.cpp

HEADERS += \
    imgui_user.h \
    imgui_src/imconfig.h \
    imgui_src/imgui.h \
    imgui_src/imgui_internal.h \
    imgui_src/imstb_rectpack.h \
    imgui_src/imstb_textedit.h \
    imgui_src/imstb_truetype.h \
    imnodes/imnodes.h \
    ../../Src/luabinding/binder.h \
    ../../Src/2dsg/mouseevent.h \
    ../../Src/2dsg/keyboardevent.h

LIBS += -L"../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs

DEFINES += GID_LIBRARY
