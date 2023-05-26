cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

project("VKSP"
    LANGUAGES CXX
    VERSION 1.0.0
    HOMEPAGE_URL "")
set(VKSP_EXECUTABLE_NAME "vksp")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS OFF) # Vendor-specific Extensions

# [SECTION]: Options

# [SECTION]: Project
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
file(GLOB_RECURSE VKSP_MODULE_INTERFACES "VKSP/*.ixx")
file(GLOB_RECURSE VKSP_MODULE_IMPLEMENTS "VKSP/*.cpp")

source_group(TREE
    ${CMAKE_CURRENT_SOURCE_DIR}
    FILES 
    ${VKSP_MODULE_INTERFACES} 
    ${VKSP_MODULE_IMPLEMENTS})

add_subdirectory(library)
add_executable( ${VKSP_EXECUTABLE_NAME} 
                ${VKSP_MODULE_INTERFACES} 
                ${VKSP_MODULE_IMPLEMENTS})
target_link_libraries(${VKSP_EXECUTABLE_NAME} PRIVATE VKSP_LIBRARIES)