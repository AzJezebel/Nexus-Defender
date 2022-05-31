// Fill out your copyright notice in the Description page of Project Settings.

#include "DwarfAnimInstance.h"
#include "Minion.h"

#include "GameFramework/CharacterMovementComponent.h"

void UDwarfAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	AMinion* owner = Cast<AMinion>(TryGetPawnOwner());

	if (owner != nullptr)
	{
		const float moveSpeed = owner->GetCharacterMovement()->Velocity.Size();

		IsWalking = (!FMath::IsNearlyZero(moveSpeed));

		if(IsWalking)
		{
			const float maxSpeed = owner->GetCharacterMovement()->MaxWalkSpeed;
			WalkBlendRatio = (moveSpeed / maxSpeed) * 100.f;
		}

		IsAttacking = owner->AnimAtk;
		IsDead = owner->AnimDead;
	}
}