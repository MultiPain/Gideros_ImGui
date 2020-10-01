QT -= core ui

TARGET = imgui_beta_docking

TEMPLATE = lib

INCLUDEPATH += ../../Sdk/include
INCLUDEPATH += ../../Sdk/include/gideros
INCLUDEPATH += ../../Src/2dsg
INCLUDEPATH += ../../Src/2dsg/gfxbackends
INCLUDEPATH += ../../Src/libgideros
INCLUDEPATH += ../../Src/libgid/include
INCLUDEPATH += ../../Src/luabinding
INCLUDEPATH += ../../Src/lua/src

SOURCES += \
    main.cpp \
    binder.cpp \
    imgui_user.cpp \
    ../../Src/2dsg/Matrices.cpp \
    ../../Src/2dsg/mouseevent.cpp \
    ../../Src/2dsg/keyboardevent.cpp \
    imgui_src_docking/imgui.cpp \
    imgui_src_docking/imgui_demo.cpp \
    imgui_src_docking/imgui_draw.cpp \
    imgui_src_docking/imgui_widgets.cpp

HEADERS += \
    ../../Src/2dsg/mouseevent.h \
    ../../Src/2dsg/keyboardevent.h \
    binder.h \
    imgui_user.h \
    imgui_src_docking/imconfig.h \
    imgui_src_docking/imgui.h \
    imgui_src_docking/imgui_internal.h \
    imgui_src_docking/imstb_rectpack.h \
    imgui_src_docking/imstb_textedit.h \
    imgui_src_docking/imstb_truetype.h


LIBS += -L"../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs

DEFINES += GID_LIBRARY
