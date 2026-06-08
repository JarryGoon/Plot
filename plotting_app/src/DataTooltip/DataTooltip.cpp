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

#define TOOLTIP_TEXT_BUFFER_SIZE 128

bool tooltip_button(bool* is_active, const ImVec2& size)
{
    bool    was_clicked = false;
    ImVec4* colors      = ImGui::GetStyle().Colors;

    ImVec4 active_color(0.2f, 0.4f, 0.8f, 1.0f);

    if(*is_active)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, active_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, active_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, active_color);
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_Button]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_ButtonHovered]);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, active_color);
    }

    if (ImGui::Button("Data Tooltip", size))
    {
        *is_active = !*is_active;
        was_clicked = true;
    }

    ImGui::PopStyleColor(3);

    return was_clicked;
}

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

CursorAction draw_tooltip(const TooltipData &tooltip_data, const std::string &id, const std::string &window_name,
                          bool is_fixed,                   ToolTipType print_type)
{
    static std::map<std::string, TooltipPos> tooltip_pos_map;

    CursorAction action = None;

    ImVec2 data;

    ImVec2 text_size;
    ImVec2 tooltip_pix_pos;
    ImVec2 tooltip_win_size;

    std::string      tooltip_win_id;
    ImGuiWindowFlags tooltip_win_flags;

    TooltipPos update_pos;

    ImVec2 plot_pos;
    ImVec2 plot_size;

    double wn;
    double zeta;

    const char *line_name;
    char        text_buf[TOOLTIP_TEXT_BUFFER_SIZE];

    // ============================================================================================================== //
    // 1. Draw Datapoint
    // ============================================================================================================== //

    data      = tooltip_data.tooltip_data;
    line_name = tooltip_data.line_id.c_str();

    draw_datapoint(data, id, is_fixed);

    // ============================================================================================================== //
    // 2. Tooltip Text
    // ============================================================================================================== //

    switch(print_type)
    {
        case PHASE:
            snprintf(text_buf, TOOLTIP_TEXT_BUFFER_SIZE,
                     "%s\nFreq. : %.3f rad/s\nPhase: %.3f deg",
                     line_name, data.x, data.y);
            break;

        case MAGNITUDE:
            snprintf(text_buf, TOOLTIP_TEXT_BUFFER_SIZE,
                     "%s\nFreq.: %.3f rad/s\nMag. : %.3f dB",
                     line_name, data.x, data.y);
            break;

        case PZ:
            wn   = std::sqrt(data.x * data.x + data.y * data.y);
            zeta = -data.x / wn;
            if(tooltip_data.tooltip_type == POLE)
                snprintf(text_buf, TOOLTIP_TEXT_BUFFER_SIZE,
                         "%s(Pole)\nFreq.: %.3f rad/s\nZeta : %.3f",
                         line_name, wn, zeta);
            else
                snprintf(text_buf, TOOLTIP_TEXT_BUFFER_SIZE,
                         "%s(Zero)\nFreq.: %.3f rad/s\nZeta : %.3f",
                         line_name, wn, zeta);
            break;

        default:
            snprintf(text_buf, TOOLTIP_TEXT_BUFFER_SIZE,
                     "%s\nX: %.3f\nY: %.3f",
                     line_name, data.x, data.y);
            break;
    }

    // ============================================================================================================== //
    // 3. Initialize Tooltip Window
    // ============================================================================================================== //

    // Create window ID
    tooltip_win_id = "2DDataTooltip_" + window_name + "_" + id;

    text_size        = ImGui::CalcTextSize(text_buf);
    tooltip_win_size = ImVec2(text_size.x + TOOLTIP_GAP, text_size.y + TOOLTIP_GAP); // 약간의 여백(Padding) 추가
    tooltip_pix_pos  = ImPlot::PlotToPixels(data.x, data.y);

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
    // 4. Generate Tooltip Window
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
            if (ImGui::MenuItem("Delete"))    action = DeleteCurrent;
            if (ImGui::MenuItem("Clear All")) action = ClearAll;
            ImGui::EndPopup();
        }

    }

    ImGui::EndChild();
    ImGui::PopStyleColor(2);

    return action;
}

void plot2d_tooltip_data(std::vector<TooltipData> *vec_tooltip_data, const std::string &window_name,
                         ToolTipType print_type)
{
    ImPlotItem* item;

    CursorAction action;

    ToolTipType tooltip_type;

    int cursor_to_delete = -1;
    bool clear_all_requested = false;

    for(int i = 0; i < vec_tooltip_data->size(); i++)
    {
        item = ImPlot::GetItem((*vec_tooltip_data)[i].line_id.c_str());
        if(item != nullptr && !item->Show) continue;

        tooltip_type = (*vec_tooltip_data)[i].tooltip_type;

        if(print_type != tooltip_type && print_type != PZ)
            continue;

        ImGui::PushID(i);

        // 커서를 화면에 그리고, 반환된 액션 처리 (배열의 요소들은 고정이므로 is_fixed = true)
        action = draw_tooltip((*vec_tooltip_data)[i], std::to_string(i),
                              window_name, true, print_type);

        if (action == DeleteCurrent) cursor_to_delete = i;
        else if (action == ClearAll) clear_all_requested = true;

        ImGui::PopID();
    }

    if (clear_all_requested)
        vec_tooltip_data->clear();
    else if (cursor_to_delete != -1)
        vec_tooltip_data->erase(vec_tooltip_data->begin() + cursor_to_delete);
}
