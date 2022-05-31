// Fill out your copyright notice in the Description page of Project Settings.


#include "BTServiceSenseTurret.h"
#include "Turret.h"
#include "AIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"

void UBTServiceSenseTurret::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* controller = OwnerComp.GetAIOwner();
	UAIPerceptionComponent* AIPerception = nullptr;

	if (controller != nullptr)
	{
		AIPerception = controller->FindComponentByClass<UAIPerceptionComponent>();
	}

	if (AIPerception == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Percepetion nullptr"));
		return;
	}

	TArray<AActor*> PerceivedActors;
	AIPerception->GetCurrentlyPerceivedActors(PerceivedSense, PerceivedActors);
	

	if (PerceivedActors.Num() != 0)
	{
		for (auto actorSeen : PerceivedActors)
		{

			if (actorSeen->ActorHasTag("Structure"))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), actorSeen);
				return;
			}
		}
	}
	OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
}
