#include "impl_ImGui_nodes.h"
#include "GColor.h"
#include "Helpers.h"

#define BIND_IENUM(L, value, name) lua_pushinteger(L, value); lua_setfield(L, -2, name);
#define BIND_FENUM(L, value, name) lua_pushnumber(L, value); lua_setfield(L, -2, name);

using namespace ax;

static char keyWeak = ' ';

void ImNodes_impl::bind_enums(lua_State* L)
{
	lua_getglobal(L, "ImNodeEditor");

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
	NodeEditor::SetCurrentEditor(ctx);
	g_pushInstance(L, "ImNodeEditor", ctx);

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
	int hex = luaL_checkinteger(L, 3);
	float alpha = luaL_optnumber(L, 4, 1.0f);

	const ImVec4 color = GColor::toVec4(hex, alpha);
	NodeEditor::PushStyleColor(colorIndex, color);
	return 1;
}

int ImNodes_impl::PopStyleColor(lua_State* L)
{
	int count = luaL_optinteger(L, 2, 1);
	NodeEditor::PopStyleColor(count);
	return 0;
}

// TODO: color arg?
int ImNodes_impl::PushStyleVar(lua_State* L)
{
	NodeEditor::StyleVar index = (NodeEditor::StyleVar)luaL_checkinteger(L, 2);
	if (lua_gettop(L) == 3)
	{
		float val = luaL_checknumber(L, 3);
		NodeEditor::PushStyleVar(index, val);
	}
	else if (lua_gettop(L) == 4)
	{
		const ImVec2 val = luaL_checkvec2(L, 3);
		NodeEditor::PushStyleVar(index, val);
	}
	else // mb color?
	{
		ImVec4 val = luaL_checkvec4(L, 3);
		NodeEditor::PushStyleVar(index, val);
	}
	return 0;
}

int ImNodes_impl::PopStyleVar(lua_State* L)
{
	int count = luaL_optinteger(L, 2, 1);
	NodeEditor::PopStyleVar(count);
	return 0;
}

int ImNodes_impl::Begin(lua_State* L)
{

	NodeEditor::EditorContext* ctx = getPtr<NodeEditor::EditorContext>(L, "ImNodeEditor");
	NodeEditor::SetCurrentEditor(ctx);

	const char* id = luaL_checkstring(L, 2);
	const ImVec2 size = luaL_optvec2(L, 3);

	NodeEditor::Begin(id, size);
	return 0;
}

int ImNodes_impl::End(lua_State* L)
{
	NodeEditor::End();
	return 0;
}

int ImNodes_impl::BeginNode(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	NodeEditor::BeginNode(id);
	return 0;
}

int ImNodes_impl::BeginPin(lua_State* L)
{
	NodeEditor::PinId id = luaL_checkinteger(L, 2);
	NodeEditor::PinKind kind = (NodeEditor::PinKind)luaL_checkinteger(L, 3);
	NodeEditor::BeginPin(id, kind);
	return 0;
}

int ImNodes_impl::PinRect(lua_State* L)
{
	const ImVec2 a = luaL_checkvec2(L, 2);
	const ImVec2 b = luaL_checkvec2(L, 4);

	NodeEditor::PinRect(a, b);
	return 0;
}

int ImNodes_impl::PinPivotRect(lua_State* L)
{
	const ImVec2 a = luaL_checkvec2(L, 2);
	const ImVec2 b = luaL_checkvec2(L, 4);
	NodeEditor::PinPivotRect(a, b);
	return 0;
}

int ImNodes_impl::PinPivotSize(lua_State* L)
{
	const ImVec2 size = luaL_checkvec2(L, 2);
	NodeEditor::PinPivotSize(size);
	return 0;
}

int ImNodes_impl::PinPivotScale(lua_State* L)
{
	const ImVec2 scale = luaL_checkvec2(L, 2);
	NodeEditor::PinPivotScale(scale);
	return 0;
}

int ImNodes_impl::PinPivotAlignment(lua_State* L)
{
	const ImVec2 align = luaL_checkvec2(L, 2);
	NodeEditor::PinPivotAlignment(align);
	return 0;
}

