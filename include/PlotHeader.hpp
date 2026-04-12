//
// Created by jarrygoon on 260324.
//

#ifndef CONTROL_LIBRARY_PLOTHEADER_H
#define CONTROL_LIBRARY_PLOTHEADER_H

#include <stdint.h>

enum PlotType
{
    NONE,
    EXIT_APP,
    PLOT_2D,
    BODE
};

struct PlotHeader
{
    uint32_t data_size;
    PlotType flags;
    char     window_name[256];
    char     line_name[256];
};

#endif //CONTROL_LIBRARY_PLOTHEADER_H