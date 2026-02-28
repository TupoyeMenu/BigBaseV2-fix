
include(FetchContent)

FetchContent_Declare(
    gtav_classes
    GIT_REPOSITORY https://github.com/Mr-X-GTA/GTAV-Classes-1.git
    GIT_TAG        9fa80bd650d37afd70056ec0f0c987cbc62e8a74
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR "why" # FetchContent_Populate is depricated now, so we set this to a non existing directory.
)
message("GTAV-Classes")
FetchContent_MakeAvailable(gtav_classes)
