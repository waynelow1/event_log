cmake_minimum_required(VERSION 3.19)
project(EventLogTestApp LANGUAGES CXX)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(EventLogTestApp
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    eventsimulator.h eventsimulator.cpp
)

target_link_libraries(EventLogTestApp
    PRIVATE
        Qt::Core
        Qt::Widgets
        EventLogWidget
)
target_link_libraries(EventLogTestApp PRIVATE Qt6::Core)

add_subdirectory(../EventLogWidget/ ${CMAKE_BINARY_DIR}/eventlogwidget_build)

include(GNUInstallDirs)

install(TARGETS EventLogTestApp
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET EventLogTestApp
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
