#include "DataTooltip/TooltipFunc.hpp"

#include <map>

#define TOOLTIP_OFFSET 5.0f

void draw_datapoint(const ImVec2 &cursor, const std::string &id, bool is_fixed)
{
    ImPlotSpec spec_scatter;
    ImPlotSpec spec_line;

    spec_scatter.Marker          = ImPlotMarker_Circle;
    spec_scatter.MarkerSize      = 2;
    spec_scatter.MarkerFillColor = ImVec4(0.7, 0.7, 0.7, 1);
    spec_scatter.LineColor       = ImVec4(0.7, 0.7, 0.7, 1);
    ImPlot::PlotScatter(("##SnapPoint_" + id).c_str(), &cursor.x, &cursor.y, 1, spec_scatter);

    if(!is_fixed)
    {
        spec_line.LineColor = ImVec4(0.8, 0.8, 0.8, 1);
        ImPlot::PlotInfLines(("##VLine_" + id).c_str(), &cursor.x, 1, spec_line);

        spec_line.Flags = ImPlotInfLinesFlags_Horizontal;
        ImPlot::PlotInfLines(("##HLine_" + id).c_str(), &cursor.y, 1, spec_line);
    }
}

void init_tooltip_window(const ImVec2 &tooltip_pos, ImVec2 tooltip_win_size, bool is_fixed, TooltipPos tooltip_pos_type)
{
    float  offset_x, offset_y;
    ImVec2 corner_pos;

    switch(tooltip_pos_type)
    {
        default:
        case BottomRight:
            offset_x   = TOOLTIP_OFFSET;
            offset_y   = TOOLTIP_OFFSET;
            corner_pos = ImVec2(tooltip_pos.x + offset_x,
                                tooltip_pos.y + offset_y);
            break;
        case TopRight:
            offset_x   = TOOLTIP_OFFSET;
            offset_y   = -TOOLTIP_OFFSET - tooltip_win_size.y;
            corner_pos = ImVec2(tooltip_pos.x + offset_x,
                                tooltip_pos.y + offset_y + tooltip_win_size.y);
            break;
        case TopLeft:
            offset_x   = -TOOLTIP_OFFSET - tooltip_win_size.x;
            offset_y   = -TOOLTIP_OFFSET - tooltip_win_size.y;
            corner_pos = ImVec2(tooltip_pos.x + offset_x + tooltip_win_size.x,
                                tooltip_pos.y + offset_y + tooltip_win_size.y);
            break;
        case BottomLeft:
            offset_x   = -TOOLTIP_OFFSET - tooltip_win_size.x;
            offset_y   = TOOLTIP_OFFSET;
            corner_pos = ImVec2(tooltip_pos.x + offset_x + tooltip_win_size.x,
                                tooltip_pos.y + offset_y);
            break;
    }

    ImGui::SetCursorScreenPos(ImVec2(tooltip_pos.x + offset_x, tooltip_pos.y + offset_y));

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.3f, 0.9f));

    if(is_fixed) ImPlot::GetPlotDrawList()->AddLine(tooltip_pos, corner_pos, IM_COL32(200, 200, 200, 255), 1.5);
}

TooltipPos drag_tooltip(const std::string &tooltip_win_id, const ImVec2 &tooltip_pix_pos)
{
    static std::string active_tooltip_id;

    if(!ImGui::IsMouseDown(ImGuiMouseButton_Left)) active_tooltip_id = "";
    if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) active_tooltip_id = tooltip_win_id;

    if(active_tooltip_id == tooltip_win_id)
    {
        ImVec2 mouse_pos = ImGui::GetMousePos();

        if(mouse_pos.x >= tooltip_pix_pos.x && mouse_pos.y >= tooltip_pix_pos.y)
            return BottomRight;
        if(mouse_pos.x <= tooltip_pix_pos.x && mouse_pos.y <= tooltip_pix_pos.y)
            return TopLeft;
        if(mouse_pos.x <= tooltip_pix_pos.x && mouse_pos.y >= tooltip_pix_pos.y)
            return BottomLeft;

        return TopRight;
    }

    return NoUpdate;
}
