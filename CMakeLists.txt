cmake_minimum_required(VERSION 3.16)
project(minispot)

set(CMAKE_CXX_STANDARD 20)

add_executable(minispot WIN32
	src/main.cpp
	src/auth/auth.cpp
	src/utils/utils.cpp
	src/webview/webview.cpp
	src/webview/async/asyncwv2.cpp
	resources.rc
)

set_source_files_properties(resources.rc
	PROPERTIES OBJECT_DEPENDS "${CMAKE_SOURCE_DIR}/web/dist/index.html"
)

target_include_directories(minispot
	PRIVATE ${CMAKE_SOURCE_DIR}/WebView2/build/native/include
)

target_link_libraries(minispot
	PRIVATE
		user32.lib
		ole32.lib
		advapi32.lib
		Shlwapi.lib
		dwmapi.lib
		winhttp.lib
		crypt32.lib
		windowsapp.lib
		${CMAKE_SOURCE_DIR}/WebView2/build/native/x64/WebView2LoaderStatic.lib
)
