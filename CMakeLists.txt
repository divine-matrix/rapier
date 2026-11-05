cmake_minimum_required(VERSION 3.21)
project(rapier VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

option(RAPIER_USE_OPENGL "Enable OpenGL support" ON)

find_package(SDL3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
if(RAPIER_USE_OPENGL)
  find_package(OpenGL REQUIRED)
endif()

add_library(rapier)
add_library(rapier::rapier ALIAS rapier)
add_subdirectory(src)

target_include_directories(rapier PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(rapier PUBLIC cxx_std_17)
target_link_libraries(rapier PUBLIC SDL3::SDL3 imgui::imgui)

if(RAPIER_USE_OPENGL)
  target_link_libraries(rapier PUBLIC OpenGL::GL)
  target_compile_definitions(rapier PUBLIC RAPIER_HAS_OPENGL)
endif()

# ---- build examples ----
option(RAPIER_BUILD_EXAMPLES "Build examples" ${PROJECT_IS_TOP_LEVEL})
if(RAPIER_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# ---- install / find_package support (only when top-level) ----
if(PROJECT_IS_TOP_LEVEL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(TARGETS rapier EXPORT rapierTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  install(EXPORT rapierTargets
    NAMESPACE rapier::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rapier)

  configure_package_config_file(
    cmake/rapierConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/rapierConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rapier)
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/rapierConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/rapierConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/rapierConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rapier)
endif()
