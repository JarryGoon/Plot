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

#include "PlotFigure.hpp"
#include "PlotHeader.hpp"
#include "CommManager.hpp"

#include <zmq.hpp>

static zmq::context_t context(1);
static zmq::socket_t  socket(context, zmq::socket_type::pull);
static zmq::socket_t  rep_socket(context, zmq::socket_type::rep);

typedef std::_Rb_tree_iterator<std::pair<const std::string, PlotWindow>> plot_window_iterator;
typedef std::pair<plot_window_iterator, bool>                            window_map_pair;

void init_comm_manager()
{
    socket.bind("tcp://*:5555");
    rep_socket.bind("tcp://*:5556");
}

void send_comm_state()
{
    zmq::message_t msg;

    if(rep_socket.recv(msg, zmq::recv_flags::dontwait))
        rep_socket.send(zmq::message_t("PONG", 4), zmq::send_flags::none);
}

PlotType receive_plot_data(std::map<std::string, PlotWindow>* plot_windows)
{
    zmq::message_t     msg_header, msg_x, msg_y, msg_z;
    zmq::recv_result_t res1, res2, res3, res4;

    PlotHeader* header;
    double*     x_ptr;
    double*     y_ptr;
    double*     z_ptr = nullptr;

    bool recv_z_data;

    std::vector<double> x_vec;
    std::vector<double> y_vec;
    std::vector<double> z_vec;

    uint32_t subplot_x;
    uint32_t subplot_y;
    uint32_t subplot_idx;

    std::string window_name;
    std::string plot_name;
    std::string line_name;

    plot_window_iterator plot_window_iter;
    window_map_pair      map_pair;

    PlotWindow* plot_window;
    PlotFigure* plot_data;
    LineData*   line_data;

    PlotType plot_type;

    int figure_num = 1;

    // ============================================================================================================== //
    // Receive and process plot data message
    // ============================================================================================================== //

    res1 = socket.recv(msg_header, zmq::recv_flags::dontwait);
    if (!res1) return NONE;

    res2 = socket.recv(msg_x, zmq::recv_flags::none);
    res3 = socket.recv(msg_y, zmq::recv_flags::none);
    res4 = socket.recv(msg_z, zmq::recv_flags::none);
    if (!res2 || !res3 || !res4) return NONE;

    if(msg_z.size()) recv_z_data = true;
    else             recv_z_data = false;

    // ============================================================================================================== //
    // Data parsing
    // ============================================================================================================== //

    header = static_cast<PlotHeader*>(msg_header.data());
    x_ptr  = static_cast<double*>(msg_x.data());
    y_ptr  = static_cast<double*>(msg_y.data());

    if(recv_z_data)
        z_ptr  = static_cast<double*>(msg_z.data());

    window_name = header->window_name;
    // plot_name   = header->plot_name;
    line_name   = header->line_name;
    plot_type   = header->flags;

    x_vec.assign(x_ptr, x_ptr + header->data_size);
    y_vec.assign(y_ptr, y_ptr + header->data_size);

    if(recv_z_data)
        z_vec.assign(z_ptr, z_ptr + header->data_size);

    // ============================================================================================================== //
    // Save Data to buffer of plotting windows
    // ============================================================================================================== //

    // Load Plotting Window
    // If map not has window, same name, create new window
    if(window_name.empty())
    {
        while(true)
        {
            window_name = "Figure " + std::to_string(figure_num);

            if(plot_windows->find(window_name) == plot_windows->end())
                break;

            figure_num++;
        }
    }

    map_pair         = plot_windows->try_emplace(window_name, window_name);
    plot_window_iter = map_pair.first;
    plot_window      = &(plot_window_iter->second);

    // Load Plot Data
    plot_data = plot_window->add_plot_data("", plot_type);

    // Load Line Data
    line_data = plot_data->add_line_data(line_name);

    line_data->x = std::move(x_vec);
    line_data->y = std::move(y_vec);

    if(recv_z_data)
        line_data->z = std::move(z_vec);

    return header->flags;
}

void clear_comm_manager()
{
    socket.set(zmq::sockopt::linger, 0);
    socket.close();
    rep_socket.close();
    context.close();
}
