#ifndef CONTROL_LIBRARY_PLOT_HPP
#define CONTROL_LIBRARY_PLOT_HPP

#include <vector>

void plot(const std::vector<double> &x, const std::vector<double> &y,
          const char* win_name = ""   , const char* line_name = "");

void plotting_app_bode(const std::vector<double> &freq, const std::vector<double> &mag, const std::vector<double> &phase);

#endif //CONTROL_LIBRARY_PLOT_HPP
