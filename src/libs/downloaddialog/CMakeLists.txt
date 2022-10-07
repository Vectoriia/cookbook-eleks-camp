add_library(DownloadDialog  STATIC
    dialog.cpp
    listmodel.cpp
    dialog.ui
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Concurrent REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Concurrent REQUIRED)

target_link_libraries(DownloadDialog PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Concurrent) 