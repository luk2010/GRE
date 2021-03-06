# DefaultControllers Plugin Project
project( BasicDefinitionsPlugin LANGUAGES CXX )

# Enables C++11 features.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Sources files
file(GLOB headers "${GRE_ROOT_DIRECTORY}/Plugins/BasicDefinitions/inc/*.h")
file(GLOB sources "${GRE_ROOT_DIRECTORY}/Plugins/BasicDefinitions/src/*.cpp")

# Adds includes
include_directories( PUBLIC
	"${GRE_ROOT_DIRECTORY}/Engine/inc"
	"${GRE_ROOT_DIRECTORY}/Plugins/BasicDefinitions/inc"
	"$<INSTALL_INTERFACE:include>"
	PRIVATE src
)

# Creates plugin target
add_library(BasicDefinitions SHARED ${sources} ${headers})
target_link_libraries(BasicDefinitions gre)

set_target_properties( BasicDefinitions
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${GRE_PLUGIN_DIRECTORY}
	    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${GRE_PLUGIN_DIRECTORY}
	    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${GRE_PLUGIN_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY ${GRE_PLUGIN_DIRECTORY}
	    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${GRE_PLUGIN_DIRECTORY}
	    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${GRE_PLUGIN_DIRECTORY}
        RUNTIME_OUTPUT_DIRECTORY ${GRE_PLUGIN_DIRECTORY}
	    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GRE_PLUGIN_DIRECTORY}
	    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${GRE_PLUGIN_DIRECTORY}
)

# Try to set C++11 Flags for Xcode Projects.
if(${CMAKE_GENERATOR} MATCHES "Xcode")

    macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
        set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY}
                      ${XCODE_VALUE})
    endmacro (set_xcode_property)

    set_xcode_property(BasicDefinitions CLANG_CXX_LANGUAGE_STANDARD "c++11")
    set_xcode_property(BasicDefinitions CLANG_CXX_LIBRARY "libc++")

    set_property(TARGET BasicDefinitions PROPERTY CXX_STANDARD 11)
    set_property(TARGET BasicDefinitions PROPERTY CXX_STANDARD_REQUIRED ON)

else()

    set_property(TARGET BasicDefinitions PROPERTY CXX_STANDARD 11)
    set_property(TARGET BasicDefinitions PROPERTY CXX_STANDARD_REQUIRED ON)

endif(${CMAKE_GENERATOR} MATCHES "Xcode")
