#static lib 

add_library(InputOutputWriter "InputOuptutBlocks.h" "InputOuptutBlocks.cpp")
set(InputOutputIncludeDir "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)
