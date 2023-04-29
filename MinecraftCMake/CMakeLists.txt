#root folder

#main project

cmake_minimum_required(VERSION 3.22)

project(AssignmentProject)

add_subdirectory(commoncode)

add_subdirectory(staticlib)

add_subdirectory(3rdParty)

add_subdirectory(tool)

target_include_directories(
	Tool PUBLIC
	"${InputOutputIncludeDir}"
)

target_include_directories(
	InputOutputWriter PUBLIC
	"${CommonCodeIncludeDir}"
	"${ThirdPartyHeaderOnlyIncludeDir}"
)

target_link_libraries(
	Tool PUBLIC InputOutputWriter
)