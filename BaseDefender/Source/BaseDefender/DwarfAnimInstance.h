// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DwarfAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BASEDEFENDER_API UDwarfAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DWARF)
		bool IsWalking = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DWARF)
		bool IsSlowed = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DWARF)
		bool IsAttacking= false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DWARF)
		bool IsDead= false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DWARF, meta = (ClampMin = "0.0", ClampMax = "100.0"))
		float WalkBlendRatio = 0.f;

	void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	
};
