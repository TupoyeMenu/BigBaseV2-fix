if(ENABLE_GUI)
	target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ENABLE_GUI)
endif()

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")
