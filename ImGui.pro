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
    imgui/imgui.cpp \
    imgui/imgui_demo.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui_widgets.cpp \
    imgui/imgui_tables.cpp \
    imgui_user.cpp \
    custom/TextEditor.cpp \
    custom/node-editor/crude_json.cpp \
    custom/node-editor/imgui_canvas.cpp \
    custom/node-editor/imgui_node_editor.cpp \
    custom/node-editor/imgui_node_editor_api.cpp \
    ../../Src/2dsg/bitmapdata.cpp \
    ../../Src/2dsg/Matrices.cpp \
    ../../Src/2dsg/mouseevent.cpp \
    ../../Src/2dsg/touchevent.cpp \
    ../../Src/2dsg/keyboardevent.cpp \
    main.cpp

HEADERS += \
    imgui/imconfig.h \
    imgui/imgui.h \
    imgui/imgui_internal.h \
    imgui/imstb_rectpack.h \
    imgui/imstb_textedit.h \
    imgui/imstb_truetype.h \
    imgui_user.h \
    custom/TextEditor.h \
    custom/node-editor/imgui_node_editor_internal.inl \
    custom/node-editor/imgui_node_editor_internal.h \
    custom/node-editor/imgui_node_editor.h \
    custom/node-editor/imgui_extra_math.inl \
    custom/node-editor/imgui_extra_math.h \
    custom/node-editor/imgui_canvas.h \
    custom/node-editor/imgui_bezier_math.inl \
    custom/node-editor/imgui_bezier_math.h \
    custom/node-editor/crude_json.h \
    ../../Src/2dsg/bitmapdata.h \
    ../../Src/2dsg/mouseevent.h \
    ../../Src/2dsg/touchevent.h \
    ../../Src/2dsg/keyboardevent.h

LIBS += -L"../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs

DEFINES += GID_LIBRARY
