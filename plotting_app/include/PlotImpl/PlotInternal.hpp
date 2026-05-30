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

#ifndef PLOT_PLOTINTERNAL_HPP
#define PLOT_PLOTINTERNAL_HPP

#include "PlottingType.hpp"

#include <map>

namespace PlotImpl {

void draw_line(const std::string &                    window_name   ,
               const std::map<std::string, LineData> &line_datas    , std::vector<TooltipData> *vec_tooltip_data,
               bool                                   is_cursor_data, ToolTipType               plot_type);

// void draw_pzmap(const std::string &window_name, PlotWindow &plot_window);

}

#endif //PLOT_PLOTINTERNAL_HPP