int ImNodes_impl::EndPin(lua_State* L)
{
	NodeEditor::EndPin();
	return 0;
}

int ImNodes_impl::Group(lua_State* L)
{
	const ImVec2 size = luaL_checkvec2(L, 2);
	NodeEditor::Group(size);
	return 0;
}

int ImNodes_impl::EndNode(lua_State* L)
{
	NodeEditor::EndNode();
	return 0;
}

int ImNodes_impl::BeginGroupHint(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, NodeEditor::BeginGroupHint(id));
	return 1;
}

int ImNodes_impl::GetGroupMin(lua_State* L)
{
	ImVec2 min = NodeEditor::GetGroupMin();
	lua_pushvec2(L, min);
	return 2;
}

int ImNodes_impl::GetGroupMax(lua_State* L)
{
	ImVec2 max = NodeEditor::GetGroupMax();
	lua_pushvec2(L, max);
	return 2;
}

int ImNodes_impl::GetHintForegroundDrawList(lua_State* L)
{
	ImDrawList* list = NodeEditor::GetHintForegroundDrawList();
	g_pushInstance(L, "ImDrawList", list);
	return 1;
}

int ImNodes_impl::GetHintBackgroundDrawList(lua_State* L)
{
	ImDrawList* list = NodeEditor::GetHintBackgroundDrawList();
	g_pushInstance(L, "ImDrawList", list);
	return 1;
}

int ImNodes_impl::GetNodeBackgroundDrawList(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	ImDrawList* list = NodeEditor::GetNodeBackgroundDrawList(id);
	g_pushInstance(L, "ImDrawList", list);
	return 1;
}

int ImNodes_impl::EndGroupHint(lua_State* L)
{
	NodeEditor::EndGroupHint();
	return 0;
}

int ImNodes_impl::Link(lua_State* L)
{
	NodeEditor::LinkId id = luaL_checkinteger(L, 2);
	NodeEditor::PinId startId = luaL_checkinteger(L, 3);
	NodeEditor::PinId endId = luaL_checkinteger(L, 4);
	const ImVec4 color = GColor::toVec4opt(L, 5);
	float thickness = luaL_optnumber(L, 7, 1.0f);

	lua_pushboolean(L, NodeEditor::Link(id, startId, endId, color, thickness));
	return 1;
}

int ImNodes_impl::Flow(lua_State* L)
{
	NodeEditor::LinkId id = luaL_checkinteger(L, 2);
	NodeEditor::Flow(id);
	return 0;
}

int ImNodes_impl::BeginCreate(lua_State* L)
{
	const ImVec4 color = GColor::toVec4opt(L, 2);
	float thickness = luaL_optnumber(L, 4, 1.0f);
	lua_pushboolean(L, NodeEditor::BeginCreate(color, thickness));
	return 1;
}

int ImNodes_impl::QueryNewLink(lua_State* L)
{
	NodeEditor::PinId startId = 0;
	NodeEditor::PinId endId = 0;

	if (lua_gettop(L) > 2)
	{
		const ImVec4 color = GColor::toVec4(L, 2);
		float thickness = luaL_optnumber(L, 4, 1.0f);
		lua_pushboolean(L, NodeEditor::QueryNewLink(&startId, &endId, color, thickness));
		lua_pushinteger(L, startId.Get());
		lua_pushinteger(L, endId.Get());
	}
	else
	{
		lua_pushboolean(L, QueryNewLink(&startId, &endId));
		lua_pushinteger(L, startId.Get());
		lua_pushinteger(L, endId.Get());
	}
	return 3;
}

int ImNodes_impl::QueryNewNode(lua_State* L)
{
	NodeEditor::PinId id = 0;

	if (lua_gettop(L) > 2)
	{
		const ImVec4 color = GColor::toVec4(L, 3);
		float thickness = luaL_optnumber(L, 5, 1.0f);
		lua_pushboolean(L, NodeEditor::QueryNewNode(&id, color, thickness));
		lua_pushinteger(L, id.Get());
	}
	else
	{
		lua_pushboolean(L, NodeEditor::QueryNewNode(&id));
		lua_pushinteger(L, id.Get());
	}
	return 2;
}

