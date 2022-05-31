// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class BASEDEFENDER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UStaticMeshComponent* projectile = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UProjectileMovementComponent* movement;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SPECS)
		float Damage = 50.f;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetRot(FRotator& rot);
};