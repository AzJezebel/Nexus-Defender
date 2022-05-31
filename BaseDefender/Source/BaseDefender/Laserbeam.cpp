// Fill out your copyright notice in the Description page of Project Settings.

#include "Laserbeam.h"

#include "DrawDebugHelpers.h"
#include "Minion.h"
#include "Raider.h"
#include "HPComponent.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALaserbeam::ALaserbeam()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	body = CreateDefaultSubobject<UStaticMeshComponent>(FName("TurretBody"));
	SetRootComponent(body);

	HPComp = CreateDefaultSubobject<UHPComponent>(FName("HPComponent"));

	Laser = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	Laser->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));

	if (CylinderAsset.Succeeded())
	{
		Laser->SetStaticMesh(CylinderAsset.Object);
		Laser->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f + 500.f));
		Laser->SetWorldScale3D(FVector(1.0f));
	}

	Laser->SetVisibility(false);

	SphereTrigger = CreateDefaultSubobject<USphereComponent>(FName("SphereTrigger"));
	if (SphereTrigger != nullptr)
	{
		SphereTrigger->SetupAttachment(body);
		SphereTrigger->SetSphereRadius(TurretRange);
		SphereTrigger->SetGenerateOverlapEvents(true);
		//SphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALaserbeam::OnMyOverlapBegin);
		//SphereTrigger->OnComponentEndOverlap.AddDynamic(this, &ALaserbeam::OnMyOverlapEnd);
	}


}

// Called when the game starts or when spawned
void ALaserbeam::BeginPlay()
{
	Super::BeginPlay();

	if (body == nullptr)
		return;
	
	SphereTrigger->UpdateOverlaps();
}

// Called every frame
void ALaserbeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HPComp->GetCurrentHP() <= 0)
		this->Destroy();

	SelectTarget();

	if (CurrentTarget != nullptr && FirstCall)
	{
		GetWorldTimerManager().SetTimer(LaserTimer, this, &ALaserbeam::DeathBeam, Frequency, true, 2.5f);
		FirstCall = false;
	}
}

bool ALaserbeam::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
	int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
	FHitResult hit;
	FCollisionQueryParams params;
	bool seen = GetWorld()->LineTraceSingleByChannel(hit, ObserverLocation, GetActorLocation(), ECC_Visibility, params);

	if(seen && hit.Actor != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("Seen : %s"), seen == true ? TEXT("true") : TEXT("false"));
		return false;
	}

	return true;
}

void ALaserbeam::SelectTarget()
{
	TArray<AActor*> MinionsInZone;
	TArray<AActor*> RaidersInZone;
	SphereTrigger->GetOverlappingActors(MinionsInZone, AMinion::StaticClass());
	SphereTrigger->GetOverlappingActors(RaidersInZone, ARaider::StaticClass());
	CurrentTarget = nullptr;
	if (RaidersInZone.Num() == 0 && MinionsInZone.Num() == 0)
	{
		FirstCall = true;
		Damage = BaseDamge;
		return;
	}

	if (RaidersInZone.Num() > 0)
	{
		CurrentTarget = RaidersInZone[0];
		float closest = FVector::Distance(GetActorLocation(), CurrentTarget->GetActorLocation());

		//Changed to 0 for the angle check
		for (int i = 0; i < RaidersInZone.Num(); i++)
		{
			if (float distance = FVector::Distance(GetActorLocation(), RaidersInZone[i]->GetActorLocation()) < closest)
			{
				CurrentTarget = RaidersInZone[i];
				closest = distance;
			}
		}
	}
	else if (MinionsInZone.Num() > 0)
	{
		CurrentTarget = MinionsInZone[0];
		float closest = FVector::Distance(GetActorLocation(), CurrentTarget->GetActorLocation());

		//Changed to 0 for the angle check
		for (int i = 0; i < MinionsInZone.Num(); i++)
		{
			if (float distance = FVector::Distance(GetActorLocation(), MinionsInZone[i]->GetActorLocation()) < closest)
			{
				CurrentTarget = MinionsInZone[i];
				closest = distance;
			}
		}
	}
}

void ALaserbeam::DeathBeam()
{
	if (CurrentTarget != nullptr)
	{
		Damage += Increment;
		FMath::Clamp(Damage, BaseDamge, 30.f);
		FDamageEvent dmgEvent;
		CurrentTarget->TakeDamage(Damage, dmgEvent, GetInstigatorController(), this);

		////Cant use that in build
		//const FName TraceTag("MyTraceTag");
		//GetWorld()->DebugDrawTraceTag = TraceTag;
		//FCollisionQueryParams CollisionParams;
		//CollisionParams.TraceTag = TraceTag;
		FVector start = { GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + HeadHeight };
		FVector end = CurrentTarget->GetActorLocation();

		//DrawDebugLine(GetWorld(), start, end, FColor(255, 0, 0), false, 0.1f, 0, 2);

		Laser->SetVisibility(true);
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(start, end);
		Laser->SetWorldRotation(FRotator{ rotation.Pitch - 90, rotation.Yaw, rotation.Roll });
		FVector scale(0.1f, 0.1f, (end - start).Size());
		Laser->SetRelativeScale3D(scale);
	}
	else
	{
		Laser->SetVisibility(false);
		GetWorldTimerManager().ClearTimer(LaserTimer);
	}
}

float ALaserbeam::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HPComp->RemoveHP(DamageAmount);

	return DamageAmount;
}