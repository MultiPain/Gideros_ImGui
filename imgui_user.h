#include "imgui_src_docking/imgui.h"

enum ImGuiInputTextFlags_Extended
{
    ImGuiInputTextFlags_NoBackground = 1 << 19
};

namespace ImGui
{
    IMGUI_API bool ImageButtonWithText(ImTextureID texId,const char* label,const ImVec2& imageSize=ImVec2(0,0), const ImVec2& uv0 = ImVec2(0,0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));

    IMGUI_API bool FilledSliderScalar(const char* label, bool mirror, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
    IMGUI_API bool FilledSliderScalarN(const char* label, bool mirror, ImGuiDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
    IMGUI_API bool FilledSliderFloat(const char* label, bool mirror, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
    IMGUI_API bool FilledSliderFloat2(const char* label, bool mirror, float v[2], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool FilledSliderFloat3(const char* label, bool mirror, float v[3], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool FilledSliderFloat4(const char* label, bool mirror, float v[4], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool FilledSliderAngle(const char* label, bool mirror, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg", ImGuiSliderFlags flags = 0);
    IMGUI_API bool FilledSliderInt(const char* label, bool mirror, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool FilledSliderInt2(const char* label, bool mirror, int v[2], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool FilledSliderInt3(const char* label, bool mirror, int v[3], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool FilledSliderInt4(const char* label, bool mirror, int v[4], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool VFilledSliderFloat(const char* label, bool mirror, const ImVec2& size, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
    IMGUI_API bool VFilledSliderInt(const char* label, bool mirror, const ImVec2& size, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
    IMGUI_API bool VFilledSliderScalar(const char* label, bool mirror, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);

    IMGUI_API void ImageFilled(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& bg_col = ImVec4(1,1,1,0), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));

    IMGUI_API void Internal_DockBuilderDockWindow(const char* window_name, ImGuiID node_id);
    IMGUI_API ImGuiDockNode* Internal_DockBuilderGetNode(ImGuiID node_id);
    IMGUI_API void Internal_DockBuilderSetNodePos(ImGuiID node_id, ImVec2 pos);
    IMGUI_API void Internal_DockBuilderSetNodeSize(ImGuiID node_id, ImVec2 size);
    IMGUI_API ImGuiID Internal_DockBuilderAddNode(ImGuiID node_id = 0, ImGuiDockNodeFlags flags = 0);
    IMGUI_API void Internal_DockBuilderRemoveNode(ImGuiID node_id);
    IMGUI_API void Internal_DockBuilderRemoveNodeChildNodes(ImGuiID node_id);
    IMGUI_API void Internal_DockBuilderRemoveNodeDockedWindows(ImGuiID node_id, bool clear_settings_refs = true);
    IMGUI_API ImGuiID Internal_DockBuilderSplitNode(ImGuiID id, ImGuiDir split_dir, float size_ratio_for_node_at_dir, ImGuiID* out_id_at_dir, ImGuiID* out_id_at_opposite_dir);
    IMGUI_API void Internal_DockBuilderCopyNode(ImGuiID src_node_id, ImGuiID dst_node_id, ImVector<ImGuiID>* out_node_remap_pairs);
    IMGUI_API void Internal_DockBuilderCopyWindowSettings(const char* src_name, const char* dst_name);
    IMGUI_API void Internal_DockBuilderCopyDockSpace(ImGuiID src_dockspace_id, ImGuiID dst_dockspace_id, ImVector<const char*>* in_window_remap_pairs);
    IMGUI_API void Internal_DockBuilderFinish(ImGuiID node_id);
}
