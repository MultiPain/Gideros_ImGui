#include "impl_ImGui_nodes.h"

#define BIND_IENUM(L, value, name) lua_pushinteger(L, value); lua_setfield(L, -2, name);
#define BIND_FENUM(L, value, name) lua_pushnumber(L, value); lua_setfield(L, -2, name);

using namespace ax;

static char keyWeak = ' ';

void ImNodes_impl::bind_enums(lua_State* L)
{
	lua_getglobal(L, "ImNodes");

	BIND_IENUM(L, (int)NodeEditor::PinKind::Input, "PinKind_Input");
	BIND_IENUM(L, (int)NodeEditor::PinKind::Output, "PinKind_Output");

	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_Bg, "StyleColor_Bg");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_Grid, "StyleColor_Grid");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_NodeBg, "StyleColor_NodeBg");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_NodeBorder, "StyleColor_NodeBorder");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_HovNodeBorder, "StyleColor_HovNodeBorder");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_SelNodeBorder, "StyleColor_SelNodeBorder");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_NodeSelRect, "StyleColor_NodeSelRect");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_NodeSelRectBorder, "StyleColor_NodeSelRectBorder");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_HovLinkBorder, "StyleColor_HovLinkBorder");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_SelLinkBorder, "StyleColor_SelLinkBorder");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_LinkSelRect, "StyleColor_LinkSelRect");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_LinkSelRectBorder, "StyleColor_LinkSelRectBorder");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_PinRect, "StyleColor_PinRect");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_PinRectBorder, "StyleColor_PinRectBorder");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_Flow, "StyleColor_Flow");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_FlowMarker, "StyleColor_FlowMarker");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_GroupBg, "StyleColor_GroupBg");
	BIND_IENUM(L, NodeEditor::StyleColor::StyleColor_GroupBorder, "StyleColor_GroupBorder");

	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_NodePadding, "StyleVar_NodePadding");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_NodeRounding, "StyleVar_NodeRounding");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_NodeBorderWidth, "StyleVar_NodeBorderWidth");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_HoveredNodeBorderWidth, "StyleVar_HoveredNodeBorderWidth");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_SelectedNodeBorderWidth, "StyleVar_SelectedNodeBorderWidth");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_PinRounding, "StyleVar_PinRounding");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_PinBorderWidth, "StyleVar_PinBorderWidth");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_LinkStrength, "StyleVar_LinkStrength");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_SourceDirection, "StyleVar_SourceDirection");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_TargetDirection, "StyleVar_TargetDirection");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_ScrollDuration, "StyleVar_ScrollDuration");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_FlowMarkerDistance, "StyleVar_FlowMarkerDistance");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_FlowSpeed, "StyleVar_FlowSpeed");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_FlowDuration, "StyleVar_FlowDuration");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_PivotAlignment, "StyleVar_PivotAlignment");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_PivotSize, "StyleVar_PivotSize");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_PivotScale, "StyleVar_PivotScale");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_PinCorners, "StyleVar_PinCorners");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_PinRadius, "StyleVar_PinRadius");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_PinArrowSize, "StyleVar_PinArrowSize");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_PinArrowWidth, "StyleVar_PinArrowWidth");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_GroupRounding, "StyleVar_GroupRounding");
	BIND_IENUM(L, NodeEditor::StyleVar::StyleVar_GroupBorderWidth, "StyleVar_GroupBorderWidth");


	lua_pop(L, 1);
}

int ImNodes_impl::initNodes(lua_State* L)
{

	NodeEditor::EditorContext* ctx = NodeEditor::CreateEditor();
	g_pushInstance(L, "ImNode", ctx);

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, ctx);
	lua_pop(L, 1);

	return 1;
}

int ImNodes_impl::destroyNodes(void* L)
{
	void* data = GIDEROS_DTOR_UDATA(L);
	NodeEditor::EditorContext* ptr = static_cast<NodeEditor::EditorContext*>(data);
	NodeEditor::DestroyEditor(ptr);

	return 0;
}

int ImNodes_impl::GetStyle(lua_State* L)
{
	NodeEditor::Style& style = NodeEditor::GetStyle();
	g_pushInstance(L, "ImNodeStyle", &style);

	return 1;
}

