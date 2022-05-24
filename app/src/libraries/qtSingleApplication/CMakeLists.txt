add_library(qtsingleapplication SHARED "qtlocalpeer.cpp" "qtsingleapplication.cpp")
target_link_libraries(qtsingleapplication PRIVATE ${QT_LIBS})
target_compile_definitions(qtsingleapplication PRIVATE QT_QTSINGLEAPPLICATION_LIBRARY)

