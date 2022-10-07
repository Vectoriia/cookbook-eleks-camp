add_library(CreateReceiptView  STATIC
	createreceiptview.cpp
	createreceiptveiw.ui

)
target_include_directories(CreateReceiptView PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Gui REQUIRED)

target_link_libraries(CreateReceiptView PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui ReceiptView ViewWidget)

