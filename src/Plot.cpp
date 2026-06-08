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

#include "Plot.hpp"
#include "PlotHeader.hpp"

#include <iostream>

Plot::Plot()
{
    _subplot_rows = 1;
    _subplot_cols = 1;
    _subplot_idx  = 0;

    _curr_win_name  = "Figure 1";
    _curr_plot_name = "";

    _context = zmq::context_t(1);
    _socket  = zmq::socket_t(_context, zmq::socket_type::push);
}

Plot& Plot::instance()
{
    static Plot instance;
    return instance;
}

void Plot::figure(const std::string& name)
{
    _curr_win_name = name;
}

void Plot::figure(uint32_t num)
{
    _curr_win_name = "Figure " + std::to_string(num);
}

void Plot::subplot(uint32_t rows, uint32_t cols, uint32_t idx)
{
    _subplot_rows = rows;
    _subplot_cols = cols;
    _subplot_idx  = idx;
}

void Plot::title(const std::string& title)
{
    _curr_plot_name = title;
}

void Plot::plot(const std::vector<double> &x, const std::vector<double> &y, const char* line_name)
{
    if(x.empty() || y.empty())
    {
        std::cerr << "x and y vectors must not be empty" << std::endl;
        return;
    }

    if(x.size() != y.size())
    {
        std::cerr << "x and y vectors must be of the same size" << std::endl;
        return;
    }

    _init_plotting_app();

    _send_data(_curr_win_name.c_str(), _curr_plot_name.c_str(), line_name,
               x, y, {}, PLOT_2D);
}

void Plot::live_plot(double x, double y, const char *line_name)
{
    _init_plotting_app();

    _send_data(_curr_win_name.c_str(), _curr_plot_name.c_str(), line_name,
               std::vector<double>{x}, std::vector<double>{y}, {}, PLOT_LIVE);
}

void Plot::bode(const std::vector<double> &freq, const std::vector<double> &mag, const std::vector<double> &phase,
                const char*                line_name)
{
    if(freq.size() != mag.size() || mag.size() != phase.size())
    {
        std::cerr << "freq, mag, and phase vectors must be of the same size" << std::endl;
        return;
    }

    _init_plotting_app();

    _send_data(_curr_win_name.c_str(), _curr_plot_name.c_str(), line_name, freq, mag, phase, BODE);
}

void Plot::pzmap(const std::vector<std::complex<double>> &pole, const std::vector<std::complex<double>> &zero,
                 const char*                              line_name)
{
    size_t num_pole = pole.size() << 1;
    size_t num_zero = zero.size() << 1;

    std::vector<double> vec_pole(num_pole);
    std::vector<double> vec_zero(num_zero);

    for(size_t i = 0; i < pole.size(); i++)
    {
        vec_pole.at(i << 1)       = pole[i].real();
        vec_pole.at((i << 1) + 1) = pole[i].imag();
    }

    for(size_t i = 0; i < zero.size(); i++)
    {
        vec_zero.at(i << 1)       = zero[i].real();
        vec_zero.at((i << 1) + 1) = zero[i].imag();
    }

    _init_plotting_app();

    _send_data(_curr_win_name.c_str(), _curr_plot_name.c_str(), line_name, vec_pole, vec_zero, {}, PZMAP);
}
