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
    imgui_nodes/crude_json.cpp \
    imgui_nodes/imgui_bezier_math.inl \
    imgui_nodes/imgui_canvas.cpp \
    imgui_nodes/imgui_extra_math.inl \
    imgui_nodes/imgui_node_editor.cpp \
    imgui_nodes/imgui_node_editor_api.cpp \
    imgui_nodes/imgui_node_editor_internal.inl \
    TextEditor.cpp \
    ../../gideros_master/2dsg/bitmapdata.cpp \
    ../../gideros_master/2dsg/Matrices.cpp \
    ../../gideros_master/2dsg/mouseevent.cpp \
    ../../gideros_master/2dsg/touchevent.cpp \
    ../../gideros_master/2dsg/keyboardevent.cpp \
    ../../gideros_master/luabinding/stackchecker.cpp \
    impl_ImGui_nodes.cpp \
    main.cpp

HEADERS += \
    GColor.h \
    Helpers.h \
    imgui_src/imconfig.h \
    imgui_src/imgui.h \
    imgui_src/imgui_internal.h \
    imgui_src/imstb_rectpack.h \
    imgui_src/imstb_textedit.h \
    imgui_src/imstb_truetype.h \
    imgui_user.h \
    imgui_nodes/crude_json.h \
    imgui_nodes/imgui_bezier_math.h \
    imgui_nodes/imgui_canvas.h \
    imgui_nodes/imgui_extra_math.h \
    imgui_nodes/imgui_node_editor.h \
    imgui_nodes/imgui_node_editor_internal.h \
    TextEditor.h \
    ../../gideros_master/2dsg/bitmapdata.h \
    ../../gideros_master/2dsg/mouseevent.h \
    ../../gideros_master/2dsg/touchevent.h \
    ../../gideros_master/2dsg/keyboardevent.h  \
    ../../gideros_master/luabinding/stackchecker.h \
    impl_ImGui_nodes.h

LIBS += -L"../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs #-lpystring

DEFINES += GID_LIBRARY
