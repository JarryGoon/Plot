//
// Created by jarrygoon on 260325.
//

#ifndef CONTROL_LIBRARY_PLOTTINGFUNC_HPP
#define CONTROL_LIBRARY_PLOTTINGFUNC_HPP

#include <SDL3/SDL.h>

#include "PlottingType.hpp"

void init_ImPlot(SDL_GLContext* gl_context, SDL_Window** window);

bool event_polling(SDL_Window* window);

void new_frame();

void renderer(SDL_Window* window);

void clear_ImPlot(const SDL_GLContext* gl_context, SDL_Window* window);

void plot_2d(const std::string &window_name, PlotWindow &plot_window);

void bode_plot(const std::string &window_name, PlotWindow &plot_window);

#endif //CONTROL_LIBRARY_PLOTTINGFUNC_HPP