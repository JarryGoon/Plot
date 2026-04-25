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
#include <cstdlib>
#include <chrono>
#include <thread>

#include <zmq.hpp>

static zmq::context_t context(1);
static zmq::socket_t  socket(context, zmq::socket_type::push);

bool connect_plotting_app()
{
    zmq::send_result_t send_result;
    zmq::recv_result_t recv_result;
    zmq::message_t     msg;

    zmq::socket_t req_socket(context, zmq::socket_type::req);

    // 1. Connect Check
    req_socket.set(zmq::sockopt::rcvtimeo, 100);
    req_socket.set(zmq::sockopt::linger, 0);

    req_socket.connect("tcp://127.0.0.1:5556");

    // 2. Send PING
    send_result = req_socket.send(zmq::message_t("PING", 4), zmq::send_flags::none);
    if(!send_result) return false;

    // 3. Receive PONG
    recv_result = req_socket.recv(msg, zmq::recv_flags::none);

    return recv_result.has_value();
}

void send_data(const char* win_name, const char* line_name,
               const std::vector<double>& x, const std::vector<double>& y, const std::vector<double> &z,
               PlotType flags)
{
    PlotHeader header;
    std::strncpy(header.window_name, win_name, sizeof(header.window_name));
    std::strncpy(header.line_name, line_name, sizeof(header.line_name));

    header.data_size = x.size();
    header.flags = flags;

    zmq::message_t msg_header;
    zmq::message_t msg_x;
    zmq::message_t msg_y((void*)y.data(), y.size() * sizeof(double));
    zmq::message_t msg_z((void*)z.data(), z.size() * sizeof(double));

    msg_header = zmq::message_t(&header, sizeof(PlotHeader));
    msg_x      = zmq::message_t(x.data(), x.size() * sizeof(double));
    msg_y      = zmq::message_t(y.data(), y.size() * sizeof(double));

    if(z.empty()) msg_z = zmq::message_t();
    else          msg_z = zmq::message_t(z.data(), z.size() * sizeof(double));

    socket.send(msg_header, zmq::send_flags::sndmore);
    socket.send(msg_x, zmq::send_flags::sndmore);
    socket.send(msg_y, zmq::send_flags::none);
    socket.send(msg_z, zmq::send_flags::none);
}

void init_plotting_app()
{
    static auto last_check_time = std::chrono::system_clock::time_point::min();
    static bool is_connected = false;

    auto now = std::chrono::system_clock::now();
    long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_check_time).count();

    bool app_ready;

    if(!is_connected || elapsed > 1000)
    {
        last_check_time = now;

        if(!connect_plotting_app())
        {
            std::system("Plotting_App &");

            app_ready = false;
            for(int i = 0; i < 20; i++)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                if(connect_plotting_app())
                {
                    app_ready = true;
                    break;
                }
            }

            if(!app_ready)
            {
                std::cerr << "Failed to connect to plotting app" << std::endl;
                return;
            }
        }

        if(!is_connected)
        {
            socket.connect("tcp://127.0.0.1:5555");
            is_connected = true;
        }
    }
}

void Plot::plot(const std::vector<double> &x, const std::vector<double> &y,
          const char* win_name        , const char* line_name)
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

    init_plotting_app();

    send_data(win_name, line_name, x, y, {}, PLOT_2D);
}

void Plot::bode_plot(const std::vector<double> &freq, const std::vector<double> &mag, const std::vector<double> &phase)
{
    if(freq.size() != mag.size() || mag.size() != phase.size())
    {
        std::cerr << "freq, mag, and phase vectors must be of the same size" << std::endl;
        return;
    }

    init_plotting_app();

    send_data("", "", freq, mag, phase, BODE);
}

void Plot::pzmap_plot(const std::vector<double> &real, const std::vector<double> &imag, const std::vector<double> &pz)
{
    if(real.size() != imag.size() || imag.size() != pz.size())
    {
        std::cerr << "real, imag, and pz vectors must be of the same size" << std::endl;
        return;
    }

    init_plotting_app();

    send_data("", "", real, imag, pz, PZMAP);
}
