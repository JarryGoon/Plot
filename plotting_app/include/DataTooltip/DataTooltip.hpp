//
// Created by jarrygoon on 260403.
//

#ifndef PLOT_DATATOOLTIP_HPP
#define PLOT_DATATOOLTIP_HPP

#include "PlottingType.hpp"

typedef enum CursorAction {
    None,
    DeleteCurrent,
    ClearAll
} CursorAction;

ImVec2 find_closest_point(ImVec2 mouse, const std::vector<double> &x_data, const std::vector<double> &y_data);

CursorAction draw_tooltip(const ImVec2 &cursor, const std::string &id, const std::string &window_name,
                          const char* print_format, bool is_fixed);

void plot_tooltip_data(std::vector<TooltipData> &cursor_data, const std::string &window_name,
                       const char* print_format, ToolTipType print_type = DEFAULT);

#endif //PLOT_DATATOOLTIP_HPP