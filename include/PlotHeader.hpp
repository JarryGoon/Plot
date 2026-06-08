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

#ifndef CONTROL_LIBRARY_PLOTHEADER_H
#define CONTROL_LIBRARY_PLOTHEADER_H

#include <stdint.h>

enum PlotType
{
    NONE,
    EXIT_APP,
    PLOT_2D,
    PLOT_LIVE,
    BODE,
    PZMAP
};

struct PlotHeader
{
    char     window_name[256];
    char     plot_name[256];
    char     line_name[256];

    PlotType flags;

    uint32_t subplot_x;
    uint32_t subplot_y;
    uint32_t subplot_idx;

    size_t num_x;
    size_t num_y;
    size_t num_z;
};

#endif //CONTROL_LIBRARY_PLOTHEADER_H