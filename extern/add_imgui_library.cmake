if(CMAKE_VERSION VERSION_GREATER 3.12)
        add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD=1)
else()
    add_definitions("-DIMGUI_IMPL_OPENGL_LOADER_GLAD=1")
endif()

add_library(
        imgui STATIC
# Dear ImGui Base Code
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imconfig.h
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_demo.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_draw.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_internal.h
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_tables.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui_widgets.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imgui.h
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imstb_rectpack.h
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imstb_textedit.h
        ${CMAKE_CURRENT_LIST_DIR}/imgui/imstb_truetype.h

# Dear ImGui Extensions for GLFW and OpenGL 3
        ${CMAKE_CURRENT_LIST_DIR}/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui/backends/imgui_impl_glfw.h
        ${CMAKE_CURRENT_LIST_DIR}/imgui/backends/imgui_impl_opengl3.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui/backends/imgui_impl_opengl3.h
)



target_link_libraries(imgui PRIVATE glfw glad)


include_directories(${CMAKE_CURRENT_LIST_DIR}/imgui)