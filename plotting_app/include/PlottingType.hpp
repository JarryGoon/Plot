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

#ifndef CONTROL_LIBRARY_PLOTTINGTYPE_HPP
#define CONTROL_LIBRARY_PLOTTINGTYPE_HPP

#include <vector>
#include <string>

#include <imgui.h>

#include "PlotHeader.hpp"

enum ToolTipType
{
    DEFAULT,
    MAGNITUDE,
    PHASE,
    PZ,
    POLE,
    ZERO,
};

struct LineData
{
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::string         name;
};

struct TooltipData
{
    std::string line_id;
    ToolTipType tooltip_type;
    ImVec2      tooltip_data;
};

struct PlotWindow
{
    std::vector<LineData> lines;
    PlotType              plot_type = NONE;
    bool                  is_open   = true;

    bool                     is_cursor_data = false;
    std::vector<TooltipData> vec_tooltip_data;
};

#endif //CONTROL_LIBRARY_PLOTTINGTYPE_HPP