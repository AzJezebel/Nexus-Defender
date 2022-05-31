// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Minion.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	projectile = CreateDefaultSubobject<UStaticMeshComponent>(FName("Projectile"));
	SetRootComponent(projectile);

	projectile->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	
	movement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Movement"));
	if (movement != nullptr)
	{
		movement->ProjectileGravityScale = 0;
	}
	
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	movement->SetVelocityInLocalSpace(FVector::ForwardVector * movement->InitialSpeed);
	movement->Activate();
	this->SetLifeSpan(3.f);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AActor* activePlayer = UGameplayStatics::GetPlayerPawn(this, 0);
	if (activePlayer == nullptr)
		return;
	
	if (this->GetActorLocation().Z < activePlayer->GetActorLocation().Z)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Bullet Stopped, should be destroyed and trigger particle effect"));
		this->Destroy();
	}
}

void AProjectile::SetRot(FRotator& rot)
{
	projectile->SetRelativeRotationExact(rot);
	UE_LOG(LogTemp, Warning, TEXT("Projectile rotation : (%f, %f, %f)"), rot.Pitch, rot.Yaw, rot.Roll);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s HIT"), *OtherActor->GetName());

	FDamageEvent dmgEvent;
	OtherActor->TakeDamage(Damage, dmgEvent, GetInstigatorController(), this);
	
}