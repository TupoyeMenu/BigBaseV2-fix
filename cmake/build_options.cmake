option(ENABLE_GUI "Enables the GUI" ON)
option(ENABLE_EXCEPTION_HANDLER "Enables the exception hanler" ON)

set(GAME_BUILD 3411 CACHE STRING "Sets the target game build.")
set(GAME_BRANCH "LEGACY" CACHE STRING "Sets the target game branch, LEGACY or ENCHANCED")

string(TOUPPER ${GAME_BRANCH} GAME_BRANCH)

configure_file("${SRC_DIR}/config.cpp.in" "${SRC_DIR}/config.cpp" @ONLY)
