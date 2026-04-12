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

#include "PlottingFunc.hpp"

#include <algorithm>
#include <implot.h>
#include <implot_internal.h>

#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>
#include <limits>

#include "DataTooltip/DataTooltip.hpp"
#include "TypeFunc.hpp"

#define HOVERING_DIST 10

void init_ImPlot(SDL_GLContext* gl_context, SDL_Window** window)
{
    SDL_WindowFlags window_flags;

    // ============================================================================================================== //
    //                                       [Step A] SDL3 & OpenGL Initialize                                        //
    // ============================================================================================================== //

    // 1. Initialize SDL Video and Events subsystem
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) return;

    // 2. OpenGL Setting (Core Profile 3.0)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);

    // 3. Create Window
    window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    *window       = SDL_CreateWindow("Plot", 1000, 700, window_flags);
    if(!(*window)) return;

    // 4. Initialize Context
    *gl_context = SDL_GL_CreateContext(*window);
    SDL_GL_SetSwapInterval(0);

    // ============================================================================================================== //
    //                                      [Step 2] ImGui & ImPlot Initialize                                        //
    // ============================================================================================================== //

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui_ImplSDL3_InitForOpenGL(*window, *gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");
}

bool event_polling(SDL_Window* window)
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        if(event.type == SDL_EVENT_QUIT)
            return true;
        if(event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
            return true;
    }

    return false;
}