int ImNodes_impl::AcceptNewItem(lua_State* L)
{
	if (lua_gettop(L) > 1)
	{
		const ImVec4 color = GColor::toVec4(L, 2);
		float thickness = luaL_optnumber(L, 4, 1.0f);
		lua_pushboolean(L, NodeEditor::AcceptNewItem(color, thickness));
	}
	else
	{
		lua_pushboolean(L, NodeEditor::AcceptNewItem());
	}
	return 1;
}

int ImNodes_impl::RejectNewItem(lua_State* L)
{
	if (lua_gettop(L) > 1)
	{
		const ImVec4 color = GColor::toVec4(L, 2);
		float thickness = luaL_optnumber(L, 4, 1.0f);
		NodeEditor::RejectNewItem(color, thickness);
	}
	else
	{
		NodeEditor::RejectNewItem();
	}
	return 0;
}

int ImNodes_impl::EndCreate(lua_State* L)
{
	NodeEditor::EndCreate();
	return 0;
}

int ImNodes_impl::BeginDelete(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::BeginDelete());
	return 1;
}

int ImNodes_impl::QueryDeletedLink(lua_State* L)
{
	NodeEditor::LinkId id = 0;

	lua_pushboolean(L, NodeEditor::QueryDeletedLink(&id));
	lua_pushinteger(L, id.Get());
	return 2;
}

int ImNodes_impl::QueryDeletedLink2(lua_State* L)
{
	NodeEditor::LinkId id = 0;
	NodeEditor::PinId startId = 0;
	NodeEditor::PinId endId = 0;

	lua_pushboolean(L, NodeEditor::QueryDeletedLink(&id, &startId, &endId));
	lua_pushinteger(L, id.Get());
	lua_pushinteger(L, startId.Get());
	lua_pushinteger(L, endId.Get());
	return 4;
}

int ImNodes_impl::QueryDeletedNode(lua_State* L)
{
	NodeEditor::NodeId id = 0;
	lua_pushboolean(L, NodeEditor::QueryDeletedNode(&id));
	lua_pushinteger(L, id.Get());
	return 2;
}

int ImNodes_impl::AcceptDeletedItem(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::AcceptDeletedItem());
	return 1;
}

int ImNodes_impl::RejectDeletedItem(lua_State* L)
{
	NodeEditor::RejectDeletedItem();
	return 0;
}

int ImNodes_impl::EndDelete(lua_State* L)
{
	NodeEditor::EndDelete();
	return 0;
}

int ImNodes_impl::SetNodePosition(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	const ImVec2 pos = luaL_checkvec2(L, 3);
	NodeEditor::SetNodePosition(id, pos);
	return 0;
}

int ImNodes_impl::GetNodePosition(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	ImVec2 pos = NodeEditor::GetNodePosition(id);
	lua_pushvec2(L, pos);
	return 2;
}

int ImNodes_impl::GetNodeSize(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	ImVec2 size = NodeEditor::GetNodeSize(id);
	lua_pushvec2(L, size);
	return 2;
}

int ImNodes_impl::CenterNodeOnScreen(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	NodeEditor::CenterNodeOnScreen(id);
	return 0;
}

int ImNodes_impl::RestoreNodeState(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	NodeEditor::RestoreNodeState(id);
	return 0;
}

int ImNodes_impl::Suspend(lua_State* L)
{
	NodeEditor::Suspend();
	return 0;
}

int ImNodes_impl::Resume(lua_State* L)
{
	NodeEditor::Resume();
	return 0;
}

int ImNodes_impl::IsSuspended(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::IsSuspended());
	return 1;
}

int ImNodes_impl::IsActive(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::IsActive());
	return 1;
}

int ImNodes_impl::HasSelectionChanged(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::HasSelectionChanged());
	return 1;
}

