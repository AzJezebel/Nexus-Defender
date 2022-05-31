// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Raider.generated.h"

class USphereComponent;
class UHPComponent;

UCLASS()
class BASEDEFENDER_API ARaider : public ACharacter
{
	GENERATED_BODY()

		UPROPERTY()
		AActor* ActivePlayer;
		AActor* ActiveNexus;
public:
	// Sets default values for this character's properties
	ARaider();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		USphereComponent* SphereTrigger = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UHPComponent* HPComp = nullptr;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float AtkRange = 50.f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float OffsetNexusSize = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SPECS)
		float Damage = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SPECS)
		float AtkFrequency = 4.f;

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
	void AggroNexus();

private:
	void AtkPlayer();
	void AtkNexus();
	
	bool PlayerInRange = false;
	bool NexusInRange = false;
	FTimerHandle AtkPlayerT;
	FTimerHandle AtkNexusT;
	FTimerHandle AtkDelay;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
