// Fill out your copyright notice in the Description page of Project Settings.

#include "TurretPH.h"
#include "BuildTrapAnimInstance.h"
#include "Components/BoxComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ATurretPH::ATurretPH()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	body = CreateDefaultSubobject<UStaticMeshComponent>(FName("TurretBody"));
	SetRootComponent(body);

	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(FName("BoxTrigger"));

	if (BoxTrigger != nullptr)
	{
		BoxTrigger->SetupAttachment(body);
		BoxTrigger->SetGenerateOverlapEvents(true);
		BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATurretPH::OnMyOverlapBegin);
		BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &ATurretPH::OnMyOverlapEnd);
	}
}

void ATurretPH::ResetAnimBuilding()
{
	USkeletalMeshComponent* skMesh = FindComponentByClass<USkeletalMeshComponent>();

	if (skMesh != nullptr)
	{
		skMesh->SetVisibility(false);
	}

	//ResetAnim

	Spawn();
}

// Called when the game starts or when spawned
void ATurretPH::BeginPlay()
{
	Super::BeginPlay();

	ActivePlayer = UGameplayStatics::GetPlayerPawn(this, 0);

	if (ActivePlayer == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player not found!"));
	}
	RegisterBuiltEvent();
}

// Called every frame
void ATurretPH::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(ActiveTurret))
	{
		USkeletalMeshComponent* skMesh = FindComponentByClass<USkeletalMeshComponent>();

		if (skMesh != nullptr)
		{
			skMesh->SetVisibility(true);
		}
		Spawned = false;
	}

	if (IsDone)
	{
		Spawn();
	}
}

void ATurretPH::SpawnTurret()
{
	if (!Spawned)
	{
		FVector spawnPos = this->GetActorLocation();
		FRotator spawnRot = this->GetActorRotation();
		FActorSpawnParameters spawnSett;
		spawnSett.bNoFail = true;
		ActiveTurret = GetWorld()->SpawnActor<AActor>(RocketLaucher, spawnPos, spawnRot, spawnSett);
		Spawned = true;
		Building = false;
		IsDone = false;
		ToSpawn = TurretType::NONE;
	}
}

void ATurretPH::SpawnGField()
{
	if (!Spawned)
	{
		FVector spawnPos = this->GetActorLocation();
		FRotator spawnRot = this->GetActorRotation();
		FActorSpawnParameters spawnSett;
		spawnSett.bNoFail = true;
		ActiveTurret = GetWorld()->SpawnActor<AActor>(GravityField, spawnPos, spawnRot, spawnSett);
		Spawned = true;
		Building = false;
		IsDone = false;
		ToSpawn = TurretType::NONE;
	}
}

void ATurretPH::SpawnLBeam()
{
	if (!Spawned)
	{
		FVector spawnPos = this->GetActorLocation();
		FRotator spawnRot = this->GetActorRotation();
		FActorSpawnParameters spawnSett;
		spawnSett.bNoFail = true;
		ActiveTurret = GetWorld()->SpawnActor<AActor>(LaserBeam, spawnPos, spawnRot, spawnSett);
		Building = false;
		Spawned = true;
		IsDone = false;
		ToSpawn = TurretType::NONE;
	}
}

void ATurretPH::Spawn()
{
	if (ToSpawn == TurretType::NONE)
		return;
	if (ToSpawn == TurretType::ROCKETLAUNCHER)
		SpawnTurret();
	else if (ToSpawn == TurretType::GRAVITYFIELD)
		SpawnGField();
	else if (ToSpawn == TurretType::LASERBEAM)
		SpawnLBeam();
}

void ATurretPH::RegisterBuiltEvent()
{
	USkeletalMeshComponent* skMesh = FindComponentByClass<USkeletalMeshComponent>();

	if (skMesh != nullptr)
	{
		UBuildTrapAnimInstance* myAnimInst = Cast<UBuildTrapAnimInstance>(skMesh->GetAnimInstance());

		if (myAnimInst != nullptr)
		{
			myAnimInst->OnTrapDoneBuilding.AddDynamic(this, &ATurretPH::ResetAnimBuilding);
		}
	}
}

void ATurretPH::CallSpawn(TurretType type)
{
	if (CloseEnough && !Spawned)
	{
		UE_LOG(LogTemp, Warning, TEXT("Callspawn"));
		Building = true;
		ToSpawn = type;

		USkeletalMeshComponent* skMesh = FindComponentByClass<USkeletalMeshComponent>();

		if (skMesh != nullptr)
		{
			UBuildTrapAnimInstance* myAnimInst = Cast<UBuildTrapAnimInstance>(skMesh->GetAnimInstance());

			if (myAnimInst != nullptr)
			{
				myAnimInst->IsDone = false;
				myAnimInst->IsBuilding = true;
			}
		}
	}
}

void ATurretPH::OnMyOverlapBegin(class UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == ActivePlayer)
	{
		CloseEnough = true;
	}
}

void ATurretPH::OnMyOverlapEnd(class UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == ActivePlayer)
	{
		CloseEnough = false;
	}
}