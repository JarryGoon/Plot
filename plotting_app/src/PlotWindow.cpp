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


#include "PlotWindow.hpp"

PlotWindow::PlotWindow(const std::string &window_name)
{
    _window_name = window_name;
    _is_open     = true;
}

PlotFigure* PlotWindow::add_plot_data(const std::string &data_name, PlotType type)
{
    for(PlotFigure& figure: _plot_data)
    {
        if(figure.plot_name == data_name && figure.get_plot_type() == type)
            return &figure;
    }

    _plot_data.emplace_back(type);

    return &_plot_data.back();
}

PlotFigure* PlotWindow::get_plot_data(const std::string &data_name, PlotType type)
{
    for(PlotFigure& figure: _plot_data)
    {
        if(figure.plot_name == data_name && figure.get_plot_type() == type)
            return &figure;
    }

    return nullptr;
}

void PlotWindow::draw()
{
    if (!ImGui::Begin(_window_name.c_str(), &_is_open)) return;

    for(PlotFigure& plot : _plot_data)
    {
        switch(plot.get_plot_type())
        {
            case PLOT_2D:
                plot.plot2d(_window_name);
                break;

            case BODE:
                plot.bode(_window_name);
                break;

            case PZMAP:
                plot.pzmap(_window_name);
                break;

            default:
                break;
        }
    }

    ImGui::End();
}
