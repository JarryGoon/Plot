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

#ifndef CONTROL_LIBRARY_PLOT_HPP
#define CONTROL_LIBRARY_PLOT_HPP

#include <vector>
#include <complex>

#include <zmq.hpp>

class Plot final
{
public:
    Plot();

    ~Plot() = default;

    void figure(const std::string& name);
    void figure(uint32_t num);

    void subplot(uint32_t rows, uint32_t cols, uint32_t idx);

    void title(const std::string& title);

    void plot(const std::vector<double> &x, const std::vector<double> &y,
              const char* line_name = "") const;

    void bode(const std::vector<double> &freq, const std::vector<double> &mag, const std::vector<double> &phase,
              const char* line_name = "") const;

    void pzmap(const std::vector<std::complex<double>> &pole, const std::vector<std::complex<double>> &zero,
                 const char* line_name = "") const;

private:
    uint32_t subplot_rows;
    uint32_t subplot_cols;
    uint32_t subplot_idx;

    std::string _curr_win_name;
    std::string _curr_plot_name;

    static zmq::context_t _context;
    static zmq::socket_t  _socket;

    static bool _connect_plotting_app();

    static void _send_data(const char* win_name, const char* plot_name, const char* line_name,
                           const std::vector<double>& x, const std::vector<double>& y, const std::vector<double> &z,
                           uint32_t flags);

    static void _init_plotting_app();
};

#endif //CONTROL_LIBRARY_PLOT_HPP
