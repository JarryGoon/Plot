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

void draw_pzmap(const std::string                     &window_name   ,
                const std::map<std::string, LineData> &line_datas    , std::vector<TooltipData> *vec_tooltip_data,
                bool                                   is_cursor_data)
{
    ImPlotSpec  spec;
    ImPlotPoint mouse;

    ImVec2      mouse_pos;

    ImVec2      mouse_pos_pix;
    ImVec2      curr_tooltip_point_pix;

    ToolTipType tooltip_type;

    double cand_data_pole_dist;
    double cand_data_zero_dist;
    double cand_data_dist;
    double curr_data_dist = std::numeric_limits<double>::max();

    TooltipData cand_tooltip_data_pole;
    TooltipData cand_tooltip_data_zero;
    TooltipData cand_tooltip_data;
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
    // 2. Draw Pole & Zero
    // ============================================================================================================== //

    // Set marker fill alpha
    spec.FillAlpha = 0.f;

    for(std::pair<std::string, LineData> line : line_datas)
    {
        line_name = line.first.c_str();
        line_data = line.second;

        // Pole
        spec.Marker  = ImPlotMarker_Cross;
        tooltip_type = POLE;

        cand_tooltip_data_pole = pz_processing(line_data.x, line_name, spec, mouse_pos_pix, tooltip_type);
        cand_data_pole_dist    = vec_dist(cand_tooltip_data_pole.tooltip_data, mouse_pos);

        // Zero
        spec.Marker  = ImPlotMarker_Circle;
        tooltip_type = ZERO;

        cand_tooltip_data_zero = pz_processing(line_data.y, line_name, spec, mouse_pos_pix, tooltip_type);
        cand_data_zero_dist    = vec_dist(cand_tooltip_data_zero.tooltip_data, mouse_pos);

        // Check disable line
        item = ImPlot::GetItem(line_name);
        if(item != nullptr && !item->Show) continue;

        // Find closest point on lines
        if(cand_data_pole_dist < cand_data_zero_dist)
        {
            cand_tooltip_data = cand_tooltip_data_pole;
            cand_data_dist    = cand_data_pole_dist;
        }
        else
        {
            cand_tooltip_data = cand_tooltip_data_zero;
            cand_data_dist    = cand_data_zero_dist;
        }

        if (cand_data_dist < curr_data_dist)
        {
            curr_data_dist    = cand_data_dist;
            curr_tooltip_data = cand_tooltip_data;
        }
    }

    // ============================================================================================================== //
    // 3. Draw Data Tooltip
    // ============================================================================================================== //

    plot2d_tooltip_data(vec_tooltip_data, window_name, PZ);

    add_cursor_data_flag = is_cursor_data          &&
                           ImPlot::IsPlotHovered() &&
                           ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    curr_tooltip_point_pix = ImPlot::PlotToPixels(curr_tooltip_data.tooltip_data);

    if(vec_dist(mouse_pos_pix, curr_tooltip_point_pix) < HOVERING_DIST)
    {

        draw_tooltip(curr_tooltip_data, "HOVER", window_name, false, PZ);

        if (add_cursor_data_flag)
            vec_tooltip_data->push_back(curr_tooltip_data);
    }
}

TooltipData pz_processing(const std::vector<double> &data, const char* line_name, const ImPlotSpec &spec,
                          const ImVec2 &mouse_pos_pix, ToolTipType tooltip_type)
{
    TooltipData cand_tooltip_data;

    double real, imag;

    ImVec2 data_pos_pix;

    double dx, dy;
    double dist_sq;
    double min_dist_sq = std::numeric_limits<double>::max();

    for(int i = 0; i < data.size(); i += 2)
    {
        real = data[i];
        imag = data[i + 1];

        data_pos_pix = ImPlot::PlotToPixels(real, imag);

        dx = mouse_pos_pix.x - data_pos_pix.x;
        dy = mouse_pos_pix.y - data_pos_pix.y;

        dist_sq = dx * dx + dy * dy;

        ImPlot::PlotScatter(line_name, &real, &imag, 1, spec);

        // Update tooltip candidate data
        if(dist_sq < min_dist_sq)
        {
            min_dist_sq = dist_sq;
            cand_tooltip_data.line_id        = line_name;
            cand_tooltip_data.tooltip_data.x = real;
            cand_tooltip_data.tooltip_data.y = imag;
            cand_tooltip_data.tooltip_type   = tooltip_type;
        }
    }

    return cand_tooltip_data;
}

}
