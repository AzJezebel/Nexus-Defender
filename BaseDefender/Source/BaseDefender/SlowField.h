// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlowField.generated.h"

class USphereComponent;
class UHPComponent;

UCLASS()
class BASEDEFENDER_API ASlowField : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASlowField();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		UStaticMeshComponent* body = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		USphereComponent* SphereTrigger = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UHPComponent* HPComp = nullptr;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float TurretRange = 350.f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float Damage = 0.5f;

	UPROPERTY(EditAnywhere, Category = SPECS, meta = (ClampMin = "0.01", ClampMax = "1"))
		float SlowModifier = 0.15f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float Duration = 1.5f;

	UPROPERTY(EditAnywhere, Category = SPECS)
		float Frequency = 1.f;

	UPROPERTY(BlueprintReadOnly)
		bool RingVisible = false;

private:
	float MaxSpeedSave = 0.f;
	bool SpeedSetted = false;
	bool CallEvent = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnMyOverlapBegin(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnMyOverlapEnd(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
		void ZoneEffect();
	UFUNCTION()
		void CancelEffect();

	void ZoneTimer();

private:
	FTimerHandle FireTimer;
	FTimerHandle EffectTick;
	TArray<AActor*> ActorsInZone;
	bool FirstCall = false;
	bool doTime = false;
	float tickTimer = 0.f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
