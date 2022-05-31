// Fill out your copyright notice in the Description page of Project Settings.

#include "Minion.h"
#include "Turret.h"
#include "Laserbeam.h"
#include "SlowField.h"
#include "HPComponent.h"

//#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime\Engine\Classes\Kismet\KismetMathLibrary.h>

// Sets default values
AMinion::AMinion()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HPComp = CreateDefaultSubobject<UHPComponent>(FName("HPComponent"));

	SphereTrigger = CreateDefaultSubobject<USphereComponent>(FName("SphereTrigger"));
	if (SphereTrigger != nullptr)
	{
		SphereTrigger->SetupAttachment(this->GetRootComponent());
		//SphereTrigger->SetSphereRadius(AtkRange);
		SphereTrigger->SetGenerateOverlapEvents(true);
		//SphereTrigger->SetCollisionResponseToChannel(ECollisionChannel::, ECollisionResponse::ECR_Overlap);
		SphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMinion::OnMyOverlapBegin);
		SphereTrigger->OnComponentEndOverlap.AddDynamic(this, &AMinion::OnMyOverlapEnd);
	}
}

// Called when the game starts or when spawned
void AMinion::BeginPlay()
{
	Super::BeginPlay();

	this->GetCharacterMovement()->JumpZVelocity = 0;

	ActivePlayer = UGameplayStatics::GetPlayerPawn(this, 0);

	if (ActivePlayer == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player not found!"));
	}
}

// Called every frame
void AMinion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HPComp->GetCurrentHP() <= 0)
	{
		APlayerController* myContr = Cast<APlayerController>(this->GetController());
		if (myContr != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Minion unpossess!"));
			myContr->UnPossess();
		}
		UE_LOG(LogTemp, Warning, TEXT("FAIL Minion unpossess!"));
		AnimDead = true;
		this->SetLifeSpan(1.f);
		SetActorTickEnabled(false);
	}

	//UpdateOverlapped();
}

// Called to bind functionality to input
void AMinion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMinion::OnMyOverlapBegin(UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == ActivePlayer && !TurretInRange)
	{
		PlayerInRange = true;
		GetWorldTimerManager().SetTimer(AtkPlayerT, this, &AMinion::AggroPlayer, AtkFrequency, true, 0.5f);
	}

	if (OtherComp->ComponentHasTag("BodyTrigger") && OtherActor->ActorHasTag("Structure") && !TurretInRange)
	{
		ActiveTurret = OtherActor;
		TurretInRange = true;
		GetWorldTimerManager().SetTimer(AtkTurrT, this, &AMinion::AggroTurret, AtkFrequency, true, 0.5f);
	}

	if (OtherActor->ActorHasTag("Nexus"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("NEXUS OVERLAPPED W MINION"));
		ActiveNexus = OtherActor;
		NexusInRange = true;
		GetWorldTimerManager().SetTimer(AtkNexusT, this, &AMinion::AggroNexus, AtkFrequency, true, 0.5f);
	}
}

void AMinion::OnMyOverlapEnd(UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == ActivePlayer)
	{
		PlayerInRange = false;
		GetWorldTimerManager().ClearTimer(AtkPlayerT);
	}

	/*(OtherActor->IsA<ATurret>() || OtherActor->IsA<ASlowField>() || OtherActor->IsA<ALaserbeam>())*/
	if (OtherActor->ActorHasTag("Structure") && OtherActor == ActiveTurret)
	{
		TurretInRange = false;
		GetWorldTimerManager().ClearTimer(AtkTurrT);
	}

	if (OtherActor->ActorHasTag("Nexus"))
	{
		NexusInRange = false;
		GetWorldTimerManager().ClearTimer(AtkNexusT);
	}
}

float AMinion::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HPComp->RemoveHP(DamageAmount);

	return DamageAmount;
}

