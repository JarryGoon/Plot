/*
* Copyright 2026 JarryGoon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "DataTooltip/DataTooltip.hpp"

#include <map>
#include <limits>

#include <implot.h>
#include <implot_internal.h>

#include "DataTooltip/TooltipFunc.hpp"

#define TOOLTIP_GAP 16.0f

ImVec2 find_closest_point(ImVec2 mouse, const std::vector<double> &x_data, const std::vector<double> &y_data)
{
    ImVec2 pixel_pos;
    ImVec2 mouse_pixel_pos;

    int    best_idx;
    double dist_sq;
    double min_dist_sq;

    double dx, dy;

    best_idx    = 0;
    min_dist_sq = std::numeric_limits<double>::max();

    mouse_pixel_pos = ImPlot::PlotToPixels(mouse.x, mouse.y);

    for(size_t i = 0; i < x_data.size(); i++)
    {
        pixel_pos = ImPlot::PlotToPixels(x_data[i], y_data[i]);

        dx = mouse_pixel_pos.x - pixel_pos.x;
        dy = mouse_pixel_pos.y - pixel_pos.y;

        dist_sq = dx * dx + dy * dy;

        if(dist_sq < min_dist_sq)
        {
            min_dist_sq = dist_sq;
            best_idx = i;
        }
    }

    return {static_cast<float>(x_data[best_idx]), static_cast<float>(y_data[best_idx])};
}

CursorAction draw_tooltip(const ImVec2 &cursor, const std::string &id, const std::string &window_name,
                          const char* print_format, bool is_fixed)
{
    static std::map<std::string, TooltipPos> tooltip_pos_map;

    CursorAction action = None;

    char   text_buf[TOOLTIP_TEXT_BUFFER_SIZE];
    ImVec2 text_size;
    ImVec2 tooltip_pix_pos;
    ImVec2 tooltip_win_size;

    std::string      tooltip_win_id;
    ImGuiWindowFlags tooltip_win_flags;

    TooltipPos update_pos;

    ImVec2 plot_pos;
    ImVec2 plot_size;

    // ============================================================================================================== //
    //                                                1. Draw Datapoint                                               //
    // ============================================================================================================== //

    draw_datapoint(cursor, id, is_fixed);

    // ============================================================================================================== //
    //                                           2. Initialize Tooltip Window                                         //
    // ============================================================================================================== //

    // Create window ID
    tooltip_win_id = "2DDataTooltip_" + window_name + "_" + id;

    snprintf(text_buf, TOOLTIP_TEXT_BUFFER_SIZE, print_format, cursor.x, cursor.y);

    text_size        = ImGui::CalcTextSize(text_buf);
    tooltip_win_size = ImVec2(text_size.x + TOOLTIP_GAP, text_size.y + TOOLTIP_GAP); // 약간의 여백(Padding) 추가
    tooltip_pix_pos  = ImPlot::PlotToPixels(cursor.x, cursor.y);

    // Check Tooltip Position
    // If it doesn't exist, set the default position
    if(id == "HOVER" || tooltip_pos_map.find(tooltip_win_id) == tooltip_pos_map.end())
    {
        plot_pos  = ImPlot::GetPlotPos();
        plot_size = ImPlot::GetPlotSize();

        if(tooltip_pix_pos.x + tooltip_win_size.x > plot_pos.x + plot_size.x)
        {
            if(tooltip_pix_pos.y - tooltip_win_size.y < plot_pos.y)
                tooltip_pos_map[tooltip_win_id] = BottomLeft;
            else
                tooltip_pos_map[tooltip_win_id] = TopLeft;
        }
        else
        {
            if(tooltip_pix_pos.y - tooltip_win_size.y < plot_pos.y)
                tooltip_pos_map[tooltip_win_id] = BottomRight;
            else
                tooltip_pos_map[tooltip_win_id] = TopRight;
        }
    }

    init_tooltip_window(tooltip_pix_pos, tooltip_win_size, is_fixed, tooltip_pos_map[tooltip_win_id]);

    tooltip_win_flags = ImGuiWindowFlags_NoScrollbar |
                        ImGuiWindowFlags_NoScrollWithMouse;

    // If not fixed state, ignore clicks (Inputs) to make it ghost-like
    if (!is_fixed) tooltip_win_flags |= ImGuiWindowFlags_NoInputs;

    // ============================================================================================================== //
    //                                            3. Generate Tooltip Window                                          //
    // ============================================================================================================== //

    if (ImGui::BeginChild(tooltip_win_id.c_str(), tooltip_win_size, true, tooltip_win_flags))
    {
        // Display tooltip text
        ImGui::TextUnformatted(text_buf);

        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::InvisibleButton("##DragBox", tooltip_win_size);

        // Tooltip drag event
        update_pos = drag_tooltip(tooltip_win_id, tooltip_pix_pos);
        if(update_pos != NoUpdate)
            tooltip_pos_map[tooltip_win_id] = update_pos;

        // Delete tooltip top-up
        if (ImGui::BeginPopupContextItem("Popup", ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem("Delete")) action = DeleteCurrent;
            if (ImGui::MenuItem("Clear All")) action = ClearAll;
            ImGui::EndPopup();
        }

    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    return action;
}

void plot_tooltip_data(std::vector<TooltipData> &cursor_data, const std::string &window_name,
                       const char* print_format, ToolTipType print_type)
{
    ImPlotItem* item;

    CursorAction action;

    int cursor_to_delete = -1;
    bool clear_all_requested = false;

    for(int i = 0; i < cursor_data.size(); i++)
    {
        item = ImPlot::GetItem(cursor_data[i].line_id.c_str());
        if(item != nullptr && !item->Show) continue;

        if(print_type != DEFAULT && print_type != cursor_data[i].tooltip_type)
            continue;

        ImGui::PushID(i);

        // 커서를 화면에 그리고, 반환된 액션 처리 (배열의 요소들은 고정이므로 is_fixed = true)
        action = draw_tooltip(cursor_data[i].tooltip_data, std::to_string(i), window_name, print_format, true);

        if (action == DeleteCurrent) cursor_to_delete = i;
        else if (action == ClearAll) clear_all_requested = true;

        ImGui::PopID();
    }

    if (clear_all_requested)
        cursor_data.clear();
    else if (cursor_to_delete != -1)
        cursor_data.erase(cursor_data.begin() + cursor_to_delete);
}
