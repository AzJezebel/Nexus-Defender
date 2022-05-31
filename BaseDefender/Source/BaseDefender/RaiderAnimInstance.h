// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RaiderAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BASEDEFENDER_API URaiderAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RAIDER)
		bool IsWalking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RAIDER)
		bool IsSlowed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RAIDER)
		bool IsAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RAIDER)
		bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RAIDER, meta = (ClampMin = "0.0", ClampMax = "100.0"))
		float WalkBlendRatio = 0.f;

	void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

};