int ImNodes_impl::GetStyleColorName(lua_State* L)
{
	NodeEditor::StyleColor colorIndex = (NodeEditor::StyleColor)luaL_checkinteger(L, 2);
	const char* name = NodeEditor::GetStyleColorName(colorIndex);

	lua_pushstring(L, name);
	return 1;
}

int ImNodes_impl::PushStyleColor(lua_State* L)
{
	NodeEditor::StyleColor colorIndex = (NodeEditor::StyleColor)luaL_checkinteger(L, 2);
	const ImVec4 color;
	NodeEditor::PushStyleColor(colorIndex, color);
	return 0;
}

int ImNodes_impl::PopStyleColor(lua_State* L)
{

	return 0;
}

int ImNodes_impl::PushStyleVar(lua_State* L)
{

	return 0;
}

int ImNodes_impl::PopStyleVar(lua_State* L)
{

	return 0;
}

int ImNodes_impl::Begin(lua_State* L)
{

	return 0;
}

int ImNodes_impl::End(lua_State* L)
{

	return 0;
}

int ImNodes_impl::BeginNode(lua_State* L)
{

	return 0;
}

int ImNodes_impl::BeginPin(lua_State* L)
{

	return 0;
}

int ImNodes_impl::PinRect(lua_State* L)
{

	return 0;
}

int ImNodes_impl::PinPivotRect(lua_State* L)
{

	return 0;
}

int ImNodes_impl::PinPivotSize(lua_State* L)
{

	return 0;
}

int ImNodes_impl::PinPivotScale(lua_State* L)
{

	return 0;
}

int ImNodes_impl::PinPivotAlignment(lua_State* L)
{

	return 0;
}

int ImNodes_impl::EndPin(lua_State* L)
{

	return 0;
}

int ImNodes_impl::Group(lua_State* L)
{

	return 0;
}

int ImNodes_impl::EndNode(lua_State* L)
{

	return 0;
}

