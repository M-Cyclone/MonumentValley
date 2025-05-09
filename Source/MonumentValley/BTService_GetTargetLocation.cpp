// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_GetTargetLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameCore/Player/MvPlayerAvatar.h"

void UBTService_GetTargetLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (ensure(BlackboardComp))
    {
        // if (const AMvPlayerAvatar* PlayerAvatar = Cast<AMvPlayerAvatar>(BlackboardComp->GetValueAsObject("PlayerAvatar")))
        // {
        //     if (UNLIKELY(PlayerAvatar->IsThisFrameSetTarget()))
        //     {
        //         BlackboardComp->SetValueAsVector("TargetLocation", PlayerAvatar->GetTargetWorldLocation());
        //     }
        // }
    }
}
