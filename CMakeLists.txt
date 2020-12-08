cmake_minimum_required (VERSION 3.0)
project (Skybox-Painter VERSION 0.1)

find_package(OpenGL REQUIRED)

# GLAD
set(GLAD_DIR external/glad)
set(GLAD_SOURCES
    ${GLAD_DIR}/src/glad.c
    )
add_library(glad ${GLAD_SOURCES})
target_include_directories(glad PRIVATE ${GLAD_DIR}/include)

# GLFW
set(GLFW_DIR external/glfw)
add_subdirectory(${GLFW_DIR})

# GLM
set(GLM_DIR external/glm)
add_subdirectory(${GLM_DIR})

# STB
set(STB_DIR external/stb)

# GLSL PARSER
set(GLSl_PARSER_DIR external/glsl-parser)
set(GLSL_PARSER_SOURCES
	external/glsl-parser/util.cpp
	external/glsl-parser/ast.cpp
	external/glsl-parser/lexer.cpp
	external/glsl-parser/parser.cpp
	)
add_library(glsl-parser ${GLSL_PARSER_SOURCES})
target_include_directories(glsl-parser PRIVATE ${GLSL_PARSER_DIR})

set(GRAPHICS_LIBS
	${OPENGL_LIBRARY}
	glad
	glfw
	glm
	glsl-parser
)


set(RUN_EXEC skyboxpainter)
file(GLOB run_SRC
	"src/*.cpp"
)

add_executable(${RUN_EXEC}
	${run_SRC}
)
target_link_libraries(${RUN_EXEC}
	${GRAPHICS_LIBS}
)
target_include_directories(${RUN_EXEC}
	PRIVATE include
	PRIVATE external
	PRIVATE ${GLFW_DIR}/include
    PRIVATE ${GLAD_DIR}/include
	PRIVATE ${GLM_DIR}/include
	PRIVATE ${STB_DIR}
)


# Shaders
add_custom_command(TARGET ${RUN_EXEC}
	POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${RUN_EXEC}>/shaders
	COMMENT "Copying shaders" VERBATIM
	)