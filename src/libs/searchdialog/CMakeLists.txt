add_library(SearchDialog  STATIC
    searchdialog.cpp
)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

target_link_libraries(SearchDialog PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Database)