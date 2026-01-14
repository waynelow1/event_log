
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 6.9 REQUIRED COMPONENTS Core Sql)

add_library (EventLogWidget STATIC
    eventlogwidget.cpp
    eventlogwidget.h
    eventlogwidget.ui
    EventFilterProxy.h
    DisplayViewDelegate.h
)


target_link_libraries(EventLogWidget PUBLIC
    Qt::Core
    Qt::Widgets
    Qt::Sql
)
target_link_libraries(EventLogWidget PRIVATE Qt6::Core)

# required to include the header for the custom widget in bin build directory
target_include_directories(EventLogWidget PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
