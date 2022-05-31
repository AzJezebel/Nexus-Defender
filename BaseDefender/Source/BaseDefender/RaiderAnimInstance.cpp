// Fill out your copyright notice in the Description page of Project Settings.


#include "RaiderAnimInstance.h"
#include "Raider.h"

#include "GameFramework/CharacterMovementComponent.h"

void URaiderAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ARaider* owner = Cast<ARaider>(TryGetPawnOwner());

	if (owner != nullptr)
	{
		const float moveSpeed = owner->GetCharacterMovement()->Velocity.Size();

		IsWalking = (!FMath::IsNearlyZero(moveSpeed));

		if (IsWalking)
		{
			const float maxSpeed = owner->GetCharacterMovement()->MaxWalkSpeed;
			WalkBlendRatio = (moveSpeed / maxSpeed) * 100.f;
		}

		IsAttacking = owner->AnimAtk;
		IsDead = owner->AnimDead;
	}
}
