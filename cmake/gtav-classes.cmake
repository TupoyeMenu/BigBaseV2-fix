
include(FetchContent)

FetchContent_Declare(
    gtav_classes
    GIT_REPOSITORY https://github.com/Mr-X-GTA/GTAV-Classes-1.git
    GIT_TAG        e803b20ec3d6cf20d6b254c915e088bd010f006e
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR "why" # FetchContent_Populate is depricated now, so we set this to a non existing directory.
)
message("GTAV-Classes")
FetchContent_MakeAvailable(gtav_classes)
