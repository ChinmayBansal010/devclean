cmake_minimum_required(VERSION 3.20)

project(devclean VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include_directories(include)

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

target_link_libraries(devclean_core PRIVATE nlohmann_json::nlohmann_json)

add_executable(devclean
    src/main.cpp
)

add_executable(devclean_tests
    tests/cli_tests.cpp
)

target_link_libraries(devclean PRIVATE devclean_core)
target_link_libraries(devclean_tests PRIVATE devclean_core)

install(TARGETS devclean RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES docs/devclean.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES scripts/devclean-completion.bash DESTINATION ${CMAKE_INSTALL_DATADIR}/bash-completion/completions RENAME devclean)

set(CPACK_PACKAGE_NAME "devclean")
set(CPACK_PACKAGE_VENDOR "devclean")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Developer cache cleaner")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "devclean@example.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "devclean@example.com")
set(CPACK_GENERATOR "TGZ;DEB;RPM")
include(CPack)

if(MSVC)
    target_compile_options(devclean PRIVATE /W4)
else()
    target_compile_options(devclean PRIVATE -Wall -Wextra -Wpedantic)
endif()