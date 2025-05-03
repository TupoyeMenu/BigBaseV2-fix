include(FetchContent)

message("AsyncLogger")
FetchContent_Declare(
    AsyncLogger
    GIT_REPOSITORY https://github.com/Yimura/AsyncLogger.git
    GIT_TAG 6fcfd90b3f4ca4dae09c4a96e9a506e6aea06472
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(AsyncLogger)

set_property(TARGET AsyncLogger PROPERTY CXX_STANDARD 20)
target_compile_definitions(AsyncLogger PRIVATE CXX_FORMAT_SUPPORT)
