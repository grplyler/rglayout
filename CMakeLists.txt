cmake_minimum_required(VERSION 3.16)

project(rglayout C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(vendor/raylib)

# Sidebar demo executable
add_executable(3panel-demo examples/3panel.c)
target_include_directories(3panel-demo PRIVATE vendor/raylib/src)
target_include_directories(3panel-demo PRIVATE vendor/raygui/src)
target_link_libraries(3panel-demo raylib)

# Toolbar demo executable
add_executable(nested-demo examples/nested.c)
target_include_directories(nested-demo PRIVATE vendor/raylib/src)
target_include_directories(nested-demo PRIVATE vendor/raygui/src)
target_link_libraries(nested-demo raylib)

add_executable(form-demo examples/form.c)
target_include_directories(form-demo PRIVATE vendor/raylib/src)
target_include_directories(form-demo PRIVATE vendor/raygui/src)
target_link_libraries(form-demo raylib)
