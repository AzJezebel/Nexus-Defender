// Fill out your copyright notice in the Description page of Project Settings.

#include "SlowField.h"
#include "Minion.h"
#include "Raider.h"
#include "HPComponent.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASlowField::ASlowField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	body = CreateDefaultSubobject<UStaticMeshComponent>(FName("TurretBody"));
	SetRootComponent(body);

	HPComp = CreateDefaultSubobject<UHPComponent>(FName("HPComponent"));

	SphereTrigger = CreateDefaultSubobject<USphereComponent>(FName("SphereTrigger"));
	if (SphereTrigger != nullptr)
	{
		SphereTrigger->SetupAttachment(body);
		SphereTrigger->SetSphereRadius(TurretRange);
		SphereTrigger->SetGenerateOverlapEvents(true);
		SphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASlowField::OnMyOverlapBegin);
		SphereTrigger->OnComponentEndOverlap.AddDynamic(this, &ASlowField::OnMyOverlapEnd);
	}
}

// Called when the game starts or when spawned
void ASlowField::BeginPlay()
{
	Super::BeginPlay();

	if (body == nullptr)
		return;
}

// Called every frame
void ASlowField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HPComp->GetCurrentHP() <= 0)
		this->Destroy();

	if (ActorsInZone.Num() > 0 && tickTimer <= Duration + Frequency)
		tickTimer += DeltaTime;

	if (tickTimer >= Duration + Frequency)
		tickTimer = 0;

	if (tickTimer <= Duration)
	{
		ZoneEffect();
	}
	else
	{
		RingVisible = false;
		CancelEffect();
	}
}

void ASlowField::ZoneEffect()
{
	if (ActorsInZone.Num() == 0)
		return;
	
	RingVisible = false;
	
	if (CallEvent)
		RingVisible = true;

	FDamageEvent dmgEvent;

	for (auto enemy : ActorsInZone)
	{
		if (enemy->IsA<AMinion>())
			Cast<AMinion>(enemy)->GetCharacterMovement()->MaxWalkSpeed = MaxSpeedSave * SlowModifier;
		else if (enemy->IsA<ARaider>())
			Cast<ARaider>(enemy)->GetCharacterMovement()->MaxWalkSpeed = MaxSpeedSave * SlowModifier;

		enemy->TakeDamage(Damage, dmgEvent, GetInstigatorController(), this);
	}
	CallEvent = false;
}

void ASlowField::CancelEffect()
{
	for (auto enemy : ActorsInZone)
	{
		if (enemy->IsA<AMinion>())
			Cast<AMinion>(enemy)->GetCharacterMovement()->MaxWalkSpeed = MaxSpeedSave;
		else if (enemy->IsA<ARaider>())
			Cast<ARaider>(enemy)->GetCharacterMovement()->MaxWalkSpeed = MaxSpeedSave;
	}
	CallEvent = true;
}

void ASlowField::ZoneTimer()
{
	if (!FireTimer.IsValid() && ActorsInZone.Num() > 0 && FirstCall == false)
	{
		GetWorldTimerManager().SetTimer(FireTimer, this, &ASlowField::ZoneEffect, Duration, true, 0.f);
		FirstCall = true;
	}
	GetWorldTimerManager().SetTimer(EffectTick, this, &ASlowField::ZoneEffect, 0.16f, true, 0.f);
	doTime = true;
}

void ASlowField::OnMyOverlapBegin(class UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AMinion>() || OtherActor->IsA<ARaider>())
	{
		ActorsInZone.Add(OtherActor);
		if (!SpeedSetted)
		{
			if (OtherActor->IsA<AMinion>())
				MaxSpeedSave = Cast<AMinion>(OtherActor)->GetCharacterMovement()->MaxWalkSpeed;
			else
				MaxSpeedSave = Cast<ARaider>(OtherActor)->GetCharacterMovement()->MaxWalkSpeed;

			SpeedSetted = true;
		}
	}
}

void ASlowField::OnMyOverlapEnd(class UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<AMinion>())
	{
		Cast<AMinion>(OtherActor)->GetCharacterMovement()->MaxWalkSpeed = MaxSpeedSave;
	}
	if (OtherActor->IsA<ARaider>())
	{
		Cast<ARaider>(OtherActor)->GetCharacterMovement()->MaxWalkSpeed = MaxSpeedSave;
	}

	ActorsInZone.RemoveSwap(OtherActor);

	if (ActorsInZone.Num() == 0)
	{
		GetWorldTimerManager().ClearTimer(FireTimer);
		FirstCall = false;
	}
}

float ASlowField::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HPComp->RemoveHP(DamageAmount);

	return DamageAmount;
}