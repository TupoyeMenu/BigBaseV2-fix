include(FetchContent)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG        c1a7c3843bd1a5fe3eb779b64c0d823bca3dc339
    GIT_PROGRESS TRUE
)
message("MinHook")
FetchContent_MakeAvailable(minhook)
set_property(TARGET minhook PROPERTY CXX_STANDARD 20)
