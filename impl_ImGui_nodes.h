#ifndef IMPL_IMGUI_NODES_H
#define IMPL_IMGUI_NODES_H

#include "lua.hpp"
#include "luautil.h"
#include "imgui_nodes/imgui_node_editor.h"
#include "gplugin.h"

namespace ImNodes_impl
{
void bind_enums(lua_State* L);
int initNodes(lua_State* L);
int destroyNodes(void* L);

int GetStyle(lua_State* L);
int GetStyleColorName(lua_State* L);
int PushStyleColor(lua_State* L);
int PopStyleColor(lua_State* L);
int PushStyleVar(lua_State* L);
int PopStyleVar(lua_State* L);
int Begin(lua_State* L);
int End(lua_State* L);
int BeginNode(lua_State* L);
int BeginPin(lua_State* L);
int PinRect(lua_State* L);
int PinPivotRect(lua_State* L);
int PinPivotSize(lua_State* L);
int PinPivotScale(lua_State* L);
int PinPivotAlignment(lua_State* L);
int EndPin(lua_State* L);
int Group(lua_State* L);
int EndNode(lua_State* L);
int BeginGroupHint(lua_State* L);
int GetGroupMin(lua_State* L);
int GetGroupMax(lua_State* L);
int GetHintForegroundDrawList(lua_State* L);
int GetHintBackgroundDrawList(lua_State* L);
int GetNodeBackgroundDrawList(lua_State* L);
int EndGroupHint(lua_State* L);
int Link(lua_State* L);
int Flow(lua_State* L);
int BeginCreate(lua_State* L);
int QueryNewLink(lua_State* L);
int QueryNewNode(lua_State* L);
int AcceptNewItem(lua_State* L);
int RejectNewItem(lua_State* L);
int EndCreate(lua_State* L);
int BeginDelete(lua_State* L);
int QueryDeletedLink(lua_State* L);
int QueryDeletedLink2(lua_State* L);
int QueryDeletedNode(lua_State* L);
int AcceptDeletedItem(lua_State* L);
int RejectDeletedItem(lua_State* L);
int EndDelete(lua_State* L);
int SetNodePosition(lua_State* L);
int GetNodePosition(lua_State* L);
int GetNodeSize(lua_State* L);
int CenterNodeOnScreen(lua_State* L);
int RestoreNodeState(lua_State* L);
int Suspend(lua_State* L);
int Resume(lua_State* L);
int IsSuspended(lua_State* L);
int IsActive(lua_State* L);
int HasSelectionChanged(lua_State* L);
int GetSelectedObjectCount(lua_State* L);
int GetSelectedNodes(lua_State* L);
int GetSelectedLinks(lua_State* L);
int ClearSelection(lua_State* L);
int SelectNode(lua_State* L);
int SelectLink(lua_State* L);
int DeselectNode(lua_State* L);
int DeselectLink(lua_State* L);
int DeleteNode(lua_State* L);
int DeleteLink(lua_State* L);
int NavigateToContent(lua_State* L);
int NavigateToSelection(lua_State* L);
int ShowNodeContextMenu(lua_State* L);
int ShowPinContextMenu(lua_State* L);
int ShowLinkContextMenu(lua_State* L);
int ShowBackgroundContextMenu(lua_State* L);
int EnableShortcuts(lua_State* L);
int AreShortcutsEnabled(lua_State* L);
int BeginShortcut(lua_State* L);
int AcceptCut(lua_State* L);
int AcceptCopy(lua_State* L);
int AcceptPaste(lua_State* L);
int AcceptDuplicate(lua_State* L);
int AcceptCreateNode(lua_State* L);
int GetActionContextSize(lua_State* L);
int GetActionContextNodes(lua_State* L);
int GetActionContextLinks(lua_State* L);
int EndShortcut(lua_State* L);
int GetCurrentZoom(lua_State* L);
int GetDoubleClickedNode(lua_State* L);
int GetDoubleClickedPin(lua_State* L);
int GetDoubleClickedLink(lua_State* L);
int IsBackgroundClicked(lua_State* L);
int IsBackgroundDoubleClicked(lua_State* L);
int PinHadAnyLinks(lua_State* L);
int GetScreenSize(lua_State* L);
int ScreenToCanvas(lua_State* L);
int CanvasToScreen(lua_State* L);

int ImNodesStyleSetNodePadding(lua_State* L);
int ImNodesStyleGetNodePadding(lua_State* L);
int ImNodesStyleSetNodeRounding(lua_State* L);
int ImNodesStyleGetNodeRounding(lua_State* L);
int ImNodesStyleSetNodeBorderWidth(lua_State* L);
int ImNodesStyleGetNodeBorderWidth(lua_State* L);
int ImNodesStyleSetHoveredNodeBorderWidth(lua_State* L);
int ImNodesStyleGetHoveredNodeBorderWidth(lua_State* L);
int ImNodesStyleSetSelectedNodeBorderWidth(lua_State* L);
int ImNodesStyleGetSelectedNodeBorderWidth(lua_State* L);
int ImNodesStyleSetPinRounding(lua_State* L);
int ImNodesStyleGetPinRounding(lua_State* L);
int ImNodesStyleSetPinBorderWidth(lua_State* L);
int ImNodesStyleGetPinBorderWidth(lua_State* L);
int ImNodesStyleSetLinkStrength(lua_State* L);
int ImNodesStyleGetLinkStrength(lua_State* L);
int ImNodesStyleSetSourceDirection(lua_State* L);
int ImNodesStyleGetSourceDirection(lua_State* L);
int ImNodesStyleSetTargetDirection(lua_State* L);
int ImNodesStyleGetTargetDirection(lua_State* L);
int ImNodesStyleSetScrollDuration(lua_State* L);
int ImNodesStyleGetScrollDuration(lua_State* L);
int ImNodesStyleSetFlowMarkerDistance(lua_State* L);
int ImNodesStyleGetFlowMarkerDistance(lua_State* L);
int ImNodesStyleSetFlowSpeed(lua_State* L);
int ImNodesStyleGetFlowSpeed(lua_State* L);
int ImNodesStyleSetFlowDuration(lua_State* L);
int ImNodesStyleGetFlowDuration(lua_State* L);
int ImNodesStyleSetPivotAlignment(lua_State* L);
int ImNodesStyleGetPivotAlignment(lua_State* L);
int ImNodesStyleSetPivotSize(lua_State* L);
int ImNodesStyleGetPivotSize(lua_State* L);
int ImNodesStyleSetPivotScale(lua_State* L);
int ImNodesStyleGetPivotScale(lua_State* L);
int ImNodesStyleSetPinCorners(lua_State* L);
int ImNodesStyleGetPinCorners(lua_State* L);
int ImNodesStyleSetPinRadius(lua_State* L);
int ImNodesStyleGetPinRadius(lua_State* L);
int ImNodesStyleSetPinArrowSize(lua_State* L);
int ImNodesStyleGetPinArrowSize(lua_State* L);
int ImNodesStyleSetPinArrowWidth(lua_State* L);
int ImNodesStyleGetPinArrowWidth(lua_State* L);
int ImNodesStyleSetGroupRounding(lua_State* L);
int ImNodesStyleGetGroupRounding(lua_State* L);
int ImNodesStyleSetGroupBorderWidth(lua_State* L);
int ImNodesStyleGetGroupBorderWidth(lua_State* L);
int ImNodesStyleSetColor(lua_State* L);
int ImNodesStyleGetColor(lua_State* L);

int nodes_loader(lua_State* L);
}

#endif // IMPL_IMGUI_NODES_H
