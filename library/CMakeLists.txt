add_library(VKSP_LIBRARIES INTERFACE)

# SPIRV-Reflect
if (NOT TARGET spirv-reflect-static)
    message(STATUS "\nLoading SPIRV-Reflect (spirv-reflect-static)")
    OPTION(SPIRV_REFLECT_EXECUTABLE     "Build spirv-reflect executable" OFF)
    OPTION(SPIRV_REFLECT_EXAMPLES       "Build stripper examples" OFF)
    OPTION(SPIRV_REFLECT_STRIPPER       "Build stripper utility" OFF)
    OPTION(SPIRV_REFLECT_STATIC_LIB     "Build a SPIRV-Reflect static library" ON)
    OPTION(SPIRV_REFLECT_BUILD_TESTS    "Build the SPIRV-Reflect test suite" OFF)
    OPTION(SPIRV_REFLECT_ENABLE_ASSERTS "Enable asserts for debugging" OFF)
    add_subdirectory(SPIRV-Reflect-2023.4.8)

    target_link_libraries(VKSP_LIBRARIES INTERFACE spirv-reflect-static)
endif()