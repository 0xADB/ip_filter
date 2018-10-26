cmake_minimum_required(VERSION 3.2)

project(ip_filter VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

configure_file(version.h.in autoversion.h)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

add_executable(ip_filter main.cpp)
add_library(ipfilter ip_filter.cpp)
add_executable(test_ip_filter test_main.cpp)

set_target_properties(ip_filter ipfilter test_ip_filter PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-Weffc++"
  )

set_target_properties(test_ip_filter PROPERTIES
  COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
  INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR};${CMAKE_CURRENT_SOURCE_DIR}/src"
  )
  
  get_target_property(compile_incs test_ip_filter INCLUDE_DIRECTORIES)
  message(STATUS "includes: ${compile_incs}")

if (IP_FILTER_BENCH)
  target_compile_definitions(test_ip_filter PRIVATE IP_FILTER_BENCH)
endif()

target_link_libraries(
  ip_filter
  ipfilter
  )

target_link_libraries(
  test_ip_filter
  ipfilter
  ${Boost_LIBRARIES}
  )

configure_file(test_data.tsv ${CMAKE_BINARY_DIR}/test_data.tsv)

install(TARGETS ip_filter RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

message("-- CPACK_PACKAGE_VERSION: " ${CPACK_PACKAGE_VERSION})

set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_PACKAGE_CONTACT "ad.beregovoy@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An IPv4 processing utility")
set(CPACK_PACKAGE_DESCRIPTION "An utility that prints input IPv4 address list in reverse lexicographical order and then prints addresses that meet certain condirions")

#set(CPACK_SOURCE_IGNORE_FILES "\\.travis\\.yml;\\.git;*\\.swp")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_DEBIAN_PACKAGE_SECTION "Development")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Anton Beregovoy <${CPACK_PACKAGE_CONTACT}>")

include(CPack)

enable_testing()

add_test(ip_filter_tests test_ip_filter)
