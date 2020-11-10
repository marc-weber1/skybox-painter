cmake_minimum_required (VERSION 3.0)
project (Model-Converter VERSION 0.1)


file(GLOB run_SRC
	"*.cpp"
)

add_executable(run
	${run_SRC}
)
target_include_directories(run
	PRIVATE .
)