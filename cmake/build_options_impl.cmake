if(ENABLE_GUI)
	target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ENABLE_GUI)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui)
	target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR})
endif()

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")
