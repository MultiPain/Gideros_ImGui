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
    imgui_src/imgui_tables.cpp \
    imgui_user.cpp \
    TextEditor.cpp \
    implot_src/implot.cpp \
    implot_src/implot_demo.cpp \
    implot_src/implot_items.cpp\
    ../../Src/2dsg/bitmapdata.cpp \
    ../../Src/2dsg/Matrices.cpp \
    ../../Src/2dsg/mouseevent.cpp \
    ../../Src/2dsg/touchevent.cpp \
    ../../Src/2dsg/keyboardevent.cpp \
	../../Src/luabinding/stackchecker.cpp \
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
    implot_src/implot.h \
    implot_src/implot_internal.h \
    ../../Src/2dsg/bitmapdata.h \
    ../../Src/2dsg/mouseevent.h \
    ../../Src/2dsg/touchevent.h \
    ../../Src/2dsg/keyboardevent.h  \
	../../Src/luabinding/stackchecker.h

LIBS += -L"../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs -lpystring

DEFINES += GID_LIBRARY