int ImNodes_impl::GetSelectedObjectCount(lua_State* L)
{
	lua_pushinteger(L, NodeEditor::GetSelectedObjectCount());
	return 1;
}

int ImNodes_impl::GetSelectedNodes(lua_State* L)
{
	int size = luaL_checkinteger(L, 2);
	NodeEditor::NodeId* list = new NodeEditor::NodeId[size];
	int count = NodeEditor::GetSelectedNodes(list, size);

	lua_createtable(L, count, 0);

	for (int i = 0; i < count; i++)
	{
		lua_pushnumber(L, list[i].Get());
		lua_rawseti(L, -2, i + 1);
	}

	delete[] list;
	return 1;
}

int ImNodes_impl::GetSelectedLinks(lua_State* L)
{
	int size = luaL_checkinteger(L, 2);
	NodeEditor::LinkId* list = new NodeEditor::LinkId[size];
	int count = NodeEditor::GetSelectedLinks(list, size);

	lua_createtable(L, count, 0);

	for (int i = 0; i < count; i++)
	{
		lua_pushnumber(L, list[i].Get());
		lua_rawseti(L, -2, i + 1);
	}

	delete[] list;
	return 1;
}

int ImNodes_impl::ClearSelection(lua_State* L)
{
	NodeEditor::ClearSelection();
	return 0;
}

int ImNodes_impl::SelectNode(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	bool append = luaL_optboolean(L, 3, false);
	SelectNode(id, append);
	return 0;
}

int ImNodes_impl::SelectLink(lua_State* L)
{
	NodeEditor::LinkId id = luaL_checkinteger(L, 2);
	bool append = luaL_optboolean(L, 3, false);
	SelectLink(id, append);
	return 0;
}

int ImNodes_impl::DeselectNode(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	NodeEditor::DeselectNode(id);
	return 0;
}

int ImNodes_impl::DeselectLink(lua_State* L)
{
	NodeEditor::LinkId id = luaL_checkinteger(L, 2);
	NodeEditor::DeselectLink(id);
	return 0;
}

int ImNodes_impl::DeleteNode(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, NodeEditor::DeleteNode(id));
	return 1;
}

int ImNodes_impl::DeleteLink(lua_State* L)
{
	NodeEditor::LinkId id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, DeleteLink(id));
	return 1;
}

int ImNodes_impl::NavigateToContent(lua_State* L)
{
	float duration = luaL_optnumber(L, 2, -1.0f);
	NodeEditor::NavigateToContent(duration);
	return 0;
}

int ImNodes_impl::NavigateToSelection(lua_State* L)
{
	bool zoomIn = luaL_optboolean(L, 2, false);
	float duration = luaL_optnumber(L, 3, -1.0f);
	NodeEditor::NavigateToSelection(zoomIn, duration);
	return 0;
}

int ImNodes_impl::ShowNodeContextMenu(lua_State* L)
{
	NodeEditor::NodeId id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, NodeEditor::ShowNodeContextMenu(&id));
	lua_pushinteger(L, id.Get());
	return 2;
}

int ImNodes_impl::ShowPinContextMenu(lua_State* L)
{
	NodeEditor::PinId id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, NodeEditor::ShowPinContextMenu(&id));
	lua_pushinteger(L, id.Get());
	return 2;
}

int ImNodes_impl::ShowLinkContextMenu(lua_State* L)
{
	NodeEditor::LinkId id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, NodeEditor::ShowLinkContextMenu(&id));
	lua_pushinteger(L, id.Get());
	return 2;
}

int ImNodes_impl::ShowBackgroundContextMenu(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::ShowBackgroundContextMenu());
	return 1;
}

int ImNodes_impl::EnableShortcuts(lua_State* L)
{
	bool enable = lua_toboolean(L, 2);
	NodeEditor::EnableShortcuts(enable);
	return 0;
}

int ImNodes_impl::AreShortcutsEnabled(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::AreShortcutsEnabled());
	return 1;
}

int ImNodes_impl::BeginShortcut(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::BeginShortcut());
	return 1;
}

