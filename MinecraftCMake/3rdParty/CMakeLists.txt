#ThirdPartyHeaderOnly

file(GLOB Headers "*.h")

add_library(ThirdPartyHeaderOnly INTERFACE ${Headers})

Set(ThirdPartyHeaderOnlyIncludeDir "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)