void AMinion::AggroPlayer()
{
	if (PlayerInRange && ActivePlayer != nullptr)
	{
		FVector toTarg = ActivePlayer->GetActorLocation() - GetActorLocation();
		float angle = ((acosf(FVector::DotProduct(toTarg.GetSafeNormal(), GetActorForwardVector()))) * (180.f / 3.1415926f));

		if (angle <= 45.f && (toTarg.Size() < AtkRange))
		{
			AnimAtk = true;
			GetWorldTimerManager().SetTimer(AtkDelay, this, &AMinion::AtkPlayer, 0.25f, false, 0.5f);
		}
	}
}

void AMinion::AtkPlayer()
{
	if (PlayerInRange && ActivePlayer != nullptr)
	{
		FDamageEvent dmgEvent;
		ActivePlayer->TakeDamage(Damage, dmgEvent, GetInstigatorController(), this);
	}

	AnimAtk = false;
}

void AMinion::AggroTurret()
{
	if (TurretInRange && ActiveTurret != nullptr)
	{
		FVector toTarg = ActiveTurret->GetActorLocation() - GetActorLocation();

		if (toTarg.Size() < AtkRange)
		{
			AnimAtk = true;
			GetWorldTimerManager().SetTimer(AtkDelay, this, &AMinion::AtkTurret, 0.25f, false, 0.5f);
		}
	}
}

void AMinion::AtkTurret()
{
	if (TurretInRange && ActiveTurret != nullptr)
	{
		FDamageEvent dmgEvent;
		ActiveTurret->TakeDamage(Damage * DmgMult, dmgEvent, GetInstigatorController(), this);
	}

	AnimAtk = false;
}

void AMinion::AggroNexus()
{
	if (NexusInRange && ActiveNexus != nullptr)
	{
		FVector toTarg = ActiveNexus->GetActorLocation() - GetActorLocation();

		if (toTarg.Size() < AtkRange + OffsetNexusSize)
		{
			AnimAtk = true;
			GetWorldTimerManager().SetTimer(AtkDelay, this, &AMinion::AtkNexus, 0.25f, false, 0.5f);
		}
	}
}

void AMinion::AtkNexus()
{
	if (NexusInRange && ActiveNexus != nullptr)
	{
		FDamageEvent dmgEvent;
		ActiveNexus->TakeDamage(Damage * DmgMult, dmgEvent, GetInstigatorController(), this);
		//UE_LOG(LogTemp, Warning, TEXT("Nexus boom minion"));
	}

	AnimAtk = false;
}

void AMinion::UpdateOverlapped()
{
	TArray<AActor*> RLInZone;
	TArray<AActor*> LBInZone;
	TArray<AActor*> GFInZone;
	SphereTrigger->GetOverlappingActors(RLInZone, ATurret::StaticClass());
	SphereTrigger->GetOverlappingActors(LBInZone, ALaserbeam::StaticClass());
	SphereTrigger->GetOverlappingActors(GFInZone, ASlowField::StaticClass());

	StructInZone.Empty();
	if (RLInZone.Num() > 0)
	{
		for (auto turr : RLInZone)
		{
			StructInZone.Emplace(turr);
		}
	}
	if (LBInZone.Num() > 0)
	{
		for (auto turr : LBInZone)
		{
			StructInZone.Emplace(turr);
		}
	}
	if (GFInZone.Num() > 0)
	{
		for (auto turr : GFInZone)
		{
			StructInZone.Emplace(turr);
		}
	}

	if (StructInZone.Num() <= 0)
	{
		AtkTurrT.IsValid() ? GetWorldTimerManager().ClearTimer(AtkTurrT) : (void)AtkTurrT;
		return;
	}
	float dist = FVector::Distance(GetActorLocation(), StructInZone[0]->GetActorLocation());

	for (auto turr : StructInZone)
	{
		if (float newDist = FVector::Distance(GetActorLocation(), turr->GetActorLocation()) < dist)
		{
			dist = newDist;
			ActiveTurret = turr;
		}
	}
	if (ActiveTurret != nullptr)
		GetWorldTimerManager().SetTimer(AtkTurrT, this, &AMinion::AggroTurret, AtkFrequency, true, 0.5f);
}