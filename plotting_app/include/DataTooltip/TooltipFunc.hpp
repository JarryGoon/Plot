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

#ifndef PLOT_TOOLTIPFUNC_HPP
#define PLOT_TOOLTIPFUNC_HPP

#include <string>
#include <implot.h>

typedef enum TooltipPos
{
    NoUpdate,
    TopRight,
    TopLeft,
    BottomRight,
    BottomLeft,
} TooltipPos;

void draw_datapoint(const ImVec2 &cursor, const std::string &id, bool is_fixed);

void init_tooltip_window(const ImVec2 &tooltip_pos,
                         ImVec2        tooltip_win_size,
                         bool          is_fixed,
                         TooltipPos    tooltip_pos_type);

TooltipPos drag_tooltip(const std::string &tooltip_win_id, const ImVec2 &tooltip_pix_pos);

#endif //PLOT_TOOLTIPFUNC_HPP