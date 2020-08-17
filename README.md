# API

!VERY IMPORTANT!
every color related argument is actualy 2 arguments: color it self in hex format: 0xRRGGBB AND alpha [0..1]
for example:
ImGui:drawListAddRect(p_min_x, p_min_y, p_max_x, p_max_y, color, [rounding = 0, rounding_corners = ImGui.CornerFlags_All, thickness = 1])
usage: imgui:drawListAddRect(0,0, 100,100, 0xff0000, 1, ROUNDING, ROUNDING_CORNERS, THICKNESS)</br>
                                           ^-color   ^-alpha 

```lua
-------------------------------------------------------------------------------
------------------------------------ ENUMS ------------------------------------
-------------------------------------------------------------------------------

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


-------------------------------------------------------------------------------
----------------------------- DRAW LIST COMMANDS ------------------------------
---------------- ImDrawList *list = ImGui::GetWindowDrawList() ----------------
-------------------------------------------------------------------------------

ImGui:drawListPushClipRect(clip_rect_min_x, clip_rect_min_y, clip_rect_max_x, clip_rect_max_y, [intersect_with_current_clip_rect = false])
ImGui:drawListPushClipRectFullScreen()
ImGui:drawListPopClipRect()
ImGui:drawListPushTextureID(texture)
ImGui:drawListPopTextureID()
x, y = ImGui:drawListGetClipRectMin()
x, y = ImGui:drawListGetClipRectMax()
ImGui:drawListAddLine(p1_x, p1_y, p2_x, p2_y, color, [alpha = 1, thickness = 1])
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
-- WIP:
--ImGui:drawListPathLineTo()
--ImGui:drawListPathLineToMergeDuplicate()
--ImGui:drawListPathFillConvex()
--ImGui:drawListPathStroke()
--ImGui:drawListPathArcTo()
--ImGui:drawListPathArcToFast()
--ImGui:drawListPathBezierCurveTo()
--ImGui:drawListPathRect()

-- WIP:
--ImGui:addFonts()

-------------------------------------------------------------------------------
----------------------------------- INPUTS ------------------------------------
-------------------------------------------------------------------------------

ImGui:onMouseHover(event)
ImGui:onMouseMove(event)
ImGui:onMouseDown(event)
ImGui:onMouseUp(event)
ImGui:onMouseWheel(event)

ImGui:onKeyUp(event)
ImGui:onKeyDown(event)
ImGui:onKeyChar(event)

-------------------------------------------------------------------------------
------------------------------- DEFAULT STYLES --------------------------------
-------------------------------------------------------------------------------

ImGui:setDarkStyle()
ImGui:setLightStyle()
ImGui:setClassicStyle()

-------------------------------------------------------------------------------
--------------------------------- IO Functions --------------------------------
------------------------ ImGuiIO& io = ImGui::GetIO(); ------------------------
-------------------------------------------------------------------------------

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

bool = ImGui:isMouseDown()
bool = ImGui:isMouseClicked()
bool = ImGui:isMouseReleased()
bool = ImGui:isMouseDoubleClicked()
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
number = ImGui:getMouseDelta()
number = ImGui:getMouseDownSec()

-------------------------------------------------------------------------------
------------------------------ WINDGETS & STUFF -------------------------------
-------------------------------------------------------------------------------
ImGui:beginChild(id, [w = 0, h = 0, borderFlag = false, ImGuiWindowFlags = 0)
ImGui:endChild()
 
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
 
scaleX, scaleY = ImGui:getContentRegionMax()
w, h = ImGui:getContentRegionAvail()
x, y = ImGui:getWindowContentRegionMin()
x, y = ImGui:getWindowContentRegionMax()
w = ImGui:getWindowContentRegionWidth()
 
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
 
ImGui:pushStyleColor(hex, [alpha = 1])
ImGui:popStyleColor([count = 1])
ImGui:pushStyleVar(idx, value) OR ImGui:pushStyleVar(idx, value1, value2)
ImGui:popStyleVar([count = 1])
--ImGui:getStyleColorVec4()
--ImGui:getColorU32()
fontSize = ImGui:getFontSize()
 
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
 
ImGui:pushSID(strID) OR ImGui:pushSID(strBegin, strEnd)
ImGui:pushNID(id)
ImGui:popID()
ImGui:getSID(strBegin, strEnd)
 
ImGui:textUnformatted(text, [textEnd])
ImGui:text(text)
ImGui:textColored(text, hex, [alpha = 1])
ImGui:textDisabled(text)
ImGui:textWrapped(text)
ImGui:labelText(text, label)
ImGui:bulletText(text)
 
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
openFlag = ImGui:beginCombo(text, previewText, [ImGuiComboFlags = 0])
ImGui:endCombo()
currentItem, isOpen = ImGui:combo(label, currentItem, items) -- items (table): {"item1", "item2", ...}
 
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
 
value, isDragingFlag = ImGui:sliderFloat(label, value, [incStep = 1, min = 0, max = 0, formatString = "%.3f", power = 1])
value1, value2, isDragingFlag = ImGui:sliderFloat2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%.3f", power = 1])
value1, value2, value3, isDragingFlag = ImGui:sliderFloat3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%.3f", power = 1])
value1, value2, value3, value4, isDragingFlag = ImGui:sliderFloat4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%.3f", power = 1])
valueInRad, isDragingFlag = ImGui:sliderAngle(label, valueInRad, [min_degrees = -360, max_degrees = 360, formatString = "%.0f deg"])
value, isDragingFlag = ImGui:sliderInt(label, value, [incStep = 1, min = 0, max = 0, formatString = "%d"])
value1, value2, isDragingFlag = ImGui:sliderInt2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%d"])
value1, value2, value3, isDragingFlag = ImGui:sliderInt3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%d"])
value1, value2, value3, value4, isDragingFlag = ImGui:sliderInt4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%d"])
value, isDragingFlag = ImGui:sliderScalar(label, ImGuiDataType, value, [min = nil, max = nil, formatString = nil])
value, isDragingFlag = ImGui:vSliderFloat(label, w, h, value, min, max, [formatString = "%.3f", power = 1])
value, isDragingFlag = ImGui:vSliderInt(label, w, h, value, min, max, [formatString = "%d"])
value, isDragingFlag = ImGui:vSliderScalar(label, w, h, ImGuiDataType, [min = nil, max = nil, formatString = nil, power = 1])
 
-- WIP
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
 
-- Colors
hexColor, isTouchingFlag = ImGui:colorEdit3(label, color, [ImGuiColorEditFlags = 0]) -- alpha ignored, no need to pass it!
hexColor, alpha, isTouchingFlag = ImGui:colorEdit4(label, color, [ImGuiColorEditFlags = 0])
hexColor, isTouchingFlag = ImGui:colorPicker3(label, color, [ImGuiColorEditFlags = 0])
hexColor, alpha, originalColor, originalAlpha, isTouchingFlag = ImGui:colorPicker4(label, color, [originalColor = 0xffffff, 1, ImGuiColorEditFlags = 0])
isHoveringFlag = ImGui:colorButton(stringID, color, [ImGuiColorEditFlags = 0, w = 0, h = 0])
ImGui:setColorEditOptions(ImGuiColorEditFlags)
isOpenFlag = ImGui:treeNode(label, [formatString])
ImGui:treeNodeEx(label, ImGui.TreeNodeFlags, [formatString])
ImGui:treePush(str_id)
ImGui:treePop()
number = ImGui:getTreeNodeToLabelSpacing()
ImGui:collapsingHeader()
ImGui:setNextItemOpen()
ImGui:selectable()
ImGui:listBox()
ImGui:listBoxHeader()
ImGui:listBoxHeader2()
ImGui:listBoxFooter()
ImGui:plotLines()
ImGui:plotHistogram()
ImGui:value()
ImGui:beginMenuBar()
ImGui:endMenuBar()
ImGui:beginMainMenuBar()
ImGui:endMainMenuBar()
ImGui:beginMenu()
ImGui:endMenu()
ImGui:menuItem()
ImGui:menuItemWithShortcut()
ImGui:beginTooltip()
ImGui:endTooltip()
ImGui:setTooltip()
ImGui:beginPopupModal()
ImGui:endPopup()
ImGui:openPopup()
ImGui:openPopupContextItem()
ImGui:closeCurrentPopup()
ImGui:beginPopupContextItem()
ImGui:beginPopupContextWindow()
ImGui:beginPopupContextVoid()
ImGui:isPopupOpen()
ImGui:columns()
ImGui:nextColumn()
ImGui:getColumnIndex()
ImGui:getColumnWidth()
ImGui:setColumnWidth()
ImGui:getColumnOffset()
ImGui:setColumnOffset()
ImGui:getColumnsCount()
ImGui:beginTabBar()
ImGui:endTabBar()
ImGui:beginTabItem()
ImGui:endTabItem()
ImGui:setTabItemClosed()
ImGui:pushClipRect()
ImGui:popClipRect()
ImGui:setItemDefaultFocus()
ImGui:setKeyboardFocusHere()
ImGui:isItemHovered()
ImGui:isItemActive()
ImGui:isItemFocused()
ImGui:isItemClicked()
ImGui:isItemVisible()
ImGui:isItemEdited()
ImGui:isItemActivated()
ImGui:isItemDeactivated()
ImGui:isItemDeactivatedAfterEdit()
ImGui:isItemToggledOpen()
ImGui:isAnyItemHovered()
ImGui:isAnyItemActive()
ImGui:isAnyItemFocused()
ImGui:getItemRectMin()
ImGui:getItemRectMax()
ImGui:getItemRectSize()
ImGui:setItemAllowOverlap()

-- Windows
isOpenFlag = ImGui:beginWindow()
ImGui:endWindow()
 
-- Render
ImGui:newFrame()
ImGui:render()
ImGui:endFrame()
 
-- Display size
ImGui:setDisplaySize(x, y)
ImGui:setDisplayScale(x, y)
 
-- Demos
isOpenFlag = ImGui:showUserGuide()
isOpenFlag = ImGui:showDemoWindow()
isOpenFlag = ImGui:showAboutWindow()
isOpenFlag = ImGui:showStyleEditor()
isOpenFlag = ImGui:showFontSelector()
isOpenFlag = ImGui:showMetricsWindow()
isOpenFlag = ImGui:showStyleSelector()
```
