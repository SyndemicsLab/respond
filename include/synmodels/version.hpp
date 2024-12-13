#ifndef VERSION_HPP_
#define VERSION_HPP_

#define SYNMODELS_VER_MAJOR 1
#define SYNMODELS_VER_MINOR 15
#define SYNMODELS_VER_PATCH 0

#define SYNMODELS_TO_VERSION(major, minor, patch)                              \
    (major * 10000 + minor * 100 + patch)
#define SYNMODELS_VERSION                                                      \
    SYNMODELS_TO_VERSION(SYNMODELS_VER_MAJOR, SYNMODELS_VER_MINOR,             \
                         SYNMODELS_VER_PATCH)

#endif
