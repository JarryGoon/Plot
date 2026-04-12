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

#include <iostream>
#include <map>

#include "PlottingFunc.hpp"
#include "CommManager.hpp"

static std::map<std::string, PlotWindow> plot_windows;

int main()
{
    SDL_GLContext gl_context = nullptr;
    SDL_Window*   window     = nullptr;

    std::string window_name;
    PlotWindow* plot_window;
    PlotType    flags;

    init_comm_manager();

    init_ImPlot(&gl_context, &window);
    if(!window)
    {
        std::cout << "SDL3 & OpenGL Initialization Failed!" << std::endl;
        return -1;
    }

    while(true)
    {
        if(event_polling(window)) break;

        new_frame();

        send_comm_state();

        flags = receive_plot_data(&plot_windows);

        if(flags == EXIT_APP) break;

        for(auto it = plot_windows.begin(); it != plot_windows.end();)
        {
            window_name = it->first;
            plot_window = &(it->second);

            switch(plot_window->plot_type)
            {
                case PLOT_2D:
                    plot_2d(window_name, *plot_window);
                    break;

                case BODE:
                    bode_plot(window_name, *plot_window);
                    break;

                default:
                    break;
            }

            if(!plot_window->is_open)
                it = plot_windows.erase(it);
            else
                ++it;
        }

        renderer(window);
    }

    clear_ImPlot(&gl_context, window);
    clear_comm_manager();

    return 0;
}
