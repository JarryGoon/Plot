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

#include "PlotHeader.hpp"
#include "CommManager.hpp"

#include <zmq.hpp>

static zmq::context_t context(1);
static zmq::socket_t  socket(context, zmq::socket_type::pull);
static zmq::socket_t  rep_socket(context, zmq::socket_type::rep);

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

    PlotType plot_type;

    std::string window_name;
    int         figure_num = 1;

    std::string line_name;
    int         line_num = 1;

    LineData    new_line;
    bool        line_exists;


    res1 = socket.recv(msg_header, zmq::recv_flags::dontwait);
    if (!res1) return NONE;

    res2 = socket.recv(msg_x, zmq::recv_flags::none);
    res3 = socket.recv(msg_y, zmq::recv_flags::none);
    res4 = socket.recv(msg_z, zmq::recv_flags::none);
    if (!res2 || !res3 || !res4) return NONE;

    if(msg_z.size()) recv_z_data = true;
    else             recv_z_data = false;

    header = static_cast<PlotHeader*>(msg_header.data());
    x_ptr  = static_cast<double*>(msg_x.data());
    y_ptr  = static_cast<double*>(msg_y.data());

    if(recv_z_data)
        z_ptr  = static_cast<double*>(msg_z.data());

    window_name = header->window_name;
    line_name   = header->line_name;
    plot_type   = header->flags;

    x_vec.assign(x_ptr, x_ptr + header->data_size);
    y_vec.assign(y_ptr, y_ptr + header->data_size);

    if(recv_z_data)
        z_vec.assign(z_ptr, z_ptr + header->data_size);

    if(window_name.empty())
    {
        while(true)
        {
            window_name = "Figure " + std::to_string(figure_num);

            if(plot_windows->find(window_name) == plot_windows->end()) break;
            if((*plot_windows)[window_name].plot_type == plot_type) break;

            figure_num++;
        }
    }

    if(line_name.empty())
    {
        while(true)
        {
            line_name  = "Data " + std::to_string(line_num);
            line_exists = false;

            for (auto& line : (*plot_windows)[window_name].lines)
            {
                if (line.name == line_name)
                {
                    line_exists = true;
                    break;
                }
            }

            if(!line_exists) break;

            line_num++;
        }
    }

    line_exists = false;
    for (auto& line : (*plot_windows)[window_name].lines)
    {
        if (line.name == line_name)
        {
            line.x = std::move(x_vec);
            line.y = std::move(y_vec);

            if(recv_z_data)
                line.z = std::move(z_vec);

            line_exists = true;
            break;
        }
    }

    if (!line_exists)
    {
        new_line = {std::move(x_vec), std::move(y_vec), std::move(z_vec), line_name};
        (*plot_windows)[window_name].lines.push_back(new_line);
        (*plot_windows)[window_name].plot_type = plot_type;
    }

    return header->flags;
}

void clear_comm_manager()
{
    socket.set(zmq::sockopt::linger, 0);
    socket.close();
    rep_socket.close();
    context.close();
}
