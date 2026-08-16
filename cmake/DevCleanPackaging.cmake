include(GNUInstallDirs)

install(TARGETS devclean
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/devclean
    FILES_MATCHING PATTERN "*.hpp"
)

if(EXISTS "${CMAKE_SOURCE_DIR}/docs/man/devclean.1")
    install(FILES "${CMAKE_SOURCE_DIR}/docs/man/devclean.1"
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif()
