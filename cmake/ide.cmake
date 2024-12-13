# ---------------------------------------------------------------------------------------
# IDE support for headers
# ---------------------------------------------------------------------------------------
set(SYNMODELS_HEADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/../include")

file(GLOB SYNMODELS_TOP_HEADERS "${SYNMODELS_HEADERS_DIR}/*.hpp")
file(GLOB SYNMODELS_DATA_HEADERS "${SYNMODELS_HEADERS_DIR}/data/*.hpp")
file(GLOB SYNMODELS_KERNELS_HEADERS "${SYNMODELS_HEADERS_DIR}/kernels/*.hpp")
file(GLOB SYNMODELS_MODELS_HEADERS "${SYNMODELS_HEADERS_DIR}/models/*.hpp")
set(SYNMODELS_ALL_HEADERS ${SYNMODELS_TOP_HEADERS} ${SYNMODELS_DATA_HEADERS} ${SYNMODELS_KERNELS_HEADERS} ${SYNMODELS_MODELS_HEADERS})

source_group("Header Files" FILES ${SYNMODELS_TOP_HEADERS})
source_group("Header Files\\data" FILES ${SYNMODELS_DATA_HEADERS})
source_group("Header Files\\kernels" FILES ${SYNMODELS_KERNELS_HEADERS})
source_group("Header Files\\models" FILES ${SYNMODELS_MODELS_HEADERS})