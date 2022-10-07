set(UI_FORMS 
    mainwindow.ui
)

add_library(Ui STATIC
    mainwindow.cpp
    ${UI_FORMS}
)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

target_link_libraries(Ui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets )
