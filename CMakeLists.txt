cmake_minimum_required(VERSION 3.21)
project(sabre VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

option(sabre_USE_OPENGL "Enable OpenGL support" ON)

find_package(SDL3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
if(sabre_USE_OPENGL)
  find_package(OpenGL REQUIRED)
endif()

add_library(sabre)
add_library(sabre::sabre ALIAS sabre)
add_subdirectory(src)

target_include_directories(sabre PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(sabre PUBLIC cxx_std_17)
target_link_libraries(sabre PUBLIC SDL3::SDL3 imgui::imgui)

if(sabre_USE_OPENGL)
  target_link_libraries(sabre PUBLIC OpenGL::GL)
  target_compile_definitions(sabre PUBLIC sabre_HAS_OPENGL)
endif()

# ---- build examples ----
option(sabre_BUILD_EXAMPLES "Build examples" ${PROJECT_IS_TOP_LEVEL})
if(sabre_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# ---- install / find_package support (only when top-level) ----
if(PROJECT_IS_TOP_LEVEL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(TARGETS sabre EXPORT sabreTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  install(EXPORT sabreTargets
    NAMESPACE sabre::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sabre)

  configure_package_config_file(
    cmake/sabreConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/sabreConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sabre)
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/sabreConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

  install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/sabreConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/sabreConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sabre)
endif()
