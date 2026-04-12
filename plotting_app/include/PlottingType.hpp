//
// Created by jarrygoon on 260325.
//

#ifndef CONTROL_LIBRARY_PLOTTINGTYPE_HPP
#define CONTROL_LIBRARY_PLOTTINGTYPE_HPP

#include <vector>
#include <string>

#include <imgui.h>

#include "PlotHeader.hpp"

enum ToolTipType
{
    DEFAULT,
    MAGNITUDE,
    PHASE
};

struct LineData
{
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::string         name;
};

struct TooltipData
{
    std::string line_id;
    ToolTipType tooltip_type;
    ImVec2      tooltip_data;
};

struct PlotWindow
{
    std::vector<LineData> lines;
    PlotType              plot_type = NONE;
    bool                  is_open   = true;

    bool                     is_cursor_data = false;
    std::vector<TooltipData> vec_tooltip;
};

#endif //CONTROL_LIBRARY_PLOTTINGTYPE_HPP