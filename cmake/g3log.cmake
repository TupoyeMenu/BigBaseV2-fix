include(FetchContent)

set(ADD_FATAL_EXAMPLE OFF CACHE INTERNAL "")
set(INSTALL_G3LOG OFF CACHE INTERNAL "")
set(G3_SHARED_LIB OFF CACHE INTERNAL "")
set(G3_SHARED_RUNTIME OFF CACHE INTERNAL "")
set(ADD_G3LOG_UNIT_TEST OFF CACHE INTERNAL "")

FetchContent_Declare(
    g3log
    GIT_REPOSITORY https://github.com/KjellKod/g3log.git
    GIT_TAG        c1dd801e4213ab46ac6c648d2e3d464ce4dd13e5
    GIT_PROGRESS TRUE
)
message("g3log")
FetchContent_MakeAvailable(g3log)
set_property(TARGET g3log PROPERTY CXX_STANDARD 17)
