#pragma once

#include "Logging/LogMacros.h"

class UObject;

MONUMENTVALLEY_API DECLARE_LOG_CATEGORY_EXTERN(LogMv, Log, All);
MONUMENTVALLEY_API DECLARE_LOG_CATEGORY_EXTERN(LogMvExperience, Log, All);
MONUMENTVALLEY_API DECLARE_LOG_CATEGORY_EXTERN(LogMvAbilitySystem, Log, All);

MONUMENTVALLEY_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
