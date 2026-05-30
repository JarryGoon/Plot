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


#ifndef PLOT_PLOTWINDOW_HPP
#define PLOT_PLOTWINDOW_HPP

#include "PlotFigure.hpp"
#include "PlottingType.hpp"

class PlotWindow final
{
public:
    PlotWindow(const std::string &window_name);

    bool get_is_open() const { return _is_open; }

    PlotFigure* add_plot_data(const std::string &data_name, PlotType type);

    PlotFigure* get_plot_data(const std::string &data_name, PlotType type);

    void draw();

private:
    bool        _is_open;
    std::string _window_name;

    std::vector<PlotFigure> _plot_data;
};


#endif //PLOT_PLOTWINDOW_HPP
