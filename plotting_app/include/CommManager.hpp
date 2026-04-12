//
// Created by jarrygoon on 260329.
//

#ifndef CONTROL_LIBRARY_COMMMANAGER_HPP
#define CONTROL_LIBRARY_COMMMANAGER_HPP

#include <map>

#include "PlottingType.hpp"

void init_comm_manager();

void send_comm_state();

PlotType receive_plot_data(std::map<std::string, PlotWindow>* plot_windows);

void clear_comm_manager();

#endif //CONTROL_LIBRARY_COMMMANAGER_HPP