// Copyright RemRemRemRe, All Rights Reserved.

#pragma once

#include "RemMetaTags.h"
#include "RemSpringItOnTags.generated.h"

#define REM_API REMSPRINGITON_API

UCLASS(Config = RemSpringItOnTags)
class REM_API URemSpringItOnTags : public URemMetaTags
{
	GENERATED_BODY()
};

#undef REM_API
