CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(LOLTOML)

OPTION(ENABLE_TESTING "Enable testing" ON)
OPTION(BUILD_EXAMPLES "Build examples" OFF)

IF (ENABLE_TESTING)
    ENABLE_TESTING()
    ADD_SUBDIRECTORY(tests)
ENDIF()

IF (BUILD_EXAMPLES)
    ADD_SUBDIRECTORY(examples)
ENDIF()

INSTALL(
    DIRECTORY
        include/
    DESTINATION include
    COMPONENT development
)
