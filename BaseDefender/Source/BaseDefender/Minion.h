// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Minion.generated.h"

class USphereComponent;
class UHPComponent;

UCLASS()
class BASEDEFENDER_API AMinion : public ACharacter
{
	GENERATED_BODY()

		UPROPERTY()
		AActor* ActivePlayer;
	UPROPERTY()
		AActor* ActiveTurret;
	UPROPERTY()
		AActor* ActiveNexus;
public:
	// Sets default values for this character's properties
	AMinion();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		USphereComponent* SphereTrigger = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UHPComponent* HPComp = nullptr;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float AtkRange = 150.f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float OffsetNexusSize = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SPECS)
		float Damage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SPECS)
		float DmgMult = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SPECS)
		float AtkFrequency = 2.f;

	bool AnimAtk = false;
	bool AnimDead = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnMyOverlapBegin(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnMyOverlapEnd(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void AggroPlayer();
	void AtkPlayer();
	void AggroTurret();
	void AtkTurret();
	void AggroNexus();
	void AtkNexus();
	void UpdateOverlapped();

private:
	bool PlayerInRange = false;
	bool TurretInRange = false;
	bool NexusInRange = false;
	FTimerHandle AtkPlayerT;
	FTimerHandle AtkTurrT;
	FTimerHandle AtkNexusT;
	FTimerHandle AtkDelay;
	FTimerHandle DeathDelay;
	TArray<AActor*> StructInZone;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
