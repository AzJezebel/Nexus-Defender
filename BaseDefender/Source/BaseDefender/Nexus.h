// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Nexus.generated.h"

class USphereComponent;
class UHPComponent;

UCLASS()
class BASEDEFENDER_API ANexus : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANexus();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UHPComponent* HPComp = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
