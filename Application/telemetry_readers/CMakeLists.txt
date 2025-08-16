cmake_minimum_required(VERSION 3.10)

# -----------------------------
# Configuration variables
# -----------------------------
set(COMP_NAME telemetry_forza7)
set(SPECIFIC_SRC_DIR forza_7)

project(${COMP_NAME} LANGUAGES CXX)

# -----------------------------
# Collect source files
# -----------------------------
file(GLOB_RECURSE COMMON_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
)

file(GLOB_RECURSE SPECIFIC_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/${SPECIFIC_SRC_DIR}/*.cpp"
)

# -----------------------------
# Create static library
# -----------------------------
add_library(${COMP_NAME} STATIC
    ${COMMON_SOURCES}
    ${SPECIFIC_SOURCES}
)

# Include directories
target_include_directories(${COMP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_SOURCE_DIR}/${SPECIFIC_SRC_DIR}
)

# Set C++ standard
target_compile_features(${COMP_NAME} PUBLIC cxx_std_17)

# -----------------------------
# cppcheck integration
# -----------------------------
find_program(CPPCHECK cppcheck)

if(CPPCHECK)
    message(STATUS "cppcheck found: ${CPPCHECK}")

    # Only .hpp files for cppcheck
    file(GLOB_RECURSE COMMON_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/common/*.hpp")
    file(GLOB_RECURSE SPECIFIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${SPECIFIC_SRC_DIR}/*.hpp")

    add_custom_target(run_cppcheck
        COMMAND ${CPPCHECK}
            --enable=all
            --inconclusive
            --quiet
            --std=c++17
            --language=c++
            ${COMMON_SOURCES} ${SPECIFIC_SOURCES} ${COMMON_HEADERS} ${SPECIFIC_HEADERS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running cppcheck on ${COMP_NAME} sources"
        VERBATIM
    )

    add_dependencies(${COMP_NAME} run_cppcheck)
else()
    message(WARNING "cppcheck not found, skipping static analysis")
endif()
