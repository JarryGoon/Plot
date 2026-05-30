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

#include "PlotImpl/PlotInternal.hpp"

#include <limits>
#include <implot.h>
#include <implot_internal.h>

#include "TypeFunc.hpp"
#include "DataTooltip/DataTooltip.hpp"

#define HOVERING_DIST 10

namespace PlotImpl {

void draw_line(const std::string &                    window_name   ,
               const std::map<std::string, LineData> &line_datas    , std::vector<TooltipData> *vec_tooltip_data,
               bool                                   is_cursor_data, ToolTipType               plot_type)
{
    ImPlotPoint mouse;
    ImVec2      mouse_pos;
    ImVec2      cand_tooltip_point;
    ImVec2      curr_tooltip_point = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};

    ImVec2 mouse_pos_pix;
    ImVec2 cand_tooltip_point_pix;

    std::string tooltip_line_id;

    TooltipData curr_tooltip_data;

    bool add_cursor_data_flag;

    const char* line_name;
    LineData    line_data;
    ImPlotItem* item;

    // ============================================================================================================== //
    // 1. Get Mouse Position
    // ============================================================================================================== //

    mouse         = ImPlot::GetPlotMousePos();
    mouse_pos.x   = static_cast<float>(mouse.x);
    mouse_pos.y   = static_cast<float>(mouse.y);
    mouse_pos_pix = ImPlot::PlotToPixels(mouse.x, mouse.y);

    // ============================================================================================================== //
    // 2. Draw Plot Line
    // ============================================================================================================== //

    for(std::pair<std::string, LineData> line : line_datas)
    {
        // Get line name
        line_name = line.first.c_str();
        line_data = line.second;

        // Draw plot line
        ImPlot::PlotLine(line_name, line_data.x.data(), line_data.y.data(), line_data.x.size());

        // Check disable line
        item = ImPlot::GetItem(line_name);
        if(item != nullptr && !item->Show) continue;

        // Find closest point on lines
        cand_tooltip_point = find_closest_point(mouse_pos, line_data.x, line_data.y);
        if (vec_dist(cand_tooltip_point, mouse_pos) < vec_dist(curr_tooltip_point, mouse_pos))
        {
            curr_tooltip_point = cand_tooltip_point;
            tooltip_line_id  = line_name;
        }
    }

    // ============================================================================================================== //
    // 3. Draw Data Tooltip
    // ============================================================================================================== //

    plot2d_tooltip_data(vec_tooltip_data, window_name, plot_type);

    cand_tooltip_point_pix = ImPlot::PlotToPixels(curr_tooltip_point);

    add_cursor_data_flag = is_cursor_data             &&
                           ImPlot::IsPlotHovered()    &&
                           ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    if(vec_dist(mouse_pos_pix, cand_tooltip_point_pix) < HOVERING_DIST)
    {
        curr_tooltip_data.tooltip_data = curr_tooltip_point;
        curr_tooltip_data.line_id      = tooltip_line_id;
        curr_tooltip_data.tooltip_type = plot_type;

        if (add_cursor_data_flag)
            vec_tooltip_data->push_back(curr_tooltip_data);

        draw_tooltip(curr_tooltip_data, "HOVER", "HOVER", false, plot_type);
    }
}

