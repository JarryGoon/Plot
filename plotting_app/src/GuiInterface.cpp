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

#include "GuiInterface.hpp"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_opengl3_loader.h>

#include <implot.h>

void init_ImPlot(SDL_GLContext* gl_context, SDL_Window** window)
{
    SDL_WindowFlags window_flags;

    // ============================================================================================================== //
    // 1. SDL3 & OpenGL Initialize
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
    // 2. ImGui & ImPlot Initialize
    // ============================================================================================================== //

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui_ImplSDL3_InitForOpenGL(*window, *gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    // ============================================================================================================== //
    // 3. Set Color
    // ============================================================================================================== //

    ImGui::StyleColorsLight();

    ImPlotStyle& style = ImPlot::GetStyle();
    style.Colors[ImPlotCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImPlotCol_PlotBg]  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    style.Colors[ImPlotCol_AxisText] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImPlotCol_AxisGrid] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
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
