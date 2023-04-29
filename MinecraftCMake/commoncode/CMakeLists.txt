# common code subdirectory

#library is geen exec_program

#header only library
add_library(common_code INTERFACE "Globals.h")

set(
	CommonCodeIncludeDir 
	"${CMAKE_CURRENT_SOURCE_DIR}" 
	PARENT_SCOPE
)