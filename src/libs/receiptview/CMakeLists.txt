add_library(ReceiptView  STATIC
    receiptview.cpp
    receiptview.ui
)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

target_link_libraries(ReceiptView PRIVATE Qt${QT_VERSION_MAJOR}::Widgets CalculateDialog ViewWidget)
