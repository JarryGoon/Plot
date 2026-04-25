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

#ifndef PLOT_DATATOOLTIP_HPP
#define PLOT_DATATOOLTIP_HPP

#include "PlottingType.hpp"

typedef enum CursorAction {
    None,
    DeleteCurrent,
    ClearAll
} CursorAction;

ImVec2 find_closest_point(ImVec2 mouse, const std::vector<double> &x_data, const std::vector<double> &y_data);

CursorAction draw_tooltip(const TooltipData &tooltip_data, const std::string &id, const std::string &window_name,
                          bool is_fixed,                   ToolTipType print_type = DEFAULT);

void plot2d_tooltip_data(std::vector<TooltipData> &vec_tooltip_data, const std::string &window_name,
                         ToolTipType print_type = DEFAULT);

#endif //PLOT_DATATOOLTIP_HPP