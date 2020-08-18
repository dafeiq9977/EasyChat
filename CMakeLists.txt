cmake_minimum_required (VERSION 2.6)

project("EasyChat")


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc")  # deal with MSVC Warning C4530
ENDIF(MSVC)

message("info: CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message("info: PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
message("info: CMAKE_THREAD_LIBS_INIT=${CMAKE_THREAD_LIBS_INIT}")

add_executable( ${PROJECT_NAME}
	src/Client.cpp
	src/EasyChat.cpp
	src/FileUtils.cpp
	src/MsgInterpreter.cpp
	src/Server.cpp
	src/stdafx.cpp
)

target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
)

target_link_libraries( ${PROJECT_NAME}   
		PRIVATE 
			${WS2_LIBRARY} 

)