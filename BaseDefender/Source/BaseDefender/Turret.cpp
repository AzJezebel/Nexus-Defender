// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"

#include "DrawDebugHelpers.h"
#include "HPComponent.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATurret::ATurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HPComp = CreateDefaultSubobject<UHPComponent>(FName("HPComponent"));

	body = CreateDefaultSubobject<UStaticMeshComponent>(FName("TurretBody"));
	ExplosionSphere = CreateDefaultSubobject<UStaticMeshComponent>(FName("Explosion"));
	SetRootComponent(body);

	ExplosionVisu = CreateDefaultSubobject<UParticleSystem>(TEXT("ExplosionFeedback"));
	ExplosionVisu->SecondsBeforeInactive = 0.0f;
	//ExplosionVisu->bAutoDestroy = false;
	//ExplosionVisu->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	ExplosionVisu = Particle.Object;

	if (ExplosionVisu)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVisu, { this->GetActorLocation().X, this->GetActorLocation().Y, (this->GetActorLocation().Z + SpawnHeight) });

	ExplosionSphere->SetVisibility(false);

	SphereTrigger = CreateDefaultSubobject<USphereComponent>(FName("SphereTrigger"));
	if (SphereTrigger != nullptr)
	{
		SphereTrigger->SetupAttachment(body);
		SphereTrigger->SetSphereRadius(TurretRange);
		SphereTrigger->SetGenerateOverlapEvents(true);
		//SphereTrigger->SetCollisionResponseToChannel(ECollisionChannel::, ECollisionResponse::ECR_Overlap);
		SphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATurret::OnMyOverlapBegin);
		SphereTrigger->OnComponentEndOverlap.AddDynamic(this, &ATurret::OnMyOverlapEnd);
	}
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HPComp->GetCurrentHP() <= 0)
		this->Destroy();

	if (body == nullptr)
		return;

	if (!canAtk)
		atkTimer -= DeltaTime;

	if (atkTimer <= 0)
	{
		ExplosionSphere->SetVisibility(false);
		canAtk = true;
		AnimAtk = false;
	}

	LaunchProjectile();
	//{
	//	const float dTMove = FMath::Sin(LevTime + DeltaTime) - FMath::Sin(LevTime);
	//	FRotator spin(0, RotationPerSec * DeltaTime, 0);
	//	FVector updown(0, 0, dTMove * MoveAmplitude);
	//	body->AddRelativeRotation(spin);
	//	body->AddRelativeLocation(updown);
	//	LevTime += DeltaTime;
	//}
}

void ATurret::CallFire()
{
	KeyPressed = !KeyPressed;
}

void ATurret::LaunchProjectile()
{
	//Cast damage here + aoe
	//So dmg is always applied and enemy never dodges laucnhed atk
	TArray<AActor*> enemiesInRange;
	TArray<AActor*> AOETargets;
	SphereTrigger->GetOverlappingActors(enemiesInRange);

	if (enemiesInRange.Num() == 0)
	{
		targetInRange = nullptr;
		return;
	}

	AActor* inRange = enemiesInRange[0];
	targetInRange = inRange;
	FRotator lookat = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), inRange->GetActorLocation());
	SetActorRotation({ 0, lookat.Yaw, 0 });

	if (canAtk)
	{
		float closest = FVector::Distance(GetActorLocation(), inRange->GetActorLocation());

		//Changed to 0 for the angle check
		for (int i = 0; i < enemiesInRange.Num(); i++)
		{
			if (float distance = FVector::Distance(GetActorLocation(), enemiesInRange[i]->GetActorLocation()) < closest)
			{
				inRange = enemiesInRange[i];
				closest = distance;
			}
		}

		FDamageEvent dmgEvent;
		inRange->TakeDamage(Damage, dmgEvent, GetInstigatorController(), this);
		FVector targetLoc = inRange->GetActorLocation();
		FVector start = { targetLoc.X, targetLoc.Y, targetLoc.Z - (AOERadius / 2) };

		for (int i = 0; i < enemiesInRange.Num(); i++)
		{
			AActor* in = enemiesInRange[i];
			if (in == inRange)
				continue;

			FVector inPos = in->GetActorLocation();
			bool xValid = (inPos.X <  targetLoc.X + AOERadius && inPos.X > targetLoc.X - AOERadius);
			bool YValid = (inPos.Y <  targetLoc.Y + AOERadius && inPos.Y > targetLoc.Y - AOERadius);
			if (xValid && YValid)
			{
				in->TakeDamage(AOEDamage, dmgEvent, GetInstigatorController(), this);
			}
		}

		FVector spawnPos = { this->GetActorLocation().X, this->GetActorLocation().Y, (this->GetActorLocation().Z + SpawnHeight) };

		FRotator spawnRot = UKismetMathLibrary::FindLookAtRotation(spawnPos, inRange->GetActorLocation());
		FActorSpawnParameters spawnSett;
		spawnSett.bNoFail = true;

		//Maybe particle instead;
		AActor* spawnRef = GetWorld()->SpawnActor<AActor>(BulletToSpawn, spawnPos, spawnRot, spawnSett);

		if (spawnRef == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bullet spawnref nullptr"));
		}

		ExplosionSphere->SetWorldLocation({ start.X, start.Y, GetActorLocation().Z });
		ExplosionSphere->SetVisibility(true);
		ShoxExplode = true;

		//static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
		//ExplosionVisu = Particle.Object;

		//if(ExplosionVisu)
		//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVisu, start);

		//const FName TraceTag("MyTraceTag");
		//GetWorld()->DebugDrawTraceTag = TraceTag;
		//FCollisionQueryParams CollisionParams;
		//CollisionParams.TraceTag = TraceTag;
		//DrawDebugSphere(GetWorld(), start, AOERadius, 64, FColor(255, 0, 0), false, 0.1f, 0, 3);

		atkTimer = Frequency;
		canAtk = false;
	}
}

void ATurret::OnMyOverlapBegin(class UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//if (!FireTimer.IsValid())
	//{
	//	GetWorldTimerManager().SetTimer(FireTimer, this, &ATurret::LaunchProjectile, Frequency, true, 0.f);
	//}
}

void ATurret::OnMyOverlapEnd(class UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//TArray<AActor*> enemiesInRange;
	//SphereTrigger->GetOverlappingActors(enemiesInRange);

	//if (enemiesInRange.Num() == 0)
	//{
	//	GetWorldTimerManager().ClearTimer(FireTimer);
	//}
}

float ATurret::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HPComp->RemoveHP(DamageAmount);

	return DamageAmount;
}