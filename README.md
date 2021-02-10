# DOCKING API
* [Docking](#docking)
* [Dock builder](#dock-builder-beta)
* [Dock node](#dock-node-beta)
* [Tab Bar](#tab-bar-beta)
* [Tab Item](#tab-item-beta)
* [ENUMS](#enums)
	- [WindowFlags] (#windowflags)
    - [ConfigFlags](#configflags)
    - [TabBarFlags](#TabBarFlags)
    - [TabItemFlags](#TabItemFlags)
    - [DockNodeFlags](#docknodeflags)

## Docking
```lua
ImGui:dockSpace(ImGuiID, w, h, [ImGuiDockNodeFlags = 0])
ImGuiID = ImGui:dockSpaceOverViewport([ImGuiDockNodeFlags = 0])
ImGui:setNextWindowDockID(ImGuiID, [ImGuiCond = 0])
ImGuiID = ImGui:getWindowDockID()
flag = ImGui:isWindowDocked()
```
[To top](#api)
## Dock builder
```lua
ImGui:dockBuilderDockWindow(window_name, node_id)
ImGuiDockNode = ImGui:dockBuilderGetNode(node_id)
ImGui:dockBuilderSetNodePos(node_id, x, y)
ImGui:dockBuilderSetNodeSize(node_id, w, h)
node_id = ImGui:dockBuilderAddNode([node_id = 0, ImGuiDockNodeFlags = 0])
ImGui:dockBuilderRemoveNode(node_id)
ImGui:dockBuilderRemoveNodeChildNodes(node_id)
ImGui:dockBuilderRemoveNodeDockedWindows(node_id, clear_settings_refs_flag)
node_id, out_id_at_dir, out_id_at_opposite_dir = ImGui:dockBuilderSplitNode(node_id, ImGuiDir, size_ratio_for_node_at_dir, out_id_at_dir, out_id_at_opposite_dir)
-- ImGui:dockBuilderCopyNode(src_node_id, dst_node_id) -- W.I.P.
ImGui:dockBuilderCopyWindowSettings(src_name, dst_name)
-- ImGui:dockBuilderCopyDockSpace(src_dockspace_id, dst_dockspace_id) -- W.I.P.
ImGui:dockBuilderFinish(node_id)
```
[To top](#api)
### Dock node
```lua
number = ImGuiDockNode:getID()
number = ImGuiDockNode:getSharedFlags()
number = ImGuiDockNode:getLocalFlags()
ImGuiDockNode = ImGuiDockNode:getParentNode()
ImGuiDockNode, ImGuiDockNode = ImGuiDockNode:getChildNodes()
ImGuiTabBar = ImGuiDockNode:getTabBar()
x, y = ImGuiDockNode:getPos()
w, h = ImGuiDockNode:getSize()
w, h = ImGuiDockNode:getSizeRef()
number = ImGuiDockNode:getSplitAxis()
number = ImGuiDockNode:getState() 
-- states:
-- 0: Unknown
-- 1: Host window hidden because single window
-- 2: Host window hidden because windows are resizing
-- 3: Host window visible

ImGuiDockNode = ImGuiDockNode:getCentralNode()
ImGuiDockNode = ImGuiDockNode:getOnlyNodeWithWindows()
number = ImGuiDockNode:getLastFrameAlive()
number = ImGuiDockNode:getLastFrameActive()
number = ImGuiDockNode:getLastFrameFocused()
number = ImGuiDockNode:getLastFocusedNodeId()
flag = ImGuiDockNode:getSelectedTabId()
flag = ImGuiDockNode:getWantCloseTabId()
number = ImGuiDockNode:getAuthorityForPos()
number = ImGuiDockNode:getAuthorityForSize()
number = ImGuiDockNode:getAuthorityForViewport()
flag = ImGuiDockNode:isVisible()
flag = ImGuiDockNode:isFocused()
flag = ImGuiDockNode:hasCloseButton()
flag = ImGuiDockNode:hasWindowMenuButton()
ImGuiDockNode:enableCloseButton(flag)
flag = ImGuiDockNode:isCloseButtonEnable()
flag = ImGuiDockNode:wantCloseAll()
flag = ImGuiDockNode:wantLockSizeOnce()
flag = ImGuiDockNode:wantMouseMove()
flag = ImGuiDockNode:wantHiddenTabBarUpdate()
flag = ImGuiDockNode:wantHiddenTabBarToggle()
flag = ImGuiDockNode:isMarkedForPosSizeWrite()
flag = ImGuiDockNode:isRootNode()
flag = ImGuiDockNode:isDockSpace()
flag = ImGuiDockNode:isFloatingNode()
flag = ImGuiDockNode:isCentralNode()
flag = ImGuiDockNode:isHiddenTabBar()
flag = ImGuiDockNode:isNoTabBar()
flag = ImGuiDockNode:isSplitNode()
flag = ImGuiDockNode:isLeafNode()
flag = ImGuiDockNode:isEmpty()
ImGuiDockNodeFlags = ImGuiDockNode:getMergedFlags()
x1,y1, x2,y2 = ImGuiDockNode:rect()
```
[To top](#api)
### Tab Bar
```lua
table = ImGuiTabBar:getTabs() -- table of ImGuiTabItem
ImGuiTabItem = ImGuiTabBar:getTab(index) -- get single ImGuiTabItem
number = ImGuiTabBar:getTabCount()
number = ImGuiTabBar:getFlags()
number = ImGuiTabBar:getID()
number = ImGuiTabBar:getSelectedTabId()
number = ImGuiTabBar:getNextSelectedTabId()
number = ImGuiTabBar:getVisibleTabId()
number = ImGuiTabBar:getCurrFrameVisible()
number = ImGuiTabBar:getPrevFrameVisible()
x1,y1, x2,y2 = ImGuiTabBar:getBarRect()
number = ImGuiTabBar:getCurrTabsContentsHeight()
number = ImGuiTabBar:getPrevTabsContentsHeight()
number = ImGuiTabBar:getWidthAllTabs()
number = ImGuiTabBar:getWidthAllTabsIdeal()
number = ImGuiTabBar:getScrollingAnim()
number = ImGuiTabBar:getScrollingTarget()
number = ImGuiTabBar:getScrollingTargetDistToVisibility()
number = ImGuiTabBar:getScrollingSpeed()
number = ImGuiTabBar:getScrollingRectMinX()
number = ImGuiTabBar:getScrollingRectMaxX()
number = ImGuiTabBar:getReorderRequestTabId()
number = ImGuiTabBar:getReorderRequestDir()
number = ImGuiTabBar:getBeginCount()
flag = ImGuiTabBar:wantLayout()
flag = ImGuiTabBar:visibleTabWasSubmitted()
flag = ImGuiTabBar:getTabsAddedNew()
number = ImGuiTabBar:getTabsActiveCount()
number = ImGuiTabBar:getLastTabItemIdx()
number = ImGuiTabBar:getItemSpacingY()
x, y = ImGuiTabBar:getFramePadding()
x, y = ImGuiTabBar:getBackupCursorPos()
string = ImGuiTabBar:getTabsNames()
string = ImGuiTabBar:getTabOrder(ImGuiTabItem) -- try not use it very often
string = ImGuiTabBar:getTabName(ImGuiTabItem) -- try not use it very often
```
[To top](#api)
### Tab Item
```lua
number = ImGuiTabItem:getID()
number = ImGuiTabItem:getFlags()
number = ImGuiTabItem:getLastFrameVisible()
number = ImGuiTabItem:getLastFrameSelected()
number = ImGuiTabItem:getOffset()
number = ImGuiTabItem:getWidth()
number = ImGuiTabItem:getContentWidth()
number = ImGuiTabItem:getNameOffset()
number = ImGuiTabItem:getBeginOrder()
number = ImGuiTabItem:getIndexDuringLayout()
flag = ImGuiTabItem:wantClose()
```
### WindowFlags
```lua
ImGui.WindowFlags_NoDocking
```
### ConfigFlags
```lua
ImGui.ConfigFlags_DockingEnable
```
[To top](#api)
[To top](#api)
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
[To top](#api)
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
[To top](#api)
### DockNodeFlags
```lua
ImGui.DockNodeFlags_None
ImGui.DockNodeFlags_KeepAliveOnly
ImGui.DockNodeFlags_NoDockingInCentralNode
ImGui.DockNodeFlags_PassthruCentralNode
ImGui.DockNodeFlags_NoSplit
ImGui.DockNodeFlags_NoResize
ImGui.DockNodeFlags_AutoHideTabBar
```
[To top](#api)
