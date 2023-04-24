include(FetchContent)

set(ADD_FATAL_EXAMPLE OFF CACHE INTERNAL "")
set(INSTALL_G3LOG OFF CACHE INTERNAL "")
set(G3_SHARED_LIB OFF CACHE INTERNAL "")
set(G3_SHARED_RUNTIME OFF CACHE INTERNAL "")

FetchContent_Declare(
    g3log
    GIT_REPOSITORY https://github.com/KjellKod/g3log.git
    GIT_TAG        b249fb6c6cb5c6badb19d2ccece730dd0278ac6b
    GIT_PROGRESS TRUE
)
message("g3log")
FetchContent_MakeAvailable(g3log)
set_property(TARGET g3log PROPERTY CXX_STANDARD 17)
