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

#ifndef PLOT_PLOTDATA_HPP
#define PLOT_PLOTDATA_HPP

#include <map>

#include "PlottingType.hpp"

class PlotFigure
{
public:
    std::string plot_name;

    PlotFigure(PlotType type);

    PlotType get_plot_type() const {return _plot_type; }

    LineData* add_line_data(const std::string& line_name);

    /**
     *
     * @param line_name Line name
     * @return
     */
    LineData* get_line_data(const std::string& line_name);

    void plot2d(const std::string &window_name);

    void bode(const std::string &window_name);

    void pzmap(const std::string &window_name);

private:
    PlotType _plot_type;

    bool _is_cursor_data;

    std::map<std::string, LineData> _line_data;

    std::vector<TooltipData> _vec_tooltip_data;
};

#endif //PLOT_PLOTDATA_HPP
