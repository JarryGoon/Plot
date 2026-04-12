//
// Created by jarrygoon on 260406.
//

#ifndef PLOT_TOOLTIPFUNC_HPP
#define PLOT_TOOLTIPFUNC_HPP

#include <string>
#include <implot.h>

#define TOOLTIP_TEXT_BUFFER_SIZE 128

typedef enum TooltipPos
{
    NoUpdate,
    TopRight,
    TopLeft,
    BottomRight,
    BottomLeft,
} TooltipPos;

void draw_datapoint(const ImVec2 &cursor, const std::string &id, bool is_fixed);

void init_tooltip_window(const ImVec2 &tooltip_pos,
                         ImVec2        tooltip_win_size,
                         bool          is_fixed,
                         TooltipPos    tooltip_pos_type);

TooltipPos drag_tooltip(const std::string &tooltip_win_id, const ImVec2 &tooltip_pix_pos);

#endif //PLOT_TOOLTIPFUNC_HPP