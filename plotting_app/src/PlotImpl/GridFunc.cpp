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

#include "PlotImpl/GridFunc.hpp"

#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdio>

#define NUM_CIRCLE  5.0
#define NUM_SEGMENT 100
#define NUM_RAY     18

#define M_3_PI_4 (M_PI_2 + M_PI_4)
#define M_5_PI_4 (M_PI + M_PI_4)

namespace PlotImpl {

void radial_grid(ImPlotAxisFlags axis_flags)
{
    ImPlotRect window_limit;
    ImPlotSpec spec;

    double max_x, max_y, max_r;

    double raw_step;
    double step;

    double order;
    double base;
    double multiplier;

    double theta;

    std::vector<double> arc_x(NUM_SEGMENT + 1);
    std::vector<double> arc_y(NUM_SEGMENT + 1);

    double ray_x[2];
    double ray_y[2];

    double edge_x, edge_y;

    double zeta;
    ImVec2 zeta_label_offset(0, 0);

    double min_x_intercept;
    double dx_pix, dy_pix;
    double len_pix;
    ImVec2 center_pix;
    ImVec2 edge_pix;

    double zero = 0;
    char   label[32];

    // ============================================================================================================== //
    // 1. Hide Default Axis Grid Lines and Tick Marks
    // ============================================================================================================== //

    ImPlot::SetupAxis(ImAxis_X1, "Real", ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | axis_flags);
    ImPlot::SetupAxis(ImAxis_Y1, "Imag", ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | axis_flags);

    // ============================================================================================================== //
    // 2. Calculate Maximum Radial and Concentric Circle Steps
    // ============================================================================================================== //

    window_limit = ImPlot::GetPlotLimits();

    max_x = std::max(std::abs(window_limit.X.Min), std::abs(window_limit.X.Max));
    max_y = std::max(std::abs(window_limit.Y.Min), std::abs(window_limit.Y.Max));
    max_r = std::sqrt(max_x * max_x + max_y * max_y);

    raw_step   = max_r / NUM_CIRCLE;
    order      = std::floor(std::log10(raw_step));
    base       = std::pow(10, order);
    multiplier = raw_step / base;

    if(multiplier < 2.0)       step = base;
    else if (multiplier < 5.0) step = 2.0 * base;
    else                       step = 5.0 * base;

    // ============================================================================================================== //
    // 3. Axes Lines
    // ============================================================================================================== //

    spec.LineColor  = ImVec4(0.4, 0.4, 0.4, 1);
    spec.LineWeight = 1.5;

    ImPlot::PlotInfLines("##RealAxis", &zero, 1, spec);

    spec.Flags = ImPlotInfLinesFlags_Horizontal;
    ImPlot::PlotInfLines("##ImagAxis", &zero, 1, spec);

    // ============================================================================================================== //
    // 4. Draw Concentric Circles
    // ============================================================================================================== //

    spec.LineColor  = ImVec4(0.7, 0.7, 0.7, 1);
    spec.LineWeight = 1;
    spec.Flags      = ImPlotFlags_None;

    for(double r = step; r <= max_r; r += step)
    {
        for(int i = 0; i <= NUM_SEGMENT; i++)
        {
            theta = M_PI / 2.0 + M_PI * i / NUM_SEGMENT;

            arc_x[i] = r * std::cos(theta);
            arc_y[i] = r * std::sin(theta);
        }

        ImPlot::PlotLine("##GridCircle", arc_x.data(), arc_y.data(), NUM_SEGMENT + 1, spec);

        snprintf(label, sizeof(label), "%.2f", r);
        ImPlot::PlotText(label, -r, 0, ImVec2(-15, -5), spec);
    }

    // ============================================================================================================== //
    // 5. Draw Radiar ray
    // ============================================================================================================== //

    for(int i = 1; i < NUM_RAY; i++)
    {
        theta = M_PI * i / NUM_RAY + M_PI_2;

        if(std::abs(theta + M_PI) < 1e-6) continue;

        ray_x[0] = 0.0;
        ray_y[0] = 0.0;

        ray_x[1] = max_r * std::cos(theta);
        ray_y[1] = max_r * std::sin(theta);

        ImPlot::PlotLine("##GridRay", ray_x, ray_y, 2, spec);

        // Tooltip
        zeta = std::abs(std::cos(theta));
        snprintf(label, sizeof(label), "%.2f", zeta);

        min_x_intercept = ray_y[1] * window_limit.X.Min / ray_x[1];

        if(window_limit.Y.Min < min_x_intercept && min_x_intercept < window_limit.Y.Max)
        {
            edge_x = std::max(ray_x[1], window_limit.X.Min);
            edge_y = ray_y[1] * (edge_x / ray_x[1]);
        }
        else
        {
            if(ray_y[1] > 0) edge_y = std::min(ray_y[1], window_limit.Y.Max);
            else             edge_y = std::max(ray_y[1], window_limit.Y.Min);

            edge_x = ray_x[1] * (edge_y / ray_y[1]);
        }

        center_pix = ImPlot::PlotToPixels(0, 0);
        edge_pix   = ImPlot::PlotToPixels(edge_x, edge_y);

        dx_pix = edge_pix.x - center_pix.x;
        dy_pix = edge_pix.y - center_pix.y;
        len_pix = std::sqrt(dx_pix * dx_pix + dy_pix * dy_pix);

        if(len_pix > 1e-6)
        {
            zeta_label_offset.x = -dx_pix / len_pix * 20;
            zeta_label_offset.y = -dy_pix / len_pix * 20;
        }

        zeta_label_offset.y = std::max(std::min(zeta_label_offset.y, 10.f), -10.f);

        ImPlot::PlotText(label, edge_x, edge_y, zeta_label_offset);
    }
}

}
