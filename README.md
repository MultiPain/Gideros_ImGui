# Dear ImGui LUA binding for [Gideros mobile](http://giderosmobile.com/)
[Dear ImGui](https://github.com/ocornut/imgui)

!VERY IMPORTANT!</br> 
every color related argument is actually 2 arguments: color itself in hex format: 0xRRGGBB AND alpha [0..1]</br> 
for example:</br> 
```DrawList:addRect(p_min_x, p_min_y, p_max_x, p_max_y, color, [rounding = 0, rounding_corners = ImGui.CornerFlags_All, thickness = 1])```</br> 
usage: ```DrawList:addRect(0,0, 100,100, 0xff0000, 1, ROUNDING, ROUNDING_CORNERS, THICKNESS)```</br> 

# Constructor
```lua
-- fontAtlas: copy fonts
-- mouseListeners: adds internal mouse event listeners
-- keyboardListeners: adds internal keyboard event listeners
-- touchListeners: adds internal touch event listeners
ImGui.new([fontAtlas = nil, mouseListeners = true, keyboardListeners = true, touchListeners = false])
```
## EXPERIMENTAL
```lua
p_open = ImGui:showLog(title, p_open, [ImGuiWindowFlags = 0]) -- draw log window
ImGui:writeLog(text)
```

## FONTS 
```lua
IO = imgui:getIO()
FontAtlas = IO:getFonts()

Font = FontAtlas:addFont(ttf_font_path, font_size, [options])

-- options (table): all parameters are optional
--	    fontDataOwnedByAtlas - bool
--	    pixelSnapH - bool
--	    mergeMode - bool
--	    fontNo - number
--	    oversampleH - number
--	    oversampleV - number
--	    glyphExtraSpacingX - number
--	    glyphExtraSpacingY - number
--	    glyphOffsetX - number
--	    glyphOffsetY - number
--	    glyphMinAdvanceX - number
--	    glyphMaxAdvanceX - number
--	    rasterizerFlags - number
--	    rasterizerMultiply - number
--	
--	    glyphs - table:
--		    text(string): represents avaliable chars
--		    chars(table): list of specific char code (example: {0x7262, ...})
--		    ranges(table): predefined glyph ranges (example: {ImGui.GlyphRanges_Default, ImGui.GlyphRanges_Japanese, ...})
FontAtlas:addFonts(fontsDescription)
-- fontsDescriptions(talbe):
--      description(table):
--          ttf_font_path(string): path to a font
--          font_size(number): font size
--          options(table): see description above
-- example:
-- FontAtlas:addFonts{ {"fonts/DroidSans.ttf", 16}, {"fonts/ProggyTiny.ttf", 16} }

Font = FontAtlas:getFont([index]) -- get font by index (if index is 0 or nil you will get default font instance)
FontAtlas:build() -- call after multiple FontAtlas:addFont(...) calls to update ImGui font atlas 
FontAtlas:clearInputData()
FontAtlas:clearTexData()
FontAtlas:clearFonts()
FontAtlas:clear()
table = FontAtlas:getFonts() -- returns a table with all fonts (included default)
flag = FontAtlas:isBuilt()
number = FontAtlas:addCustomRectRegular(width, height)
number = FontAtlas:addCustomRectFontGlyph(font, id, width, height, advance_x, [offset_x, offset_y])
w, h, x, y, glyph_id, offset_x, offset_y, font, is_packed_flag = FontAtlas:getCustomRectByIndex(index)

ImGui:pushFont(font)  -- font (table): object returned by FontAtlas:addFont(...) or FontAtlas:getFont([index])
ImGui:popFont()
```
### Minimal example:
```lua
local UI = ImGui.new()
local IO = UI:getIO()
local FontAtlas = IO:getFonts()
local VDS_font = FontAtlas:addFont("fonts/VDS.ttf", 16, {
    oversampleH = 2,
    oversampleV = 2,
	glyphs = {
		ranges = {ImGui.GlyphRanges_Cyrillic}
	}
})
IO:setFontDefault(VDS_font)
FontAtlas:build()
stage:addChild(UI)

-- you can use multiple fonts at the same time
function enterFrame(e)
	UI:newFrame(e)
	
	imgui:pushFont(font1)
	imgui:text("Font1")
	imgui:popFont()
	
	imgui:pushFont(font2)
	imgui:text("Font2")
	imgui:popFont()
	
	UI:render()
	UI:endFrame()
end
```
### Glyphs example:
```lua
local fonts = io:getFonts()
fonts:addFont(font_path, font_size, {
	glyphs = {
		ranges = {
			{
				0x2590,0x2593, -- range1
				0x2660,0x266B  -- range2
				-- ...
			},
			ImGui.GlyphRanges_Cyrillic,
			{
				0x01C0, 0x01C3 -- range3
			},
			ImGui.GlyphRanges_Korean
		},
 
		-- same structure:
		ranges = {
			{
				0x2590,0x2593, -- range1
				0x2660,0x266B, -- range2
				0x01C0,0x01C3  -- range3
				-- ...
			}, 
			ImGui.GlyphRanges_Cyrillic, 
			ImGui.GlyphRanges_Korean
		}
	},
	mergeMode = true, -- merge into previous font
})
fonts:build()
 
-- To use specific icons:
local icon = utf8.char(0x2590)
 
ImGui:text("My icon >>" .. icon .. " << !!!")
```

## Font
```lua
number = Font:getFontSize()
FontAtlas = Font:getContainerAtlas()
Font:setScale(number)
number = Font:getScale()
number = Font:getAscent()
number = Font:getDescent()
boo = Font:isLoaded()
string = Font:getDebugName()
w, h = Font:calcTextSizeA(size, max_width, wrap_width, string)
Font:calcWordWrapPositionA(scale, string, wrap_width) -- not tested
```

## INPUTS

```lua
ImGui:onMouseHover(event)
ImGui:onMouseMove(event)
ImGui:onMouseDown(event)
ImGui:onMouseUp(event)
ImGui:onMouseWheel(event)


ImGui:onTouchMove(event)
ImGui:onTouchBegin(event)
ImGui:onTouchEnd(event)
ImGui:onTouchCancel(event)

ImGui:onKeyUp(event)
ImGui:onKeyDown(event)
ImGui:onKeyChar(event)
```
### Usage example
```lua
local UI = ImGui.new(false, false, false, false)
-- Mouse
stage:addEventListener("mouseHover", function(e) UI:onMouseHover(e) end)
stage:addEventListener("mouseMove", function(e) UI:onMouseMove(e) end)
stage:addEventListener("mouseDown", function(e) UI:onMouseDown(e) end)
stage:addEventListener("mouseUp", function(e) UI:onMouseUp(e) end)
stage:addEventListener("mouseWheel", function(e) UI:onMouseWheel(e) end)
-- Touch
stage:addEventListener("touchesCancel", function(e) ui:onTouchCancel(e) end)
stage:addEventListener("touchesMove", function(e) ui:onTouchMove(e) end)
stage:addEventListener("touchesBegin", function(e) ui:onTouchBegin(e) end)
stage:addEventListener("touchesEnd", function(e) ui:onTouchEnd(e) end)
-- Keyboard
stage:addEventListener("keyUp", function(e) UI:onKeyUp(e) end)
stage:addEventListener("keyDown", function(e) UI:onKeyDown(e) end)
stage:addEventListener("keyChar", function(e) UI:onKeyChar(e) end)
```

## Available KeyCodes
List of KeyCodes that can be used with IO:isKeyDown/IO:isKeyPressed/IO:isKeyReleased
```lua
KeyCode.TAB
KeyCode.LEFT
KeyCode.RIGHT
KeyCode.UP
KeyCode.DOWN
KeyCode.PAGE_UP
KeyCode.PAGE_DOWN
KeyCode.HOME
KeyCode.END
KeyCode.DELETE
KeyCode.BACKSPACE
KeyCode.ENTER
KeyCode.ESC
KeyCode.INSERT
KeyCode.A
KeyCode.C
KeyCode.V
KeyCode.X
KeyCode.Y
KeyCode.Z
```

## Style setters/getters
### Get style settings instance
```lua
local Style = ImGui:getStyle()
```
```lua
Style:setColor(ImGuiCol, color)
color = Style:getColor(ImGuiCol)
Style:setAlpha(value)
value = Style:getAlpha()
Style:setWindowRounding(value)
value = Style:getWindowRounding()
Style:setWindowBorderSize(value)
value = Style:getWindowBorderSize()
Style:setChildRounding(value)
value = Style:getChildRounding()
Style:setChildBorderSize(value)
value = Style:getChildBorderSize()
Style:setPopupRounding(value)
value = Style:getPopupRounding()
Style:setPopupBorderSize(value)
value = Style:getPopupBorderSize()
Style:setFrameRounding(value)
value = Style:getFrameRounding()
Style:setFrameBorderSize(value)
value = Style:getFrameBorderSize()
Style:setIndentSpacing(value)
value = Style:getIndentSpacing()
Style:setColumnsMinSpacing(value)
value = Style:getColumnsMinSpacing()
Style:setScrollbarSize(value)
value = Style:getScrollbarSize()
Style:setScrollbarRounding(value)
value = Style:getScrollbarRounding()
Style:setGrabMinSize(value)
value = Style:getGrabMinSize()
Style:setGrabRounding(value)
value = Style:getGrabRounding()
Style:setLogSliderDeadzone(value)
value = Style:getLogSliderDeadzone()
Style:setTabRounding(value)
value = Style:getTabRounding()
Style:setTabBorderSize(value)
value = Style:getTabBorderSize()
Style:setTabMinWidthForCloseButton(value)
value = Style:getTabMinWidthForCloseButton()
Style:setMouseCursorScale(value)
value = Style:getMouseCursorScale()
Style:setCurveTessellationTol(value)
value = Style:getCurveTessellationTol()
Style:setCircleSegmentMaxError(value)
value = Style:getCircleSegmentMaxError()
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

## DEFAULT STYLES 
```lua
ImGui:setDarkStyle()
ImGui:setLightStyle()
ImGui:setClassicStyle()
```

## Color convert
```lua
r, g, b, a = ImGui:colorConvertHEXtoRGB(color, [alpha = 1])
hex = ImGui:colorConvertRGBtoHEX(r, g, b)
h, s, v = ImGui:colorConvertRGBtoHSV(r, g, b)
r, g, b = ImGui:colorConvertHSVtoRGB(h, s, v)
h, s, v = colorConvertHEXtoHSV(hex)
hex = colorConvertHSVtoHEX(h, s, v)
```

## IO Functions
### Get IO instance
```lua
local IO = ImGui:getIO()
```

```lua
IO:setFontDefault(font)
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
IO:setNavInput(ImGuiNavInput, value) -- see enums
value = IO:getNavInput(ImGuiNavInput)
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

# WIDGETS & STUFF
## Windows
```lua
-- resizeCallback (function): applies if 'ImGui:setNextWindowSizeConstraints(min_w, min_h, max_w, max_h)' 
-- was called BEFORE 'ImGui:beginWindow(...)'
p_open, draw = ImGui:beginWindow(label, p_open, [ImGuiWindowFlags = 0, resizeCallback])
draw = ImGui:beginWindow(label, nil, [ImGuiWindowFlags = 0, resizeCallback]) -- do not show "X" button
p_open, draw = ImGui:beginFullScreenWindow(label, p_open, [ImGuiWindowFlags = 0, resizeCallback]) -- start a window with no borders, no paddings, no rounding and ImGui.WindowFlags_Fullscreen flag
draw = ImGui:beginFullScreenWindow(label, nil, [ImGuiWindowFlags = 0, resizeCallback]) -- do not show "X" button
ImGui:endWindow()
```

## Child Windows
```lua
ImGui:beginChild(id, [w = 0, h = 0, borderFlag = false, ImGuiWindowFlags = 0])
ImGui:endChild()
```

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

x1,y1, x2,y2 = ImGui:getWindowBounds() -- returns window region rectangle in global coordinates
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

## Content region 
```lua
scaleX, scaleY = ImGui:getContentRegionMax()
w, h = ImGui:getContentRegionAvail()
x, y = ImGui:getWindowContentRegionMin()
x, y = ImGui:getWindowContentRegionMax()
w = ImGui:getWindowContentRegionWidth()
```

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

## Parameters stacks (shared)
```lua
ImGui:pushStyleColor(ImGuiCol, color)
ImGui:popStyleColor([count = 1])
ImGui:pushStyleVar(ImGuiStyleVar, value) OR ImGui:pushStyleVar(ImGuiStyleVar, value1, value2)
ImGui:popStyleVar([count = 1])
color, alpha = ImGui:getStyleColor(ImGuiCol)
fontSize = ImGui:getFontSize()
```

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
ImGui:setCursorScreenPos(x, y)
ImGui:alignTextToFramePadding()
lineH = ImGui:getTextLineHeight()
lineH = ImGui:getTextLineHeightWithSpacing()
frameH = ImGui:getFrameHeight()
frameH = ImGui:getFrameHeightWithSpacing()
```

## ID stack/scopes
```lua
ImGui:pushID(anyValue) 
ImGui:pushID(strBegin, strEnd)
ImGui:popID()
number = ImGui:getID(anyValue)
number = ImGui:getID(strBegin, strEnd)
```

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

## Widgets: Main
```lua
flag = ImGui:button(text, [w = 0, h = 0])
flag = ImGui:smallButton(text)
flag = ImGui:invisibleButton(stringID, [w = 0, h = 0])
flag = ImGui:arrowButton(stringID, [ImGuiDir = 0])
flag = ImGui:checkbox(text, flag)
flags, isChanged = ImGui:checkboxFlags(label, [flags = 0, falgs_value = 0])
number, isChanged = ImGui:radioButton(text, number, number)
isChanged = ImGui:radioButton(text, flag)
ImGui:progressBar(fraction, [anchorX = -1, anchorY = 0, overlayString = nil])
ImGui:bullet()
```

## Widgets: Images
```lua
-- Images are streched
ImGui:image(texture, w, h, [tintColor = 0xffffff, 1, borderColor = 0xffffff, 0])
ImGui:imageFilled(texture, w, h, [tint_color = 0xffffff, 1, bg_color = 0xffffff, 0, border_color = 0xffffff, 0])
pressFlag = ImGui:imageButton(texture, w, h, [padding = -1, tintColor = 0xffffff, 1, borderColor = 0xffffff, 0])
pressFlag = ImGui:imageButtonWithText(texture, text, w, h, [padding = -1, bg_color = 0xffffff, 0, tintColor = 0xffffff, 1])
-- Scales image to fit space (keeps aspect ratio)
ImGui:scaledImage(texture, w, h, [tintColor = 0xffffff, 1, borderColor = 0xffffff, 0, anchorX = 0.5, anchorY = 0.5])
ImGui:scaledImageFilled(texture, w, h, [tintColor = 0xffffff, 1, bgColor = 0xffffff, 0, borderColor = 0xffffff, 0, anchorX = 0.5, anchorY = 0.5])
pressFlag = ImGui:scaledImageButton(texture, w, h, [padding = -1, tintColor = 0xffffff, 1, bgColor = 0xffffff, 0, anchorX = 0.5, anchorY = 0.5])
pressFlag = ImGui:scaledImageButtonWithText(texture, text, w, h, [padding = -1, bgColor = 0xffffff, 0,tintColor = 0xffffff, 1, nil, nil, anchorX = 0.5, anchorY = 0.5])
```

## Widgets: Combo Box
```lua
openFlag = ImGui:beginCombo(text, previewText, [ImGuiComboFlags = 0])
ImGui:endCombo()
currentItem, isOpen = ImGui:combo(label, currentItem, items) -- items (table): {"item1", "item2", ...}
```

## Widgets: Drags 
```lua
value, isChanged = ImGui:dragFloat(label, value, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, isChanged = ImGui:dragFloat2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, isChanged = ImGui:dragFloat3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isChanged = ImGui:dragFloat4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
valueMin, valueMax, isChanged = ImGui:dragFloatRange2(label, valueMin, valueMax, [incStep = 1, min = 0, max = 0, formatMinString = "%.3f", formatMaxString = formatMinString, ImGuiSliderFlags = 0])
 
value, isChanged = ImGui:dragInt(label, value, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, isChanged = ImGui:dragInt2(label, value1, value2, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, isChanged = ImGui:dragInt3(label, value1, value2, value3, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isChanged = ImGui:dragInt4(label, value1, value2, value3, value4, [incStep = 1, min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
v_current_min, v_current_max, flag = ImGui:dragIntRange2(label, v_current_min, v_current_max, [v_speed = 1, v_min = 0, v_max = 0, format = "%d", format_max = nil, ImGuiSliderFlags = 0])
ImGui:dragScalar(label, ImGuiDataType, value, v_speed, [v_min = nil, v_max = nil, format = nil, ImGuiSliderFlags = 0])
```

## Widgets: Sliders
```lua
value, isChanged = ImGui:sliderFloat(label, value, [min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, isChanged = ImGui:sliderFloat2(label, value1, value2, [min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, isChanged = ImGui:sliderFloat3(label, value1, value2, value3, [min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isChanged = ImGui:sliderFloat4(label, value1, value2, value3, value4, [min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
valueInRad, isChanged = ImGui:sliderAngle(label, valueInRad, [min_degrees = -360, max_degrees = 360, formatString = "%.0f deg", ImGuiSliderFlags = 0])
value, isChanged = ImGui:sliderInt(label, value, [min = 0, max = 0, formatString = "%d, ImGuiSliderFlags = 0"])
value1, value2, isChanged = ImGui:sliderInt2(label, value1, value2, [min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, isChanged = ImGui:sliderInt3(label, value1, value2, value3, [min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isChanged = ImGui:sliderInt4(label, value1, value2, value3, value4, [min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value, isChanged = ImGui:sliderScalar(label, ImGuiDataType, value, [min = nil, max = nil, formatString = nil, ImGuiSliderFlags = 0])
value, isChanged = ImGui:vSliderFloat(label, w, h, value, min, max, [formatString = "%.3f", ImGuiSliderFlags = 0])
value, isChanged = ImGui:vSliderInt(label, w, h, value, min, max, [formatString = "%d", ImGuiSliderFlags = 0])
value, isChanged = ImGui:vSliderScalar(label, w, h, ImGuiDataType, [min = nil, max = nil, formatString = nil, ImGuiSliderFlags = 0])

value, isChanged = ImGui:filledSliderFloat(label, mirrorFlag, value, [min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, isChanged = ImGui:filledSliderFloat2(label, mirrorFlag, value1, value2, [min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, isChanged = ImGui:filledSliderFloat3(label, mirrorFlag, value1, value2, value3, [min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isChanged = ImGui:filledSliderFloat4(label, mirrorFlag, value1, value2, value3, value4, [min = 0, max = 0, formatString = "%.3f", ImGuiSliderFlags = 0])
valueInRad, isChanged = ImGui:filledSliderAngle(label, mirrorFlag, valueInRad, [min_degrees = -360, max_degrees = 360, formatString = "%.0f deg", ImGuiSliderFlags = 0])
value, isChanged = ImGui:filledSliderInt(label, mirrorFlag, value, [min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, isChanged = ImGui:filledSliderInt2(label, mirrorFlag, value1, value2, [min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, isChanged = ImGui:filledSliderInt3(label, mirrorFlag, value1, value2, value3, [min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value1, value2, value3, value4, isChanged = ImGui:filledSliderInt4(label, mirrorFlag, value1, value2, value3, value4, [min = 0, max = 0, formatString = "%d", ImGuiSliderFlags = 0])
value, isChanged = ImGui:filledSliderScalar(label, mirrorFlag, ImGuiDataType, value, [min = nil, max = nil, formatString = nil, ImGuiSliderFlags = 0])
value, isChanged = ImGui:vFilledSliderFloat(label, mirrorFlag, w, h, value, min, max, [formatString = "%.3f", ImGuiSliderFlags = 0])
value, isChanged = ImGui:vFilledSliderInt(label, mirrorFlag, w, h, value, min, max, [formatString = "%d", ImGuiSliderFlags = 0])
value, isChanged = ImGui:vFilledSliderScalar(label, mirrorFlag, w, h, ImGuiDataType, [min = nil, max = nil, formatString = nil, ImGuiSliderFlags = 0])
```

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

### Input text callbacks
```lua
ImGui:inputText(label, text, bufferSize, [ImGuiInputTextFlags = 0, callbackFunction])

callbackFunction = function(CallbackData)
	-- do something with data
	-- see below
end
```
### CallbackData
```lua
ImGuiInputTextFlags = CallbackData:getEventFlag()
ImGuiInputTextFlags = CallbackData:getFlags()
number = CallbackData:getEventChar()
CallbackData:setEventChar(number)
number = CallbackData:getEventKey()
string = CallbackData:getBuf()
CallbackData:setBuf(string)
number = CallbackData:getBufTextLen()
CallbackData:setBufTextLen(number)
number = CallbackData:getBufSize()
bool = CallbackData:getBufDirty()
CallbackData:setBufDirty(bool)
number = CallbackData:getCursorPos()
CallbackData:setCursorPos(number)
number = CallbackData:getSelectionStart()
CallbackData:setSelectionStart(number)
number = CallbackData:getSelectionEnd()
CallbackData:setSelectionEnd(number)
CallbackData:deleteChars(position, bytesCount)
CallbackData:insertChars(position, text)
CallbackData:selectAll()
CallbackData:clearSelection()
bool = CallbackData:hasSelection()
```

### Example
```lua
require "ImGui"

ui = ImGui.new()
stage:addChild(ui)

local testMessage1 = ""
local testMessage2 = ""
local testMessage3 = ""

-- Add ".." at the end of current input string
function myCallback1(data)
	data:insertChars(data:getCursorPos(), "..")
end

-- Replace all chars if UP/DOWN arrow is pressed
function myCallback2(data)
	local key = data:getEventKey()
	if (key == KeyCode.UP) then
		data:deleteChars(0, data:getBufTextLen())
		data:insertChars(0, "Pressed Up!")
		data:selectAll()
	elseif (key == KeyCode.DOWN) then
		data:deleteChars(0, data:getBufTextLen())
		data:insertChars(0, "Pressed Down!")
		data:selectAll()
	end
end

-- Switch case of the first char
function myCallback3(data)
	local buf = data:getBuf()
	local s = buf:sub(1,1)
	if ((s >= 'a' and s <= 'z') or (s >= 'A' and s <= 'Z')) then 
		local first = string.char(string.byte(s) ~ 32)
		data:setBuf(first .. buf:sub(2))
		data:setBufDirty(true)
	end
end

function enterFrame(e)
	ui:newFrame(e)
	
	testMessage1 = ui:inputText(
		"Label1", 
		testMessage1, 
		64, 
		ImGui.InputTextFlags_CallbackCompletion, 
		myCallback1
	)
	testMessage2 = ui:inputText(
		"Label2", 
		testMessage2, 
		64, 
		ImGui.InputTextFlags_CallbackHistory, 
		myCallback2
	)
	testMessage3 = ui:inputText(
		"Label3", 
		testMessage3, 
		64, 
		ImGui.InputTextFlags_CallbackEdit, 
		myCallback3
	)
	
	ui:render()
	ui:endFrame()
end

stage:addEventListener("enterFrame", enterFrame)
```

## Widgets: Color Editor/Picker
```lua
hexColor, isTouchingFlag = ImGui:colorEdit3(label, color, [ImGuiColorEditFlags = 0]) -- alpha ignored, no need to pass it!
hexColor, alpha, isTouchingFlag = ImGui:colorEdit4(label, color, [ImGuiColorEditFlags = 0])
hexColor, isTouchingFlag = ImGui:colorPicker3(label, color, [ImGuiColorEditFlags = 0])
hexColor, alpha, originalColor, originalAlpha, isTouchingFlag = ImGui:colorPicker4(label, color, [originalColor = 0xffffff, 1, ImGuiColorEditFlags = 0])
isHoveringFlag = ImGui:colorButton(stringID, color, [ImGuiColorEditFlags = 0, w = 0, h = 0])
ImGui:setColorEditOptions(ImGuiColorEditFlags)
```

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

## Widgets: Selectables
```lua
result?, selected = ImGui:selectable(label, selected, [ImGuiSelectableFlags = 0, w = 0, h = 0])
```

## Widgets: List Boxes
```lua
current_item, isOpenFlag = ImGui:listBox(label, current_item, itemTable, [max_visible_items = -1]) -- itemTable: {"Item0", "Item1", ...}
result? = ImGui:listBoxHeader(label, [w = 0, h = 0])
result? = ImGui:listBoxHeader2(label, items_count)
ImGui:listBoxFooter()
```

## Widgets: Data Plotting
```lua
ImGui:plotLines(label, pointsTable, [values_offset = 0, overlay_text = nil, scale_min = math.huge, scale_max = math.huge, w = 0, h = 0]) -- pointsTable: {0.01, 0.5, 10, -50, ...}
ImGui:plotHistogram(label, pointsTable, [values_offset = 0, overlay_text = nil, scale_min = math.huge, scale_max = math.huge, w = 0, h = 0])"plotLines"
```

## Widgets: Value() Helpers
```lua
ImGui:value(prefix, bool)
ImGui:value(prefix, number)
ImGui:value(prefix, float, formatString)
```

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

## Popups, Modals
```lua
result? = ImGui:beginPopup(str_id, [ImGuiWindowFlags = 0])
p_open, result? = ImGui:beginPopupModal(str_id, p_open, [ImGuiWindowFlags = 0])
ImGui:endPopup()
ImGui:openPopup(str_id, [ImGuiPopupFlags = 0])
ImGui:openPopupContextItem(str_id, [ImGuiPopupFlags = 0]) -- reanmed in 1.79 (can be still used until 1.80)
ImGui:openPopupOnItemClick(str_id, [ImGuiPopupFlags = 0])
ImGui:closeCurrentPopup()
result? = ImGui:beginPopupContextItem(str_id, [ImGuiPopupFlags = 0])
result? = ImGui:beginPopupContextWindow(str_id, [ImGuiPopupFlags = 0])
result? = ImGui:beginPopupContextVoid(str_id, [ImGuiPopupFlags = 0])
result? = ImGui:isPopupOpen(str_id, [ImGuiPopupFlags = 0])
```

## Tables
```lua
flag = ImGui:beginTable(str_id, column, [ImGuiTableFlags = 0, outer_w = 0, outer_h = 0, inner_width = 0])
ImGui:endTable()
ImGui:tableNextRow([ImGuiTableRowFlags = 0, min_row_height = 0])
flag = ImGui:tableNextColumn()
flag = ImGui:tableSetColumnIndex(column_n)
ImGui:tableSetupColumn(label, [ImGuiTableColumnFlags = 0, init_width_or_weight = 0, user_id = 0])
ImGui:tableSetupScrollFreeze(cols, rows)
ImGui:tableHeadersRow()
TableSortSpecs = ImGui:tableGetSortSpecs() -- see below
number = ImGui:tableGetColumnCount()
number = ImGui:tableGetColumnIndex()
number = ImGui:tableGetRowIndex()
string = ImGui:tableGetColumnName([column_n = -1])
ImGuiTableColumnFlags = ImGui:tableGetColumnFlags([column_n = -1])
ImGui:tableSetBgColor(ImGuiTableBgTarget, color, [column_n = -1])
```

## Table sort specs
```lua
-- TableSortSpecs = ImGui:tableGetSortSpecs()
number = TableSortSpecs:getSpecsCount()
flag = TableSortSpecs:isSpecsDirty()
TableSortSpecs:setSpecsDirty(flag)
table = TableSortSpecs:getColumnSortSpecs() -- see below
```

## Table column sort specs
```lua
-- table = TableSortSpecs:getColumnSortSpecs()
-- each value of this table is an object that have this functions:
number = item:getColumnUserID() 
number = item:getColumnIndex() -- 0 based
number = item:getSortOrder() -- used in multi sorted tables
number = item:getSortDirection() -- ImGui.SortDirection_Ascending OR ImGui.SortDirection_Descending
```
Example: https://github.com/MultiPain/Gideros_examples/blob/master/ImGuiTablesDemo/assets/TablesDemo.lua</br>

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

## Tab Bars, Tabs
```lua
bool = ImGui:beginTabBar(str_id, [ImGuiTabBarFlags = 0])
ImGui:endTabBar()
p_open, bool = ImGui:beginTabItem(label, p_open, [ImGuiTabItemFlags = 0])
ImGui:endTabItem()
ImGui:setTabItemClosed(tab_or_docked_window_label)
ImGui:tabItemButton(label, [ImGuiTabItemFlags = 0])
```

## Logging/Capture
```lua
ImGui:logToTTY(auto_open_depth = -1) 
ImGui:logToFile(auto_open_depth = -1, filename = nil) 
ImGui:logToClipboard(auto_open_depth = -1) 
ImGui:logFinish() 
ImGui:logButtons() 
ImGui:logText(text) 
```

## Drag and drop
```lua
flag = ImGui:beginDragDropSource([ImGuiDragDropFlags flags = 0])
flag = ImGui:setNumDragDropPayload(str_type, number, [ImGuiCond cond = 0])
flag = ImGui:setStrDragDropPayload(str_type, string, [ImGuiCond cond = 0])
ImGui:endDragDropSource()
flag = ImGui:beginDragDropTarget()
ImGuiPayload = ImGui:acceptDragDropPayload(type, [ImGuiDragDropFlags flags = 0])
ImGui:endDragDropTarget()
ImGuiPayload = ImGui:getDragDropPayload()
```
### Usage example
```lua
local names = {
	"Bobby", "Beatrice", "Betty",
	"Brianna", "Barry", "Bernard",
	"Bibi", "Blaine", "Bryn"
}
-- modes:
local Mode_Copy = 0
local Mode_Move = 1
local Mode_Swap = 2
 
local mode = 0 -- current mode

function onEnterFrame(e)
	UI:newFrame(e)
 
	if (UI:radioButton("Copy", mode == Mode_Copy)) then mode = Mode_Copy end UI:sameLine()
	if (UI:radioButton("Move", mode == Mode_Move)) then mode = Mode_Move end UI:sameLine()
	if (UI:radioButton("Swap", mode == Mode_Swap)) then mode = Mode_Swap end
 
	for i,v in ipairs(names) do
		UI:pushID(i)
		if (((i-1) % 3) ~= 0) then UI:sameLine() end
 
		UI:button(v, 60, 60)
 
		if (UI:beginDragDropSource(ImGui.DragDropFlags_None)) then
			--UI:setStrDragDropPayload("DND_DEMO_CELL", "ID_"..i) -- used for strings
			UI:setNumDragDropPayload("DND_DEMO_CELL", i) -- used for numbers
 
			if (mode == Mode_Copy) then UI:text(("Copy %s"):format(v)) end
			if (mode == Mode_Move) then UI:text(("Move %s"):format(v)) end
			if (mode == Mode_Swap) then UI:text(("Swap %s"):format(v)) end
			UI:endDragDropSource()
		end
 
		if (UI:beginDragDropTarget()) then
			local payload = UI:acceptDragDropPayload("DND_DEMO_CELL")
			if (payload) then
				--local payload_n = tonumber(payload:getStrData():sub(4))  -- if "setStrDragDropPayload" was used
				local payload_n = payload:getNumData() -- if "setNumDragDropPayload" was used
 
				if (mode == Mode_Copy) then
					names[i] = names[payload_n];
				end
				if (mode == Mode_Move) then
					names[i] = names[payload_n];
					names[payload_n] = "";
				end
 
				if (mode == Mode_Swap) then
					names[i], names[payload_n] = names[payload_n], names[i]
				end
			end
			UI:endDragDropTarget()
		end
		UI:popID()
	end
	UI:render()
	UI:endFrame()
end

stage:addEventListener("enterFrame", onEnterFrame)
```

### Payload
```lua
number = ImGuiPayload:getNumData()
string = ImGuiPayload:getStrData()
ImGuiPayload:clear()
number = ImGuiPayload:getDataSize()
flag = ImGuiPayload:isDataType(type) -- type must be the same as in "ImGui:acceptDragDropPayload(type)"
flag = ImGuiPayload:isPreview()
flag = ImGuiPayload:isDelivery()
```

## Clipping
```lua
ImGui:pushClipRect(minX, minY, maxX, maxY, intersect_with_current_clip_rect)
ImGui:popClipRect()
```

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

## Text Utilities
```lua
w, h = ImGui:calcTextSize(text, [hide_text_after_double_hash = false, wrap_width = -1])
```

## Inputs Utilities: Keyboard
```lua
number = ImGui:getKeyIndex(ImGuiKey)
flag = ImGui:isKeyDown(user_key_index)
flag = ImGui:isKeyPressed(user_key_index, [repeat = true])
flag = ImGui:isKeyReleased(user_key_index)
number = ImGui:getKeyPressedAmount(key_index, repeat_delay, rate)
ImGui:captureKeyboardFromApp([want_capture_keyboard_value = true])
```

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
ImGui:captureMouseFromApp([want_capture_mouse_value = true])
ImGui:setAutoUpdateCursor(flag) -- uses application:set("cursor", name) to modify native cursor
flag = ImGui:getAutoUpdateCursor()
```

## Render
```lua
ImGui:newFrame()
ImGui:render()
ImGui:endFrame()
```

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

## TextEditor
```lua
-- otherTextEditor: another "ImGuiTextEditor" instance to copy setting 
TextEditor = ImGuiTextEditor.new([otherTextEditor])
```
## Functions
```lua
LanguageDefinition = TextEditor:getLanguageCPP()
LanguageDefinition = TextEditor:getLanguageGLSL()
LanguageDefinition = TextEditor:getLanguageHLSL()
LanguageDefinition = TextEditor:getLanguageC()
LanguageDefinition = TextEditor:getLanguageSQL()
LanguageDefinition = TextEditor:getLanguageAngelScript()
LanguageDefinition = TextEditor:getLanguageLua()

TextEditor:setLanguageDefinition(LanguageDefinition)
LanguageDefinition = TextEditor:getLanguageDefinition()

Palette = TextEditor:getPaletteDark()
Palette = TextEditor:getPaletteLight()
Palette = TextEditor:getPaletteRetro()

TextEditor:setPalette(Palette)
Palette = TextEditor:getPalette()

TextEditor:setPaletteColor(TE_ColorIndex, color) -- see TextEditor enums below
TextEditor:getPaletteColor(TE_ColorIndex)

TextEditor:loadPalette(table) -- 42 elements table
-- example:
--[[ dark palete:
TextEditor:loadPalette{
	0x7f7f7f, 1,	-- Default
	0xd69c56, 1,	-- Keyword
	0x00ff00, 1,	-- Number
	0x7070e0, 1,	-- String
	0x70a0e0, 1,	-- Char literal
	0xffffff, 1,	-- Punctuation
	0x408080, 1,	-- Preprocessor
	0xaaaaaa, 1,	-- Identifier
	0x9bc64d, 1,	-- Known identifier
	0xc040a0, 1,	-- Preproc identifier
	0x206020, 1,	-- Comment (single line)
	0x406020, 1,	-- Comment (multi line)
	0x101010, 1,	-- Background
	0xe0e0e0, 1,	-- Cursor
	0xa06020, 0.5,	-- Selection
	0x0020ff, 0.5,	-- ErrorMarker
	0xf08000, 0.25, -- Breakpoint
	0x707000, 1,	-- Line number
	0x000000, 0.25, -- Current line fill
	0x808080, 0.25, -- Current line fill (inactive)
	0xa0a0a0, 0.25, -- Current line edge
}
]]

-- see below
TextEditor:setErrorMarkers(ErrorMarkers)
TextEditor:setBreakpoints(Breakpoints)

TextEditor:render(string_id, [w = 0, h = 0, border = 0])

TextEditor:setText(string)
TextEditor:getText()
TextEditor:setTextLines(table) -- set editor text using table. Structure: {"line 1", "line 2", "line 3", ...}
table = TextEditor:getTextLines()

string = TextEditor:getSelectedText()
string = TextEditor:getCurrentLineText()

number = TextEditor:getTotalLines()
bool = TextEditor:isOverwrite()

bool = TextEditor:setReadOnly()
bool = TextEditor:isReadOnly()
bool = TextEditor:isTextChanged()
bool = TextEditor:isCursorPositionChanged()

TextEditor:setColorizerEnable()
bool = TextEditor:isColorizerEnabled()

line, column = TextEditor:getCursorPosition() -- 0 based line & column number 
TextEditor:setCursorPosition(line, column)

TextEditor:setHandleMouseInputs(bool)
bool = TextEditor:isHandleMouseInputsEnabled()

TextEditor:setHandleKeyboardInputs(bool)
bool = TextEditor:isHandleKeyboardInputsEnabled()

TextEditor:setTextEditorChildIgnored(bool)
bool = TextEditor:isTextEditorChildIgnored()

TextEditor:setShowWhitespaces(bool)
bool = TextEditor:isShowingWhitespaces()

TextEditor:setTabSize(size)
size = TextEditor:getTabSize()

TextEditor:insertText(string)

TextEditor:moveUp([amount = 1, select = false])
TextEditor:moveDown([amount = 1, select = false])
TextEditor:moveLeft([amount = 1, select = false])
TextEditor:moveRight([amount = 1, select = false])
TextEditor:moveTop([select = false])
TextEditor:moveBottom([select = false])
TextEditor:moveHome([select = false])
TextEditor:moveEnd([select = false])

TextEditor:setSelectionStart(line, column)
TextEditor:setSelectionEnd(line, column)
TextEditor:setSelection(startLine, startColumn, endLine, endColumn)
TextEditor:selectWordUnderCursor()
TextEditor:selectAll()
bool = TextEditor:hasSelection()

TextEditor:copy()
TextEditor:cut()
TextEditor:paste()
TextEditor:delete()

bool = TextEditor:canUndo()
bool = TextEditor:canRedo()
TextEditor:undo()
TextEditor:redo()
```


### LanguageDefinition
```lua
string = LanguageDefinition:getName()
```

### ErrorMarkers
```lua
ErrorMarkers = ImGuiErrorMarkers.new()

ErrorMarkers:add(line, message)
ErrorMarkers:remove(line)
message = ErrorMarkers:get(line)
number = ErrorMarkers:getSize()
```

### Breakpoints
```lua
Breakpoints = ImGuiBreakpoints.new()

Breakpoints:add(line)
Breakpoints:remove(line)
bool = Breakpoints:get(line)
number = Breakpoints:getSize()
```


## ENUMS

### FocusedFlags
```lua
ImGui.FocusedFlags_ChildWindows
ImGui.FocusedFlags_AnyWindow
ImGui.FocusedFlags_RootWindow
ImGui.FocusedFlags_RootAndChildWindows
ImGui.FocusedFlags_None
```

### PopupFlags
```lua
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
```

### HoveredFlags
```lua
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
```

### InputTextFlags
```lua
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
ImGui.InputTextFlags_NoBackground -- custom constant, used to disable background
```

### NavInput
```lua
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
```

### TabBarFlags
```lua
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
```

### TreeNodeFlags
```lua
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
```

### StyleVar
```lua
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
ImGui.StyleVar_CellPadding
```

### Col
```lua
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
ImGui.Col_TableHeaderBg
ImGui.Col_TableBorderStrong
ImGui.Col_TableBorderLight
ImGui.Col_TableRowBg
ImGui.Col_TableRowBgAlt
```

### DataType
```lua
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
```

### Dir
```lua
ImGui.Dir_None
ImGui.Dir_Left
ImGui.Dir_Up
ImGui.Dir_Down
ImGui.Dir_Right
```

### WindowFlags
```lua
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
ImGui.WindowFlags_FullScreen -- custom constant, used to create a fullscreen window
```

### TabItemFlags
```lua
ImGui.TabItemFlags_SetSelected
ImGui.TabItemFlags_NoCloseWithMiddleMouseButton
ImGui.TabItemFlags_NoTooltip
ImGui.TabItemFlags_None
ImGui.TabItemFlags_NoPushId
ImGui.TabItemFlags_UnsavedDocument
ImGui.TabItemFlags_Leading
ImGui.TabItemFlags_Trailing
ImGui.TabItemFlags_NoReorder
```

### ComboFlags
```lua
ImGui.ComboFlags_HeightSmall
ImGui.ComboFlags_HeightLarge
ImGui.ComboFlags_PopupAlignLeft
ImGui.ComboFlags_None
ImGui.ComboFlags_NoPreview
ImGui.ComboFlags_HeightRegular
ImGui.ComboFlags_HeightMask
ImGui.ComboFlags_NoArrowButton
ImGui.ComboFlags_HeightLargest
```

### Cond
```lua
ImGui.Cond_Appearing
ImGui.Cond_None
ImGui.Cond_Always
ImGui.Cond_FirstUseEver
ImGui.Cond_Once
```

### SelectableFlags
```lua
ImGui.SelectableFlags_None
ImGui.SelectableFlags_SpanAllColumns
ImGui.SelectableFlags_AllowItemOverlap
ImGui.SelectableFlags_DontClosePopups
ImGui.SelectableFlags_AllowDoubleClick
ImGui.SelectableFlags_Disabled
```

### MouseCursor
```lua
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
```

### MouseButton
```lua
ImGui.MouseButton_Right
ImGui.MouseButton_Middle
ImGui.MouseButton_Left
```

### ColorEditFlags
```lua
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
```

### DragDropFlags
```lua
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
```

### CornerFlags
```lua
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
```

### ConfigFlags
```lua
ImGui.ConfigFlags_None                   
ImGui.ConfigFlags_NavEnableKeyboard      
ImGui.ConfigFlags_NavEnableGamepad       
ImGui.ConfigFlags_NavEnableSetMousePos   
ImGui.ConfigFlags_NavNoCaptureKeyboard   
ImGui.ConfigFlags_NoMouse                
ImGui.ConfigFlags_NoMouseCursorChange
ImGui.ConfigFlags_IsSRGB                 
ImGui.ConfigFlags_IsTouchScreen
```

### BackendFlags
```lua
ImGui.BackendFlags_None
ImGui.BackendFlags_HasGamepad
ImGui.BackendFlags_HasMouseCursors
ImGui.BackendFlags_HasSetMousePos
ImGui.BackendFlags_RendererHasVtxOffset
```

### SliderFlags
```lua
ImGui.SliderFlags_None          
ImGui.SliderFlags_ClampOnInput -- renamed in 1.79 to "SliderFlags_AlwaysClamp" (can be still used until 1.80)
ImGui.SliderFlags_AlwaysClamp
ImGui.SliderFlags_Logarithmic  
ImGui.SliderFlags_NoRoundToFormat
ImGui.SliderFlags_NoInput
```

### GlyphRanges
```lua
ImGui.GlyphRanges_Default,
ImGui.GlyphRanges_Korean,
ImGui.GlyphRanges_ChineseFull,
ImGui.GlyphRanges_ChineseSimplifiedCommon,
ImGui.GlyphRanges_Japanese,
ImGui.GlyphRanges_Cyrillic,
ImGui.GlyphRanges_Thai,
ImGui.GlyphRanges_Vietnamese
```

### ItemFlags
```lua
ImGui.ItemFlags_Disabled
ImGui.ItemFlags_ButtonRepeat
```

### TableBgTarget
```lua
ImGui.TableBgTarget_None
ImGui.TableBgTarget_RowBg0
ImGui.TableBgTarget_RowBg1
ImGui.TableBgTarget_CellBg
```


### TableColumnFlags
```lua
ImGui.TableColumnFlags_None
ImGui.TableColumnFlags_DefaultHide
ImGui.TableColumnFlags_DefaultSort
ImGui.TableColumnFlags_WidthStretch
ImGui.TableColumnFlags_WidthFixed
ImGui.TableColumnFlags_NoResize
ImGui.TableColumnFlags_NoReorder
ImGui.TableColumnFlags_NoHide
ImGui.TableColumnFlags_NoClip
ImGui.TableColumnFlags_NoSort
ImGui.TableColumnFlags_NoSortAscending
ImGui.TableColumnFlags_NoSortDescending
ImGui.TableColumnFlags_NoHeaderWidth
ImGui.TableColumnFlags_PreferSortAscending
ImGui.TableColumnFlags_PreferSortDescending
ImGui.TableColumnFlags_IndentEnable
ImGui.TableColumnFlags_IndentDisable
ImGui.TableColumnFlags_IsEnabled
ImGui.TableColumnFlags_IsVisible
ImGui.TableColumnFlags_IsSorted
ImGui.TableColumnFlags_IsHovered
```


### TableFlags
```lua
ImGui.TableFlags_None
ImGui.TableFlags_Resizable
ImGui.TableFlags_Reorderable
ImGui.TableFlags_Hideable
ImGui.TableFlags_Sortable
ImGui.TableFlags_NoSavedSettings
ImGui.TableFlags_ContextMenuInBody
ImGui.TableFlags_RowBg
ImGui.TableFlags_BordersInnerH 
ImGui.TableFlags_BordersOuterH
ImGui.TableFlags_BordersInnerV
ImGui.TableFlags_BordersOuterV
ImGui.TableFlags_BordersH
ImGui.TableFlags_BordersV
ImGui.TableFlags_BordersInner
ImGui.TableFlags_BordersOuter
ImGui.TableFlags_Borders
ImGui.TableFlags_NoBordersInBody
ImGui.TableFlags_NoBordersInBodyUntilResize
ImGui.TableFlags_SizingFixedFit
ImGui.TableFlags_SizingFixedSame
ImGui.TableFlags_SizingStretchProp
ImGui.TableFlags_SizingStretchSame
ImGui.TableFlags_NoHostExtendX
ImGui.TableFlags_NoHostExtendY
ImGui.TableFlags_NoKeepColumnsVisible
ImGui.TableFlags_PreciseWidths
ImGui.TableFlags_NoClip
ImGui.TableFlags_PadOuterX
ImGui.TableFlags_NoPadOuterX
ImGui.TableFlags_NoPadInnerX
ImGui.TableFlags_ScrollX
ImGui.TableFlags_ScrollY
ImGui.TableFlags_SortMulti
ImGui.TableFlags_SortTristate
```


### TableColumnFlags
```lua
ImGui.TableColumnFlags_None
ImGui.TableColumnFlags_DefaultHide
ImGui.TableColumnFlags_DefaultSort
ImGui.TableColumnFlags_WidthStretch
ImGui.TableColumnFlags_WidthFixed
ImGui.TableColumnFlags_NoResize
ImGui.TableColumnFlags_NoReorder
ImGui.TableColumnFlags_NoHide
ImGui.TableColumnFlags_NoClip
ImGui.TableColumnFlags_NoSort
ImGui.TableColumnFlags_NoSortAscending
ImGui.TableColumnFlags_NoSortDescending
ImGui.TableColumnFlags_NoHeaderWidth
ImGui.TableColumnFlags_PreferSortAscending
ImGui.TableColumnFlags_PreferSortDescending
ImGui.TableColumnFlags_IndentEnable
ImGui.TableColumnFlags_IndentDisable
ImGui.TableColumnFlags_IsEnabled
ImGui.TableColumnFlags_IsVisible
ImGui.TableColumnFlags_IsSorted
ImGui.TableColumnFlags_IsHovered
```


### TableRowFlags
```lua
ImGui.TableRowFlags_None
ImGui.TableRowFlags_Headers
```


### SortDirection
```lua
ImGui.SortDirection_None
ImGui.SortDirection_Ascending
ImGui.SortDirection_Descending
```


### TE_ColorIndex
```lua
ImGui.TE_Default
ImGui.TE_Keyword
ImGui.TE_Number
ImGui.TE_String
ImGui.TE_CharLiteral
ImGui.TE_Punctuation
ImGui.TE_Preprocessor
ImGui.TE_Identifier
ImGui.TE_KnownIdentifier
ImGui.TE_PreprocIdentifier
ImGui.TE_Comment
ImGui.TE_MultiLineComment
ImGui.TE_Background
ImGui.TE_Cursor
ImGui.TE_Selection
ImGui.TE_ErrorMarker
ImGui.TE_Breakpoint
ImGui.TE_LineNumber
ImGui.TE_CurrentLineFill
ImGui.TE_CurrentLineFillInactive
ImGui.TE_CurrentLineEdge
```


## DRAW LISTS

### Window draw list
```lua
local list = ImGui:getWindowDrawList()
```
### Background draw list
```lua
local list = ImGui:getBackgroundDrawList()
```
### Foreground draw list
```lua
local list = ImGui:getForegroundDrawList()
```

### Draw lists commands
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
DrawList:addFontText(font, font_size, pos_x, pos_y, color, text, [wrap_with = 0, cpu_fine_clip_rect_x, cpu_fine_clip_rect_y, cpu_fine_clip_rect_w, cpu_fine_clip_rect_h])
DrawList:addPolyline(pointsTable, color, closed, thickness) -- pointsTable (table), color (number), closed (bool), thickness (number)
DrawList:addConvexPolyFilled(pointsTable, color) -- pointsTable (table), color (number)
DrawList:addBezierCubic(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color, thickness, [num_segments = 0])
DrawList:addBezierQuadratic(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, color, thickness, [num_segments = 0])
DrawList:addImage(texture, x, y, x + w, y + h, [tintColor = 0xffffff, tintAlpha = 1]) 
DrawList:addImageQuad(texture, x, y, x + w, y, x + w, y + h, x, y + h, [tintColor = 0xffffff, tintAlpha = 1, uv0x = 0, uv0y = 0, uv1x = 1, uv1y = 0, uv2x = 1, uv2y = 1, uv3x = 
DrawList:addImageRounded(texture, x, y, x + w, y + h, tintColor, tintAlpha, roundRadius, [CornerFlags = ImGui.CorenerFlags_All])
DrawList:pathClear()
DrawList:pathLineTo(x, y)
DrawList:pathLineToMergeDuplicate(x, y)
DrawList:pathFillConvex(color)
DrawList:pathStroke(color, closed, [thickness = 1])
DrawList:pathArcTo(centerX, centerY, radius, a_min, a_max, [num_segments = 10])
DrawList:pathArcToFast(centerX, centerY, radius, a_min, a_max)
DrawList:pathBezierCubicCurveTo(p2x, p2y, p3x, p3y, p4x, p4y, [num_segments = 0])
DrawList:pathBezierQuadraticCurveTo(p2x, p2y, p3x, p3y, [num_segments = 0])
DrawList:pathRect(minX, minY, maxX, maxY, [rounding = 0, ImDrawCornerFlags = 0])
-- CUSTOM
-- rotate any draw list item around its center point
DrawList:rotateBegin()
DrawList:rotateEnd(radians)
-- example:
...
local list = ImGui:getWindowDrawList()
list:rotateBegin()
list:addLine(100, 100, 100, 250, 0xff0000, 1, 10)
list:rotateEnd(math.pi/2.2)
...
```
### Usage example
<img src="https://user-images.githubusercontent.com/1312968/99901217-4697fa80-2cb5-11eb-9e80-c469cc69b848.gif"></br>
```lua
-- reference: https://github.com/ocornut/imgui/issues/3606#issuecomment-731726406
UI = ImGui.new()
IO = UI:getIO()
local w = 320
local h = 180
IO:setDisplaySize(w*2,h*2)

local cos,sin,sqrt=math.cos,math.sin,math.sqrt
local p = {-1,-1, 1,-1, 1,1, -1,1}
local function conv(z,szx,szy,ox,oy,vx,vy) return ((vx/z)*szx*5+szx*0.5)+ox,((vy/z)*szy*5+szy*0.5)+oy end
local function R(vx, vy, ng) ng*=0.1 local cosn = cos(ng) local sinn = sin(ng) return vx*cosn-vy*sinn, vx*sinn+vy*cosn end
local function FX(d,ax,ay,bx,by,sw,sh,t)
	d:addRectFilled(ax,ay,bx,by,0,1,0)
	t *= 4
	for i = 0, 19 do
		local z=21-i-(t-(t//1))*2
		local ng,ot0,ot1=-t*2.1+z,-t+z*0.2,-t+(z+1)*0.2
		local s,of,pts={cos((t+z)*0.1)*0.2+1,sin((t+z)*0.1)*0.2+1,cos((t+z+1)*0.1)*0.2+1,sin((t+z+1)*0.1)*0.2+1},{cos(ot0)*0.3,sin(ot0)*0.3,cos(ot1)*0.3,sin(ot1)*0.3},{}
		for j=0,7 do local i,n = ((j%4)+1)*2,j//4 pts[j*2+1],pts[j*2+2]=conv((z+n)*2,sw,sh,ax,ay,R(p[i-1]*s[n*2+1]+of[n*2+1],p[i-0]*s[n*2+2]+of[n*2+2],ng+n)) end
		for j=0,3 do local it=((((i&1) ~= 0) and 0.5 or 0.6)+j*0.05)*((21-z)/21) d:addConvexPolyFilled({pts[j*2+1],pts[j*2+2],pts[((j+1)%4)*2+1],pts[((j+1)%4)*2+2],pts[(((j+1)%4)+4)*2+1],pts[(((j+1)%4)+4)*2+2],pts[(j+4)*2+1],pts[(j+4)*2+2]},UI:colorConvertRGBtoHEX(UI:colorConvertHSVtoRGB(0.6+sin(t*0.03)*0.5,1,sqrt(it)))) end
	end
end

function onEnterFrame(e)
	UI:newFrame(e)
	UI:beginWindow("FX", nil, ImGui.WindowFlags_AlwaysAutoResize)
	UI:invisibleButton("canvas", w, h)
	local minX, minY = UI:getItemRectMin()
	local maxX, maxY = UI:getItemRectMax()
	local draw_list = UI:getWindowDrawList()
	draw_list:pushClipRect(minX, minY, maxX, maxY)
	FX(draw_list,minX,minY,maxX,maxY,w,h,UI:getTime())
	draw_list:popClipRect()
	UI:endWindow()
	UI:render()
	UI:endFrame()
end

stage:addChild(UI)
stage:addEventListener("enterFrame", onEnterFrame)
```
