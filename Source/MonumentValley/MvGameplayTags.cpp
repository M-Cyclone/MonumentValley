#include "MvGameplayTags.h"

#include "MvLogChannels.h"

namespace MvGameplayTags
{
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_AutoRunning, "Status.AutoRunning", "Target is auto-running.");

// Custom Movement Modes
const TMap<uint8, FGameplayTag> CustomMovementModeTagMap = {
    // Fill these in with your custom modes
};

FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
{
    const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
    FGameplayTag                Tag     = Manager.RequestGameplayTag(FName(*TagString), false);

    if (!Tag.IsValid() && bMatchPartialString)
    {
        FGameplayTagContainer AllTags;
        Manager.RequestAllGameplayTags(AllTags, true);

        for (const FGameplayTag& TestTag : AllTags)
        {
            if (TestTag.ToString().Contains(TagString))
            {
                UE_LOG(LogMv,
                       Display,
                       TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."),
                       *TagString,
                       *TestTag.ToString());
                Tag = TestTag;
                break;
            }
        }
    }

    return Tag;
}
}  // namespace MvGameplayTags
