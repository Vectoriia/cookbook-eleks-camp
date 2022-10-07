add_library(CalculateDialog  STATIC
    calculatedialog.cpp
    calculatedialog.ui
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

target_link_libraries(CalculateDialog PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
