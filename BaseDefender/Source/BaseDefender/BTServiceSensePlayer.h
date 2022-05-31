// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTServiceSensePlayer.generated.h"

/**
 * 
 */
UCLASS()
class BASEDEFENDER_API UBTServiceSensePlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = SettingsAI)
		TSubclassOf<class UAISense> PerceivedSense = nullptr;
};
