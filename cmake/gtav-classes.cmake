
include(FetchContent)

FetchContent_Declare(
    gtav_classes
    GIT_REPOSITORY https://github.com/Yimura/GTAV-Classes.git
    GIT_TAG        aebd69542e58fab8975da76c3e555b122ddef5d6
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR "why" # FetchContent_Populate is depricated now, so we set this to a non existing directory.
)
message("GTAV-Classes")
FetchContent_MakeAvailable(gtav_classes)
