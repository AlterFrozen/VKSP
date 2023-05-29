cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

project("VKSP"
    LANGUAGES CXX
    VERSION 1.0.0
    HOMEPAGE_URL "https://github.com/AlterFrozen/VKSP")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Explicitly require the standard
set(CMAKE_CXX_EXTENSIONS OFF) # Vendor-specific Extensions

# [SECTION]: Options
option(VKSP_EXECUTABLE      "Compile VKSP (Byte shader and Vulkan Pipeline infomation)" ON)
option(VKSP_EXECUTABLE_LITE "Compile VKSP Lite version (Only byte shader)"              ON)

# [SECTION]: Project
if(VKSP_EXECUTABLE)
    add_subdirectory(library)
    add_executable( "vksp"
                    "VKSP/vksp_utils.h"
                    "VKSP/vksp.cc")
    target_link_libraries("vksp" PRIVATE VKSP_LIBRARIES)
endif(VKSP_EXECUTABLE)

if (VKSP_EXECUTABLE_LITE)
    set(VKSP_EXECUTABLE_NAME "vksp-lite")
    add_executable( "vksp-lite" 
                    "VKSP/vksp_utils.h" 
                    "VKSP/vksp-lite.cc")
endif(VKSP_EXECUTABLE_LITE)

