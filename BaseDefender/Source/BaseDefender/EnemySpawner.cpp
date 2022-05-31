// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	float freq = WaveFrequency + (SpawnFrequency * NbPerWaves) + FirstSpawnTime;
	GetWorldTimerManager().SetTimer(WaveTimer, this, &AEnemySpawner::StartWave, freq, true, FirstSpawnTime);
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemySpawner::StartWave()
{
	//UE_LOG(LogTemp, Warning, TEXT("NewWave launched"));
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::SpawnEnemy, SpawnFrequency, true, 0.f);
}

void AEnemySpawner::SpawnEnemy()
{
	FActorSpawnParameters spawnSett;
	spawnSett.Owner = this;
	spawnSett.bNoFail = true;
	spawnSett.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	AActor* spawnRef = nullptr;
	FVector spawnPos = this->GetActorLocation() +
		this->GetActorForwardVector() * 5;
	FRotator spawnRot = this->GetActorRotation();
	if (Count == RaiderPosition)
	{
		spawnRef = GetWorld()->SpawnActor<AActor>(RaiderToSpawn, spawnPos, spawnRot, spawnSett);
	}
	else
	{
		spawnRef = GetWorld()->SpawnActor<AActor>(MinionToSpawn, spawnPos, spawnRot, spawnSett);
	}
	if (spawnRef == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy to spawn nullptr"));
	}
	else
		Count++;

	if (Count == NbPerWaves)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Clear timer"));
		GetWorldTimerManager().ClearTimer(SpawnTimer);
		Count = 0;
	}
}