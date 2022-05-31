// Fill out your copyright notice in the Description page of Project Settings.

#include "Raider.h"
#include "HPComponent.h"
#include "Nexus.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ARaider::ARaider()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HPComp = CreateDefaultSubobject<UHPComponent>(FName("HPComponent"));

	SphereTrigger = CreateDefaultSubobject<USphereComponent>(FName("SphereTrigger"));
	if (SphereTrigger != nullptr)
	{
		SphereTrigger->SetupAttachment(this->GetRootComponent());
		SphereTrigger->SetSphereRadius(AtkRange);
		SphereTrigger->SetGenerateOverlapEvents(true);
		SphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARaider::OnMyOverlapBegin);
		SphereTrigger->OnComponentEndOverlap.AddDynamic(this, &ARaider::OnMyOverlapEnd);
	}
}

// Called when the game starts or when spawned
void ARaider::BeginPlay()
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
void ARaider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HPComp->GetCurrentHP() <= 0)
	{
		APlayerController* myContr = Cast<APlayerController>(this->GetController());
		if (myContr != nullptr)
		{
			myContr->UnPossess();
		}
		AnimDead = true;
		this->SetLifeSpan(1.f);
		SetActorTickEnabled(false);
	}
}

// Called to bind functionality to input
void ARaider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARaider::OnMyOverlapBegin(UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == ActivePlayer && NexusInRange == false)
	{
		ActivePlayer = OtherActor;
		PlayerInRange = true;
		GetWorldTimerManager().SetTimer(AtkPlayerT, this, &ARaider::AggroPlayer, AtkFrequency, true, 0.5f);
	}

	if (OtherActor->IsA<ANexus>() || OtherActor->ActorHasTag("Nexus"))
	{
		//UE_LOG(LogTemp, Warning, TEXT("NEXUS OVERLAPPED W RAIDER"));
		ActiveNexus = OtherActor;
		NexusInRange = true;
		GetWorldTimerManager().SetTimer(AtkNexusT, this, &ARaider::AggroNexus, AtkFrequency, true, 0.5f);
	}
}

void ARaider::OnMyOverlapEnd(UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == ActivePlayer)
	{
		PlayerInRange = false;
		GetWorldTimerManager().ClearTimer(AtkPlayerT);
	}

	if (OtherActor->IsA<ANexus>() || OtherActor->ActorHasTag("Nexus"))
	{
		NexusInRange = false;
		GetWorldTimerManager().ClearTimer(AtkNexusT);
	}
}

float ARaider::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HPComp->RemoveHP(DamageAmount);

	return DamageAmount;
}

void ARaider::AggroPlayer()
{
	if (PlayerInRange && ActivePlayer != nullptr)
	{
		FVector toTarg = ActivePlayer->GetActorLocation() - GetActorLocation();
		float angle = ((acosf(FVector::DotProduct(toTarg.GetSafeNormal(), GetActorForwardVector()))) * (180.f / 3.1415926f));

		if (angle <= 45.f && (toTarg.Size() < AtkRange))
		{
			AnimAtk = true;
			GetWorldTimerManager().SetTimer(AtkDelay, this, &ARaider::AtkPlayer, 0.25f, false, 0.5f);
		}
	}
}

void ARaider::AtkPlayer()
{
	if (PlayerInRange && ActivePlayer != nullptr)
	{
		FDamageEvent dmgEvent;
		ActivePlayer->TakeDamage(Damage, dmgEvent, GetInstigatorController(), this);
	}

	AnimAtk = false;
}

void ARaider::AggroNexus()
{
	if (NexusInRange && ActiveNexus != nullptr)
	{
		FVector toTarg = ActiveNexus->GetActorLocation() - GetActorLocation();
		float angle = ((acosf(FVector::DotProduct(toTarg.GetSafeNormal(), GetActorForwardVector()))) * (180.f / 3.1415926f));

		if (/*angle <= 45.f &&*/ (toTarg.Size() < (AtkRange + OffsetNexusSize)))
		{
			AnimAtk = true;
			GetWorldTimerManager().SetTimer(AtkDelay, this, &ARaider::AtkNexus, 0.25f, false, 0.5f);
		}
	}
}

void ARaider::AtkNexus()
{
	if (NexusInRange && ActiveNexus != nullptr)
	{
		FDamageEvent dmgEvent;
		ActiveNexus->TakeDamage(Damage, dmgEvent, GetInstigatorController(), this);
		//UE_LOG(LogTemp, Warning, TEXT("Nexus boom raider"));
	}

	AnimAtk = false;
}