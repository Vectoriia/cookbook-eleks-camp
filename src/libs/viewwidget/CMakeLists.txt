add_library(ViewWidget  STATIC
    viewwidget.cpp
)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

target_link_libraries(ViewWidget PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ReceiptView CreateReceiptView)
