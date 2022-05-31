// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretPH.generated.h"

class UBoxComponent;

UCLASS()
class BASEDEFENDER_API ATurretPH : public APawn
{
	GENERATED_BODY()

		UPROPERTY()
		AActor* ActivePlayer;
	UPROPERTY()
		AActor* ActiveTurret;
public:
	// Sets default values for this actor's properties
	ATurretPH();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		UStaticMeshComponent* body = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		UBoxComponent* BoxTrigger = nullptr;

	enum class TurretType
	{
		ROCKETLAUNCHER = 0,
		GRAVITYFIELD = 1,
		LASERBEAM = 2,
		NONE
	};

	UFUNCTION(BlueprintCallable)
		const bool GetInRange() { return CloseEnough; };

	UFUNCTION(BlueprintCallable)
		const bool GetIsSpawned() const { return Spawned; }
	
	UFUNCTION(BlueprintCallable)
		const bool GetIsBuilding() const { return Building; }

	UFUNCTION(BlueprintCallable)
		void ResetAnimBuilding();

	void SetIsDone(bool b) { IsDone = b; }

	bool Building = false;
	bool Idle = false;

private:
	bool CloseEnough = false;
	bool Spawned = false;
	bool IsDone = false;
	TurretType ToSpawn = TurretType::NONE;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BASE)
		TSubclassOf<AActor> RocketLaucher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BASE)
		TSubclassOf<AActor> GravityField;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BASE)
		TSubclassOf<AActor> LaserBeam;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BASE)
	//	TSubclassOf<AActor> TurretToSpawn;

	UFUNCTION()
		void SpawnTurret();
	UFUNCTION()
		void SpawnGField();
	UFUNCTION()
		void SpawnLBeam();

	void Spawn();
	void RegisterBuiltEvent();

	UFUNCTION()
		void OnMyOverlapBegin(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnMyOverlapEnd(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CallSpawn(TurretType type);
};
