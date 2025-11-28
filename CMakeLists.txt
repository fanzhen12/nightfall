# CMakeLists.txt
cmake_minimum_required(VERSION 3.15)
project(ProjectNightfall VERSION 0.1.0 LANGUAGES CXX)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# ===== 第三方库配置 =====

# SFML
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
add_subdirectory(external/SFML)

# spdlog
set(SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(external/spdlog)

# nlohmann_json
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
add_subdirectory(external/nlohmann_json)

# EnTT (header-only)
set(ENTT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/entt/src)

# 收集所有源文件
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

file(GLOB_RECURSE HEADERS 
    "src/*.h"
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 链接所有库
target_link_libraries(${PROJECT_NAME} 
    sfml-graphics 
    sfml-window 
    sfml-system 
    sfml-audio
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    ${ENTT_INCLUDE_DIR}
)

# 复制资源文件到构建目录
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Windows 特殊处理：复制 SFML DLL 到可执行文件目录
if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE:sfml-audio>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()