int ImNodes_impl::BeginGroupHint(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetGroupMin(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetGroupMax(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetHintForegroundDrawList(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetHintBackgroundDrawList(lua_State* L)
{

	return 0;
}

int ImNodes_impl::EndGroupHint(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetNodeBackgroundDrawList(lua_State* L)
{

	return 0;
}

int ImNodes_impl::Link(lua_State* L)
{

	return 0;
}

int ImNodes_impl::Flow(lua_State* L)
{

	return 0;
}

int ImNodes_impl::BeginCreate(lua_State* L)
{

	return 0;
}

int ImNodes_impl::QueryNewLink(lua_State* L)
{

	return 0;
}

int ImNodes_impl::QueryNewNode(lua_State* L)
{

	return 0;
}

int ImNodes_impl::AcceptNewItem(lua_State* L)
{

	return 0;
}

int ImNodes_impl::RejectNewItem(lua_State* L)
{

	return 0;
}

int ImNodes_impl::EndCreate(lua_State* L)
{

	return 0;
}

int ImNodes_impl::BeginDelete(lua_State* L)
{

	return 0;
}

int ImNodes_impl::QueryDeletedLink(lua_State* L)
{

	return 0;
}

int ImNodes_impl::QueryDeletedNode(lua_State* L)
{

	return 0;
}

int ImNodes_impl::AcceptDeletedItem(lua_State* L)
{

	return 0;
}

int ImNodes_impl::RejectDeletedItem(lua_State* L)
{

	return 0;
}

int ImNodes_impl::EndDelete(lua_State* L)
{

	return 0;
}

int ImNodes_impl::SetNodePosition(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetNodePosition(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetNodeSize(lua_State* L)
{

	return 0;
}

int ImNodes_impl::CenterNodeOnScreen(lua_State* L)
{

	return 0;
}

int ImNodes_impl::RestoreNodeState(lua_State* L)
{

	return 0;
}

int ImNodes_impl::Suspend(lua_State* L)
{

	return 0;
}

int ImNodes_impl::Resume(lua_State* L)
{

	return 0;
}

int ImNodes_impl::IsSuspended(lua_State* L)
{

	return 0;
}

int ImNodes_impl::IsActive(lua_State* L)
{

	return 0;
}

int ImNodes_impl::HasSelectionChanged(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetSelectedObjectCount(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetSelectedNodes(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetSelectedLinks(lua_State* L)
{

	return 0;
}

int ImNodes_impl::ClearSelection(lua_State* L)
{

	return 0;
}

int ImNodes_impl::SelectNode(lua_State* L)
{

	return 0;
}

int ImNodes_impl::SelectLink(lua_State* L)
{

	return 0;
}

int ImNodes_impl::DeselectNode(lua_State* L)
{

	return 0;
}

int ImNodes_impl::DeselectLink(lua_State* L)
{

	return 0;
}

int ImNodes_impl::DeleteNode(lua_State* L)
{

	return 0;
}

int ImNodes_impl::DeleteLink(lua_State* L)
{

	return 0;
}

int ImNodes_impl::NavigateToContent(lua_State* L)
{

	return 0;
}

int ImNodes_impl::NavigateToSelection(lua_State* L)
{

	return 0;
}

int ImNodes_impl::ShowNodeContextMenu(lua_State* L)
{

	return 0;
}

int ImNodes_impl::ShowPinContextMenu(lua_State* L)
{

	return 0;
}

int ImNodes_impl::ShowLinkContextMenu(lua_State* L)
{

	return 0;
}

int ImNodes_impl::ShowBackgroundContextMenu(lua_State* L)
{

	return 0;
}

int ImNodes_impl::EnableShortcuts(lua_State* L)
{

	return 0;
}

int ImNodes_impl::AreShortcutsEnabled(lua_State* L)
{

	return 0;
}

int ImNodes_impl::BeginShortcut(lua_State* L)
{

	return 0;
}

int ImNodes_impl::AcceptCut(lua_State* L)
{

	return 0;
}

int ImNodes_impl::AcceptCopy(lua_State* L)
{

	return 0;
}

int ImNodes_impl::AcceptPaste(lua_State* L)
{

	return 0;
}

int ImNodes_impl::AcceptDuplicate(lua_State* L)
{

	return 0;
}

int ImNodes_impl::AcceptCreateNode(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetActionContextSize(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetActionContextNodes(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetActionContextLinks(lua_State* L)
{

	return 0;
}

int ImNodes_impl::EndShortcut(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetCurrentZoom(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetDoubleClickedNode(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetDoubleClickedPin(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetDoubleClickedLink(lua_State* L)
{

	return 0;
}

int ImNodes_impl::IsBackgroundClicked(lua_State* L)
{

	return 0;
}

int ImNodes_impl::IsBackgroundDoubleClicked(lua_State* L)
{

	return 0;
}

int ImNodes_impl::PinHadAnyLinks(lua_State* L)
{

	return 0;
}

int ImNodes_impl::GetScreenSize(lua_State* L)
{

	return 0;
}

int ImNodes_impl::ScreenToCanvas(lua_State* L)
{

	return 0;
}

int ImNodes_impl::CanvasToScreen(lua_State* L)
{

	return 0;
}

int ImNodes_impl::nodes_loader(lua_State* L)
{

	const luaL_Reg nodesStyleFunctionsList[] = {
		{NULL, NULL}
	};

	g_createClass(L, "ImNodeStyle", NULL, NULL, NULL, nodesStyleFunctionsList);

	const luaL_Reg nodesFunctionsList[] = {
		{"getStyleColorName", GetStyleColorName},

		{"pushStyleColor", PushStyleColor},
		{"popStyleColor", PopStyleColor},
		{"pushStyleVar", PushStyleVar},
		{"pushStyleVar", PushStyleVar},
		{"pushStyleVar", PushStyleVar},
		{"popStyleVar", PopStyleVar},
		{"begin", Begin},
		{"end", End},
		{"beginNode", BeginNode},
		{"beginPin", BeginPin},
		{"pinRect", PinRect},
		{"pinPivotRect", PinPivotRect},
		{"pinPivotSize", PinPivotSize},
		{"pinPivotScale", PinPivotScale},
		{"pinPivotAlignment", PinPivotAlignment},
		{"endPin", EndPin},
		{"group", Group},
		{"endNode", EndNode},
		{"beginGroupHint", BeginGroupHint},
		{"getGroupMin", GetGroupMin},
		{"getGroupMax", GetGroupMax},
		{"getHintForegroundDrawList", GetHintForegroundDrawList},
		{"getHintBackgroundDrawList", GetHintBackgroundDrawList},
		{"endGroupHint", EndGroupHint},


		// TODO: Add a way to manage node background channels
		{"getNodeBackgroundDrawList", GetNodeBackgroundDrawList},
		{"link", Link},
		{"flow", Flow},
		{"beginCreate", BeginCreate},
		{"queryNewLink", QueryNewLink},
		{"queryNewLink", QueryNewLink},
		{"queryNewNode", QueryNewNode},
		{"queryNewNode", QueryNewNode},
		{"acceptNewItem", AcceptNewItem},
		{"acceptNewItem", AcceptNewItem},
		{"rejectNewItem", RejectNewItem},
		{"rejectNewItem", RejectNewItem},
		{"endCreate", EndCreate},
		{"beginDelete", BeginDelete},
		{"queryDeletedLink", QueryDeletedLink},
		{"queryDeletedNode", QueryDeletedNode},
		{"acceptDeletedItem", AcceptDeletedItem},
		{"rejectDeletedItem", RejectDeletedItem},
		{"endDelete", EndDelete},
		{"setNodePosition", SetNodePosition},
		{"getNodePosition", GetNodePosition},
		{"getNodeSize", GetNodeSize},
		{"centerNodeOnScreen", CenterNodeOnScreen},
		{"restoreNodeState", RestoreNodeState},
		{"suspend", Suspend},
		{"resume", Resume},
		{"isSuspended", IsSuspended},
		{"isActive", IsActive},
		{"hasSelectionChanged", HasSelectionChanged},
		{"getSelectedObjectCount", GetSelectedObjectCount},
		{"getSelectedNodes", GetSelectedNodes},
		{"getSelectedLinks", GetSelectedLinks},
		{"clearSelection", ClearSelection},
		{"selectNode", SelectNode},
		{"selectLink", SelectLink},
		{"deselectNode", DeselectNode},
		{"deselectLink", DeselectLink},
		{"deleteNode", DeleteNode},
		{"deleteLink", DeleteLink},
		{"navigateToContent", NavigateToContent},
		{"navigateToSelection", NavigateToSelection},
		{"showNodeContextMenu", ShowNodeContextMenu},
		{"showPinContextMenu", ShowPinContextMenu},
		{"showLinkContextMenu", ShowLinkContextMenu},
		{"showBackgroundContextMenu", ShowBackgroundContextMenu},
		{"enableShortcuts", EnableShortcuts},
		{"areShortcutsEnabled", AreShortcutsEnabled},
		{"beginShortcut", BeginShortcut},
		{"acceptCut", AcceptCut},
		{"acceptCopy", AcceptCopy},
		{"acceptPaste", AcceptPaste},
		{"acceptDuplicate", AcceptDuplicate},
		{"acceptCreateNode", AcceptCreateNode},
		{"getActionContextSize", GetActionContextSize},
		{"getActionContextNodes", GetActionContextNodes},
		{"getActionContextLinks", GetActionContextLinks},
		{"endShortcut", EndShortcut},
		{"getCurrentZoom", GetCurrentZoom},
		{"getDoubleClickedNode", GetDoubleClickedNode},
		{"getDoubleClickedPin", GetDoubleClickedPin},
		{"getDoubleClickedLink", GetDoubleClickedLink},
		{"isBackgroundClicked", IsBackgroundClicked},
		{"isBackgroundDoubleClicked", IsBackgroundDoubleClicked},
		{"pinHadAnyLinks", PinHadAnyLinks},
		{"getScreenSize", GetScreenSize},
		{"screenToCanvas", ScreenToCanvas},
		{"canvasToScreen", CanvasToScreen},


		{NULL, NULL}
	};

	g_createClass(L, "ImNode", NULL, initNodes, destroyNodes, nodesFunctionsList);

	return 0;
}
