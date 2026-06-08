/*
 * Copyright 2026 JarryGoon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law tillage, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "PlotFigure.hpp"

#include <algorithm>

#include <implot.h>
#include <implot_internal.h>

#include "PlotImpl/GridFunc.hpp"
#include "DataTooltip/DataTooltip.hpp"
#include "PlotImpl/PlotInternal.hpp"

PlotFigure::PlotFigure(PlotType type)
{
    _plot_type      = type;
    _is_cursor_data = false;
}

LineData* PlotFigure::add_line_data(const std::string& line_name)
{
    std::string name;

    uint32_t line_num = 1;

    name = line_name;

    if(name.empty())
    {
        while(true)
        {
            name = "Data " + std::to_string(line_num);

            if(_line_data.find(name) == _line_data.end())
                break;

            line_num++;
        }
    }

    _line_data.try_emplace(name, LineData());

    return &_line_data.at(name);
}

LineData* PlotFigure::get_line_data(const std::string& line_name)
{
    auto iter = _line_data.find(line_name);

    if (iter == _line_data.end())
        return nullptr;

    return &_line_data.at(line_name);
}

void PlotFigure::plot2d(const std::string &window_name)
{
    ImPlotAxisFlags axis_flags;

    // ---------------------------------------------------------------------------------------------------------- //
    // Create Check-Box
    // ---------------------------------------------------------------------------------------------------------- //

    // If enabled, you can add data cursor data
    // ImGui::Checkbox("Data Cursor", &_is_cursor_data);
    tooltip_button(&_is_cursor_data);

    if (!ImPlot::BeginPlot("##Plot", ImVec2(-1, -1), ImPlotFlags_NoMouseText)) return;

    // ------------------------------------------------------------------------------------------------------ //
    // Axis Setup
    // ------------------------------------------------------------------------------------------------------ //

    // When data cursor is enabled, lock the axes to prevent panning
    axis_flags = _is_cursor_data ? ImPlotAxisFlags_Lock : 0;

    ImPlot::SetupAxis(ImAxis_X1, nullptr, axis_flags);
    ImPlot::SetupAxis(ImAxis_Y1, nullptr, axis_flags);

    PlotImpl::draw_line(window_name, _line_data, &_vec_tooltip_data, _is_cursor_data, DEFAULT);

    ImPlot::EndPlot();
}

void PlotFigure::bode(const std::string &window_name)
{
    ImPlotAxisFlags axis_flags;

    LineData line_data;

    double x_min_curr, x_max_curr;
    double x_min, x_max;

    x_min = std::numeric_limits<double>::max();
    x_max = std::numeric_limits<double>::min();

    for(std::pair<std::string, LineData> line : _line_data)
    {
        line_data  = line.second;
        x_min_curr = *std::min_element(line_data.x.begin(), line_data.x.end());
        x_max_curr = *std::max_element(line_data.x.begin(), line_data.x.end());

        if(x_min_curr < x_min) x_min = x_min_curr;
        if(x_max_curr > x_max) x_max = x_max_curr;
    }

    ImGui::Checkbox("Data Cursor", &_is_cursor_data);

    if (ImGui::Button("Reset Plot"))
    {
        ImPlot::SetNextAxisToFit(ImAxis_X1);
        ImPlot::SetNextAxisToFit(ImAxis_Y1);
    }

    if (!ImPlot::BeginSubplots("##BodePlot", 2, 1, ImVec2(-1, -1), ImPlotSubplotFlags_LinkAllX)) return;

    axis_flags = _is_cursor_data ? ImPlotAxisFlags_Lock : 0;

    // ====================================================================================================== //
    // 1. Magnitude Plot
    // ====================================================================================================== //

    if(ImPlot::BeginPlot("##Magnitude", ImVec2(-1, -1), ImPlotFlags_NoMouseText))
    {
        // Set X Axis to Log10 Scale
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, x_min, x_max);
        ImPlot::SetupAxis(ImAxis_X1, nullptr, axis_flags);
        ImPlot::SetupAxis(ImAxis_Y1, "Magnitude (dB)", axis_flags);

        PlotImpl::draw_line(window_name, _line_data, &_vec_tooltip_data, _is_cursor_data, MAGNITUDE);

        ImPlot::EndPlot();
    }

    // ====================================================================================================== //
    //                                               Phase Plot                                               //
    // ====================================================================================================== //

    if(ImPlot::BeginPlot("##Phase", ImVec2(-1, -1), ImPlotFlags_NoMouseText))
    {
        // Set X Axis to Log10 Scale
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxis(ImAxis_X1, "Frequency (rad/s)", axis_flags);
        ImPlot::SetupAxis(ImAxis_Y1, "Phase (deg)", axis_flags);

        PlotImpl::draw_line(window_name, _line_data, &_vec_tooltip_data, _is_cursor_data, PHASE);

        ImPlot::EndPlot();
    }

    ImPlot::EndSubplots();
}

void PlotFigure::pzmap(const std::string &window_name)
{
    ImPlotAxisFlags axis_flags;

    ImGui::Checkbox("Data Cursor", &_is_cursor_data);

    if(!ImPlot::BeginPlot("##pzmap", ImVec2(-1, -1), ImPlotFlags_NoMouseText)) return;

    axis_flags = _is_cursor_data ? ImPlotAxisFlags_Lock : 0;

    PlotImpl::radial_grid(axis_flags);

    PlotImpl::draw_pzmap(window_name, _line_data, &_vec_tooltip_data, _is_cursor_data);

    ImPlot::EndPlot();
}
