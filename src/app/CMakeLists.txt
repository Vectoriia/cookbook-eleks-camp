add_executable(CookBook main.cpp)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Gui REQUIRED)

target_link_libraries(CookBook PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Ui SearchDialog Sidebar Database CalculateDialog ReceiptView CreateReceiptView DownloadDialog pthread)


set_target_properties(CookBook PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)
install(TARGETS CookBook DESTINATION app COMPONENT binaries)

