include(FetchContent)

set(JSON_MultipleHeaders OFF)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
message("json")
FetchContent_MakeAvailable(json)
