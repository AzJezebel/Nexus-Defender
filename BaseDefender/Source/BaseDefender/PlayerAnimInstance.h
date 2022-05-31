// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BASEDEFENDER_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PLAYER)
		bool IsWalking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PLAYER)
		bool IsSlowed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PLAYER)
		bool IsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PLAYER)
		bool IsDead = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PLAYER)
		bool IsHit = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PLAYER)
		bool AllowAtk = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PLAYER, meta = (ClampMin = "0.0", ClampMax = "100.0"))
		float WalkBlendRatio = 0.f;

	void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void SetAnimAtk(bool b);

protected:

};
