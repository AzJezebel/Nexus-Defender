// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BuildTrapAnimInstance.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrapBuilt);

/**
 * 
 */
UCLASS()
class BASEDEFENDER_API UBuildTrapAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BUILD)
		bool IsDone = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BUILD)
		bool IsBuilding = false;
	
	UPROPERTY(EditAnywhere, BlueprintCallable, Category = BUILD)
		FOnTrapBuilt OnTrapDoneBuilding;

	void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

};