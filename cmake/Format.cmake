set(url https://raw.githubusercontent.com/StableCoder/cmake-scripts/main/formatting.cmake)
set(cmake_path ${CMAKE_BINARY_DIR}/format.cmake)
message(STATUS "Downloading clang format util `format.cmake` from ${url}")
file(DOWNLOAD "${url}" "${cmake_path}")
include(${cmake_path})
message(STATUS "Downloaded succesfully.")