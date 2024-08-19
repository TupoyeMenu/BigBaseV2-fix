include(FetchContent)

FetchContent_Declare(
    FMT
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        1a79bbfa83b0942fca3f2794655b6b91bb682583
    GIT_PROGRESS TRUE
)
message("fmt")
FetchContent_MakeAvailable(FMT)