int ImNodes_impl::AcceptCut(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::AcceptCut());
	return 1;
}

int ImNodes_impl::AcceptCopy(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::AcceptCopy());
	return 1;
}

int ImNodes_impl::AcceptPaste(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::AcceptPaste());
	return 1;
}

int ImNodes_impl::AcceptDuplicate(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::AcceptDuplicate());
	return 1;
}

int ImNodes_impl::AcceptCreateNode(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::AcceptCreateNode());
	return 1;
}

int ImNodes_impl::GetActionContextSize(lua_State* L)
{
	lua_pushinteger(L, NodeEditor::GetActionContextSize());
	return 1;
}

int ImNodes_impl::GetActionContextNodes(lua_State* L)
{
	int size = luaL_checkinteger(L, 2);
	NodeEditor::NodeId* list = new NodeEditor::NodeId[size];
	int count = NodeEditor::GetActionContextNodes(list, size);

	lua_createtable(L, count, 0);

	for (int i = 0; i < count; i++)
	{
		lua_pushnumber(L, list[i].Get());
		lua_rawseti(L, -2, i + 1);
	}

	delete[] list;
	return 1;
}

int ImNodes_impl::GetActionContextLinks(lua_State* L)
{
	int size = luaL_checkinteger(L, 2);
	NodeEditor::LinkId* list = new NodeEditor::LinkId[size];
	int count = NodeEditor::GetActionContextLinks(list, size);

	lua_createtable(L, count, 0);

	for (int i = 0; i < count; i++)
	{
		lua_pushnumber(L, list[i].Get());
		lua_rawseti(L, -2, i + 1);
	}

	delete[] list;
	return 1;
}

int ImNodes_impl::EndShortcut(lua_State* L)
{
	NodeEditor::EndShortcut();
	return 0;
}

int ImNodes_impl::GetCurrentZoom(lua_State* L)
{
	lua_pushnumber(L, NodeEditor::GetCurrentZoom());
	return 2;
}

int ImNodes_impl::GetDoubleClickedNode(lua_State* L)
{
	NodeEditor::NodeId id = NodeEditor::GetDoubleClickedNode();
	lua_pushinteger(L, id.Get());
	return 1;
}

int ImNodes_impl::GetDoubleClickedPin(lua_State* L)
{
	NodeEditor::PinId id = NodeEditor::GetDoubleClickedPin();
	lua_pushinteger(L, id.Get());
	return 1;
}

int ImNodes_impl::GetDoubleClickedLink(lua_State* L)
{
	NodeEditor::LinkId id = NodeEditor::GetDoubleClickedLink();
	lua_pushinteger(L, id.Get());
	return 1;
}

int ImNodes_impl::IsBackgroundClicked(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::IsBackgroundClicked());
	return 1;
}

int ImNodes_impl::IsBackgroundDoubleClicked(lua_State* L)
{
	lua_pushboolean(L, NodeEditor::IsBackgroundDoubleClicked());
	return 1;
}

int ImNodes_impl::PinHadAnyLinks(lua_State* L)
{
	NodeEditor::PinId id = luaL_checkinteger(L, 2);
	lua_pushboolean(L, NodeEditor::PinHadAnyLinks(id));
	return 1;
}

int ImNodes_impl::GetScreenSize(lua_State* L)
{
	ImVec2 size = NodeEditor::GetScreenSize();
	lua_pushvec2(L, size);
	return 2;
}

int ImNodes_impl::ScreenToCanvas(lua_State* L)
{
	ImVec2 pos1 = luaL_checkvec2(L, 2);
	ImVec2 pos2 = NodeEditor::ScreenToCanvas(pos1);
	lua_pushvec2(L, pos2);
	return 2;
}

int ImNodes_impl::CanvasToScreen(lua_State* L)
{
	ImVec2 pos1 = luaL_checkvec2(L, 2);
	ImVec2 pos2 = NodeEditor::CanvasToScreen(pos1);
	lua_pushvec2(L, pos2);
	return 2;
}

//////////////////////////////////////////////////////////////////////////////////////

