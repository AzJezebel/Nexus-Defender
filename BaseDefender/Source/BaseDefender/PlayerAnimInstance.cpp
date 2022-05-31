// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "BaseDefenderCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABaseDefenderCharacter* owner = Cast<ABaseDefenderCharacter>(TryGetPawnOwner());

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
		//if (owner->AnimAtk == true)
		//	IsAttacking = true;

		if (AllowAtk)
		{
			owner->SetAllowAtk(true);
			AllowAtk = false;
		}
		
		IsDead = owner->AnimDead;
	}
}

void UPlayerAnimInstance::SetAnimAtk(bool b)
{
	ABaseDefenderCharacter* owner = Cast<ABaseDefenderCharacter>(TryGetPawnOwner());

	if (owner != nullptr)
	{
		owner->AnimAtk = b;
	}
}
