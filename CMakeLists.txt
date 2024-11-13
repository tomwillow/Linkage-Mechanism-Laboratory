# for FetchContent
cmake_minimum_required (VERSION 3.11 FATAL_ERROR)

set(PROJECT_NAME LinkageMechanismLaboratory)
project(${PROJECT_NAME})

# C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE -D_UNICODE)
#add_compile_options(/W4 /WX)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

file(GLOB SOURCE_CODE
    *.cpp
    *.h
    *.rc
    Common/*.h
    Common/*.cpp
    Control/*.h
    Control/*.cpp
    Dialog/*.h
    Dialog/*.cpp
    Draw/*.h
    Draw/*.cpp
    Element/*.h
    Element/*.cpp
    gifenc/*.h
    gifenc/*.c
    gifenc/*.cpp
    Solver/*.h
    Solver/*.cpp
    Tool/*.h
    Tool/*.cpp
    )

add_executable(LinkageMechanismLaboratory ${SOURCE_CODE})

target_include_directories(LinkageMechanismLaboratory PRIVATE
	Common
	Control
	Dialog
	Draw
	Element
	gifenc
	Solver
	Tool
	)

set_target_properties(LinkageMechanismLaboratory PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")