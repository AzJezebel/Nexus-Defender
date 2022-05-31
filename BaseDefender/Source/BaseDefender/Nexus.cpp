// Fill out your copyright notice in the Description page of Project Settings.


#include "Nexus.h"
#include "HPComponent.h"

// Sets default values
ANexus::ANexus()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HPComp = CreateDefaultSubobject<UHPComponent>(FName("HPComponent"));
}

// Called when the game starts or when spawned
void ANexus::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANexus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HPComp->GetCurrentHP() <= 0)
		this->Destroy();
}

float ANexus::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HPComp->RemoveHP(DamageAmount);
		
	return DamageAmount;
}