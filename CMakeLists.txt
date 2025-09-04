cmake_minimum_required(VERSION 3.22.0)
project(lahzam
VERSION 0.0.1
HOMEPAGE_URL "https://github.com/ZXShady/lahzam"
DESCRIPTION  "Fast macro-free aggregate reflection C++ library"
LANGUAGES CXX)

if(POLICY CMP0128)
    cmake_policy(SET CMP0128 NEW)
endif()

set(INCDIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
file(GLOB_RECURSE INCS "${INCDIR}/*.hpp")

add_library(lahzam INTERFACE ${INCS})
add_library(lahzam::lahzam ALIAS lahzam)
target_compile_features(lahzam INTERFACE cxx_std_20)
 
target_include_directories(lahzam INTERFACE
    $<BUILD_INTERFACE:${INCDIR}>
    $<INSTALL_INTERFACE:include>
)

option(LAHZAM_BUILD_TESTS "Enable tests for this `lahzam` library" OFF)

if(LAHZAM_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

install(TARGETS lahzam
    EXPORT lahzamTargets
)

include(GNUInstallDirs)

install(DIRECTORY ${INCDIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT lahzamTargets
    NAMESPACE lahzam::
    DESTINATION ${CMAKE_INSTALL_DATADIR}/lahzam/cmake
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/lahzamConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lahzamConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/lahzamConfig.cmake"
    INSTALL_DESTINATION cmake
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/lahzamConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/lahzamConfigVersion.cmake"
    DESTINATION cmake
)

export(
    TARGETS lahzam
    NAMESPACE lahzam::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/lahzamTargets.cmake"
)