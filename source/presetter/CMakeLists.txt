include(${CMAKE_CURRENT_SOURCE_DIR}/../max-sdk-base/script/max-pretarget.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
    "${MAX_SDK_INCLUDES}"
)

file(GLOB PROJECT_SRC
    "*.h"
    "*.c"
)

add_library(
    ${PROJECT_NAME}
    MODULE
    ${PROJECT_SRC}
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../max-sdk-base/script/max-posttarget.cmake)
