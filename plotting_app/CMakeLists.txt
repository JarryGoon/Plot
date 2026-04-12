file(GLOB_RECURSE APP_SRC    src/*.cpp)
file(GLOB_RECURSE APP_HEADER include/*.hpp)

add_executable(Plotting_App main_plotapp.cpp ../include/PlotHeader.hpp ${APP_SRC} ${APP_HEADER})
target_link_libraries(Plotting_App PRIVATE ImPlot zmq)
target_include_directories(Plotting_App PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
                           ${CMAKE_CURRENT_BINARY_DIR}/../../include
)
