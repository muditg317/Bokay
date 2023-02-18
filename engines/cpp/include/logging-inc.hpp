#pragma once

#define CUSTOM_PREFIX_SUPPORT
#ifdef CUSTOM_PREFIX_SUPPORT
#define GLOG_CUSTOM_PREFIX_SUPPORT
#define WITH_CUSTOM_PREFIX
#endif
#include <glog/logging.h>