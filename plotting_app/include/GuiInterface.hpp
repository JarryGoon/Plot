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

#ifndef PLOT_GUIINTERFACE_HPP
#define PLOT_GUIINTERFACE_HPP

#include <SDL3/SDL.h>

void init_ImPlot(SDL_GLContext* gl_context, SDL_Window** window);

bool event_polling(SDL_Window* window);

void new_frame();

void renderer(SDL_Window* window);

void clear_ImPlot(const SDL_GLContext* gl_context, SDL_Window* window);

#endif //PLOT_GUIINTERFACE_HPP