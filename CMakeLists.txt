cmake_minimum_required(VERSION 3.5)

project(SilverDict VERSION 0.1.0 LANGUAGES CXX)

set(APPLICATION_NAME "SilverDict Desktop")
set(APPLICATION_ID "com.gmail.blandilyte.silverdict.qt")
set(EXECUTABLE_NAME sddesktop)

set(CPACK_PACKAGE_VENDOR ${APPLICATION_NAME})
set(CPACK_PACKAGE_EXECUTABLES ${EXECUTABLE_NAME} ${APPLICATION_NAME})
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${APPLICATION_NAME})
set(CPACK_PACKAGE_CONTACT blandilyte@gmail.com)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/COPYING)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets LinguistTools Network Concurrent WebEngineWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Network Concurrent WebEngineWidgets)

set(TS_FILES
	data/translations/${PROJECT_NAME}_en_GB.ts
)

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${PROJECT_BINARY_DIR}/translations)

qt_add_executable(SilverDict
	MANUAL_FINALIZATION
	src/main.cc
	src/runguard.h
	src/runguard.cc
	src/mainwindow.cc
	src/mainwindow.h
	src/mainwindow.ui
	src/remote/models.h
	src/remote/models.cc
	src/remote/remoterepository.cc
	src/remote/remoterepository.h
	src/preferences/preferences.h
	src/preferences/preferences.cc
	src/preferences/preferencesdialogue.h
	src/preferences/preferencesdialogue.cc
	src/preferences/preferencesdialogue.ui
	src/query/queryscreen.h
	src/query/queryscreen.cc
	src/query/queryscreen.ui
	src/query/wordlistmodel.h
	src/query/wordlistmodel.cc
	src/query/articleview.h
	src/query/articleview.cc
	src/query/articleview.ui
	src/edit/dictionarydialog.h
	src/edit/dictionarydialog.cc
	src/edit/dictionarydialog.ui
	src/edit/adddictionarydialog.h
	src/edit/adddictionarydialog.cc
	src/edit/adddictionarydialog.ui
	${QM_FILES}
)

qt_add_lupdate(SilverDict TS_FILES ${TS_FILES})
qt_add_lrelease(SilverDict TS_FILES ${TS_FILES})

target_include_directories(SilverDict PRIVATE
	src
)

target_link_libraries(SilverDict PRIVATE
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::Concurrent
	Qt${QT_VERSION_MAJOR}::WebEngineWidgets
)

set_target_properties(SilverDict PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER ${APPLICATION_ID}
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)

install(TARGETS SilverDict
	BUNDLE DESTINATION .
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

ADD_DEFINITIONS(-DTRANSLATIONS_PATH="${CMAKE_INSTALL_FULL_DATAROOTDIR}/translations")

install(DIRECTORY "${PROJECT_BINARY_DIR}/translations" DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}")

qt_finalize_executable(SilverDict)
