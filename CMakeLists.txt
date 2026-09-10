cmake_minimum_required(VERSION 3.15)

project(GlDrawStuff)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Testing
# Feel free to modify this to make it work on your system
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    enable_testing()


    set(VENDOR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vendor" CACHE PATH "Path to vendor")
    set(GLFW_ROOT "" CACHE PATH "Path to GLFW root")
    set(GLM_ROOT "" CACHE PATH "Path to GLM root")
    set(STB_ROOT "" CACHE PATH "Path to STB root")


    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${GLM_ROOT})

    add_library(glad STATIC ${VENDOR_PATH}/glad/src/glad.c)
    target_include_directories(glad PUBLIC ${VENDOR_PATH}/glad/include)

    add_library(glfw STATIC IMPORTED)
    target_include_directories(glfw INTERFACE ${GLFW_ROOT}/include)

    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${STB_ROOT})



    if(WIN32)
        set_target_properties(glfw PROPERTIES IMPORTED_LOCATION "${GLFW_ROOT}/build/src/glfw3.lib")
    else()
        set_target_properties(glfw PROPERTIES IMPORTED_LOCATION "${GLFW_ROOT}/build/src/libglfw3.a")
    endif()


    find_package(Threads REQUIRED)
    if(NOT WIN32)
        find_package(OpenGL REQUIRED)
        target_link_libraries(glfw INTERFACE ${CMAKE_DL_LIBS} Threads::Threads OpenGL::GL)
    endif()
   

    target_link_libraries(${PROJECT_NAME} INTERFACE
        glfw
        glad
        glm
        stb
    )


    add_executable(unit_tests
        tests/main.cpp
    )

    target_link_libraries(unit_tests PRIVATE ${PROJECT_NAME})

    add_test(NAME RunUnitTests COMMAND unit_tests)
endif()


