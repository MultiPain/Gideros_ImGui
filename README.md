# Dear ImGui LUA binding for [Gideros mobile](http://giderosmobile.com/)
[Dear ImGui](https://github.com/ocornut/imgui)

# Constructor
```lua
-- font_atlas: copy fonts
-- mouse_listeners: adds internal mouse event listeners
-- keyboard_listeners: adds internal keyboard event listeners
-- touch_listeners: adds internal touch event listeners
ImGui.new([font_atlas = nil, mouse_listeners = true, keyboard_listeners = true, touch_listeners = false])
```
## EXPERIMENTAL
```lua
p_open = ImGui:showLog(title, p_open [, ImGui.WindowFlags = 0]) -- draw log window
ImGui:writeLog(text)
```

## FONTS 
```lua
IO = imgui:getIO()
FontAtlas = IO:getFonts()

Font = FontAtlas:addFont(ttf_font_path, font_size [, options])

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
number = FontAtlas:addCustomRectFontGlyph(font, id, width, height, advance_x [, offset_x, offset_y])
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
	UI:newFrame(e.deltaTime)
	
	UI:pushFont(font1)
	UI:text("Font1")
	UI:popFont()
	
	UI:pushFont(font2)
	UI:text("Font2")
	UI:popFont()
	
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
local UI = ImGui.new(nil, false, false, false)
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
Style:setColor(ImGui.Col, color, alpha)
color, alpha = Style:getColor(ImGui.Col)
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
Style:setCellPadding(x, y)
x, y = Style:getCellPadding()
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
Style:setWindowMenuButtonPosition(ImGui.Dir)
dir = Style:getWindowMenuButtonPosition()
Style:setColorButtonPosition(ImGui.Dir)
dir = Style:getColorButtonPosition()
Style:setAntiAliasedLines(flag)
flag = Style:getAntiAliasedLines()
Style:setAntiAliasedLinesUseTex(flag)
flag = Style:getAntiAliasedLinesUseTex()
Style:setAntiAliasedFill(flag)
flag = Style:getAntiAliasedFill()
Style:setDisabledAlpha(number)
alpha = Style:getDisabledAlpha()
```

## DEFAULT STYLES 
```lua
ImGui:setDarkStyle()
ImGui:setLightStyle()
ImGui:setClassicStyle()
```

## Color convert
```lua
-- note: use DOT instead of COLON, so you can use it without creating an ImGui object
r, g, b, a = ImGui.colorConvertHEXtoRGB(color [, alpha = 1])
hex = ImGui.colorConvertRGBtoHEX(r, g, b)
h, s, v = ImGui.colorConvertRGBtoHSV(r, g, b)
r, g, b = ImGui.colorConvertHSVtoRGB(h, s, v)
h, s, v = ImGui.colorConvertHEXtoHSV(hex)
hex = ImGui.colorConvertHSVtoHEX(h, s, v)
```

## IO Functions
### Get IO instance
```lua
local IO = ImGui:getIO()
```

```lua
IO:setFontDefault(font)
ImGuiConfigFlag = IO:getConfigFlags()
IO:setConfigFlags(ImGui.ConfigFlag)
IO:addConfigFlags(ImGui.ConfigFlag)
ImGuiBackendFlag = IO:getBackendFlags()
IO:setBackendFlags(ImGui.BackendFlag)
number = IO:getIniSavingRate()
IO:setIniSavingRate(number)
string = IO:getIniFilename()
IO:setIniFilename(string)
IO:saveIniSettings([path]) -- if path is not defined the it uses default path, which is set by IO:setIniFilename()
IO:loadIniSettings([path])
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
IO:setNavInput(ImGui.NavInput, value) -- see enums
value = IO:getNavInput(ImGui.NavInput)
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
-- reset mouse buttons state
IO:resetMouseDown()
-- reset key states (including ALT/SHIFT/CTRL/SUPER (META))
IO:resetKeysDown()
-- set ALT/SHIFT/CTRL/SUPER (META) key state
IO:setModKeyDown(key_code, bool) 
-- set any key state
IO:setKeysDown(key_code, bool)
-- adds text to active text input widget
IO:addInputCharactersUTF8(text)
-- emulate wheel scrolling
IO:setMouseWheel(number)
-- sets mouse position (data only, no visual changes)
IO:setMousePos(x, y)
-- set mouse state, where index: 0 - left mouse, 1 - right, 2 - middle, 3 - unused, 4 - unused.
IO:setMouseDown(index, state) 
```

# Context
```lua
string OR nil = ImGui:getHoveredWindow()
string OR nil = ImGui:getHoveredWindowRoot()
string OR nil = ImGui:getHoveredWindowUnderMovingWindow()
string OR nil = ImGui:getMovingWindow()
string OR nil = ImGui:getActiveIdWindow()
id = ImGui:getActiveId()
id = ImGui:getActiveIdPreviousFrame()
number = ImGui:getActiveIdTimer()
id = ImGui:getActiveIdAllowOverlap()
id = ImGui:getHoveredId()
id = ImGui:getHoveredIdPreviousFrame()
number = ImGui:getHoveredIdTimer()
id = ImGui:getHoveredIdAllowOverlap()
bool = ImGui:getDragDropActive()
id = ImGui:getDragDropPayloadSourceId()
string = ImGui:getDragDropPayloadDataType()
number = ImGui:getDragDropPayloadDataSize()
```

# WIDGETS & STUFF
## Windows
```lua
p_open, draw = ImGui:beginWindow(label, p_open [, ImGui.WindowFlags = 0])
-- do not show "X" button
draw = ImGui:beginWindow(label, nil [, ImGui.WindowFlags = 0])
-- start a window with no borders, no paddings, no rounding and ImGui.WindowFlags_Fullscreen flag
p_open, draw = ImGui:beginFullScreenWindow(label, p_open [, ImGui.WindowFlags = 0]) 
-- do not show "X" button
draw = ImGui:beginFullScreenWindow(label, nil [, ImGui.WindowFlags = 0]) 
ImGui:endWindow()
```

## Disabled groups
```lua
ImGui:beginDisabled(disabledFlag)
ImGui:endDisabled()
```

## Child Windows
```lua
ImGui:beginChild(id [, w = 0, h = 0, borderFlag = false, ImGui.WindowFlags = 0])
ImGui:endChild()
```

## Windows Utilities
```lua
flag = ImGui:isWindowAppearing()
flag = ImGui:isWindowCollapsed()
flag = ImGui:isWindowFocused([ImGui.FocusedFlags = 0])
flag = ImGui:isWindowHovered([ImGui.HoveredFlags = 0])
x, y = ImGui:getWindowPos()
w, h = ImGui:getWindowSize()
w = ImGui:getWindowWidth()
h = ImGui:getWindowHeight()

x1,y1, x2,y2 = ImGui:getWindowBounds() -- returns window region rectangle in global coordinates
ImGui:setNextWindowPos(x, y [, ImGui.Cond = 0, pivotX = 0, pivotY = 0])
ImGui:setNextWindowSize(w, h [, ImGui.Cond = 0])
ImGui:setNextWindowContentSize(w, h)
ImGui:setNextWindowCollapsed(flag [, ImGui.Cond = 0])
ImGui:setNextWindowFocus()
ImGui:setNextWindowBgAlpha(alpha)
ImGui:setWindowPos(name, x, y [, ImGui.Cond = 0]) OR ImGui:setWindowPos(x, y [, ImGui.Cond = 0])
ImGui:setWindowSize(name, w, h [, ImGui.Cond = 0]) OR ImGui:setWindowSize(w, h [, ImGui.Cond = 0])
ImGui:setWindowCollapsed(name, flag [, ImGui.Cond = 0]) OR ImGui:setWindowCollapsed(flag [, ImGui.Cond = 0])
ImGui:setWindowFocus(name) OR ImGui:setWindowFocus()
ImGui:setWindowFontScale(scale)
```

### Window size constraints
Can be used to set minimum and maximum window size, plus contraint the size if needed
```lua
-- call this function before ImGui:beginWindow()
ImGui:setNextWindowSizeConstraints(min_w, min_h, max_w, max_h [, resize_callback, user_data]))
-- resizeCallback is a function:
function (callbackData [, user_data])
	-- get window position
	local x, y = callbackData:getPos()
	-- get currrent size
	local current_width, current_height = callbackData:getCurrentSize()
	-- get desired size
	local deserid_width, deserid_height = callbackData:getDesiredSize()
	-- do some math, and return desired size
	-- ...
	return desired_width, desired_height
end
```

### Example
```lua
require "ImGui"

local function stepSize(callback_data, step)
	local w, h = callback_data:getDesiredSize()
	w = (w // step) * step
	h = (h // step) * step
	return w, h
end

local ui = ImGui.new()
stage:addChild(ui)

stage:addEventListener("enterFrame", function(e)
	ui:newFrame(e.deltaTime)
	
	-- window size step is 32 (last argument, that is passed to the callback as second argument)
	ui:setNextWindowSizeConstraints(200, 200, 400, 400, stepSize, 32)
	if (ui:beginWindow("My window")) then 
		
		ui:textWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.")
	end
	ui:endWindow()
	
	ui:showDemoWindow()
	
	ui:render()
	ui:endFrame()
end)
```

## Content region 
```lua
scaleX, scaleY = ImGui:getContentRegionMax()
w, h = ImGui:getContentRegionAvail()
x, y = ImGui:getWindowContentRegionMin()
x, y = ImGui:getWindowContentRegionMax()
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
ImGui:setScrollFromPosX(x [, ratio = 0.5])
ImGui:setScrollFromPosY(y [, ratio = 0.5])
```

## Parameters stacks (shared)
```lua
ImGui:pushStyleColor(ImGui.Col, color, alpha)
ImGui:popStyleColor([count = 1])
ImGui:pushStyleVar(ImGui.StyleVar, value) OR ImGui:pushStyleVar(ImGui.StyleVar, value1, value2)
ImGui:popStyleVar([count = 1])
color, alpha = ImGui:getStyleColor(ImGui.Col)
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
ImGui:sameLine([offset_x = 0, spacing = -1])
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
ImGui:pushID(str)
ImGui:popID()
number = ImGui:getID(any_value)
number = ImGui:getID(string)
```

## Widgets: Text
```lua
ImGui:textUnformatted(text [, textEnd])
ImGui:text(text)
ImGui:textColored(text, color, alpha)
ImGui:textDisabled(text)
ImGui:textWrapped(text)
ImGui:labelText(text, label)
ImGui:bulletText(text)
```

## Widgets: Main
```lua
flag = ImGui:button(text [, w = 0, h = 0])
flag = ImGui:smallButton(text)
flag = ImGui:invisibleButton(string_ID [, w = 0, h = 0])
flag = ImGui:arrowButton(string_ID [, ImGui.Dir = 0])
flag = ImGui:checkbox(text, flag)
flags, is_changed = ImGui:checkboxFlags(label [, flags = 0, flags_value = 0])
number, is_changed = ImGui:radioButton(text, number, number)
is_changed = ImGui:radioButton(text, flag)
ImGui:progressBar(fraction [, anchor_x = -1, anchor_y = 0, overlay_string = nil])
ImGui:bullet()
```

## Widgets: Images
```lua
-- Images are streched (ImGui default functions)
ImGui:image(texture, w, h [, tint_color = 0xffffff, tint_alpha = 1, border_color = 0xffffff, border_alpha = 0])
pressFlag = ImGui:imageButton(texture, w, h [, padding = -1, tint_color = 0xffffff, tint_alpha = 1, border_color = 0xffffff, border_alpha = 0])
-- Images are scaled (extended by @MultiPain)
-- padding deprecated (use "ImGui:pushStyleVar(ImGui.StyleVar_FramePadding, x, y)/ImGui:popStyleVar()")
ImGui:scaledImage(texture, w, h [, fit_mode = ImGui.ImageScaleMode_LetterBox, keep_size = false, 
		  anchor_x = 0.5, anchor_y = 0.5, 
		  tint_col = 0xffffff, tint_alpha = 1, 
		  border_col = 0, border_alpha = 0, 
		  bg_col = 0, bg_alpha = 0])
pressFlag = ImGui:scaledImageButton(texture, w, h [, fit_mode = ImGui.ImageScaleMode_LetterBox, keep_size = false, 
				    ImGui.ButtonFlags = 0, anchor_x = 0.5, anchor_y = 0.5, 
		 		    clip_offset_x = 0, clip_offset_y = 0,
				    tint_col = 0xffffff, tint_alpha = 1, 
				    border_col = 0, border_alpha = 0, 
				    bg_col = 0, bg_alpha = 0])
pressFlag = ImGui:scaledImageButtonWithText(texture, label, image_w, image_h [, button_w = 0, button_h = 0, 
					    ImGui.ButtonFlags = 0, fit_mode = ImGui.ImageScaleMode_LetterBox, keep_size = false, 
					    anchor_x = 0.5, anchor_y = 0.5, image_side = ImGui.Dir_Left, 
					    clip_offset_x = 0, clip_offset_y = 0,
					    tint_col = 0xffffff, tint_alpha = 1, 
					    border_col = 0, border_alpha = 0, 
					    bg_col = 0, bg_alpha = 0])
```

## Widgets: Combo Box
```lua
openFlag = ImGui:beginCombo(text, preview_text [, ImGui.ComboFlags = 0])
ImGui:endCombo()
current_item, is_open = ImGui:combo(label, current_item, items) -- items (table): {"item1", "item2", ...}
```

## Widgets: Drags 
```lua
value, is_changed = ImGui:dragFloat(label, value [, inc_step = 1, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value1, value2, is_changed = ImGui:dragFloat2(label, value1, value2 [, inc_step = 1, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value1, value2, value3, is_changed = ImGui:dragFloat3(label, value1, value2, value3 [, inc_step = 1, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value1, value2, value3, value4, is_changed = ImGui:dragFloat4(label, value1, value2, value3, value4 [, inc_step = 1, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value_min, value_max, is_changed = ImGui:dragFloatRange2(label, value_min, value_max [, inc_step = 1, min = 0, max = 0, format_min_string = "%.3f", ImGui.SliderFlags = 0])
-- table must be an array of any size > 0
is_changed = ImGui:dragFloatT(label, table [, inc_step = 1, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])

value, is_changed = ImGui:dragInt(label, value [, inc_step = 1, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value1, value2, is_changed = ImGui:dragInt2(label, value1, value2 [, inc_step = 1, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value1, value2, value3, is_changed = ImGui:dragInt3(label, value1, value2, value3 [, inc_step = 1, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value1, value2, value3, value4, is_changed = ImGui:dragInt4(label, value1, value2, value3, value4 [, inc_step = 1, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
v_current_min, v_current_max, flag = ImGui:dragIntRange2(label, v_current_min, v_current_max [, v_speed = 1, v_min = 0, v_max = 0, format = "%d", format_max = nil, ImGui.SliderFlags = 0])
-- table must be an array of any size > 0
is_changed = ImGui:dragIntT(label, table [, inc_step = 1, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
```

## Widgets: Sliders
```lua
value, is_changed = ImGui:sliderFloat(label, value [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value1, value2, is_changed = ImGui:sliderFloat2(label, value1, value2 [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value1, value2, value3, is_changed = ImGui:sliderFloat3(label, value1, value2, value3 [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value1, value2, value3, value4, is_changed = ImGui:sliderFloat4(label, value1, value2, value3, value4 [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value_in_rad, is_changed = ImGui:sliderAngle(label, value_in_rad [, min_degrees = -360, max_degrees = 360, format_string = "%.0f deg", ImGui.SliderFlags = 0])
-- table must be an array of any size > 0
is_changed = ImGui:sliderFloatT(label, table [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value, is_changed = ImGui:sliderInt(label, value [, min = 0, max = 0, format_string = "%d, ImGui.SliderFlags = 0"])
value1, value2, is_changed = ImGui:sliderInt2(label, value1, value2 [, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value1, value2, value3, is_changed = ImGui:sliderInt3(label, value1, value2, value3 [, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value1, value2, value3, value4, is_changed = ImGui:sliderInt4(label, value1, value2, value3, value4 [, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
-- table must be an array of any size > 0
is_changed = ImGui:sliderIntT(label, table [, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value, is_changed = ImGui:vSliderFloat(label, w, h, value, min, max [, format_string = "%.3f", ImGui.SliderFlags = 0])
value, is_changed = ImGui:vSliderInt(label, w, h, value, min, max [, format_string = "%d", ImGui.SliderFlags = 0])

value, is_changed = ImGui:filledSliderFloat(label, mirror_flag, value [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value1, value2, is_changed = ImGui:filledSliderFloat2(label, mirror_flag, value1, value2 [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value1, value2, value3, is_changed = ImGui:filledSliderFloat3(label, mirror_flag, value1, value2, value3 [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value1, value2, value3, value4, is_changed = ImGui:filledSliderFloat4(label, mirror_flag, value1, value2, value3, value4 [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
-- table must be an array of any size > 0
is_changed = ImGui:filledSliderFloatT(label, mirror_flag, table [, min = 0, max = 0, format_string = "%.3f", ImGui.SliderFlags = 0])
value_in_rad, is_changed = ImGui:filledSliderAngle(label, mirror_flag, value_in_rad [, min_degrees = -360, max_degrees = 360, format_string = "%.0f deg", ImGui.SliderFlags = 0])
value, is_changed = ImGui:filledSliderInt(label, mirror_flag, value [, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value1, value2, is_changed = ImGui:filledSliderInt2(label, mirror_flag, value1, value2 [, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value1, value2, value3, is_changed = ImGui:filledSliderInt3(label, mirror_flag, value1, value2, value3 [, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value1, value2, value3, value4, is_changed = ImGui:filledSliderInt4(label, mirror_flag, value1, value2, value3, value4 [, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
-- table must be an array of any size > 0
is_changed = ImGui:filledSliderIntT(label, mirror_flag, table [, min = 0, max = 0, format_string = "%d", ImGui.SliderFlags = 0])
value, is_changed = ImGui:vFilledSliderFloat(label, mirror_flag, w, h, value, min, max [, format_string = "%.3f", ImGui.SliderFlags = 0])
value, is_changed = ImGui:vFilledSliderInt(label, mirror_flag, w, h, value, min, max [, format_string = "%d", ImGui.SliderFlags = 0])
```

## Widgets: Input with Keyboard
```lua
text, flag = ImGui:inputText(label, text, buffer_size [, ImGui.InputTextFlags = 0])
text, flag = ImGui:inputTextMultiline(label, text, buffer_size [, w = 0, h = 0, ImGui.InputTextFlags = 0])
text, flag = ImGui:inputTextWithHint(label, text, hint, buffer_size [, ImGui.InputTextFlags = 0])
value,  flag = ImGui:inputFloat(label, value [, step = 0, step_fast = 0, format = "%.3f", ImGui.InputTextFlags = 0])
value1, value2, flag = ImGui:inputFloat2(label, value1, value2 [, format = "%.3f", ImGui.InputTextFlags = 0])
value1, value2, value3, flag = ImGui:inputFloat3(label, value1, value2, value3 [, format = "%.3f", ImGui.InputTextFlags = 0])
value1, value2, value3, value4, flag = ImGui:inputFloat4(label, value1, value2, value3, value4 [, format = "%.3f", ImGui.InputTextFlags = 0])
-- table must be an array of any size > 0
flag = ImGui:inputFloatT(label, table [, format = "%.3f", ImGui.InputTextFlags = 0])
value,  flag = ImGui:inputInt(label, value [, step = 0, step_fast = 0, ImGui.InputTextFlags = 0])
value1, value2, flag = ImGui:inputInt2(label, value1, value2 [, ImGui.InputTextFlags = 0])
value1, value2, value3, flag = ImGui:inputInt3(label, value1, value2, value3 [, ImGui.InputTextFlags = 0])
value1, value2, value3, value4, flag = ImGui:inputInt4(label, value1, value2, value3, value4 [, ImGui.InputTextFlags = 0])
-- table must be an array of any size > 0
flag = ImGui:inputIntT(label, table [, format = "%d", ImGui.InputTextFlags = 0])
value, flag = ImGui:inputDouble(label, value [, step = 0, step_fast = 0, format = "%.6f", ImGui.InputTextFlags = 0])
```

### Input text callbacks
```lua
ImGui:inputText(label, text, buffer_size [, ImGui.InputTextFlags = 0, callback_function, user_data])
ImGui:inputTextMultiline(label, text, buffer_size [, ImGui.InputTextFlags = 0, callback_function, user_data])
ImGui:inputTextWithHint(label, text, hint, buffer_size [, ImGui.InputTextFlags = 0, callback_function, user_data])

callback_function = function(callback_data, user_data)
	-- do something with data
	-- see below
end
```
### callback_data
```lua
ImGuiInputTextFlags = callback_data:getEventFlag()
ImGuiInputTextFlags = callback_data:getFlags()
number = callback_data:getEventChar()
callback_data:setEventChar(number)
number = callback_data:getEventKey()
string = callback_data:getBuf()
callback_data:setBuf(string)
number = callback_data:getBufTextLen()
callback_data:setBufTextLen(number)
number = callback_data:getBufSize()
bool = callback_data:getBufDirty()
callback_data:setBufDirty(bool)
number = callback_data:getCursorPos()
callback_data:setCursorPos(number)
number = callback_data:getSelectionStart()
callback_data:setSelectionStart(number)
number = callback_data:getSelectionEnd()
callback_data:setSelectionEnd(number)
callback_data:deleteChars(position, bytesCount)
callback_data:insertChars(position, text)
callback_data:selectAll()
callback_data:clearSelection()
bool = callback_data:hasSelection()
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
	if (key == key_code.UP) then
		data:deleteChars(0, data:getBufTextLen())
		data:insertChars(0, "Pressed Up!")
		data:selectAll()
	elseif (key == key_code.DOWN) then
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
	ui:newFrame(e.deltaTime)
	
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
hexColor, is_touching = ImGui:colorEdit3(label, color [, ImGui.ColorEditFlags = 0]) -- alpha ignored, no need to pass it!
hexColor, alpha, is_touching = ImGui:colorEdit4(label, color [, alpha = 1, ImGui.ColorEditFlags = 0])
hexColor, is_touching = ImGui:colorPicker3(label, color [, ImGui.ColorEditFlags = 0])
hexColor, alpha, originalColor, originalAlpha, is_touching = ImGui:colorPicker4(label, color [, alpha = 1, original_color = 0xffffff, original_alpha = 1, ImGui.ColorEditFlags = 0])
isHoveringFlag = ImGui:colorButton(string_ID, color [, alpha = 1, ImGui.ColorEditFlags = 0, w = 0, h = 0])
ImGui:setColorEditOptions(ImGui.ColorEditFlags)
```

## Widgets: Trees
```lua
is_openFlag = ImGui:treeNode(label [, format_string])
ImGui:treeNodeEx(label, ImGui.TreeNodeFlags [, format_string])
ImGui:treePush(str_id)
ImGui:treePop()
number = ImGui:getTreeNodeToLabelSpacing()
is_openFlag, p_open = ImGui:collapsingHeader(label, p_open [, ImGui.TreeNodeFlags = 0])
is_openFlag = ImGui:collapsingHeader(label [, ImGui.TreeNodeFlags = 0])
ImGui:setNextItemOpen(is_open, ImGui.Cond)
```

## Widgets: Selectables
```lua
result?, selected = ImGui:selectable(label, selected [, ImGui.SelectableFlags = 0, w = 0, h = 0])
```

## Widgets: List Boxes
```lua
current_item, is_openFlag = ImGui:listBox(label, current_item, item_table [, max_visible_items = -1]) -- item_table: {"Item0", "Item1", ...}
result? = ImGui:listBoxHeader(label [, w = 0, h = 0])
result? = ImGui:listBoxHeader2(label, items_count)
ImGui:listBoxFooter()
```

## Widgets: Data Plotting
```lua
ImGui:plotLines(label, points_table [, values_offset = 0, overlay_text = nil, scale_min = math.huge, scale_max = math.huge, w = 0, h = 0]) -- points_table: {0.01, 0.5, 10, -50, ...}
ImGui:plotHistogram(label, points_table [, values_offset = 0, overlay_text = nil, scale_min = math.huge, scale_max = math.huge, w = 0, h = 0])"plotLines"
```

## Widgets: Value() Helpers
```lua
ImGui:value(prefix, bool)
ImGui:value(prefix, number)
ImGui:value(prefix, float, format_string)
```

## Widgets: Menus
```lua
result? = ImGui:beginMenuBar()
ImGui:endMenuBar()
result? = ImGui:beginMainMenuBar()
ImGui:endMainMenuBar()
result? = ImGui:beginMenu(label, enabled_flag)
ImGui:endMenu()
result? = ImGui:menuItem(label [, shortcut_string = nil, selected = false, enabled = true])
selected, result? = ImGui:menuItemWithShortcut(label, shortcut_string [, selected = false, enabled = true])
ImGui:beginTooltip()
ImGui:endTooltip()
ImGui:setTooltip(text)
```

## Popups, Modals
```lua
result? = ImGui:beginPopup(str_id [, ImGui.WindowFlags = 0])
p_open, result? = ImGui:beginPopupModal(str_id, p_open [, ImGui.WindowFlags = 0])
ImGui:endPopup()
ImGui:openPopup(str_id [, ImGui.PopupFlags = 0])
ImGui:openPopupOnItemClick(str_id [, ImGui.PopupFlags = 0])
ImGui:closeCurrentPopup()
result? = ImGui:beginPopupContextItem(str_id [, ImGui.PopupFlags = 0])
result? = ImGui:beginPopupContextWindow(str_id [, ImGui.PopupFlags = 0])
result? = ImGui:beginPopupContextVoid(str_id [, ImGui.PopupFlags = 0])
result? = ImGui:isPopupOpen(str_id [, ImGui.PopupFlags = 0])
```

## Tables
```lua
flag = ImGui:beginTable(str_id, column [, ImGui.TableFlags = 0, outer_w = 0, outer_h = 0, inner_width = 0])
ImGui:endTable()
ImGui:tableNextRow([ImGui.TableRowFlags = 0, min_row_height = 0])
flag = ImGui:tableNextColumn()
flag = ImGui:tableSetColumnIndex(column_n)
ImGui:tableSetupColumn(label [, ImGui.TableColumnFlags = 0, init_width_or_weight = 0, user_id = 0])
ImGui:tableSetupScrollFreeze(cols, rows)
ImGui:tableHeadersRow()
TableSortSpecs = ImGui:tableGetSortSpecs() -- see below
number = ImGui:tableGetColumnCount()
number = ImGui:tableGetColumnIndex()
number = ImGui:tableGetRowIndex()
string = ImGui:tableGetColumnName([column_n = -1])
ImGuiTableColumnFlags = ImGui:tableGetColumnFlags([column_n = -1])
ImGui:tableSetBgColor(ImGui.TableBgTarget, color [, alpha = 1, column_n = -1])
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
bool = ImGui:beginTabBar(str_id [, ImGui.TabBarFlags = 0])
ImGui:endTabBar()
p_open, bool = ImGui:beginTabItem(label, p_open [, ImGui.TabItemFlags = 0])
ImGui:endTabItem()
ImGui:setTabItemClosed(tab_or_docked_window_label)
ImGui:tabItemButton(label [, ImGui.TabItemFlags = 0])
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
flag = ImGui:beginDragDropSource([ImGui.DragDropFlags flags = 0])
flag = ImGui:setNumDragDropPayload(str_type, number [, ImGui.Cond cond = 0])
flag = ImGui:setStrDragDropPayload(str_type, string [, ImGui.Cond cond = 0])
ImGui:endDragDropSource()
flag = ImGui:beginDragDropTarget()
ImGuiPayload = ImGui:acceptDragDropPayload(type [, ImGui.DragDropFlags flags = 0])
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
	UI:newFrame(e.deltaTime)
 
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
ImGui:pushClipRect(min_x, min_y, max_x, max_y, intersect_with_current_clip_rect)
ImGui:popClipRect()
```

## Focus, Activation
```lua
ImGui:setItemDefaultFocus()
ImGui:setKeyboardFocusHere([offset = 0])
flag = ImGui:isItemHovered([ImGui.HoveredFlags = 0])
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
flag = ImGui:isRectVisible(w, h [, max_x, max_y])
number = ImGui:getTime()
number = ImGui:getFrameCount()
str = ImGui:getStyleColorName(idx)
out_items_display_start, out_items_display_end = ImGui:calcListClipping(items_count, items_height, out_items_display_start, out_items_display_end)
flag = ImGui:beginChildFrame(id, w, h [, ImGui.WindowFlags = 0]) -- id (number)
ImGui:endChildFrame()
```

## Text Utilities
```lua
w, h = ImGui:calcTextSize(text [, hide_text_after_double_hash = false, wrap_width = -1])
```

## Inputs Utilities: Keyboard
```lua
number = ImGui:getKeyIndex(ImGui.Key)
flag = ImGui:isKeyDown(user_key_index)
flag = ImGui:isKeyPressed(user_key_index [, repeat = true])
flag = ImGui:isKeyReleased(user_key_index)
number = ImGui:getKeyPressedAmount(key_index, repeat_delay, rate)
ImGui:captureKeyboardFromApp([want_capture_keyboard_value = true])
```

## Inputs Utilities: Mouse
```lua
flag = ImGui:isMouseDown(mouse_button)
flag = ImGui:isMouseClicked(mouse_button [, repeat = false])
flag = ImGui:isMouseReleased(mouse_button)
flag = ImGui:isMouseDoubleClicked(mouse_button)
flag = ImGui:isMouseHoveringRect(min_x, min_y, max_x, max_y [, clip = true])
flag = ImGui:isMousePosValid([x = inf, y = inf])
flag = ImGui:isAnyMouseDown()
x, y = ImGui:getMousePos()
x, y = ImGui:getMousePosOnOpeningCurrentPopup()
flag = ImGui:isMouseDragging(mouse_button [, lock_threshold = -1])
x, y = ImGui:getMouseDragDelta(mouse_button [, lock_threshold = -1])
ImGui:resetMouseDragDelta(mouse_button)
ImGuiMouseCursor = ImGui:getMouseCursor()
ImGui:setMouseCursor(ImGui.MouseCursor)
ImGui:captureMouseFromApp([want_capture_mouse_value = true])
ImGui:setAutoUpdateCursor(flag) -- uses application:set("cursor", name) to modify native cursor
flag = ImGui:getAutoUpdateCursor()
```

## Render
```lua
ImGui:newFrame(deltaTime)
ImGui:render()
ImGui:endFrame()
```

## Demos
```lua
is_openFlag = ImGui:showUserGuide()
is_openFlag = ImGui:showDemoWindow([p_open])
is_openFlag = ImGui:showAboutWindow([p_open])
is_openFlag = ImGui:showStyleEditor()
is_openFlag = ImGui:showFontSelector()
is_openFlag = ImGui:showMetricsWindow([p_open])
is_openFlag = ImGui:showStyleSelector(label)
is_openFlag = ImGui:ShowStackToolWindow([p_open])
ImGui:showLuaStyleEditor()
```

## TextEditor
```lua
-- otherTextEditor: another "ImGuiTextEditor" instance to copy setting 
TextEditor = ImGuiTextEditor.new([other_text_editor])
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

TextEditor:setPaletteColor(TE_ColorIndex, color [, alpha = 1]) -- see TextEditor enums below
color, alpha = TextEditor:getPaletteColor(TE_ColorIndex)

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
TextEditor:setErrorMarkers(error_markers)
TextEditor:setBreakpoints(breakpoints)

TextEditor:render(string_id [, w = 0, h = 0, border = 0])

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
TextEditor:setSelection(start_line, start_column, end_line, end_column)
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
ImGui.FocusedFlags_NoPopupHierarchy
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
ImGui.InputTextFlags_None
ImGui.InputTextFlags_EnterReturnsTrue
ImGui.InputTextFlags_ReadOnly
ImGui.InputTextFlags_AutoSelectAll
ImGui.InputTextFlags_AllowTabInput
ImGui.InputTextFlags_CharsScientific
ImGui.InputTextFlags_CharsDecimal
ImGui.InputTextFlags_NoUndoRedo
ImGui.InputTextFlags_CtrlEnterForNewLine
ImGui.InputTextFlags_CharsHexadecimal
ImGui.InputTextFlags_CharsNoBlank
ImGui.InputTextFlags_Password
ImGui.InputTextFlags_NoHorizontalScroll
ImGui.InputTextFlags_AlwaysInsertMode
ImGui.InputTextFlags_CharsUppercase
ImGui.InputTextFlags_NoBackground -- custom constant, used to disable background
ImGui.InputTextFlags_CallbackCompletion
ImGui.InputTextFlags_CallbackResize
ImGui.InputTextFlags_CallbackAlways
ImGui.InputTextFlags_CallbackHistory
ImGui.InputTextFlags_CallbackCharFilter
ImGui.InputTextFlags_CallbackEdit
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
ImGui.StyleVar_DisabledAlpha
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

### corner_flags
```lua
ImGui.corner_flags_None
ImGui.corner_flags_TopLeft
ImGui.corner_flags_TopRight
ImGui.corner_flags_BotLeft
ImGui.corner_flags_BotRight
ImGui.corner_flags_Top
ImGui.corner_flags_Bot
ImGui.corner_flags_Left
ImGui.corner_flags_Right
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
ImGui.TableColumnFlags_Disabled
ImGui.TableColumnFlags_NoHeaderLabel
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

## ImageScaleMode
```lua
ImGui.ImageScaleMode_LetterBox
ImGui.ImageScaleMode_FitWidth
ImGui.ImageScaleMode_FitHeight
ImGui.ImageScaleMode_Stretch
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
DrawList:pushClipRect(clip_rect_min_x, clip_rect_min_y, clip_rect_max_x, clip_rect_max_y [, intersect_with_current_clip_rect = false])
DrawList:pushClipRectFullScreen()
DrawList:popClipRect()
DrawList:pushTextureID(texture)
DrawList:popTextureID()
x, y = DrawList:getClipRectMin()
x, y = DrawList:getClipRectMax()
DrawList:addLine(p1_x, p1_y, p2_x, p2_y, color [, alpha = 1, thickness = 1])
DrawList:addRect(p_min_x, p_min_y, p_max_x, p_max_y, color [, alpha = 1, rounding = 0, rounding_corners = ImGui.CornerFlags_All, thickness = 1])
DrawList:addRectFilled(p_min_x, p_min_y, p_max_x, p_max_y, color [, alpha = 1, rounding = 0, rounding_corners = ImGui.CornerFlags_All])
DrawList:addRectFilledMultiColor(p_min_x, p_min_y, p_max_x, p_max_y, color_upr_left, color_upr_right, color_bot_right, color_bot_left)
DrawList:addQuad(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color [, alpha = 1, thickness = 1])
DrawList:addQuadFilled(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color)
DrawList:addTriangle(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, color [, alpha = 1, thickness = 1])
DrawList:addTriangleFilled(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, color)
DrawList:addCircle(center_x, center_y, radius, color [, alpha = 1, num_segments = 12, thickness = 1])
DrawList:addCircleFilled(center_x, center_y, radius, color [, alpha = 1, num_segments = 12])
DrawList:addNgon(center_x, center_y, radius, color [, alpha = 1, num_segments = 12, thickness = 1])
DrawList:addNgonFilled(center_x, center_y, radius, color [, alpha = 1, num_segments = 12])
DrawList:addText(x, y, color, alpha, text) -- x, y (number), text_begin (string), text_end (string)
DrawList:addFontText(font, font_size, pos_x, pos_y, color, alpha, text [, wrap_with = 0, cpu_fine_clip_rect_x, cpu_fine_clip_rect_y, cpu_fine_clip_rect_w, cpu_fine_clip_rect_h])
DrawList:addPolyline(points_table, color, alpha, closed, thickness) -- points_table (table), color (number), closed (bool), thickness (number)
DrawList:addConvexPolyFilled(points_table, color) -- points_table (table), color (number)
DrawList:addBezierCubic(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y, color, alpha, thickness [, num_segments = 0])
DrawList:addBezierQuadratic(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, color, alpha, thickness [, num_segments = 0])
DrawList:addImage(texture, x, y, x + w, y + h [, tint_color = 0xffffff, tint_alpha = 1]) 
DrawList:addImageQuad(texture, x, y, x + w, y, x + w, y + h, x, y + h [, tint_color = 0xffffff, tint_alpha = 1, uv0x = 0, uv0y = 0, uv1x = 1, uv1y = 0, uv2x = 1, uv2y = 1, uv3x = 
DrawList:addImageRounded(texture, x, y, x + w, y + h, tint_color, tint_alpha, round_radius [, corner_flags = ImGui.CorenerFlags_All])
DrawList:pathClear()
DrawList:pathLineTo(x, y)
DrawList:pathLineToMergeDuplicate(x, y)
DrawList:pathFillConvex(color)
DrawList:pathStroke(color, alpha, closed [, thickness = 1])
DrawList:pathArcTo(center_x, center_y, radius, a_min, a_max [, num_segments = 10])
DrawList:pathArcToFast(center_x, center_y, radius, a_min, a_max)
DrawList:pathBezierCubicCurveTo(p2x, p2y, p3x, p3y, p4x, p4y [, num_segments = 0])
DrawList:pathBezierQuadraticCurveTo(p2x, p2y, p3x, p3y [, num_segments = 0])
DrawList:pathRect(min_x, min_y, max_x, max_y [, rounding = 0, ImGui.CornerFlags = 0])
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
require "ImGui"
UI = ImGui.new()
IO = UI:getIO()
local w = 320
local h = 180
IO:setDisplaySize(w*2,h*2)

local cos,sin,sqrt=math.cos,math.sin,math.sqrt
local HSV2RGB=ImGui.colorConvertHSVtoRGB
local RGB2HEX=ImGui.colorConvertRGBtoHEX
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
		for j=0,7 do 
			local i,n = ((j%4)+1)*2,j//4 
			pts[j*2+1],pts[j*2+2]=conv((z+n)*2,sw,sh,ax,ay,R(p[i-1]*s[n*2+1]+of[n*2+1],p[i-0]*s[n*2+2]+of[n*2+2],ng+n)) 
		end
		for j=0,3 do 
			local it=((((i&1) ~= 0) and 0.5 or 0.6)+j*0.05)*((21-z)/21) 
			d:addConvexPolyFilled(
				{pts[j*2+1],pts[j*2+2],pts[((j+1)%4)*2+1],pts[((j+1)%4)*2+2],pts[(((j+1)%4)+4)*2+1],pts[(((j+1)%4)+4)*2+2],pts[(j+4)*2+1],pts[(j+4)*2+2]},
				RGB2HEX(HSV2RGB(0.6+sin(t*0.03)*0.5,1,sqrt(it)))
			)
		end
	end
end

function onEnterFrame(e)
	UI:newFrame(e.deltaTime)
	if (UI:beginWindow("FX", nil, ImGui.WindowFlags_AlwaysAutoResize)) then
	UI:invisibleButton("canvas", w, h)
	local min_x, min_y = UI:getItemRectMin()
	local max_x, max_y = UI:getItemRectMax()
	local draw_list = UI:getWindowDrawList()
	draw_list:pushClipRect(min_x, min_y, max_x, max_y)
	FX(draw_list,min_x,min_y,max_x,max_y,w,h,UI:getTime())
	draw_list:popClipRect()
	end
	UI:endWindow()
	UI:render()
	UI:endFrame()
end

stage:addChild(UI)
stage:addEventListener("enterFrame", onEnterFrame)
```
