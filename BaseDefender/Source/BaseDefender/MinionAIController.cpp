// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionAIController.h"
#include "BaseDefenderCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"

AMinionAIController::AMinionAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(FName("Perception"));

	FGenericTeamId teamID(1);
	
	this->SetGenericTeamId(teamID);	
}

AMinionAIController::AMinionAIController(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(FName("Perception"));

	FGenericTeamId teamID(1);

	this->SetGenericTeamId(teamID);
}

void AMinionAIController::BeginPlay()
{
	Super::BeginPlay();

	UAIPerceptionSystem::GetCurrent(GetWorld())->UpdateListener(*PerceptionComponent);
	
	if(MinionBehavior != nullptr)
	{
		if(!RunBehaviorTree(MinionBehavior))
		{
			UE_LOG(LogTemp, Warning, TEXT("BTree couldnt run"));
		}
	}

	TArray<AActor*> ActorsWTag;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetTag, ActorsWTag);

	if(ActorsWTag.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Target not found"));		
	}

	AActor* aitarget = ActorsWTag[0];
	if(aitarget != nullptr)
	{
		GetBlackboardComponent()->SetValueAsObject(TargetActorKey, aitarget);
	}
}

FGenericTeamId AMinionAIController::GetGenericTeamId() const
{
	return 1;
}

ETeamAttitude::Type AMinionAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const ABaseDefenderCharacter* player = Cast<ABaseDefenderCharacter>(&Other);

	if (player == nullptr)
		return ETeamAttitude::Neutral;

	AController* pController = player->GetController();
	
	if (pController == nullptr)
		return ETeamAttitude::Neutral;
	
	if(pController->IsA<AMinionAIController>())
	{
		return ETeamAttitude::Friendly;
	}

	return ETeamAttitude::Hostile;
}
