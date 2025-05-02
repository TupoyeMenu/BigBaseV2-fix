if(ENABLE_GUI)
	target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ENABLE_GUI)
	target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE imgui d3dcompiler dwmapi winmm)
	target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${imgui_SOURCE_DIR})
endif()

if(ENABLE_EXCEPTION_HANDLER)
	target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ENABLE_EXCEPTION_HANDLER)
endif()

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")
