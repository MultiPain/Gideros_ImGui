QT -= core ui

TARGET = imgui_beta
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
    imgui_user.cpp \
    ../../Src/2dsg/bitmapdata.cpp \
    ../../Src/2dsg/Matrices.cpp \
    ../../Src/2dsg/mouseevent.cpp \
    ../../Src/2dsg/touchevent.cpp \
    ../../Src/2dsg/keyboardevent.cpp \
    ../../Src/luabinding/binder.cpp \
    imgui-node-editor/crude_json.cpp \
    imgui-node-editor/imgui_canvas.cpp \
    imgui-node-editor/imgui_node_editor.cpp \
    imgui-node-editor/imgui_node_editor_api.cpp \
    main.cpp

HEADERS += \
    ../../Src/2dsg/bitmapdata.h \
    ../../Src/luabinding/binder.h \
    ../../Src/2dsg/mouseevent.h \
    ../../Src/2dsg/touchevent.h \
    ../../Src/2dsg/keyboardevent.h \
    imgui_user.h \
    imgui_src/imconfig.h \
    imgui_src/imgui.h \
    imgui_src/imgui_internal.h \
    imgui_src/imstb_rectpack.h \
    imgui_src/imstb_textedit.h \
    imgui_src/imstb_truetype.h \
    imgui-node-editor/imgui_node_editor_internal.inl \
    imgui-node-editor/imgui_node_editor_internal.h \
    imgui-node-editor/imgui_node_editor.h \
    imgui-node-editor/imgui_extra_math.inl \
    imgui-node-editor/imgui_extra_math.h \
    imgui-node-editor/imgui_canvas.h \
    imgui-node-editor/imgui_bezier_math.inl \
    imgui-node-editor/imgui_bezier_math.h \
    imgui-node-editor/crude_json.h

LIBS += -L"../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs

DEFINES += GID_LIBRARY
