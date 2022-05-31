// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Turret.generated.h"

class USphereComponent;
class UHPComponent;
class UParticleSystem;

UCLASS()
class BASEDEFENDER_API ATurret : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurret();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		UStaticMeshComponent* body = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		UStaticMeshComponent* ExplosionSphere = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		USphereComponent* SphereTrigger = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UHPComponent* HPComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UParticleSystem* ExplosionVisu;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SPECS)
		float Damage = 90.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SPECS)
		float AOEDamage = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SPECS)
		float AOERadius = 200.f;

	//UPROPERTY(EditAnywhere, Category = SPECS)
	//	float RotationPerSec = 20.f;

		//UPROPERTY(EditAnywhere, Category = SPECS)
	//	float MoveAmplitude = 30.f;
	
	UPROPERTY(EditAnywhere, Category = SPECS)
		float SpawnHeight = 200.f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float TurretRange = 350.f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float Frequency = 3.5f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		FVector ExplodePos;

	bool ShoxExplode = false;
	bool AnimAtk = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BASE)
		TSubclassOf<AActor> BulletToSpawn;

	UFUNCTION()
		void OnMyOverlapBegin(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnMyOverlapEnd(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
		void LaunchProjectile();
	
private:
	FTimerHandle FireTimer;
	AActor* targetInRange = nullptr;
	bool canAtk = false;
	float atkTimer = 0.f;

	FVector PlayerPos;
	float LevTime = 0.f;
	bool KeyPressed;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CallFire();
};
