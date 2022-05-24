add_subdirectory("libraries/qtSingleApplication")

file(GLOB SRC_1 "libraries/*.cpp")
file(GLOB_RECURSE SRC_2
    "controllers/*.cpp"
    "libraries/crypt/*.cpp"
    "libraries/helpers/*.cpp"
    "libraries/wyedit/*.cpp"
    "models/*.cpp"
    "views/*.cpp"
)

add_executable(mytetra "main.cpp" "../bin/mytetra.qrc" "../bin/icons.qrc" ${SRC_1} ${SRC_2})

target_include_directories(mytetra PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(mytetra PRIVATE qtsingleapplication ${QT_LIBS})
