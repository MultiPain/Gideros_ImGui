# API

* [ENUMS](#enums)
* [Custom drawing](#draw-list-commands)
* [Fonts](#fonts-wip)
* [Inputs](#inputs)
* [Styles](#default-styles)
* [IO](#io-functions)
* [Widgets](#windgets--stuff)
* [Windows](#windows)
* [Child windows](#child-windows)
* [Windows utilities](#windows-utilities)
* [Content region](#content-region)
* [Windows scroll](#windows-scrolling)
* [Parameters stacks](#parameters-stacks-shared)
* [Cursor / layout](#cursor--layout)
* [ID](#id-stackscopes)
* [[Windgets] Text](#widgets-text)
* [[Windgets] Main](#widgets-main)
* [[Windgets] ComboBox](#widgets-combo-box)
* [[Windgets] Drags](#widgets-drags)
* [[Windgets] Sliders](#widgets-sliders)
* [[Windgets] Text input](#widgets-input-with-keyboard)
* [[Windgets] Color pickers](#widgets-color-editorpicker)
* [[Windgets] Trees](#widgets-trees)
* [[Windgets] Selectables](#widgets-selectables)
* [[Windgets] ListBox](#widgets-list-boxes)
* [[Windgets] Data ploting](#widgets-data-plotting)
* [[Windgets] Value helper](#widgets-value-helpers)
* [[Windgets] Menus](#widgets-menus)
* [Popups / Modals](#popups-modals)
* [Columns](#columns)
* [Tabs](#tab-bars-tabs)
* [Clipping](#clipping)
* [Focus](#focus-activation)
* [Render](#render)
* [Display](#display-size)
* [ImGui Demos](#demos)

!VERY IMPORTANT!</br> 
every color related argument is actualy 2 arguments: color it self in hex format: 0xRRGGBB AND alpha [0..1]</br> 
for example:</br> 
```ImGui:drawListAddRect(p_min_x, p_min_y, p_max_x, p_max_y, color, [rounding = 0, rounding_corners = ImGui.CornerFlags_All, thickness = 1])```</br> 
usage: ```imgui:drawListAddRect(0,0, 100,100, 0xff0000, 1, ROUNDING, ROUNDING_CORNERS, THICKNESS)```</br> 

# Constructor
```lua
ImGui.new([width, height, fontsTable])
-- width (number, default = application:getContentWidth()): screen width
-- height (number, default = application:getContentHeight()): screen height
-- fontsTable (table, optional): {font1Desc, font2Desc, ...}
--	fontDesc (table): {fontName, fontSize, [oversampleH, oversampleV, offsetX, offsetY]} 
-- 		fontName (string): path to font (from Gideros project tree) 
-- 		fontSize (number): font size in px
-- 		oversampleH (number, default = 1): makes edges less sharp
-- 		oversampleV (number, default = 1): makes edges less sharp 
-- 		offsetX (number, default = 0): font offset by X
-- 		offsetY (number, default = 0): font offset by Y
```

# ENUMS
```lua
-- ImGuiFocusedFlags
ImGui.FocusedFlags_ChildWindows
ImGui.FocusedFlags_AnyWindow
ImGui.FocusedFlags_RootWindow
ImGui.FocusedFlags_RootAndChildWindows
ImGui.FocusedFlags_None

-- ImGuiPopupFlags
ImGui.PopupFlags_NoOpenOverExistingPopup
ImGui.PopupFlags_MouseButtonLeft
ImGui.PopupFlags_MouseButtonMask_
ImGui.PopupFlags_MouseButtonRight
ImGui.PopupFlags_AnyPopupId
ImGui.PopupFlags_MouseButtonDefault_
ImGui.PopupFlags_MouseButtonMiddle
ImGui.PopupFlags_None
ImGui.PopupFlags_AnyPopup
ImGui.PopupFlags_AnyPopupLevel
ImGui.PopupFlags_NoOpenOverItems

-- ImGuiHoveredFlags
ImGui.HoveredFlags_None
ImGui.HoveredFlags_RootAndChildWindows
ImGui.HoveredFlags_AllowWhenBlockedByPopup
ImGui.HoveredFlags_AllowWhenBlockedByActiveItem
ImGui.HoveredFlags_ChildWindows
ImGui.HoveredFlags_RectOnly
ImGui.HoveredFlags_AllowWhenDisabled
ImGui.HoveredFlags_AllowWhenOverlapped
ImGui.HoveredFlags_AnyWindow
ImGui.HoveredFlags_RootWindow

-- ImGuiInputTextFlags
ImGui.InputTextFlags_EnterReturnsTrue
ImGui.InputTextFlags_CallbackCompletion
ImGui.InputTextFlags_None
ImGui.InputTextFlags_CallbackResize
ImGui.InputTextFlags_ReadOnly
ImGui.InputTextFlags_AutoSelectAll
ImGui.InputTextFlags_AllowTabInput
ImGui.InputTextFlags_CharsScientific
ImGui.InputTextFlags_CallbackAlways
ImGui.InputTextFlags_CharsDecimal
ImGui.InputTextFlags_NoUndoRedo
ImGui.InputTextFlags_CallbackHistory
ImGui.InputTextFlags_CtrlEnterForNewLine
ImGui.InputTextFlags_CharsHexadecimal
ImGui.InputTextFlags_CharsNoBlank
ImGui.InputTextFlags_Password
ImGui.InputTextFlags_CallbackCharFilter
ImGui.InputTextFlags_NoHorizontalScroll
ImGui.InputTextFlags_AlwaysInsertMode
ImGui.InputTextFlags_CharsUppercase

-- ImGuiNavInput
ImGui.NavInput_FocusNext
ImGui.NavInput_TweakFast
ImGui.NavInput_Input
ImGui.NavInput_DpadRight
ImGui.NavInput_FocusPrev
ImGui.NavInput_LStickDown
ImGui.NavInput_LStickUp
ImGui.NavInput_Activate
ImGui.NavInput_LStickLeft
ImGui.NavInput_LStickRight
ImGui.NavInput_DpadLeft
ImGui.NavInput_DpadDown
ImGui.NavInput_TweakSlow
ImGui.NavInput_DpadUp
ImGui.NavInput_Menu
ImGui.NavInput_Cancel

-- ImGuiTabBarFlags
ImGui.TabBarFlags_AutoSelectNewTabs
ImGui.TabBarFlags_NoCloseWithMiddleMouseButton
ImGui.TabBarFlags_TabListPopupButton
ImGui.TabBarFlags_NoTooltip
ImGui.TabBarFlags_FittingPolicyMask_
ImGui.TabBarFlags_Reorderable
ImGui.TabBarFlags_FittingPolicyDefault_
ImGui.TabBarFlags_FittingPolicyScroll
ImGui.TabBarFlags_FittingPolicyResizeDown
ImGui.TabBarFlags_None
ImGui.TabBarFlags_NoTabListScrollingButtons

-- ImGuiTreeNodeFlags
ImGui.TreeNodeFlags_Bullet
ImGui.TreeNodeFlags_None
ImGui.TreeNodeFlags_CollapsingHeader
ImGui.TreeNodeFlags_NavLeftJumpsBackHere
ImGui.TreeNodeFlags_Framed
ImGui.TreeNodeFlags_FramePadding
ImGui.TreeNodeFlags_AllowItemOverlap
ImGui.TreeNodeFlags_OpenOnArrow
ImGui.TreeNodeFlags_SpanFullWidth
ImGui.TreeNodeFlags_NoAutoOpenOnLog
ImGui.TreeNodeFlags_Leaf
ImGui.TreeNodeFlags_NoTreePushOnOpen
ImGui.TreeNodeFlags_Selected
ImGui.TreeNodeFlags_SpanAvailWidth
ImGui.TreeNodeFlags_OpenOnDoubleClick
ImGui.TreeNodeFlags_DefaultOpen

-- ImGuiStyleVar
ImGui.StyleVar_GrabRounding
ImGui.StyleVar_Alpha
ImGui.StyleVar_WindowMinSize
ImGui.StyleVar_PopupBorderSize
ImGui.StyleVar_WindowBorderSize
ImGui.StyleVar_FrameBorderSize
ImGui.StyleVar_ItemSpacing
ImGui.StyleVar_IndentSpacing
ImGui.StyleVar_FramePadding
ImGui.StyleVar_WindowPadding
ImGui.StyleVar_ChildRounding
ImGui.StyleVar_ItemInnerSpacing
ImGui.StyleVar_WindowRounding
ImGui.StyleVar_FrameRounding
ImGui.StyleVar_TabRounding
ImGui.StyleVar_ChildBorderSize
ImGui.StyleVar_GrabMinSize
ImGui.StyleVar_ScrollbarRounding
ImGui.StyleVar_ScrollbarSize
ImGui.StyleVar_WindowTitleAlign
ImGui.StyleVar_SelectableTextAlign
ImGui.StyleVar_PopupRounding
ImGui.StyleVar_ButtonTextAlign

-- ImGuiCol_
ImGui.Col_PlotHistogram
ImGui.Col_TitleBg
ImGui.Col_Separator
ImGui.Col_HeaderActive
ImGui.Col_HeaderHovered
ImGui.Col_ButtonHovered
ImGui.Col_NavWindowingHighlight
ImGui.Col_ScrollbarGrab
ImGui.Col_FrameBg
ImGui.Col_TextSelectedBg
ImGui.Col_ScrollbarGrabActive
ImGui.Col_ModalWindowDarkening
ImGui.Col_TitleBgCollapsed
ImGui.Col_ModalWindowDimBg
ImGui.Col_ResizeGripActive
ImGui.Col_SeparatorHovered
ImGui.Col_ScrollbarGrabHovered
ImGui.Col_TabUnfocused
ImGui.Col_ScrollbarBg
ImGui.Col_ChildBg
ImGui.Col_Header
ImGui.Col_NavWindowingDimBg
ImGui.Col_CheckMark
ImGui.Col_Button
ImGui.Col_BorderShadow
ImGui.Col_DragDropTarget
ImGui.Col_MenuBarBg
ImGui.Col_TitleBgActive
ImGui.Col_SeparatorActive
ImGui.Col_Text
ImGui.Col_PlotLinesHovered
ImGui.Col_Border
ImGui.Col_TabUnfocusedActive
ImGui.Col_PlotLines
ImGui.Col_PlotHistogramHovered
ImGui.Col_ResizeGripHovered
ImGui.Col_Tab
ImGui.Col_TabHovered
ImGui.Col_PopupBg
ImGui.Col_TabActive
ImGui.Col_FrameBgActive
ImGui.Col_ButtonActive
ImGui.Col_WindowBg
ImGui.Col_SliderGrabActive
ImGui.Col_SliderGrab
ImGui.Col_NavHighlight
ImGui.Col_FrameBgHovered
ImGui.Col_TextDisabled
ImGui.Col_ResizeGrip

-- ImGuiDataType
ImGui.DataType_U8
ImGui.DataType_S64
ImGui.DataType_Float
ImGui.DataType_S16
ImGui.DataType_U16
ImGui.DataType_Double
ImGui.DataType_S8
ImGui.DataType_U32
ImGui.DataType_S32
ImGui.DataType_U64

-- ImGuiDir
ImGui.Dir_None
ImGui.Dir_Left
ImGui.Dir_Up
ImGui.Dir_Down
ImGui.Dir_Right

-- ImGuiWindowFlags
ImGui.WindowFlags_NoScrollWithMouse
ImGui.WindowFlags_None
ImGui.WindowFlags_NoScrollbar
ImGui.WindowFlags_HorizontalScrollbar
ImGui.WindowFlags_NoFocusOnAppearing
ImGui.WindowFlags_NoBringToFrontOnFocus
ImGui.WindowFlags_NoDecoration
ImGui.WindowFlags_NoCollapse
ImGui.WindowFlags_NoTitleBar
ImGui.WindowFlags_NoMove
ImGui.WindowFlags_NoInputs
ImGui.WindowFlags_NoMouseInputs
ImGui.WindowFlags_NoSavedSettings
ImGui.WindowFlags_NoNav
ImGui.WindowFlags_UnsavedDocument
ImGui.WindowFlags_NoNavFocus
ImGui.WindowFlags_AlwaysHorizontalScrollbar
ImGui.WindowFlags_AlwaysUseWindowPadding
ImGui.WindowFlags_NoNavInputs
ImGui.WindowFlags_NoResize
ImGui.WindowFlags_AlwaysVerticalScrollbar
ImGui.WindowFlags_MenuBar
ImGui.WindowFlags_NoBackground
ImGui.WindowFlags_AlwaysAutoResize

-- ImGuiTabItemFlags
ImGui.TabItemFlags_SetSelected
ImGui.TabItemFlags_NoCloseWithMiddleMouseButton
ImGui.TabItemFlags_NoTooltip
ImGui.TabItemFlags_None
ImGui.TabItemFlags_NoPushId
ImGui.TabItemFlags_UnsavedDocument

-- ImGuiComboFlags
ImGui.ComboFlags_HeightSmall
ImGui.ComboFlags_HeightLarge
ImGui.ComboFlags_PopupAlignLeft
ImGui.ComboFlags_None
ImGui.ComboFlags_NoPreview
ImGui.ComboFlags_HeightRegular
ImGui.ComboFlags_HeightMask_
ImGui.ComboFlags_NoArrowButton
ImGui.ComboFlags_HeightLargest

-- ImGuiCond
ImGui.Cond_Appearing
ImGui.Cond_None
ImGui.Cond_Always
ImGui.Cond_FirstUseEver
ImGui.Cond_Once

-- ImGuiSelectableFlags
ImGui.SelectableFlags_None
ImGui.SelectableFlags_SpanAllColumns
ImGui.SelectableFlags_AllowItemOverlap
ImGui.SelectableFlags_DontClosePopups
ImGui.SelectableFlags_AllowDoubleClick
ImGui.SelectableFlags_Disabled

-- ImGuiMouseCursor
ImGui.MouseCursor_Hand
ImGui.MouseCursor_ResizeAll
ImGui.MouseCursor_ResizeEW
ImGui.MouseCursor_Arrow
ImGui.MouseCursor_ResizeNS
ImGui.MouseCursor_None
ImGui.MouseCursor_NotAllowed
ImGui.MouseCursor_ResizeNWSE
ImGui.MouseCursor_ResizeNESW
ImGui.MouseCursor_TextInput

-- ImGuiMouseButton
ImGui.MouseButton_Right
ImGui.MouseButton_Middle
ImGui.MouseButton_Left

-- ImGuiColorEditFlags
ImGui.ColorEditFlags_AlphaPreview
ImGui.ColorEditFlags_DisplayRGB
ImGui.ColorEditFlags_DisplayHex
ImGui.ColorEditFlags_InputHSV
ImGui.ColorEditFlags_NoSidePreview
ImGui.ColorEditFlags_Uint8
ImGui.ColorEditFlags_HEX
ImGui.ColorEditFlags_AlphaPreviewHalf
ImGui.ColorEditFlags_Float
ImGui.ColorEditFlags_PickerHueWheel
ImGui.ColorEditFlags__OptionsDefault
ImGui.ColorEditFlags_InputRGB
ImGui.ColorEditFlags_HDR
ImGui.ColorEditFlags_NoPicker
ImGui.ColorEditFlags_RGB
ImGui.ColorEditFlags_AlphaBar
ImGui.ColorEditFlags_DisplayHSV
ImGui.ColorEditFlags_PickerHueBar
ImGui.ColorEditFlags_HSV
ImGui.ColorEditFlags_NoAlpha
ImGui.ColorEditFlags_NoOptions
ImGui.ColorEditFlags_NoDragDrop
ImGui.ColorEditFlags_NoInputs
ImGui.ColorEditFlags_None
ImGui.ColorEditFlags_NoSmallPreview
ImGui.ColorEditFlags_NoBorder
ImGui.ColorEditFlags_NoLabel
ImGui.ColorEditFlags_NoTooltip

-- ImGuiDragDropFlags
ImGui.DragDropFlags_SourceNoPreviewTooltip
ImGui.DragDropFlags_SourceAllowNullID
ImGui.DragDropFlags_AcceptNoDrawDefaultRect
ImGui.DragDropFlags_AcceptPeekOnly
ImGui.DragDropFlags_AcceptBeforeDelivery
ImGui.DragDropFlags_SourceNoHoldToOpenOthers
ImGui.DragDropFlags_AcceptNoPreviewTooltip
ImGui.DragDropFlags_SourceAutoExpirePayload
ImGui.DragDropFlags_SourceExtern
ImGui.DragDropFlags_None
ImGui.DragDropFlags_SourceNoDisableHover

-- ImDrawCornerFlags
ImGui.CornerFlags_None
ImGui.CornerFlags_TopLeft
ImGui.CornerFlags_TopRight
ImGui.CornerFlags_BotLeft
ImGui.CornerFlags_BotRight
ImGui.CornerFlags_Top
ImGui.CornerFlags_Bot
ImGui.CornerFlags_Left
ImGui.CornerFlags_Right
ImGui.CornerFlags_All

-- ImGuiConfigFlags
ImGui.ConfigFlags_None                   
ImGui.ConfigFlags_NavEnableKeyboard      
ImGui.ConfigFlags_NavEnableGamepad       
ImGui.ConfigFlags_NavEnableSetMousePos   
ImGui.ConfigFlags_NavNoCaptureKeyboard   
ImGui.ConfigFlags_NoMouse                
ImGui.ConfigFlags_NoMouseCursorChange    
ImGui.ConfigFlags_IsSRGB                 
ImGui.ConfigFlags_IsTouchScreen

-- ImGuiBackendFlags
ImGui.BackendFlags_None
ImGui.BackendFlags_HasGamepad
ImGui.BackendFlags_HasMouseCursors
ImGui.BackendFlags_HasSetMousePos
ImGui.BackendFlags_RendererHasVtxOffset

-- ImGuiSliderFlags
ImGui.SliderFlags_None        
ImGui.SliderFlags_ClampOnInpuе  
ImGui.SliderFlags_Logarithmic  
ImGui.SliderFlags_NoRoundToFormat
ImGui.SliderFlags_NoInput
```
[To top](#api)
# DRAW LIST COMMANDS
```lua
-- ImDrawList *list = ImGui::GetWindowDrawList()

ImGui:drawListPushClipRect(clip_rect_min_x, clip_rect_min_y, clip_rect_max_x, clip_rect_max_y, [intersect_with_current_clip_rect = false])
ImGui:drawListPushClipRectFullScreen()
ImGui:drawListPopClipRect()
ImGui:drawListPushTextureID(texture)
ImGui:drawListPopTextureID()
x, y = ImGui:drawListGetClipRectMin()
x, y = ImGui:drawListGetClipRectMax()
ImGui:drawListAddLine(p1_x, p1_y, p2_x, p2_y, color, [thickness = 1])
ImGui:drawListAddRect(p_min_x, p_min_y, p_max_x, p_max_y, color, [rounding = 0, rounding_corners = ImGui.CornerFlags_All, thickness = 1])
ImGui:drawListAddRectFilled(p_min_x, p_min_y, p_max_x, p_max_y, color, [rounding = 0, rounding_corners = ImGui.CornerFlags_All])
ImGui:drawListAddRectFilledMultiColor(p_min_x, p_min_y, p_max_x, p_max_y, color_upr_left, color_upr_right, color_bot_right, color_bot_left)
ImGui:drawListAddQuad(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color, [thickness = 1])
ImGui:drawListAddQuadFilled(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color)
ImGui:drawListAddTriangle(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, color, [thickness = 1])
ImGui:drawListAddTriangleFilled(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, color)
ImGui:drawListAddCircle(center_x, center_y, radius, color, [num_segments = 12, thickness = 1])
ImGui:drawListAddCircleFilled(center_x, center_y, radius, color, [num_segments = 12])
ImGui:drawListAddNgon(center_x, center_y, radius, color, [num_segments = 12, thickness = 1])
ImGui:drawListAddNgonFilled(center_x, center_y, radius, color, [num_segments = 12])
ImGui:drawListAddText(x, y, color, text_begin, [text_end]) -- x, y (number), text_begin (string), text_end (string)
ImGui:drawListAddPolyline(pointsTable, color, closed, thickness) -- pointsTable (table), color (number), closed (bool), thickness (number)
ImGui:drawListAddConvexPolyFilled(pointsTable, color) -- pointsTable (table), color (number)
ImGui:drawListAddBezierCurve(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color, thickness, [num_segments = 0])
ImGui:drawListAddImage(texture, p_min_x, p_min_y, [color = 0xffffff, 1, uv_max_x, uv_max_y, uv_min_x = 0, uv_min_y = 0])
ImGui:drawListAddImageQuad(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, [color = 0xffffff, 1, uv1_x, uv1_y, uv2_x, uv2_y, uv3_x, uv3_y, uv4_x, uv4_y])
ImGui:drawListAddImageRounded(p_min_x, p_min_y, color, rounding, [rounding_corners = ImGui.CornerFlags_All, uv_max_x, uv_max_y, uv_min_x, uv_min_y])
ImGui:drawListPathClear()
ImGui:drawListPathLineTo(x, y)
ImGui:drawListPathLineToMergeDuplicate(x, y)
ImGui:drawListPathFillConvex(color)
ImGui:drawListPathStroke(color, closed, [thickness = 1])
ImGui:drawListPathArcTo(centerX, centerY, radius, a_min, a_max, [num_segments = 10])
ImGui:drawListPathArcToFast(centerX, centerY, radius, a_min, a_max)
ImGui:drawListPathBezierCurveTo(p2x, p2y, p3x, p3y, p4x, p4y, [num_segments = 0])
ImGui:drawListPathRect(minX, minY, maxX, maxY, [rounding = 0, ImDrawCornerFlags = 0])
```
[To top](#api)
# FONTS (W.I.P)
```lua
--ImGui:addFonts()
```
[To top](#api)
# INPUTS

```lua
ImGui:onMouseHover(event)
ImGui:onMouseMove(event)
ImGui:onMouseDown(event)
ImGui:onMouseUp(event)
ImGui:onMouseWheel(event)

ImGui:onKeyUp(event)
ImGui:onKeyDown(event)
ImGui:onKeyChar(event)
```
[To top](#api)
# DEFAULT STYLES 
```lua
ImGui:setDarkStyle()
ImGui:setLightStyle()
ImGui:setClassicStyle()
```
[To top](#api)
# IO Functions
```lua
-- ImGuiIO& io = ImGui::GetIO();

ImGuiConfigFlag = ImGui:ioGetConfigFlags()
ImGui:ioSetConfigFlags(ImGuiConfigFlag)
ImGuiBackendFlag = ImGui:ioGetBackendFlags()
ImGui:ioSetBackendFlags(ImGuiBackendFlag)
number = ImGui:ioGetIniSavingRate()
ImGui:ioSetIniSavingRate(number)
string = ImGui:ioGetIniFilename()
ImGui:ioSetIniFilename(string)
string = ImGui:ioGetLogFilename()
ImGui:ioSetLogFilename(string)
number = ImGui:ioGetMouseDoubleClickTime()
ImGui:ioSetMouseDoubleClickTime(number)
number = ImGui:ioGetMouseDragThreshold()
ImGui:ioSetMouseDragThreshold(number)
number = ImGui:ioGetMouseDoubleClickMaxDist()
ImGui:ioSetMouseDoubleClickMaxDist(number)
index = ImGui:ioGetKeyMapValue()
ImGui:ioSetKeyMapValue(index, value)
number = ImGui:ioGetKeyRepeatDelay()
ImGui:ioSetKeyRepeatDelay(number)
number = ImGui:ioGetKeyRepeatRate()
ImGui:ioSetKeyRepeatRate(number)
number = ImGui:ioGetFontGlobalScale()
ImGui:ioSetFontGlobalScale(number)
bool = ImGui:ioGetFontAllowUserScaling()
ImGui:ioSetFontAllowUserScaling(bool)
number, number = ImGui:ioGetDisplayFramebufferScale()
ImGui:ioSetDisplayFramebufferScale(number, number)
bool = ImGui:ioGetConfigMacOSXBehaviors()
ImGui:ioSetConfigMacOSXBehaviors(bool)
bool = ImGui:ioGetConfigInputTextCursorBlink()
ImGui:ioSetConfigInputTextCursorBlink(bool)
bool = ImGui:ioGetConfigWindowsResizeFromEdges()
ImGui:ioSetConfigWindowsResizeFromEdges(bool)
bool = ImGui:ioGetConfigWindowsMoveFromTitleBarOnly()
ImGui:ioSetConfigWindowsMoveFromTitleBarOnly(bool)
number = ImGui:ioGetConfigWindowsMemoryCompactTimer()
ImGui:ioSetConfigWindowsMemoryCompactTimer(number)
string = ImGui:ioGetBackendPlatformName()
string = ImGui:ioGetBackendRendererName()
bool = ImGui:ioIsMouseDown(button)
number = ImGui:ioGetMouseWheel()
number = ImGui:ioGetMouseWheelH()
bool = ImGui:ioIsKeyCtrl()
bool = ImGui:ioIsKeyShift()
bool = ImGui:ioIsKeyAlt()
bool = ImGui:ioIsKeySuper()
bool = ImGui:ioGetKeysDown()

bool = ImGui:isMouseDown(button)
bool = ImGui:isMouseClicked(button)
bool = ImGui:isMouseReleased(button)
bool = ImGui:isMouseDoubleClicked(button)
bool = ImGui:isMouseHoveringRect()
bool = ImGui:isMousePosValid()
bool = ImGui:isAnyMouseDown()
x, y = ImGui:getMousePos()
x, y = ImGui:getMousePosOnOpeningCurrentPopup()
ImGui:isMouseDragging(button)
dx, dy = ImGui:getMouseDragDelta(button)
ImGui:resetMouseDragDelta(button)

flag = ImGui:wantCaptureMouse()
flag = ImGui:wantCaptureKeyboard()
flag = ImGui:wantTextInput()
flag = ImGui:wantSetMousePos()
flag = ImGui:wantSaveIniSettings()
flag = ImGui:isNavActive()
flag = ImGui:isNavVisible()
number = ImGui:getFramerate()
number = ImGui:getMetricsRenderVertices()
number = ImGui:getMetricsRenderIndices()
number = ImGui:getMetricsRenderWindows()
number = ImGui:getMetricsActiveWindows()
number = ImGui:getMetricsActiveAllocations()
number = ImGui:getMouseDelta(button)
number = ImGui:getMouseDownSec(button)
```
[To top](#api)
# WINDGETS & STUFF
# Windows
```lua
isOpenFlag = ImGui:beginWindow()
ImGui:endWindow()
```
[To top](#api)
# Child Windows
```lua
ImGui:beginChild(id, [w = 0, h = 0, borderFlag = false, ImGuiWindowFlags = 0)
ImGui:endChild()
```
[To top](#api)
# Windows Utilities
```lua
flag = ImGui:isWindowAppearing()
flag = ImGui:isWindowCollapsed()
flag = ImGui:isWindowFocused([ImGuiFocusedFlags = 0])
flag = ImGui:isWindowHovered([ImGuiHoveredFlags = 0])
x, y = ImGui:getWindowPos()
w, h = ImGui:getWindowSize()
w = ImGui:getWindowWidth()
h = ImGui:getWindowHeight()
 
ImGui:setNextWindowPos(x, y, [ImGuiCond = 0, pivotX = 0, pivotY = 0])
ImGui:setNextWindowSize(w, h, [ImGuiCond = 0])
ImGui:setNextWindowContentSize(w, h)
ImGui:setNextWindowCollapsed(flag, [ImGuiCond = 0])
ImGui:setNextWindowFocus()
ImGui:setNextWindowBgAlpha(alpha)
ImGui:setWindowPos(name, x, y, [ImGuiCond = 0]) OR ImGui:setWindowPos(x, y, [ImGuiCond = 0])
ImGui:setWindowSize(name, w, h, [ImGuiCond = 0]) OR ImGui:setWindowSize(w, h, [ImGuiCond = 0])
ImGui:setWindowCollapsed(name, flag, [ImGuiCond = 0]) OR ImGui:setWindowCollapsed(flag, [ImGuiCond = 0])
ImGui:setWindowFocus(name) OR ImGui:setWindowFocus()
ImGui:setWindowFontScale(scale)
```
[To top](#api)
# Content region 
```lua
scaleX, scaleY = ImGui:getContentRegionMax()
w, h = ImGui:getContentRegionAvail()
x, y = ImGui:getWindowContentRegionMin()
x, y = ImGui:getWindowContentRegionMax()
w = ImGui:getWindowContentRegionWidth()
```
[To top](#api)
# Windows Scrolling
```lua
x = ImGui:getScrollX()
y = ImGui:getScrollY()
maxX = ImGui:getScrollMaxX()
maxY = ImGui:getScrollMaxY()
ImGui:setScrollX(value)
ImGui:setScrollY(value)
ImGui:setScrollHereX([ratio = 0.5])
ImGui:setScrollHereY([ratio = 0.5])
ImGui:setScrollFromPosX(x, [ratio = 0.5])
ImGui:setScrollFromPosY(y, [ratio = 0.5])
```
[To top](#api)
# Parameters stacks (shared)
```lua
ImGui:pushStyleColor(color)
ImGui:popStyleColor([count = 1])
ImGui:pushStyleVar(idx, value) OR ImGui:pushStyleVar(idx, value1, value2)
ImGui:popStyleVar([count = 1])
color, alpha = ImGui:getStyleColor(ImGui.Col_XXX) -- pass ImGui constant
fontSize = ImGui:getFontSize()
```
[To top](#api)
# Parameters stacks (current window) 
```lua
ImGui:pushItemWidth(w)
ImGui:popItemWidth()
ImGui:setNextItemWidth(w)
w = ImGui:calcItemWidth()
ImGui:pushTextWrapPos([localX = 0])
ImGui:popTextWrapPos()
ImGui:pushAllowKeyboardFocus(flag)
ImGui:popAllowKeyboardFocus()
ImGui:pushButtonRepeat(flag)
ImGui:popButtonRepeat()
```
[To top](#api)
# Cursor / Layout
```lua
ImGui:separator()
ImGui:sameLine([offsetX = 0, spacing = -1])
ImGui:newLine()
ImGui:spacing()
ImGui:dummy(w, h)
ImGui:indent([indent = 0])
ImGui:unindent([indent = 0])
ImGui:beginGroup()
ImGui:endGroup()
 
x, y = ImGui:getCursorPos()
x = ImGui:getCursorPosX()
y = ImGui:getCursorPosY()
x, y = ImGui:getCursorStartPos()
x, y = ImGui:getCursorScreenPos()
ImGui:alignTextToFramePadding()
lineH = ImGui:getTextLineHeight()
lineH = ImGui:getTextLineHeightWithSpacing()
frameH = ImGui:getFrameHeight()
frameH = ImGui:getFrameHeightWithSpacing()
```
[To top](#api)
# ID stack/scopes
```lua
ImGui:pushID(anyValue) 
ImGui:pushID(strBegin, strEnd)
ImGui:popID()
number = ImGui:getID(anyValue)
number = ImGui:getID(strBegin, strEnd)
```
[To top](#api)
# Widgets: Text
```lua
ImGui:textUnformatted(text, [textEnd])
ImGui:text(text)
ImGui:textColored(text, color)
ImGui:textDisabled(text)
ImGui:textWrapped(text)
ImGui:labelText(text, label)
ImGui:bulletText(text)
```
[To top](#api)
# Widgets: Main
```lua
pressFlag = ImGui:button(text, [w = 0, h = 0])
pressFlag = ImGui:smallButton(text)
pressFlag = ImGui:invisibleButton(stringID, [w = 0, h = 0])
pressFlag = ImGui:arrowButton(stringID, [ImGuiDir = 0])
ImGui:image(texture, w, h, [tintColor = 0xffffff, 1, borderColor = 0xffffff, 0, uv1x = 1, uv1y = 1, uv0x = 0, uv0y = 0])
pressFlag = ImGui:imageButton(texture, w, h, [padding = -1, tintColor = 0xffffff, 1, borderColor = 0xffffff, 0, uv1x = 1, uv1y = 1, uv0x = 0, uv0y = 0])
pressFlag = ImGui:checkbox(text, flag)
ImGui:checkboxFlags(label, ) -- TODO
pressFlag = ImGui:radioButton(text, flag)
ImGui:progressBar(fraction, [anchorX = -1, anchorY = 0, overlayString = nil])
ImGui:bullet()
```
[To top](#api)
# Widgets: Combo Box
```lua
openFlag = ImGui:beginCombo(text, previewText, [ImGuiComboFlags = 0])
ImGui:endCombo()
currentItem, isOpen = ImGui:combo(label, currentItem, items) -- items (table): {"item1", "item2", ...}
```
[To top](#api)
# Widgets: Drags 
```lua
value, isDragingFlag = ImGui:dragFloat(label, value, [incStep = 1, min = 0, max = 0, formatString = "%.3f", power = 1])
value1, value2, isDragingFlag = ImGui:dragFloat2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%.3f", power = 1])
value1, value2, value3, isDragingFlag = ImGui:dragFloat3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%.3f", power = 1])
value1, value2, value3, value4, isDragingFlag = ImGui:dragFloat4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%.3f", power = 1])
valueMin, valueMax, isDragingFlag = ImGui:dragFloatRange2(label, valueMin, valueMax, [incStep = 1, min = 0, max = 0, formatMinString = "%.3f", formatMaxString = formatMinString, power = 1])
 
value, isDragingFlag = ImGui:dragInt(label, value, [incStep = 1, min = 0, max = 0, formatString = "%d"])
value1, value2, isDragingFlag = ImGui:dragInt2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%d"])
value1, value2, value3, isDragingFlag = ImGui:dragInt3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%d"])
value1, value2, value3, value4, isDragingFlag = ImGui:dragInt4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%d"])
ImGui:dragIntRange2(label, )
ImGui:dragScalar()
```
[To top](#api)
# Widgets: Sliders
```lua
value, isDragingFlag = ImGui:sliderFloat(label, value, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, isDragingFlag = ImGui:sliderFloat2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, isDragingFlag = ImGui:sliderFloat3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isDragingFlag = ImGui:sliderFloat4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
valueInRad, isDragingFlag = ImGui:sliderAngle(label, valueInRad, [min_degrees = -360, max_degrees = 360, formatString = "%.0f deg", ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:sliderInt(label, value, [incStep = 1, min = 0, max = 0, formatString = "%d, ImGuiSliderFlags = 0"])
value1, value2, isDragingFlag = ImGui:sliderInt2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, isDragingFlag = ImGui:sliderInt3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isDragingFlag = ImGui:sliderInt4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:sliderScalar(label, ImGuiDataType, value, [min = nil, max = nil, formatString = nil, ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:vSliderFloat(label, w, h, value, min, max, [formatString = "%.3f", ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:vSliderInt(label, w, h, value, min, max, [formatString = "%d", ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:vSliderScalar(label, w, h, ImGuiDataType, [min = nil, max = nil, formatString = nil, ImGuiSliderFlags = 0])

value, isDragingFlag = ImGui:filledSliderFloat(label, mirrorFlag, value, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, isDragingFlag = ImGui:filledSliderFloat2(label, mirrorFlag, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, isDragingFlag = ImGui:filledSliderFloat3(label, mirrorFlag, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isDragingFlag = ImGui:filledSliderFloat4(label, mirrorFlag, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
valueInRad, isDragingFlag = ImGui:filledSliderAngle(label, mirrorFlag, valueInRad, [min_degrees = -360, max_degrees = 360, formatString = "%.0f deg", ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:filledSliderInt(label, mirrorFlag, value, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, isDragingFlag = ImGui:filledSliderInt2(label, mirrorFlag, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, isDragingFlag = ImGui:filledSliderInt3(label, mirrorFlag, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isDragingFlag = ImGui:filledSliderInt4(label, mirrorFlag, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:filledSliderScalar(label, mirrorFlag, ImGuiDataType, value, [min = nil, max = nil, formatString = nil, ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:vFilledSliderFloat(label, mirrorFlag, w, h, value, min, max, [formatString = "%.3f", ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:vFilledSliderInt(label, mirrorFlag, w, h, value, min, max, [formatString = "%d", ImGuiSliderFlags = 0])
value, isDragingFlag = ImGui:vFilledSliderScalar(label, mirrorFlag, w, h, ImGuiDataType, [min = nil, max = nil, formatString = nil, ImGuiSliderFlags = 0])
```
[To top](#api)
# Widgets: Input with Keyboard
WIP (!!WARNING!! TRY NOT TO USE IT FOR NOW)
```lua
text, isTypingFlag = ImGui:inputText(label, initialText, [ImGuiInputTextFlags = 0])
--ImGui:InputTextMultiline()
--ImGui:InputTextWithHint()
--ImGui:InputFloat()
--ImGui:InputFloat2()
--ImGui:InputFloat3()
--ImGui:InputFloat4()
--ImGui:InputInt()
--ImGui:InputInt2()
--ImGui:InputInt3()
--ImGui:InputInt4()
--ImGui:InputDouble()
--ImGui:InputScalar()
```
[To top](#api)
# Widgets: Color Editor/Picker
```lua
hexColor, isTouchingFlag = ImGui:colorEdit3(label, color, [ImGuiColorEditFlags = 0]) -- alpha ignored, no need to pass it!
hexColor, alpha, isTouchingFlag = ImGui:colorEdit4(label, color, [ImGuiColorEditFlags = 0])
hexColor, isTouchingFlag = ImGui:colorPicker3(label, color, [ImGuiColorEditFlags = 0])
hexColor, alpha, originalColor, originalAlpha, isTouchingFlag = ImGui:colorPicker4(label, color, [originalColor = 0xffffff, 1, ImGuiColorEditFlags = 0])
isHoveringFlag = ImGui:colorButton(stringID, color, [ImGuiColorEditFlags = 0, w = 0, h = 0])
ImGui:setColorEditOptions(ImGuiColorEditFlags)
```
[To top](#api)
# Widgets: Trees
```lua
isOpenFlag = ImGui:treeNode(label, [formatString])
ImGui:treeNodeEx(label, ImGui.TreeNodeFlags, [formatString])
ImGui:treePush(str_id)
ImGui:treePop()
number = ImGui:getTreeNodeToLabelSpacing()
isOpenFlag, p_open = ImGui:collapsingHeader(label, p_open, [ImGuiTreeNodeFlags = 0])
isOpenFlag = ImGui:collapsingHeader(label, [ImGuiTreeNodeFlags = 0])
ImGui:setNextItemOpen(is_open, ImGuiCond)
```
[To top](#api)
# Widgets: Selectables
```lua
result?, selected = ImGui:selectable(label, selected, [ImGuiSelectableFlags = 0, w = 0, h = 0])
```
[To top](#api)
# Widgets: List Boxes
```lua
current_item, isOpenFlag = ImGui:listBox(label, current_item, itemTable, [max_visible_items = -1]) -- itemTable: {"Item0", "Item1", ...}
result? = ImGui:listBoxHeader(label, [w = 0, h = 0])
result? = ImGui:listBoxHeader2(label, items_count)
ImGui:listBoxFooter()
```
[To top](#api)
# Widgets: Data Plotting
```lua
ImGui:plotLines(label, pointsTable, [values_offset = 0, overlay_text = nil, scale_min = math.huge, scale_max = math.huge, w = 0, h = 0]) -- pointsTable: {0.01, 0.5, 10, -50, ...}
ImGui:plotHistogram(label, pointsTable, [values_offset = 0, overlay_text = nil, scale_min = math.huge, scale_max = math.huge, w = 0, h = 0])"plotLines"
```
[To top](#api)
# Widgets: Value() Helpers
```lua
ImGui:value(prefix, bool)
ImGui:value(prefix, number)
ImGui:value(prefix, float, formatString)
```
[To top](#api)
# Widgets: Menus
```lua
result? = ImGui:beginMenuBar()
ImGui:endMenuBar()
result? = ImGui:beginMainMenuBar()
ImGui:endMainMenuBar()
result? = ImGui:beginMenu(label, enabledFlag)
ImGui:endMenu()
result? = ImGui:menuItem(label, [shortcut = nil, selected = false, enabled = true])
selected, result? = ImGui:menuItemWithShortcut(label, shortcut, [selected = false, enabled = true])
ImGui:beginTooltip()
ImGui:endTooltip()
ImGui:setTooltip(text)
```
[To top](#api)
# Popups, Modals
```lua
result? = ImGui:beginPopup(str_id, [ImGuiWindowFlags = 0])
p_open, result? = ImGui:beginPopupModal(str_id, p_open, [ImGuiWindowFlags = 0])
ImGui:endPopup()
ImGui:openPopup(str_id, [ImGuiPopupFlags = 0])
result? = ImGui:openPopupContextItem(str_id, [ImGuiPopupFlags = 0])
ImGui:closeCurrentPopup()
result? = ImGui:beginPopupContextItem(str_id, [ImGuiPopupFlags = 0])
result? = ImGui:beginPopupContextWindow(str_id, [ImGuiPopupFlags = 0])
result? = ImGui:beginPopupContextVoid(str_id, [ImGuiPopupFlags = 0])
result? = ImGui:isPopupOpen(str_id, [ImGuiPopupFlags = 0])
```
[To top](#api)
# Columns
```lua
ImGui:columns([count = 1, id = nil, border = true])
ImGui:nextColumn()
index = ImGui:getColumnIndex()
width = ImGui:getColumnWidth([column_index = -1])
ImGui:setColumnWidth(column_index, width)
offset = ImGui:getColumnOffset([column_index = -1])
ImGui:setColumnOffset(column_index, offset)
number = ImGui:getColumnsCount()
```
[To top](#api)
# Tab Bars, Tabs
```lua
bool = ImGui:beginTabBar(str_id, [ImGuiTabBarFlags = 0])
ImGui:endTabBar()
p_open, bool = ImGui:beginTabItem(label, p_open, [ImGuiTabItemFlags = 0])
ImGui:endTabItem()
ImGui:setTabItemClosed(tab_or_docked_window_label)
```
[To top](#api)
# Clipping
```lua
ImGui:pushClipRect(minX, minY, maxX, maxY, intersect_with_current_clip_rect)
ImGui:popClipRect()
```
[To top](#api)
# Focus, Activation
```lua
ImGui:setItemDefaultFocus()
ImGui:setKeyboardFocusHere([offset = 0])
flag = ImGui:isItemHovered([ImGuiHoveredFlags = 0])
flag = ImGui:isItemActive()
flag = ImGui:isItemFocused()
flag = ImGui:isItemClicked(mouse_button)
flag = ImGui:isItemVisible()
flag = ImGui:isItemEdited()
flag = ImGui:isItemActivated()
flag = ImGui:isItemDeactivated()
flag = ImGui:isItemDeactivatedAfterEdit()
flag = ImGui:isItemToggledOpen()
flag = ImGui:isAnyItemHovered()
flag = ImGui:isAnyItemActive()
flag = ImGui:isAnyItemFocused()
x, y = ImGui:getItemRectMin()
x, y = ImGui:getItemRectMax()
w, h = ImGui:getItemRectSize()
ImGui:setItemAllowOverlap()
```
[To top](#api)
# Miscellaneous Utilities
```
flag = isRectVisible(w, h, [maxX, maxY])
number = getTime()
number = getFrameCount()
str = getStyleColorName(idx)
out_items_display_start, out_items_display_end = calcListClipping(items_count, items_height, out_items_display_start, out_items_display_end)
flag = beginChildFrame(id, w, h, [ImGuiWindowFlags = 0]) -- id (number)
endChildFrame()
```
[To top](#api)
# Text Utilities
```
w, h = calcTextSize(text, [text_end = nul, hide_text_after_double_hash = false, wrap_width = -1])
```
[To top](#api)
# Inputs Utilities: Keyboard
```
number = getKeyIndex(ImGuiKey)
flag = isKeyDown(user_key_index)
flag = isKeyPressed(user_key_index, [repeat = true])
flag = isKeyReleased(user_key_index)
number = getKeyPressedAmount(key_index, repeat_delay, rate)
captureKeyboardFromApp([want_capture_keyboard_value = true])
```
[To top](#api)
# Inputs Utilities: Mouse
```
flag = isMouseDown(mouse_button)
flag = isMouseClicked(mouse_button, [repeat = false])
flag = isMouseReleased(mouse_button)
flag = isMouseDoubleClicked(mouse_button)
flag = isMouseHoveringRect(minX, minY, maxX, maxY, [clip = true])
flag = isMousePosValid([x = inf, y = inf])
flag = isAnyMouseDown()
x, y = getMousePos()
x, y = getMousePosOnOpeningCurrentPopup()
flag = isMouseDragging(mouse_button, [lock_threshold = -1])
x, y = getMouseDragDelta(mouse_button, [lock_threshold = -1])
resetMouseDragDelta(mouse_button)
```
[To top](#api)
# Render
```lua
ImGui:newFrame()
ImGui:render()
ImGui:endFrame()
```
[To top](#api)
# Display size
```lua
ImGui:setDisplaySize(x, y)
ImGui:setDisplayScale(x, y)
```
[To top](#api)
# Demos
```lua
isOpenFlag = ImGui:showUserGuide()
isOpenFlag = ImGui:showDemoWindow()
isOpenFlag = ImGui:showAboutWindow()
isOpenFlag = ImGui:showStyleEditor()
isOpenFlag = ImGui:showFontSelector()
isOpenFlag = ImGui:showMetricsWindow()
isOpenFlag = ImGui:showStyleSelector()
```
[To top](#api)
