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

set_target_properties(mytetra PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE # rpmlint wants PIE
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/mytetra/" # for search qtSingleApplication
)

target_compile_definitions(mytetra PRIVATE APPLICATION_VERSION="${APP_VERSION}")

set(TS_FILES "../bin/resource/translations/mytetra_fr.ts" "../bin/resource/translations/mytetra_ru.ts")

if(USE_QT6)
    # require Qt 6.2
    qt_add_translations(mytetra TS_FILES ${TS_FILES} RESOURCE_PREFIX "resource/translations")
else()
    # mask deprecation QLabel.pixmap(). In Qt6 return by value works again.
    target_compile_definitions(mytetra PRIVATE "QT_DISABLE_DEPRECATED_BEFORE=0x050F00")

    configure_file("../bin/translations.qrc" ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
    qt5_create_translation(QM_FILES ${SRC_1} ${SRC_2} ${TS_FILES})
    target_sources(mytetra PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/translations.qrc" ${QM_FILES})
endif()


if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(TARGETS mytetra DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()
