add_executable(mimetex "gifsave.c" "mimetex.c")

target_compile_definitions(mimetex PRIVATE "AA") # turn on anti-aliasing
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(mimetex PRIVATE "/wd4267" "/wd4996")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(mimetex PRIVATE "-Wno-unused-parameter" "-Wno-sign-compare")
    target_link_libraries(mimetex PRIVATE "m")
endif()
