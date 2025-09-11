// Copyright RemRemRemRe, All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"
#include "Macro/RemMacroUtilities.h"

#define REM_API REMSPRINGITON_API

REM_API DECLARE_LOG_CATEGORY_EXTERN(LogRemSpringItOn, REM_DEFAULT_LOG_VERBOSITY, REM_MAX_LOG_VERBOSITY)

#undef REM_API
