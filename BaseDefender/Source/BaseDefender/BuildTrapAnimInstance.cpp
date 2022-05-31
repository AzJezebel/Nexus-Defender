// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildTrapAnimInstance.h"
#include "TurretPH.h"

void UBuildTrapAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ATurretPH* owner = Cast<ATurretPH>(TryGetPawnOwner());

	if (owner != nullptr)
	{
		//	if (owner->Building == true)
		//		IsBuilding = true;

		//	if (IsDone)
		//	{
		//		IsBuilding = false;
		//		owner->SetIsDone(true);
		//	}

		//	if(owner->GetIsSpawned() == true)
		//	{
		//		IsDone = false;
		//	}
	}
}