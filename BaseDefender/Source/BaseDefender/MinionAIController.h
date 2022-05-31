// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MinionAIController.generated.h"


UCLASS()
class BASEDEFENDER_API AMinionAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMinionAIController();
	AMinionAIController(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void BeginPlay() override;
	
	FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	
	UPROPERTY(EditDefaultsOnly, Category = AISETTINGS)
		class UBehaviorTree* MinionBehavior = nullptr;
	
		FName TargetTag = "AITarget";

		UPROPERTY(EditAnywhere, Category = AISETTINGS)
		FName TargetActorKey = "TargetActor";
};
