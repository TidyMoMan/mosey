cmake_minimum_required(VERSION 3.20)
project(talon_controller C CXX)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(Threads REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

set(PHOENIX_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/arm64/)

message(STATUS "Phoenix libs dir: ${PHOENIX_LIBS_DIR}")
file(GLOB PHOENIX_FOUND_LIBS "${PHOENIX_LIBS_DIR}/*.so")
message(STATUS "Found .so files: ${PHOENIX_FOUND_LIBS}")

find_library(PHOENIX_LIB
    NAMES CTRE_Phoenix
    PATHS ${PHOENIX_LIBS_DIR}
    NO_DEFAULT_PATH
    REQUIRED)

find_library(PHOENIX_CCI_LIB
    NAMES CTRE_PhoenixCCI
    PATHS ${PHOENIX_LIBS_DIR}
    NO_DEFAULT_PATH
    REQUIRED)

find_library(PHOENIX_TOOLS_LIB
    NAMES CTRE_PhoenixTools
    PATHS ${PHOENIX_LIBS_DIR}
    NO_DEFAULT_PATH
    REQUIRED)

message(STATUS "Phoenix lib:       ${PHOENIX_LIB}")
message(STATUS "PhoenixCCI lib:    ${PHOENIX_CCI_LIB}")
message(STATUS "PhoenixTools lib:  ${PHOENIX_TOOLS_LIB}")

add_library(Phoenix      SHARED IMPORTED GLOBAL)
add_library(PhoenixCCI   SHARED IMPORTED GLOBAL)
add_library(PhoenixTools SHARED IMPORTED GLOBAL)

set_target_properties(Phoenix PROPERTIES
    IMPORTED_LOCATION ${PHOENIX_LIB}
    IMPORTED_NO_SONAME TRUE)
set_target_properties(PhoenixCCI PROPERTIES
    IMPORTED_LOCATION ${PHOENIX_CCI_LIB}
    IMPORTED_NO_SONAME TRUE)
set_target_properties(PhoenixTools PROPERTIES
    IMPORTED_LOCATION ${PHOENIX_TOOLS_LIB}
    IMPORTED_NO_SONAME TRUE)

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(talon_controller src/talon_controller.cpp)

target_include_directories(talon_controller PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(talon_controller PUBLIC
    c_std_99
    cxx_std_20)

target_link_libraries(talon_controller
    Phoenix
    PhoenixCCI
    PhoenixTools
    rclcpp::rclcpp
    std_msgs::std_msgs
    sensor_msgs::sensor_msgs
    Threads::Threads)

target_link_options(talon_controller PRIVATE
    "-Wl,-rpath,$ORIGIN")
# ── Copy libs to binary dir for local run/test ────────────────────────────────
add_custom_command(TARGET talon_controller POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PHOENIX_LIBS_DIR}
        $<TARGET_FILE_DIR:talon_controller>)

set_target_properties(talon_controller PROPERTIES
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN")

# ── Install ───────────────────────────────────────────────────────────────────
install(TARGETS talon_controller
    DESTINATION lib/${PROJECT_NAME})

install(FILES
    ${PHOENIX_LIBS_DIR}/libCTRE_Phoenix.so
    ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixCCI.so
    ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixTools.so
    DESTINATION lib/${PROJECT_NAME})


# ── Testing ───────────────────────────────────────────────────────────────────
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()
#basically pure evil linker machinations because the libraries depend on each other
ament_environment_hooks("${CMAKE_CURRENT_SOURCE_DIR}/env-hooks/phoenix_libs.dsv.in")

ament_package()