void new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void renderer(SDL_Window* window)
{
    int display_w, display_h;

    ImGui::Render();

    SDL_GetWindowSizeInPixels(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

void clear_ImPlot(const SDL_GLContext* gl_context, SDL_Window* window)
{
    // Terminate ImGui Backend
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();

    // Terminate ImPlot & ImGui
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    // Terminate SDL
    SDL_GL_DestroyContext(*gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void plot_2d(const std::string &window_name, PlotWindow &plot_window)
{
    ImPlotAxisFlags axis_flags;
    ImPlotPoint     mouse;

    bool add_cursor_data_flag;

    ImVec2      mouse_pos;
    ImVec2      cand_cursor_data;
    ImVec2      curr_cursor_data = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    std::string tooltip_line_id;

    const char* line_name;
    ImPlotItem* item;

    if (ImGui::Begin(window_name.c_str(), &plot_window.is_open))
    {
        ImGui::Checkbox("Data Cursor", &plot_window.is_cursor_data);

        if (ImPlot::BeginPlot("##Plot", ImVec2(-1, -1), ImPlotFlags_NoMouseText))
        {
            axis_flags = plot_window.is_cursor_data ? ImPlotAxisFlags_Lock : 0;

            ImPlot::SetupAxis(ImAxis_X1, nullptr, axis_flags);
            ImPlot::SetupAxis(ImAxis_Y1, nullptr, axis_flags);

            mouse                = ImPlot::GetPlotMousePos();
            mouse_pos            = {static_cast<float>(mouse.x), static_cast<float>(mouse.y)};
            add_cursor_data_flag = plot_window.is_cursor_data &&
                                   ImPlot::IsPlotHovered()    &&
                                   ImGui::IsMouseClicked(ImGuiMouseButton_Left);

            for(LineData& line : plot_window.lines)
            {
                line_name = line.name.c_str();

                ImPlot::PlotLine(line_name, line.x.data(), line.y.data(), line.x.size());

                item = ImPlot::GetItem(line_name);
                if(item != nullptr && !item->Show) continue;

                cand_cursor_data = find_closest_point(mouse_pos, line.x, line.y);
                if (distance(cand_cursor_data, mouse_pos) < distance(curr_cursor_data, mouse_pos))
                {
                    curr_cursor_data = cand_cursor_data;
                    tooltip_line_id  = line_name;
                }
            }

            if (add_cursor_data_flag)
                plot_window.vec_tooltip.push_back({tooltip_line_id, DEFAULT, curr_cursor_data});

            plot_tooltip_data(plot_window.vec_tooltip, window_name, "X: %.3f\nY: %.3f");

            if(distance(ImPlot::PlotToPixels(mouse_pos), ImPlot::PlotToPixels(curr_cursor_data)) < HOVERING_DIST)
                draw_tooltip(curr_cursor_data, "HOVER", window_name, "X: %.3f\nY: %.3f", false);

            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}

void bode_plot(const std::string &window_name, PlotWindow &plot_window)
{
    ImPlotAxisFlags axis_flags;
    ImPlotPoint     mouse;

    bool add_cursor_data_flag;

    ImVec2      mouse_pos;
    ImVec2      cand_cursor_data;
    ImVec2      curr_cursor_data = {0, 0};
    std::string tooltip_line_id;

    const char* line_name;
    ImPlotItem* item;

    double x_min_curr, x_max_curr;
    double x_min, x_max;

    x_min = std::numeric_limits<double>::max();
    x_max = std::numeric_limits<double>::min();

    for(LineData& line : plot_window.lines)
    {
        x_min_curr = *std::min_element(line.x.begin(), line.x.end());
        x_max_curr = *std::max_element(line.x.begin(), line.x.end());

        if(x_min_curr < x_min) x_min = x_min_curr;
        if(x_max_curr > x_max) x_max = x_max_curr;
    }

    if (ImGui::Begin(window_name.c_str(), &plot_window.is_open))
    {
        ImGui::Checkbox("Data Cursor", &plot_window.is_cursor_data);

        if (ImGui::Button("Reset Plot"))
        {
            ImPlot::SetNextAxisToFit(ImAxis_X1);
            ImPlot::SetNextAxisToFit(ImAxis_Y1);
        }

        if (ImPlot::BeginSubplots("##BodePlot", 2, 1, ImVec2(500, 500), ImPlotSubplotFlags_LinkAllX))
        {
            axis_flags = plot_window.is_cursor_data ? ImPlotAxisFlags_Lock : 0;

            // ====================================================================================================== //
            //                                             Magnitude Plot                                             //
            // ====================================================================================================== //

            if(ImPlot::BeginPlot("##Magnitude", ImVec2(-1, -1), ImPlotFlags_NoMouseText))
            {
                // Set X Axis to Log10 Scale
                ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
                ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, x_min, x_max);
                ImPlot::SetupAxis(ImAxis_X1, nullptr, axis_flags);
                ImPlot::SetupAxis(ImAxis_Y1, "Magnitude (dB)", axis_flags);

                mouse                = ImPlot::GetPlotMousePos();
                mouse_pos            = {static_cast<float>(mouse.x), static_cast<float>(mouse.y)};
                add_cursor_data_flag = plot_window.is_cursor_data &&
                                       ImPlot::IsPlotHovered()    &&
                                       ImGui::IsMouseClicked(ImGuiMouseButton_Left);

                for(LineData& line : plot_window.lines)
                {
                    line_name = line.name.c_str();

                    ImPlot::PlotLine(line_name, line.x.data(), line.y.data(), line.x.size());

                    item = ImPlot::GetItem(line_name);
                    if(item != nullptr && !item->Show) continue;

                    cand_cursor_data = find_closest_point(mouse_pos, line.x, line.y);
                    if (distance(cand_cursor_data, mouse_pos) < distance(curr_cursor_data, mouse_pos))
                    {
                        curr_cursor_data = cand_cursor_data;
                        tooltip_line_id  = line_name;
                    }
                }

                if (add_cursor_data_flag)
                    plot_window.vec_tooltip.push_back({tooltip_line_id, MAGNITUDE, curr_cursor_data});

                plot_tooltip_data(plot_window.vec_tooltip, window_name,
                                  "Freq.: %.3f rad/s\nMag.: %.3f dB", MAGNITUDE);

                if(distance(ImPlot::PlotToPixels(mouse_pos), ImPlot::PlotToPixels(curr_cursor_data)) < HOVERING_DIST)
                    draw_tooltip(curr_cursor_data, "HOVER", window_name, "Freq.: %.3f rad/s\nMag.: %.3f dB", false);

                ImPlot::EndPlot();
            }

            // ====================================================================================================== //
            //                                               Phase Plot                                               //
            // ====================================================================================================== //

            if(ImPlot::BeginPlot("##Phase", ImVec2(-1, -1), ImPlotFlags_NoMouseText))
            {
                // Set X Axis to Log10 Scale
                ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
                ImPlot::SetupAxis(ImAxis_X1, "Frequency (rad/s)", axis_flags);
                ImPlot::SetupAxis(ImAxis_Y1, "Phase (deg)", axis_flags);

                mouse                = ImPlot::GetPlotMousePos();
                mouse_pos            = {static_cast<float>(mouse.x), static_cast<float>(mouse.y)};
                add_cursor_data_flag = plot_window.is_cursor_data &&
                                       ImPlot::IsPlotHovered()    &&
                                       ImGui::IsMouseClicked(ImGuiMouseButton_Left);

                for(LineData& line : plot_window.lines)
                {
                    line_name = line.name.c_str();

                    ImPlot::PlotLine(line_name, line.x.data(), line.z.data(), line.x.size());

                    item = ImPlot::GetItem(line_name);
                    if(item != nullptr && !item->Show) continue;

                    cand_cursor_data = find_closest_point(mouse_pos, line.x, line.z);
                    if (distance(cand_cursor_data, mouse_pos) < distance(curr_cursor_data, mouse_pos))
                    {
                        curr_cursor_data = cand_cursor_data;
                        tooltip_line_id  = line_name;
                    }
                }

                if (add_cursor_data_flag)
                    plot_window.vec_tooltip.push_back({tooltip_line_id, PHASE, curr_cursor_data});

                plot_tooltip_data(plot_window.vec_tooltip, window_name,
                                  "Freq.: %.3f rad/s\nPhase: %.3f deg", PHASE);

                if(distance(ImPlot::PlotToPixels(mouse_pos), ImPlot::PlotToPixels(curr_cursor_data)) < HOVERING_DIST)
                    draw_tooltip(curr_cursor_data, "HOVER", window_name, "Freq.: %.3f rad/s\nPhase: %.3f deg", false);

                ImPlot::EndPlot();
            }

            ImPlot::EndSubplots();
        }
    }
    ImGui::End();
}
