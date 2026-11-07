set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)

function(sabre_add_example name)
  add_executable(example_${name} ${ARGN})
  set_target_properties(example_${name} PROPERTIES OUTPUT_NAME ${name})
  target_link_libraries(example_${name} PRIVATE sabre::sabre)
endfunction()

# Single-file examples: examples/foo.cpp -> "foo"
file(GLOB single_files CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(src IN LISTS single_files)
  get_filename_component(name ${src} NAME_WE)
  sabre_add_example(${name} ${src})
endforeach()

# Multi-file examples: examples/bar/*.cpp -> "bar"
file(GLOB entries RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(entry IN LISTS entries)
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${entry})
    file(GLOB_RECURSE srcs CONFIGURE_DEPENDS
         ${CMAKE_CURRENT_SOURCE_DIR}/${entry}/*.cpp)
    if(srcs)
      sabre_add_example(${entry} ${srcs})
      target_include_directories(example_${entry} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/${entry})
    endif()
  endif()
endforeach()