NodeEditor::Style& getStyle(lua_State* L, int idx = 1)
{
	return *getPtr<NodeEditor::Style>(L, "ImNodeStyle", idx);
}

int ImNodes_impl::ImNodesStyleSetNodePadding(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.NodePadding = luaL_checkvec4(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetNodePadding(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	ImVec4 value = style.NodePadding;
	lua_pushvec4(L, value);
	return 4;
}

int ImNodes_impl::ImNodesStyleSetNodeRounding(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.NodeRounding = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetNodeRounding(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.NodeRounding;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetNodeBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.NodeBorderWidth = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetNodeBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.NodeBorderWidth;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetHoveredNodeBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.HoveredNodeBorderWidth = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetHoveredNodeBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.HoveredNodeBorderWidth;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetSelectedNodeBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.SelectedNodeBorderWidth = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetSelectedNodeBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.SelectedNodeBorderWidth;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetPinRounding(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.PinRounding = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetPinRounding(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.PinRounding;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetPinBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.PinBorderWidth = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetPinBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.PinBorderWidth;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetLinkStrength(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.LinkStrength = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetLinkStrength(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.LinkStrength;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetSourceDirection(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.SourceDirection = luaL_checkvec2(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetSourceDirection(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	ImVec2 value = style.SourceDirection;
	lua_pushvec2(L, value);
	return 2;
}

int ImNodes_impl::ImNodesStyleSetTargetDirection(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.TargetDirection = luaL_checkvec2(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetTargetDirection(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	ImVec2 value = style.TargetDirection;
	lua_pushvec2(L, value);
	return 2;
}

int ImNodes_impl::ImNodesStyleSetScrollDuration(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.ScrollDuration = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetScrollDuration(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.ScrollDuration;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetFlowMarkerDistance(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.FlowMarkerDistance = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetFlowMarkerDistance(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.FlowMarkerDistance;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetFlowSpeed(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.FlowSpeed = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetFlowSpeed(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.FlowSpeed;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetFlowDuration(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.FlowDuration = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetFlowDuration(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.FlowDuration;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetPivotAlignment(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.PivotAlignment = luaL_checkvec2(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetPivotAlignment(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	ImVec2 value = style.PivotAlignment;
	lua_pushvec2(L, value);
	return 2;
}

int ImNodes_impl::ImNodesStyleSetPivotSize(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.PivotSize = luaL_checkvec2(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetPivotSize(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	ImVec2 value = style.PivotSize;
	lua_pushvec2(L, value);
	return 2;
}

int ImNodes_impl::ImNodesStyleSetPivotScale(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.PivotScale = luaL_checkvec2(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetPivotScale(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	ImVec2 value = style.PivotScale;
	lua_pushvec2(L, value);
	return 2;
}

int ImNodes_impl::ImNodesStyleSetPinCorners(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.PinCorners = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetPinCorners(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	int value = style.PinCorners;
	lua_pushinteger(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetPinRadius(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.PinRadius = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetPinRadius(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.PinRadius;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetPinArrowSize(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.PinArrowSize = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetPinArrowSize(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.PinArrowSize;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetPinArrowWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.PinArrowWidth = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetPinArrowWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.PinArrowWidth;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetGroupRounding(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.GroupRounding = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetGroupRounding(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.GroupRounding;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetGroupBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	style.GroupBorderWidth = luaL_checknumber(L, 2);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetGroupBorderWidth(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	float value = style.GroupBorderWidth;
	lua_pushnumber(L, value);
	return 1;
}

int ImNodes_impl::ImNodesStyleSetColor(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	int idx = luaL_checkinteger(L, 2);
	style.Colors[idx] = GColor::toVec4(L, 3);
	return 0;
}

int ImNodes_impl::ImNodesStyleGetColor(lua_State* L)
{
	NodeEditor::Style& style = getStyle(L);
	int idx = luaL_checkinteger(L, 2);
	GColor color = GColor::toHex(style.Colors[idx]);
	lua_pushinteger(L, color.hex);
	lua_pushnumber(L, color.alpha);
	return 2;
}

//////////////////////////////////////////////////////////////////////////////////////

void ShowStyleEditor(bool* show = nullptr)
{
	if (!ImGui::Begin("Style", show))
	{
		ImGui::End();
		return;
	}

	auto paneWidth = ImGui::GetContentRegionAvail();

	auto& editorStyle = NodeEditor::GetStyle();
	ImGui::BeginHorizontal("Style buttons", ImVec2(paneWidth.x, 0.0f), 1.0f);
	ImGui::TextUnformatted("Values");
	ImGui::Spring();
	if (ImGui::Button("Reset to defaults"))
		editorStyle = NodeEditor::Style();
	ImGui::EndHorizontal();
	ImGui::Spacing();
	ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
	ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
	ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
	ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
	ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
	ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
	ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
	ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
	//ImVec2  SourceDirection;
	//ImVec2  TargetDirection;
	ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
	ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
	ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
	ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
	//ImVec2  PivotAlignment;
	//ImVec2  PivotSize;
	//ImVec2  PivotScale;
	//float   PinCorners;
	//float   PinRadius;
	//float   PinArrowSize;
	//float   PinArrowWidth;
	ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
	ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

	ImGui::Separator();

	static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_DisplayRGB;
	ImGui::BeginHorizontal("Color Mode", ImVec2(paneWidth.x, 0.0f), 1.0f);
	ImGui::TextUnformatted("Filter Colors");
	ImGui::Spring();
	ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_DisplayRGB);
	ImGui::Spring(0);
	ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_DisplayHSV);
	ImGui::Spring(0);
	ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_DisplayHex);
	ImGui::EndHorizontal();

	static ImGuiTextFilter filter;
	filter.Draw("", paneWidth.x);

	ImGui::Spacing();

	ImGui::PushItemWidth(-160);
	for (int i = 0; i < NodeEditor::StyleColor_Count; ++i)
	{
		auto name = NodeEditor::GetStyleColorName((NodeEditor::StyleColor)i);
		if (!filter.PassFilter(name))
			continue;

		ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
	}
	ImGui::PopItemWidth();

	ImGui::End();
}

int LuaShowStyleEdior(lua_State* L)
{

	if (lua_isnoneornil(L, 2))
	{
		ImGui::ShowStyleEditor();
		return 0;
	}
	bool p_open = lua_toboolean(L, 2);
	ShowStyleEditor(&p_open);
	lua_pushboolean(L, p_open);
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////

int GetCanvasScale(lua_State* L)
{
	ax::NodeEditor::Detail::EditorContext* ctx = getPtr<ax::NodeEditor::Detail::EditorContext>(L, "ImNodeEditor");
	auto view = ctx->GetView();
	lua_pushnumber(L, view.Scale);
	return 1;
}

int ImNodes_impl::nodes_loader(lua_State* L)
{

	const luaL_Reg nodesStyleFunctionsList[] = {
		{"setNodePadding", ImNodesStyleSetNodePadding},
		{"getNodePadding", ImNodesStyleGetNodePadding},

		{"setNodeRounding", ImNodesStyleSetNodeRounding},
		{"getNodeRounding", ImNodesStyleGetNodeRounding},

		{"setNodeBorderWidth", ImNodesStyleSetNodeBorderWidth},
		{"getNodeBorderWidth", ImNodesStyleGetNodeBorderWidth},

		{"setHoveredNodeBorderWidth", ImNodesStyleSetHoveredNodeBorderWidth},
		{"getHoveredNodeBorderWidth", ImNodesStyleGetHoveredNodeBorderWidth},

		{"setSelectedNodeBorderWidth", ImNodesStyleSetSelectedNodeBorderWidth},
		{"getSelectedNodeBorderWidth", ImNodesStyleGetSelectedNodeBorderWidth},

		{"setPinRounding", ImNodesStyleSetPinRounding},
		{"getPinRounding", ImNodesStyleGetPinRounding},

		{"setPinBorderWidth", ImNodesStyleSetPinBorderWidth},
		{"getPinBorderWidth", ImNodesStyleGetPinBorderWidth},

		{"setLinkStrength", ImNodesStyleSetLinkStrength},
		{"getLinkStrength", ImNodesStyleGetLinkStrength},

		{"setSourceDirection", ImNodesStyleSetSourceDirection},
		{"getSourceDirection", ImNodesStyleGetSourceDirection},

		{"setTargetDirection", ImNodesStyleSetTargetDirection},
		{"getTargetDirection", ImNodesStyleGetTargetDirection},

		{"setScrollDuration", ImNodesStyleSetScrollDuration},
		{"getScrollDuration", ImNodesStyleGetScrollDuration},

		{"setFlowMarkerDistance", ImNodesStyleSetFlowMarkerDistance},
		{"getFlowMarkerDistance", ImNodesStyleGetFlowMarkerDistance},

		{"setFlowSpeed", ImNodesStyleSetFlowSpeed},
		{"getFlowSpeed", ImNodesStyleGetFlowSpeed},

		{"setFlowDuration", ImNodesStyleSetFlowDuration},
		{"getFlowDuration", ImNodesStyleGetFlowDuration},

		{"setPivotAlignment", ImNodesStyleSetPivotAlignment},
		{"getPivotAlignment", ImNodesStyleGetPivotAlignment},

		{"setPivotSize", ImNodesStyleSetPivotSize},
		{"getPivotSize", ImNodesStyleGetPivotSize},

		{"setPivotScale", ImNodesStyleSetPivotScale},
		{"getPivotScale", ImNodesStyleGetPivotScale},

		{"setPinCorners", ImNodesStyleSetPinCorners},
		{"getPinCorners", ImNodesStyleGetPinCorners},

		{"setPinRadius", ImNodesStyleSetPinRadius},
		{"getPinRadius", ImNodesStyleGetPinRadius},

		{"setPinArrowSize", ImNodesStyleSetPinArrowSize},
		{"getPinArrowSize", ImNodesStyleGetPinArrowSize},

		{"setPinArrowWidth", ImNodesStyleSetPinArrowWidth},
		{"getPinArrowWidth", ImNodesStyleGetPinArrowWidth},

		{"setGroupRounding", ImNodesStyleSetGroupRounding},
		{"getGroupRounding", ImNodesStyleGetGroupRounding},

		{"setGroupBorderWidth", ImNodesStyleSetGroupBorderWidth},
		{"getGroupBorderWidth", ImNodesStyleGetGroupBorderWidth},

		{"setColor", ImNodesStyleSetColor},
		{"getColor", ImNodesStyleGetColor},

		{NULL, NULL}
	};

	g_createClass(L, "ImNodeStyle", NULL, NULL, NULL, nodesStyleFunctionsList);

	const luaL_Reg nodesFunctionsList[] = {
		{"getStyle", GetStyle},
		{"getStyleColorName", GetStyleColorName},
		{"showStyleEdior", LuaShowStyleEdior},

		{"getCanvasScale", GetCanvasScale},

		{"pushStyleColor", PushStyleColor},
		{"popStyleColor", PopStyleColor},
		{"pushStyleVar", PushStyleVar},
		{"pushStyleVar", PushStyleVar},
		{"pushStyleVar", PushStyleVar},
		{"popStyleVar", PopStyleVar},
		{"beginEditor", Begin},
		{"endEditor", End},
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
		{"queryNewNode", QueryNewNode},
		{"queryNewNode", QueryNewNode},
		{"acceptNewItem", AcceptNewItem},
		{"acceptNewItem", AcceptNewItem},
		{"rejectNewItem", RejectNewItem},
		{"rejectNewItem", RejectNewItem},
		{"endCreate", EndCreate},
		{"beginDelete", BeginDelete},
		{"queryDeletedLink", QueryDeletedLink},
		{"queryDeletedLink2", QueryDeletedLink2},
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

	g_createClass(L, "ImNodeEditor", NULL, initNodes, destroyNodes, nodesFunctionsList);

	bind_enums(L);

	return 0;
}
