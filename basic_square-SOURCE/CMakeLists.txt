cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Please replace "app" in the whole file with your desirec name
project(basic_square)

option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)

# Add this right after the option declaration
message(STATUS "WITH_GEANT4_UIVIS = ${WITH_GEANT4_UIVIS}")

find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

add_definitions(-DG4UI_USE)
add_definitions(-DG4VIS_USE)

# Diagnostic output
message(STATUS "Geant4_INCLUDE_DIRS: ${Geant4_INCLUDE_DIRS}")
message(STATUS "Geant4_DEFINITIONS: ${Geant4_DEFINITIONS}")
message(STATUS "Geant4_UI_DRIVERS: ${Geant4_UI_DRIVERS}")
message(STATUS "Geant4_VIS_DRIVERS: ${Geant4_VIS_DRIVERS}")
message(STATUS "Geant4_USE_FILE: ${Geant4_USE_FILE}")

include_directories(${PROJECT_SOURCE_DIR}/include)

# We presume the existence of three directories
file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB_RECURSE macros RELATIVE ${PROJECT_SOURCE_DIR} macro/*.mac)

# Enable macros for out-of-source build
foreach(_file ${macros})
  configure_file(
    ${_file}
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
    )
endforeach()

add_executable(basic_square main.cc ${sources} ${headers})
target_link_libraries(basic_square ${Geant4_LIBRARIES})

install(TARGETS basic_square DESTINATION bin)
