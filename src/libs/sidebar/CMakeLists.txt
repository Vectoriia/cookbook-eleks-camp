set(SOURCES 
    sidebar.cpp
    treeitem.cpp
    treemodel.cpp
)

add_library(Sidebar STATIC ${SOURCES})

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

target_link_libraries(Sidebar PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Database)