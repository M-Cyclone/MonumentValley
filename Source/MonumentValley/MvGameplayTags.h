#pragma once

#include "GameplayTagContainer.h"
#include "Logging/LogMacros.h"
#include "NativeGameplayTags.h"

namespace MvGameplayTags
{
MONUMENTVALLEY_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

MONUMENTVALLEY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
}  // namespace MvGameplayTags
