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

#include "PlotUtils.hpp"

#include <algorithm>
#include <implot.h>
#include <implot_internal.h>

#include <limits>

#include "PlotImpl/GridFunc.hpp"
#include "DataTooltip/DataTooltip.hpp"
#include "TypeFunc.hpp"
#include "PlotImpl/PlotInternal.hpp"

#define HOVERING_DIST 10

void plot_2d(const std::string &window_name, PlotWindow &plot_window)
{
    ImPlotAxisFlags axis_flags;

    if (ImGui::Begin(window_name.c_str(), &plot_window.is_open))
    {
        // ---------------------------------------------------------------------------------------------------------- //
        // Create Check-Box
        // ---------------------------------------------------------------------------------------------------------- //

        // If enabled, you can add data cursor data
        ImGui::Checkbox("Data Cursor", &plot_window.is_cursor_data);

        if (ImPlot::BeginPlot("##Plot", ImVec2(-1, -1), ImPlotFlags_NoMouseText))
        {
            // ------------------------------------------------------------------------------------------------------ //
            // Axis Setup
            // ------------------------------------------------------------------------------------------------------ //

            // When data cursor is enabled, lock the axes to prevent panning
            axis_flags = plot_window.is_cursor_data ? ImPlotAxisFlags_Lock : 0;

            ImPlot::SetupAxis(ImAxis_X1, nullptr, axis_flags);
            ImPlot::SetupAxis(ImAxis_Y1, nullptr, axis_flags);

            PlotImpl::draw_line(window_name, plot_window, DEFAULT);

            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}

void bode_plot(const std::string &window_name, PlotWindow &plot_window)
{
    ImPlotAxisFlags axis_flags;

    double x_min_curr, x_max_curr;
    double x_min, x_max;

    x_min = std::numeric_limits<double>::max();
    x_max = std::numeric_limits<double>::min();

    for(LineData& line : plot_window.lines)
    {
        x_min_curr = *std::min_element(line.x.begin(), line.x.end());
        x_max_curr = *std::max_element(line.x.begin(), line.x.end());

        if(x_min_curr < x_min) x_min = x_min_curr;
        if(x_max_curr > x_max) x_max = x_max_curr;
    }

    if (ImGui::Begin(window_name.c_str(), &plot_window.is_open))
    {
        ImGui::Checkbox("Data Cursor", &plot_window.is_cursor_data);

        if (ImGui::Button("Reset Plot"))
        {
            ImPlot::SetNextAxisToFit(ImAxis_X1);
            ImPlot::SetNextAxisToFit(ImAxis_Y1);
        }

        if (ImPlot::BeginSubplots("##BodePlot", 2, 1, ImVec2(-1, -1), ImPlotSubplotFlags_LinkAllX))
        {
            axis_flags = plot_window.is_cursor_data ? ImPlotAxisFlags_Lock : 0;

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

                PlotImpl::draw_line(window_name, plot_window, MAGNITUDE);

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

                PlotImpl::draw_line(window_name, plot_window, PHASE);

                ImPlot::EndPlot();
            }

            ImPlot::EndSubplots();
        }
    }
    ImGui::End();
}

void pzmap_plot(const std::string &window_name, PlotWindow &plot_window)
{
    ImPlotAxisFlags axis_flags;

    if(ImGui::Begin(window_name.c_str(), &plot_window.is_open))
    {
        ImGui::Checkbox("Data Cursor", &plot_window.is_cursor_data);

        if(ImPlot::BeginPlot("##pzmap", ImVec2(-1, -1), ImPlotFlags_NoMouseText))
        {
            axis_flags = plot_window.is_cursor_data ? ImPlotAxisFlags_Lock : 0;

            PlotImpl::radial_grid(axis_flags);



            ImPlot::EndPlot();
        }
    }

    ImGui::End();
}
