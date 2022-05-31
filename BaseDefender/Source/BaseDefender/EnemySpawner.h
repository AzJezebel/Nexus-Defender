// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class BASEDEFENDER_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		UStaticMeshComponent* body = nullptr;

private:
	bool KeyPressed;
	FTimerHandle SpawnTimer;
	FTimerHandle WaveTimer;
	int Count = 0;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StartWave();
	void SpawnEnemy();

	UPROPERTY(EditAnywhere, Category = ENEMY)
		TSubclassOf<AActor> MinionToSpawn;
	
	UPROPERTY(EditAnywhere, Category = ENEMY)
		TSubclassOf<AActor> RaiderToSpawn;

	UPROPERTY(EditAnywhere, Category = SPAWNSETTINGS)
		int RaiderPosition = 3;
	
	UPROPERTY(EditAnywhere, Category = TIMESETTINGS)
		float FirstSpawnTime = 1.f;
	
	UPROPERTY(EditAnywhere, Category = TIMESETTINGS)
		float NbPerWaves = 6.f;
	
	UPROPERTY(EditAnywhere, Category = TIMESETTINGS)
		float SpawnFrequency = 1.5f;
	
	UPROPERTY(EditAnywhere, Category = TIMESETTINGS)
		float WaveFrequency = 10.f;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
