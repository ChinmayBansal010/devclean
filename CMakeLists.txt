cmake_minimum_required(VERSION 3.20)

project(devclean VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_SANITIZERS "Build with AddressSanitizer and UBSan" OFF)

include(GNUInstallDirs)
include_directories(include)

enable_testing()

find_package(nlohmann_json CONFIG REQUIRED)

add_library(devclean_core
    src/core/Application.cpp
    src/core/ArgumentParser.cpp
    src/core/CommandDispatcher.cpp
    src/core/Config.cpp
    src/core/ScanHistory.cpp

    src/commands/AnalyzeCommand.cpp
    src/commands/ScanCommand.cpp
    src/commands/CleanCommand.cpp
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

add_executable(devclean
    src/main.cpp
)

add_executable(devclean_tests
    tests/cli_tests.cpp
)

add_executable(devclean_benchmark
    src/benchmark.cpp
)

target_link_libraries(devclean PRIVATE devclean_core)
target_link_libraries(devclean_tests PRIVATE devclean_core)
target_link_libraries(devclean_benchmark PRIVATE devclean_core)

add_test(NAME devclean_cli_tests COMMAND devclean_tests)

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
set(CPACK_GENERATOR "TGZ;DEB;RPM;ZIP")
include(CPack)

if(ENABLE_SANITIZERS)
    if(MSVC)
        target_compile_options(devclean_core PRIVATE /fsanitize=address /Zi)
        target_link_options(devclean_core PRIVATE /INCREMENTAL:NO /debug /fsanitize=address)
    else()
        target_compile_options(devclean_core PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(devclean_core PRIVATE -fsanitize=address,undefined)
    endif()
endif()

if(MSVC)
    target_compile_options(devclean PRIVATE /W4)
    target_compile_options(devclean_tests PRIVATE /W4)
    target_compile_options(devclean_benchmark PRIVATE /W4)
else()
    target_compile_options(devclean PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(devclean_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(devclean_benchmark PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)
if(CLANG_FORMAT_EXECUTABLE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i src/*.cpp src/*/*.cpp include/*.hpp include/*/*.hpp tests/*.cpp
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
if(CLANG_TIDY_EXECUTABLE)
    add_custom_target(tidy
        COMMAND ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR} src/main.cpp src/core/*.cpp src/commands/*.cpp src/scanner/*.cpp src/platform/*.cpp src/cleaner/*.cpp src/utils/*.cpp tests/cli_tests.cpp
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)
if(CPPCHECK_EXECUTABLE)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE} --enable=warning,performance,portability --std=c++23 include src tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
