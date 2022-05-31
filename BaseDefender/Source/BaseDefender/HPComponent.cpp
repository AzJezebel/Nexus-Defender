// Fill out your copyright notice in the Description page of Project Settings.

#include "HPComponent.h"

// Sets default values for this component's properties
UHPComponent::UHPComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//CurrentHPComp = MaxHPComp;
}

// Called when the game starts
void UHPComponent::BeginPlay()
{
	Super::BeginPlay();

	InitHP();
}

// Called every frame
void UHPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

const float UHPComponent::GetCurrentHP()
{
	return CurrentHPComp;
}

void UHPComponent::InitHP()
{
	CurrentHPComp = MaxHPComp;
}

void UHPComponent::AddHP(float hp)
{
	CurrentHPComp += hp;
}

void UHPComponent::RemoveHP(float hp)
{
	CurrentHPComp -= hp;
}