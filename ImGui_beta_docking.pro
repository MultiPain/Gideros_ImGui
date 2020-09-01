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
    imgui_src_docking/imgui.cpp \
    imgui_src_docking/imgui_demo.cpp \
    imgui_src_docking/imgui_draw.cpp \
    imgui_src_docking/imgui_widgets.cpp

HEADERS += \
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
