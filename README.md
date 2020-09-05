# API

* [Fonts](#fonts-wip)
* [Inputs](#inputs)
* [Style setters/getters](#style-settersgetters)
* [Styles](#default-styles)
* [IO](#io-functions)
* [Widgets](#widgets--stuff)
* [Windows](#windows)
* [Child windows](#child-windows)
* [Windows utilities](#windows-utilities)
* [Content region](#content-region)
* [Windows scroll](#windows-scrolling)
* [Parameters stacks](#parameters-stacks-shared)
* [Cursor / layout](#cursor--layout)
* [ID](#id-stackscopes)
* [[Widgets] Text](#widgets-text)
* [[Widgets] Main](#widgets-main)
* [[Widgets] ComboBox](#widgets-combo-box)
* [[Widgets] Drags](#widgets-drags)
* [[Widgets] Sliders](#widgets-sliders)
* [[Widgets] Text input](#widgets-input-with-keyboard)
* [[Widgets] Color pickers](#widgets-color-editorpicker)
* [[Widgets] Trees](#widgets-trees)
* [[Widgets] Selectables](#widgets-selectables)
* [[Widgets] ListBox](#widgets-list-boxes)
* [[Widgets] Data ploting](#widgets-data-plotting)
* [[Widgets] Value helper](#widgets-value-helpers)
* [[Widgets] Menus](#widgets-menus)
* [Popups / Modals](#popups-modals)
* [Columns](#columns)
* [Tabs](#tab-bars-tabs)
* [Dock builder (BETA) ***NEW***](#dock-builder-beta)
* [Docking (BETA)](#docking-beta)
* [Logging/Capture](#loggingcapture)
* [Drag and drop (Beta API)](#drag-and-drop)
* [Clipping](#clipping)
* [Focus](#focus-activation)
* [Utilities](#miscellaneous-utilities)
* [Render](#render)
* [ImGui Demos](#demos)
* [ENUMS](#enums)
* [Custom drawing](#draw-lists)

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
## FONTS (W.I.P)
```lua
--ImGui:addFonts()
```
[To top](#api)
## INPUTS

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
## Style setters/getters
### Get style settings instance
```lua
local Style = ImGui:getStyle()
```
```lua
Style:setColor(ImGuiCol, color)
color = Style:getColor(ImGuiCol)
Style:setAlpha(value)
value = Style:getlAlpha()
Style:setWindowRounding(value)
value = Style:getlWindowRounding()
Style:setWindowBorderSize(value)
value = Style:getlWindowBorderSize()
Style:setChildRounding(value)
value = Style:getlChildRounding()
Style:setChildBorderSize(value)
value = Style:getlChildBorderSize()
Style:setPopupRounding(value)
value = Style:getlPopupRounding()
Style:setPopupBorderSize(value)
value = Style:getlPopupBorderSize()
Style:setFrameRounding(value)
value = Style:getlFrameRounding()
Style:setFrameBorderSize(value)
value = Style:getlFrameBorderSize()
Style:setIndentSpacing(value)
value = Style:getlIndentSpacing()
Style:setColumnsMinSpacing(value)
value = Style:getlColumnsMinSpacing()
Style:setScrollbarSize(value)
value = Style:getlScrollbarSize()
Style:setScrollbarRounding(value)
value = Style:getlScrollbarRounding()
Style:setGrabMinSize(value)
value = Style:getlGrabMinSize()
Style:setGrabRounding(value)
value = Style:getlGrabRounding()
Style:setLogSliderDeadzone(value)
value = Style:getlLogSliderDeadzone()
Style:setTabRounding(value)
value = Style:getlTabRounding()
Style:setTabBorderSize(value)
value = Style:getlTabBorderSize()
Style:setTabMinWidthForUnselectedCloseButton(value)
value = Style:getlTabMinWidthForUnselectedCloseButton()
Style:setMouseCursorScale(value)
value = Style:getlMouseCursorScale()
Style:setCurveTessellationTol(value)
value = Style:getlCurveTessellationTol()
Style:setCircleSegmentMaxError(value)
value = Style:getlCircleSegmentMaxError()
Style:setWindowPadding(x, y)
x, y = Style:getWindowPadding()
Style:setWindowMinSize(x, y)
x, y = Style:getWindowMinSize()
Style:setWindowTitleAlign(x, y)
x, y = Style:getWindowTitleAlign()
Style:setFramePadding(x, y)
x, y = Style:getFramePadding()
Style:setItemSpacing(x, y)
x, y = Style:getItemSpacing()
Style:setItemInnerSpacing(x, y)
x, y = Style:getItemInnerSpacing()
Style:setTouchExtraPadding(x, y)
x, y = Style:getTouchExtraPadding()
Style:setButtonTextAlign(x, y)
x, y = Style:getButtonTextAlign()
Style:setSelectableTextAlign(x, y)
x, y = Style:getSelectableTextAlign()
Style:setDisplayWindowPadding(x, y)
x, y = Style:getDisplayWindowPadding()
Style:setDisplaySafeAreaPadding(x, y)
x, y = Style:getDisplaySafeAreaPadding()
Style:setWindowMenuButtonPosition(ImGuiDir)
dir = Style:getWindowMenuButtonPosition()
Style:setColorButtonPosition(ImGuiDir)
dir = Style:getColorButtonPosition()
Style:setAntiAliasedLines(flag)
flag = Style:getAntiAliasedLines()
Style:setAntiAliasedLinesUseTex(flag)
flag = Style:getAntiAliasedLinesUseTex()
Style:setAntiAliasedFill(flag)
flag = Style:getAntiAliasedFill()
```
[To top](#api)
## DEFAULT STYLES 
```lua
Style:setDarkStyle()
ImGui:setLightStyle()
ImGui:setClassicStyle()
```
[To top](#api)
## IO Functions
### Get IO instance
```lua
local IO = ImGui:getIO()
```

```lua
ImGuiConfigFlag = IO:getConfigFlags()
IO:setConfigFlags(ImGuiConfigFlag)
IO:addConfigFlags(ImGuiConfigFlag)
ImGuiBackendFlag = IO:getBackendFlags()
IO:setBackendFlags(ImGuiBackendFlag)
number = IO:getIniSavingRate()
IO:setIniSavingRate(number)
string = IO:getIniFilename()
IO:setIniFilename(string)
string = IO:getLogFilename()
IO:setLogFilename(string)
number = IO:getMouseDoubleClickTime()
IO:setMouseDoubleClickTime(number)
number = IO:getMouseDragThreshold()
IO:setMouseDragThreshold(number)
flag = IO:getMouseDrawCursor()
IO:setMouseDrawCursor(flag)
number = IO:getMouseDoubleClickMaxDist()
IO:setMouseDoubleClickMaxDist(number)
index = IO:getKeyMapValue()
IO:setKeyMapValue(index, value)
number = IO:getKeyRepeatDelay()
IO:setKeyRepeatDelay(number)
number = IO:getKeyRepeatRate()
IO:setKeyRepeatRate(number)
number = IO:getFontGlobalScale()
IO:setFontGlobalScale(number)
bool = IO:getFontAllowUserScaling()
IO:setFontAllowUserScaling(bool)
number, number = IO:getDisplayFramebufferScale()
IO:setDisplayFramebufferScale(number, number)
bool = IO:getConfigMacOSXBehaviors()
IO:setConfigMacOSXBehaviors(bool)
bool = IO:getConfigInputTextCursorBlink()
IO:setConfigInputTextCursorBlink(bool)
bool = IO:getConfigWindowsResizeFromEdges()
IO:setConfigWindowsResizeFromEdges(bool)
bool = IO:getConfigWindowsMoveFromTitleBarOnly()
IO:setConfigWindowsMoveFromTitleBarOnly(bool)
number = IO:getConfigWindowsMemoryCompactTimer()
IO:setConfigWindowsMemoryCompactTimer(number)
string = IO:getBackendPlatformName()
string = IO:getBackendRendererName()
bool = IO:IsMouseDown(button)
number = IO:getMouseWheel()
number = IO:getMouseWheelH()
bool = IO:isKeyCtrl()
bool = IO:isKeyShift()
bool = IO:isKeyAlt()
bool = IO:isKeySuper()
flag = IO:getKeysDown(key_index)
flag = IO:isNavActive()
flag = IO:isNavVisible()
flag = IO:wantCaptureMouse()
flag = IO:wantCaptureKeyboard()
flag = IO:wantTextInput()
flag = IO:wantSetMousePos()
flag = IO:wantSaveIniSettings()
number = IO:getFramerate()
number = IO:getMetricsRenderVertices()
number = IO:getMetricsRenderIndices()
number = IO:getMetricsRenderWindows()
number = IO:getMetricsActiveWindows()
number = IO:getMetricsActiveAllocations()
x, y = IO:getMouseDelta()
number = IO:getMouseDownSec(mouse_button)
IO:setDisplaySize(w, h)
w, h = IO:getDisplaySize()
number = IO:getDeltaTime()
```
[To top](#api)
# WIDGETS & STUFF
## Windows
```lua
-- resizeCallback (function): applies if 'ImGui:setNextWindowSizeConstraints(min_w, min_h, max_w, max_h)' 
-- was called BEFORE 'ImGui:beginWindow(...)'
p_open, draw = ImGui:beginWindow(label, p_open, [ImGuiWindowFlags = 0, resizeCallback])
draw = ImGui:beginWindow(label, nil, [ImGuiWindowFlags = 0, resizeCallback]) -- do not show close button
ImGui:endWindow()
```
[To top](#api)
## Child Windows
```lua
ImGui:beginChild(id, [w = 0, h = 0, borderFlag = false, ImGuiWindowFlags = 0])
ImGui:endChild()
```
[To top](#api)
## Windows Utilities
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
## Content region 
```lua
scaleX, scaleY = ImGui:getContentRegionMax()
w, h = ImGui:getContentRegionAvail()
x, y = ImGui:getWindowContentRegionMin()
x, y = ImGui:getWindowContentRegionMax()
w = ImGui:getWindowContentRegionWidth()
```
[To top](#api)
## Windows Scrolling
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
## Parameters stacks (shared)
```lua
ImGui:pushStyleColor(ImGuiCol, color)
ImGui:popStyleColor([count = 1])
ImGui:pushStyleVar(ImGuiStyleVar, value) OR ImGui:pushStyleVar(ImGuiStyleVar, value1, value2)
ImGui:popStyleVar([count = 1])
color, alpha = ImGui:getStyleColor(ImGuiCol)
fontSize = ImGui:getFontSize()
```
[To top](#api)
## Parameters stacks (current window) 
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
## Cursor / Layout
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
ImGui:setCursorPos(local_x, local_y)
ImGui:setCursorPosX(local_x)
ImGui:setCursorPosY(local_y)
x, y = ImGui:getCursorStartPos()
x, y = ImGui:getCursorScreenPos()
ImGui:alignTextToFramePadding()
lineH = ImGui:getTextLineHeight()
lineH = ImGui:getTextLineHeightWithSpacing()
frameH = ImGui:getFrameHeight()
frameH = ImGui:getFrameHeightWithSpacing()
```
[To top](#api)
## ID stack/scopes
```lua
ImGui:pushID(anyValue) 
ImGui:pushID(strBegin, strEnd)
ImGui:popID()
number = ImGui:getID(anyValue)
number = ImGui:getID(strBegin, strEnd)
```
[To top](#api)
## Widgets: Text
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
## Widgets: Main
```lua
pressFlag = ImGui:button(text, [w = 0, h = 0])
pressFlag = ImGui:smallButton(text)
pressFlag = ImGui:invisibleButton(stringID, [w = 0, h = 0])
pressFlag = ImGui:arrowButton(stringID, [ImGuiDir = 0])
ImGui:image(texture, w, h, [tintColor = 0xffffff, 1, borderColor = 0xffffff, 0, uv0x = 0, uv0y = 0, uv1x = 1, uv1y = 1])
ImGui:imageFilled(texture, w, h, [tint_color = 0xffffff, 1, bg_color = 0xffffff, 0, border_color = 0xffffff, 0, uv0x = 0, uv0y = 0, uv1x = 1, uv1y = 1])
pressFlag = ImGui:imageButton(texture, w, h, [padding = -1, tintColor = 0xffffff, 1, borderColor = 0xffffff, 0, uv0x = 0, uv0y = 0, uv1x = 1, uv1y = 1])
pressFlag = ImGui:checkbox(text, flag)
ImGui:checkboxFlags(label, ) -- TODO
pressFlag = ImGui:radioButton(text, flag)
ImGui:progressBar(fraction, [anchorX = -1, anchorY = 0, overlayString = nil])
ImGui:bullet()
```
[To top](#api)
## Widgets: Combo Box
```lua
openFlag = ImGui:beginCombo(text, previewText, [ImGuiComboFlags = 0])
ImGui:endCombo()
currentItem, isOpen = ImGui:combo(label, currentItem, items) -- items (table): {"item1", "item2", ...}
```
[To top](#api)
## Widgets: Drags 
```lua
value, isDragingFlag = ImGui:dragFloat(label, value, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, isDragingFlag = ImGui:dragFloat2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, isDragingFlag = ImGui:dragFloat3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isDragingFlag = ImGui:dragFloat4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
valueMin, valueMax, isDragingFlag = ImGui:dragFloatRange2(label, valueMin, valueMax, [incStep = 1, min = 0, max = 0, formatMinString = "%.3f", formatMaxString = formatMinString, ImGuiSliderFlags = 0])
 
value, isDragingFlag = ImGui:dragInt(label, value, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, isDragingFlag = ImGui:dragInt2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, isDragingFlag = ImGui:dragInt3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isDragingFlag = ImGui:dragInt4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
v_current_min, v_current_max, flag = ImGui:dragIntRange2(label, v_current_min, v_current_max, [v_speed = 1, v_min = 0, v_max = 0, format = "%d", format_max = nil, ImGuiSliderFlags = 0])
ImGui:dragScalar(label, ImGuiDataType, value, v_speed, [v_min = nil, v_max = nil, format = nil, ImGuiSliderFlags = 0])
```
[To top](#api)
## Widgets: Sliders
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
## Widgets: Input with Keyboard
```lua
text, flag = ImGui:inputText(label, text, bufferSize, [ImGuiInputTextFlags = 0])
text, flag = ImGui:inputTextMultiline(label, text, bufferSize, [w = 0, h = 0, ImGuiInputTextFlags = 0])
text, flag = ImGui:inputTextWithHint(label, text, hint, bufferSize, [ImGuiInputTextFlags = 0])
value,  flag = ImGui:inputFloat(label, value, [step = 0, step_fast = 0, format = "%.3f", ImGuiInputTextFlags = 0])
value1, value2, flag = ImGui:inputFloat2(label, value1, value2, [format = "%.3f", ImGuiInputTextFlags = 0])
value1, value2, value3, flag = ImGui:inputFloat3(label, value1, value2, value3, [format = "%.3f", ImGuiInputTextFlags = 0])
value1, value2, value3, value4, flag = ImGui:inputFloat4(label, value1, value2, value3, value4, [format = "%.3f", ImGuiInputTextFlags = 0])
value,  flag = ImGui:inputInt(label, value, [step = 0, step_fast = 0, ImGuiInputTextFlags = 0])
value1, value2, flag = ImGui:inputInt2(label, value1, value2, [ImGuiInputTextFlags = 0])
value1, value2, value3, flag = ImGui:inputInt3(label, value1, value2, value3, [ImGuiInputTextFlags = 0])
value1, value2, value3, value4, flag = ImGui:inputInt4(label, value1, value2, value3, value4, [ImGuiInputTextFlags = 0])
value, flag = ImGui:inputDouble(label, value, [step = 0, step_fast = 0, format = "%.6f", ImGuiInputTextFlags = 0])
value, flag = ImGui:inputScalar(label, ImGuiDataType, value, v_min, v_max, format, [ImGuiInputTextFlags = 0])
```
[To top](#api)
## Widgets: Color Editor/Picker
```lua
hexColor, isTouchingFlag = ImGui:colorEdit3(label, color, [ImGuiColorEditFlags = 0]) -- alpha ignored, no need to pass it!
hexColor, alpha, isTouchingFlag = ImGui:colorEdit4(label, color, [ImGuiColorEditFlags = 0])
hexColor, isTouchingFlag = ImGui:colorPicker3(label, color, [ImGuiColorEditFlags = 0])
hexColor, alpha, originalColor, originalAlpha, isTouchingFlag = ImGui:colorPicker4(label, color, [originalColor = 0xffffff, 1, ImGuiColorEditFlags = 0])
isHoveringFlag = ImGui:colorButton(stringID, color, [ImGuiColorEditFlags = 0, w = 0, h = 0])
ImGui:setColorEditOptions(ImGuiColorEditFlags)
```
[To top](#api)
## Widgets: Trees
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
## Widgets: Selectables
```lua
result?, selected = ImGui:selectable(label, selected, [ImGuiSelectableFlags = 0, w = 0, h = 0])
```
[To top](#api)
## Widgets: List Boxes
```lua
current_item, isOpenFlag = ImGui:listBox(label, current_item, itemTable, [max_visible_items = -1]) -- itemTable: {"Item0", "Item1", ...}
result? = ImGui:listBoxHeader(label, [w = 0, h = 0])
result? = ImGui:listBoxHeader2(label, items_count)
ImGui:listBoxFooter()
```
[To top](#api)
## Widgets: Data Plotting
```lua
ImGui:plotLines(label, pointsTable, [values_offset = 0, overlay_text = nil, scale_min = math.huge, scale_max = math.huge, w = 0, h = 0]) -- pointsTable: {0.01, 0.5, 10, -50, ...}
ImGui:plotHistogram(label, pointsTable, [values_offset = 0, overlay_text = nil, scale_min = math.huge, scale_max = math.huge, w = 0, h = 0])"plotLines"
```
[To top](#api)
## Widgets: Value() Helpers
```lua
ImGui:value(prefix, bool)
ImGui:value(prefix, number)
ImGui:value(prefix, float, formatString)
```
[To top](#api)
## Widgets: Menus
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
## Popups, Modals
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
## Columns
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
## Tab Bars, Tabs
```lua
bool = ImGui:beginTabBar(str_id, [ImGuiTabBarFlags = 0])
ImGui:endTabBar()
p_open, bool = ImGui:beginTabItem(label, p_open, [ImGuiTabItemFlags = 0])
ImGui:endTabItem()
ImGui:setTabItemClosed(tab_or_docked_window_label)
```
## Docking (BETA)
```lua
ImGui:dockSpace(ImGuiID, w, h, [ImGuiDockNodeFlags = 0])
ImGuiID = ImGui:dockSpaceOverViewport([ImGuiDockNodeFlags = 0])
ImGui:setNextWindowDockID(ImGuiID, [ImGuiCond = 0])
ImGuiID = ImGui:getWindowDockID()
flag = ImGui:isWindowDocked()
```
[To top](#api)
## Dock builder (BETA)
```lua
ImGui:dockBuilderDockWindow(window_name, node_id)
--ImGuiDockNode = ImGui:dockBuilderGetNode(ImGuiID) -- WIP
ImGui:dockBuilderSetNodePos(node_id, x, y)
ImGui:dockBuilderSetNodeSize(node_id, w, h)
node_id = ImGui:dockBuilderAddNode([node_id = 0, ImGuiDockNodeFlags = 0])
ImGui:dockBuilderRemoveNode(node_id)
ImGui:dockBuilderRemoveNodeChildNodes(node_id)
ImGui:dockBuilderRemoveNodeDockedWindows(node_id, clear_settings_refs_flag)
node_id, out_id_at_dir, out_id_at_opposite_dir = ImGui:dockBuilderSplitNode(node_id, ImGuiDir, size_ratio_for_node_at_dir, out_id_at_dir, out_id_at_opposite_dir)
ImGui:dockBuilderCopyNode(src_node_id, dst_node_id)
ImGui:dockBuilderCopyWindowSettings(src_name, dst_name)
ImGui:dockBuilderCopyDockSpace(src_dockspace_id, dst_dockspace_id)
ImGui:dockBuilderFinish(node_id)
```
[To top](#api)
[To top](#api)
## Logging/Capture
```lua
ImGui:logToTTY(auto_open_depth = -1) 
ImGui:logToFile(auto_open_depth = -1, filename = nil) 
ImGui:logToClipboard(auto_open_depth = -1) 
ImGui:logFinish() 
ImGui:logButtons() 
ImGui:logText(text) 
```
[To top](#api)
## Drag and drop
```lua
flag = ImGui:beginDragDropSource([ImGuiDragDropFlags flags = 0])
flag = ImGui:setDragDropPayload(str_type, number, [ImGuiCond cond = 0])
ImGui:endDragDropSource()
flag = ImGui:beginDragDropTarget()
table = ImGui:acceptDragDropPayload(type, [ImGuiDragDropFlags flags = 0])  -- W.I.P. have no return value
ImGui:endDragDropTarget()
table = ImGui:getDragDropPayload() -- W.I.P. have no return value
```
[To top](#api)
## Clipping
```lua
ImGui:pushClipRect(minX, minY, maxX, maxY, intersect_with_current_clip_rect)
ImGui:popClipRect()
```
[To top](#api)
## Focus, Activation
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
## Miscellaneous Utilities
```lua
flag = ImGui:isRectVisible(w, h, [maxX, maxY])
number = ImGui:getTime()
number = ImGui:getFrameCount()
str = ImGui:getStyleColorName(idx)
out_items_display_start, out_items_display_end = ImGui:calcListClipping(items_count, items_height, out_items_display_start, out_items_display_end)
flag = ImGui:beginChildFrame(id, w, h, [ImGuiWindowFlags = 0]) -- id (number)
ImGui:endChildFrame()
```
[To top](#api)
## Text Utilities
```lua
w, h = ImGui:calcTextSize(text, [text_end = nul, hide_text_after_double_hash = false, wrap_width = -1])
```
[To top](#api)
## Inputs Utilities: Keyboard
```lua
number = ImGui:getKeyIndex(ImGuiKey)
flag = ImGui:isKeyDown(user_key_index)
flag = ImGui:isKeyPressed(user_key_index, [repeat = true])
flag = ImGui:isKeyReleased(user_key_index)
number = ImGui:getKeyPressedAmount(key_index, repeat_delay, rate)
ImGui:captureKeyboardFromApp([want_capture_keyboard_value = true])
```
[To top](#api)
## Inputs Utilities: Mouse
```lua
flag = ImGui:isMouseDown(mouse_button)
flag = ImGui:isMouseClicked(mouse_button, [repeat = false])
flag = ImGui:isMouseReleased(mouse_button)
flag = ImGui:isMouseDoubleClicked(mouse_button)
flag = ImGui:isMouseHoveringRect(minX, minY, maxX, maxY, [clip = true])
flag = ImGui:isMousePosValid([x = inf, y = inf])
flag = ImGui:isAnyMouseDown()
x, y = ImGui:getMousePos()
x, y = ImGui:getMousePosOnOpeningCurrentPopup()
flag = ImGui:isMouseDragging(mouse_button, [lock_threshold = -1])
x, y = ImGui:getMouseDragDelta(mouse_button, [lock_threshold = -1])
ImGui:resetMouseDragDelta(mouse_button)
ImGuiMouseCursor = ImGui:getMouseCursor()
ImGui:setMouseCursor(ImGuiMouseCursor)
ImGui:CaptureMouseFromApp([want_capture_mouse_value = true])

flag = ImGui:wantCaptureMouse()
flag = ImGui:wantCaptureKeyboard()
flag = ImGui:wantTextInput()
flag = ImGui:wantSetMousePos()
flag = ImGui:wantSaveIniSettings()
flag = ImGui:isNavActive()
flag = ImGui:isNavVisible()
```
[To top](#api)
## Render
```lua
ImGui:newFrame()
ImGui:render()
ImGui:endFrame()
```
[To top](#api)
## Demos
```lua
isOpenFlag = ImGui:showUserGuide()
isOpenFlag = ImGui:showDemoWindow()
isOpenFlag = ImGui:showAboutWindow()
isOpenFlag = ImGui:showStyleEditor()
isOpenFlag = ImGui:showFontSelector()
isOpenFlag = ImGui:showMetricsWindow()
isOpenFlag = ImGui:showStyleSelector()
ImGui:showLuaStyleEditor()
```
[To top](#api)
## ENUMS
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
ImGui.PopupFlags_MouseButtonMask
ImGui.PopupFlags_MouseButtonRight
ImGui.PopupFlags_AnyPopupId
ImGui.PopupFlags_MouseButtonDefault
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
ImGui.InputTextFlags_NoBackground -- do not draw background frame

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
ImGui.TabBarFlags_FittingPolicyMask
ImGui.TabBarFlags_Reorderable
ImGui.TabBarFlags_FittingPolicyDefault
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
ImGui.WindowFlags_NoDocking

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
ImGui.ComboFlags_HeightMask
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
ImGui.ColorEditFlags_AlphaPreviewHalf
ImGui.ColorEditFlags_Float
ImGui.ColorEditFlags_PickerHueWheel
ImGui.ColorEditFlags_OptionsDefault
ImGui.ColorEditFlags_InputRGB
ImGui.ColorEditFlags_HDR
ImGui.ColorEditFlags_NoPicker
ImGui.ColorEditFlags_AlphaBar
ImGui.ColorEditFlags_DisplayHSV
ImGui.ColorEditFlags_PickerHueBar
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
ImGui.ConfigFlags_DockingEnable

-- ImGuiBackendFlags
ImGui.BackendFlags_None
ImGui.BackendFlags_HasGamepad
ImGui.BackendFlags_HasMouseCursors
ImGui.BackendFlags_HasSetMousePos
ImGui.BackendFlags_RendererHasVtxOffset

-- ImGuiSliderFlags
ImGui.SliderFlags_None        
ImGui.SliderFlags_ClampOnInput  
ImGui.SliderFlags_Logarithmic  
ImGui.SliderFlags_NoRoundToFormat
ImGui.SliderFlags_NoInput

-- ImGuiDockNodeFlags
ImGui.DockNodeFlags_None
ImGui.DockNodeFlags_KeepAliveOnly
ImGui.DockNodeFlags_NoDockingInCentralNode
ImGui.DockNodeFlags_PassthruCentralNode
ImGui.DockNodeFlags_NoSplit
ImGui.DockNodeFlags_NoResize
ImGui.DockNodeFlags_AutoHideTabBar

```
[To top](#api)
## DRAW LISTS

### Window draw list
```lua
local list = ImGui:getWindowDrawList()
```
### Background draw list
```lua
local list = ImGui:getBackgroundDrawList()
```
### Background draw list
```lua
local list = ImGui:getBackgroundDrawList()
```

## Draw lists commands
```lua
DrawList:pushClipRect(clip_rect_min_x, clip_rect_min_y, clip_rect_max_x, clip_rect_max_y, [intersect_with_current_clip_rect = false])
DrawList:pushClipRectFullScreen()
DrawList:popClipRect()
DrawList:pushTextureID(texture)
DrawList:popTextureID()
x, y = DrawList:getClipRectMin()
x, y = DrawList:getClipRectMax()
DrawList:addLine(p1_x, p1_y, p2_x, p2_y, color, [thickness = 1])
DrawList:addRect(p_min_x, p_min_y, p_max_x, p_max_y, color, [rounding = 0, rounding_corners = ImGui.CornerFlags_All, thickness = 1])
DrawList:addRectFilled(p_min_x, p_min_y, p_max_x, p_max_y, color, [rounding = 0, rounding_corners = ImGui.CornerFlags_All])
DrawList:addRectFilledMultiColor(p_min_x, p_min_y, p_max_x, p_max_y, color_upr_left, color_upr_right, color_bot_right, color_bot_left)
DrawList:addQuad(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color, [thickness = 1])
DrawList:addQuadFilled(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color)
DrawList:addTriangle(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, color, [thickness = 1])
DrawList:addTriangleFilled(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, color)
DrawList:addCircle(center_x, center_y, radius, color, [num_segments = 12, thickness = 1])
DrawList:addCircleFilled(center_x, center_y, radius, color, [num_segments = 12])
DrawList:addNgon(center_x, center_y, radius, color, [num_segments = 12, thickness = 1])
DrawList:addNgonFilled(center_x, center_y, radius, color, [num_segments = 12])
DrawList:addText(x, y, color, text_begin, [text_end]) -- x, y (number), text_begin (string), text_end (string)
DrawList:addPolyline(pointsTable, color, closed, thickness) -- pointsTable (table), color (number), closed (bool), thickness (number)
DrawList:addConvexPolyFilled(pointsTable, color) -- pointsTable (table), color (number)
DrawList:addBezierCurve(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color, thickness, [num_segments = 0])
DrawList:addImage(texture, p_min_x, p_min_y, [color = 0xffffff, 1, uv_min_x = 0, uv_min_y = 0, uv_max_x = 1, uv_max_y = 1])
DrawList:addImageQuad(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, [color = 0xffffff, 1, uv1_x, uv1_y, uv2_x, uv2_y, uv3_x, uv3_y, uv4_x, uv4_y])
DrawList:addImageRounded(p_min_x, p_min_y, color, rounding, [rounding_corners = ImGui.CornerFlags_All, uv_min_x = 0, uv_min_y = 0, uv_max_x = 1, uv_max_y = 1])
DrawList:pathClear()
DrawList:pathLineTo(x, y)
DrawList:pathLineToMergeDuplicate(x, y)
DrawList:pathFillConvex(color)
DrawList:pathStroke(color, closed, [thickness = 1])
DrawList:pathArcTo(centerX, centerY, radius, a_min, a_max, [num_segments = 10])
DrawList:pathArcToFast(centerX, centerY, radius, a_min, a_max)
DrawList:pathBezierCurveTo(p2x, p2y, p3x, p3y, p4x, p4y, [num_segments = 0])
DrawList:pathRect(minX, minY, maxX, maxY, [rounding = 0, ImDrawCornerFlags = 0])
```
[To top](#api)
