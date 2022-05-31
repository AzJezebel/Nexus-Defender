// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Perception/AISightTargetInterface.h"

#include "Laserbeam.generated.h"

class USphereComponent;
class UHPComponent;

UCLASS()
class BASEDEFENDER_API ALaserbeam : public AActor,  public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALaserbeam();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		UStaticMeshComponent* body = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		USphereComponent* SphereTrigger = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		UStaticMeshComponent* Laser = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UHPComponent* HPComp = nullptr;
	
	UPROPERTY(EditAnywhere, Category = SPECS)
		float TurretRange = 350.f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float BaseDamge = 5.f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float Increment = 1.5f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float Frequency = 0.25f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float HeadHeight = 500.f;

	
		
private:

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
		void SelectTarget();
	UFUNCTION()
		void DeathBeam();

private:
	FTimerHandle LaserTimer;
	AActor* CurrentTarget = nullptr;
	bool FirstCall = true;
	float Damage = BaseDamge;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const override;
};