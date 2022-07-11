QT -= core ui

TARGET = imgui_beta
TEMPLATE = lib

#DESTDIR = ../../plugins/

INCLUDEPATH += ../../Sdk/include
INCLUDEPATH += ../../Sdk/include/gideros
INCLUDEPATH += ../../gideros_master/2dsg
INCLUDEPATH += ../../gideros_master/2dsg/gfxbackends
INCLUDEPATH += ../../gideros_master/libgideros
INCLUDEPATH += ../../gideros_master/libgid/include
INCLUDEPATH += ../../gideros_master/luabinding
INCLUDEPATH += ../../gideros_master/lua/src

SOURCES += \
    imgui_src/imgui.cpp \
    imgui_src/imgui_demo.cpp \
    imgui_src/imgui_draw.cpp \
    imgui_src/imgui_widgets.cpp \
    imgui_src/imgui_tables.cpp \
    imgui_user.cpp \
    TextEditor.cpp \
    ../../gideros_master/2dsg/bitmapdata.cpp \
    ../../gideros_master/2dsg/Matrices.cpp \
    ../../gideros_master/2dsg/mouseevent.cpp \
    ../../gideros_master/2dsg/touchevent.cpp \
    ../../gideros_master/2dsg/keyboardevent.cpp \
    ../../gideros_master/luabinding/stackchecker.cpp \
    imnodes_src/imnodes.cpp \
    main.cpp

HEADERS += \
    imgui_src/imconfig.h \
    imgui_src/imgui.h \
    imgui_src/imgui_internal.h \
    imgui_src/imstb_rectpack.h \
    imgui_src/imstb_textedit.h \
    imgui_src/imstb_truetype.h \
    imgui_user.h \
    TextEditor.h \
    ../../gideros_master/2dsg/bitmapdata.h \
    ../../gideros_master/2dsg/mouseevent.h \
    ../../gideros_master/2dsg/touchevent.h \
    ../../gideros_master/2dsg/keyboardevent.h  \
    ../../gideros_master/luabinding/stackchecker.h \
    imnodes_src/imnodes.h \
    imnodes_src/imnodes_internal.h

LIBS += -L"../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs #-lpystring

DEFINES += GID_LIBRARY
