file(GLOB_RECURSE IMPLOT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE IMPLOT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(ImPlot STATIC ${IMPLOT_SRC} ${IMPLOT_INCLUDE})
target_include_directories(ImPlot PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/ImGui
                           ${CMAKE_CURRENT_SOURCE_DIR}
)

if(WIN32)
    target_link_libraries(ImPlot PUBLIC glfw3 opengl32 gdi32)
elseif(UNIX)
    target_link_libraries(ImPlot PUBLIC SDL3 GL m)
endif()
