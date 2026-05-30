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

#ifndef CONTROL_LIBRARY_COMMMANAGER_HPP
#define CONTROL_LIBRARY_COMMMANAGER_HPP

#include <map>

#include "PlotWindow.hpp"

void init_comm_manager();

void send_comm_state();

PlotType receive_plot_data(std::map<std::string, PlotWindow>* plot_windows);

void clear_comm_manager();

#endif //CONTROL_LIBRARY_COMMMANAGER_HPP