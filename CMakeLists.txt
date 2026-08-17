cmake_minimum_required(VERSION 3.20)

project(devclean VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DEVCLEAN_ENABLE_SANITIZERS "Build with AddressSanitizer and UBSan" OFF)
option(ENABLE_SANITIZERS "Compatibility alias for sanitizer builds" OFF)

include(GNUInstallDirs)
include(CTest)
include(cmake/DevCleanWarnings.cmake)
include(cmake/DevCleanSanitizers.cmake)
include_directories(include)

enable_testing()

find_package(nlohmann_json CONFIG REQUIRED)

file(GLOB DEVCLEAN_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.cpp")

add_library(devclean_core
    src/core/Application.cpp
    src/core/CacheInsights.cpp
    src/core/StorageBudget.cpp
    src/core/CleanupPlanner.cpp
    src/core/ArgumentParser.cpp
    src/core/CommandDispatcher.cpp
    src/core/Config.cpp
    src/core/ScanHistory.cpp
    ${DEVCLEAN_ENGINE_SOURCES}

    src/commands/AnalyzeCommand.cpp
    src/commands/DashboardCommand.cpp
    src/commands/ScanCommand.cpp
    src/commands/CleanCommand.cpp
    src/commands/DoctorCommand.cpp
    src/commands/RecommendCommand.cpp
    src/commands/StatsCommand.cpp
    src/commands/VersionCommand.cpp

    src/scanner/ScannerEngine.cpp
    src/scanner/CacheRegistry.cpp
    src/scanner/PluginLoader.cpp

    src/platform/Filesystem.cpp
    src/platform/ToolDetector.cpp

    src/cleaner/CleanEngine.cpp

    src/utils/Formatter.cpp
    src/utils/ThreadPool.cpp
    src/utils/TerminalUtils.cpp
)

target_compile_definitions(devclean_core PRIVATE DEVCLEAN_VERSION="${PROJECT_VERSION}")
target_link_libraries(devclean_core PRIVATE nlohmann_json::nlohmann_json)

add_executable(devclean src/main.cpp)
add_executable(devclean_tests tests/cli_tests.cpp)
add_executable(devclean_parser_tests tests/unit/argument_parser_tests.cpp)
add_executable(devclean_formatter_tests tests/unit/formatter_tests.cpp)
add_executable(devclean_config_tests tests/unit/config_tests.cpp)
add_executable(devclean_scanner_tests tests/unit/scanner_tests.cpp)
add_executable(devclean_scan_result_tests tests/unit/scan_result_tests.cpp)
add_executable(devclean_cache_insights_tests tests/unit/cache_insights_tests.cpp)
add_executable(devclean_doctor_tests tests/unit/doctor_tests.cpp)
add_executable(devclean_benchmark src/benchmark.cpp)

foreach(target
    devclean
    devclean_tests
    devclean_parser_tests
    devclean_formatter_tests
    devclean_config_tests
    devclean_scanner_tests
    devclean_scan_result_tests
    devclean_cache_insights_tests
    devclean_doctor_tests
    devclean_benchmark)
    target_link_libraries(${target} PRIVATE devclean_core)
    devclean_enable_warnings(${target})
endforeach()

devclean_enable_warnings(devclean_core)

add_test(NAME devclean.integration.cli COMMAND devclean_tests)
add_test(NAME devclean.unit.argument_parser COMMAND devclean_parser_tests)
add_test(NAME devclean.unit.formatter COMMAND devclean_formatter_tests)
add_test(NAME devclean.unit.config COMMAND devclean_config_tests)
add_test(NAME devclean.integration.scanner COMMAND devclean_scanner_tests)
add_test(NAME devclean.unit.scan_result COMMAND devclean_scan_result_tests)
add_test(NAME devclean.unit.cache_insights COMMAND devclean_cache_insights_tests)
add_test(NAME devclean.unit.doctor COMMAND devclean_doctor_tests)

install(TARGETS devclean devclean_benchmark RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES docs/devclean.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES scripts/devclean-completion.bash DESTINATION ${CMAKE_INSTALL_DATADIR}/bash-completion/completions RENAME devclean)

set(CPACK_PACKAGE_NAME "devclean")
set(CPACK_PACKAGE_VENDOR "devclean")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Developer cache cleaner")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "devclean@example.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "devclean@example.com")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ;ZIP")
else()
    set(CPACK_GENERATOR "TGZ;DEB;RPM;ZIP")
endif()

include(CPack)

if(DEVCLEAN_ENABLE_SANITIZERS OR ENABLE_SANITIZERS)
    devclean_enable_sanitizers(devclean_core)
    foreach(target
        devclean
        devclean_tests
        devclean_parser_tests
        devclean_formatter_tests
        devclean_config_tests
        devclean_scanner_tests
        devclean_scan_result_tests
        devclean_cache_insights_tests
        devclean_doctor_tests
        devclean_benchmark)
        devclean_enable_sanitizers(${target})
    endforeach()
endif()

find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)
if(CLANG_FORMAT_EXECUTABLE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i src/*.cpp src/*/*.cpp include/*.hpp include/*/*.hpp tests/*.cpp tests/unit/*.cpp
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
if(CLANG_TIDY_EXECUTABLE)
    add_custom_target(tidy
        COMMAND ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR} src/main.cpp src/core/*.cpp src/commands/*.cpp src/scanner/*.cpp src/platform/*.cpp src/cleaner/*.cpp src/utils/*.cpp src/engine/*.cpp tests/cli_tests.cpp tests/unit/*.cpp
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)
if(CPPCHECK_EXECUTABLE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE} --enable=warning,performance,portability --std=c++23 include src tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