// void draw_pzmap(const std::string &window_name, PlotWindow &plot_window)
// {
//     ImPlotSpec  spec;
//     ImPlotPoint mouse;
//
//     ImVec2      mouse_pos;
//     ImVec2      cand_tooltip_point;
//     ImVec2      curr_tooltip_point = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
//     std::string tooltip_line_id;
//
//     double dx, dy;
//     double dist_sq;
//     double min_dist_sq = std::numeric_limits<double>::max();
//
//     ImVec2      mouse_pos_pix;
//     ImVec2      data_pos_pix;
//     ImVec2      curr_tooltip_point_pix;
//
//     ToolTipType tooltip_type;
//     ToolTipType cand_tooltip_type;
//     ToolTipType curr_tooltip_type;
//
//     TooltipData curr_tooltip_data;
//
//     bool add_cursor_data_flag;
//
//     const char* line_name;
//     ImPlotItem* item;
//
//     // ============================================================================================================== //
//     // 1. Get Mouse Position
//     // ============================================================================================================== //
//
//     mouse         = ImPlot::GetPlotMousePos();
//     mouse_pos.x   = static_cast<float>(mouse.x);
//     mouse_pos.y   = static_cast<float>(mouse.y);
//     mouse_pos_pix = ImPlot::PlotToPixels(mouse.x, mouse.y);
//
//     // ============================================================================================================== //
//     // 2. Draw Pole & Zero
//     // ============================================================================================================== //
//
//     // Set marker fill alpha
//     spec.FillAlpha = 0.f;
//
//     for(auto &line : plot_window.lines)
//     {
//         line_name = line.name.c_str();
//
//         for(int i = 0; i < line.x.size(); i++)
//         {
//             data_pos_pix = ImPlot::PlotToPixels(line.x[i], line.y[i]);
//
//             dx = mouse_pos_pix.x - data_pos_pix.x;
//             dy = mouse_pos_pix.y - data_pos_pix.y;
//
//             dist_sq = dx * dx + dy * dy;
//
//             // Pole
//             if(line.z[i] == 0.0)
//             {
//                 spec.Marker  = ImPlotMarker_Cross;
//                 tooltip_type = POLE;
//
//                 ImPlot::PlotScatter(line_name, &line.x[i], &line.y[i], 1, spec);
//             }
//             // Zero
//             else if(line.z[i] == 1.0)
//             {
//                 spec.Marker  = ImPlotMarker_Circle;
//                 tooltip_type = ZERO;
//
//                 ImPlot::PlotScatter(line_name, &line.x[i], &line.y[i], 1, spec);
//             }
//
//             // Update tooltip candidate data
//             if(dist_sq < min_dist_sq)
//             {
//                 min_dist_sq = dist_sq;
//                 tooltip_line_id = line_name;
//                 cand_tooltip_point.x = line.x[i];
//                 cand_tooltip_point.y = line.y[i];
//                 cand_tooltip_type = tooltip_type;
//             }
//         }
//
//         item = ImPlot::GetItem(line_name);
//         if(item != nullptr && !item->Show) continue;
//
//         if (vec_dist(cand_tooltip_point, mouse_pos) < vec_dist(curr_tooltip_point, mouse_pos))
//         {
//             curr_tooltip_point = cand_tooltip_point;
//             tooltip_line_id    = line_name;
//             curr_tooltip_type  = cand_tooltip_type;
//         }
//     }
//
//     // ============================================================================================================== //
//     // 3. Draw Data Tooltip
//     // ============================================================================================================== //
//
//     plot2d_tooltip_data(plot_window.vec_tooltip_data, window_name, PZ);
//
//     add_cursor_data_flag = plot_window.is_cursor_data &&
//                            ImPlot::IsPlotHovered()    &&
//                            ImGui::IsMouseClicked(ImGuiMouseButton_Left);
//
//     curr_tooltip_point_pix = ImPlot::PlotToPixels(curr_tooltip_point);
//
//     if(vec_dist(mouse_pos_pix, curr_tooltip_point_pix) < HOVERING_DIST)
//     {
//         curr_tooltip_data.tooltip_data = curr_tooltip_point;
//         curr_tooltip_data.line_id      = tooltip_line_id;
//         curr_tooltip_data.tooltip_type = curr_tooltip_type;
//
//         draw_tooltip(curr_tooltip_data, "HOVER", window_name, false, curr_tooltip_type);
//
//         if (add_cursor_data_flag)
//             plot_window.vec_tooltip_data.push_back(curr_tooltip_data);
//     }
// }

}
