// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <string>
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "BaseDefenderCharacter.generated.h"

class UHPComponent;
class USphereComponent;

UCLASS(config = Game)
class ABaseDefenderCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

public:
	ABaseDefenderCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GAME, meta = (ClampMin = "3"))
		float SecondsToVictory = 60.f;
	
	UPROPERTY(EditAnywhere, Category = SPELL)
		bool GODMODE = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = BASE)
		USphereComponent* SphereTrigger = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BASE)
		UHPComponent* HPComp = nullptr;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CAMERA)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CAMERA)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAMERA)
		float ScrollMult = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAMERA)
		float MinCamHeight = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAMERA, meta = (ClampMin = "2", ClampMax = "99999"))
		float MaxCamHeight = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAMERA, meta = (ClampMin = "-45", ClampMax = "45"))
		FRotator CameraRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CAMERA, meta = (ClampMin = "-1500", ClampMax = "1500"))
		FVector CameraTranslator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SPELL, meta = (ClampMin = "1"))
		int PSCooldown = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SPELL)
		float AtkCooldown = 1;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SPELL, meta = (ClampMin = "10", ClampMax = "1000"))
	//	float PhaseShiftMult = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SPELL, meta = (ClampMin = "1", ClampMax = "1000"))
		float AtkRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SPELL, meta = (ClampMin = "1"))
		float AtkDamage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SPELL, meta = (ClampMin = "1"))
		float AOEDamage = 100.f;

	UFUNCTION(BlueprintCallable)
		TArray<AActor*> GetActivesStruct() const { return ActiveStructures; }

	UFUNCTION(BlueprintCallable)
		float GetNumberOfActivesStruct() const { return NumOfStructs; }

	UFUNCTION(BlueprintCallable)
		float GetCurrentPSCD() const { return PSCooldown - psCDleft; }

	UFUNCTION(BlueprintCallable)
		float GetCurrentAtkCD() const { return AtkCooldown - atkTimer; }

	UFUNCTION(BlueprintCallable)
		FString ConvertTime(float time);
	
	UFUNCTION(BlueprintCallable)
		void SetTotalTine(float time) { TotalTimeSurvived = time; }

	UFUNCTION(BlueprintCallable)
		float GetTotalTime() { return TotalTimeSurvived; }

	UPROPERTY(BlueprintReadWrite)
		bool canShift = true;
	
	UPROPERTY(BlueprintReadWrite)
		bool WinCondition = false;

	UPROPERTY(BlueprintReadWrite)
		bool pShifted = false;

	UPROPERTY(BlueprintReadWrite)
		bool IsDamaged = false;
	
	UPROPERTY(BlueprintReadWrite)
		float TimeSpent = 0.f;
	
	void SetAllowAtk(bool b) { canMeleeAtk = b; }
	bool AnimAtk = false;
	bool AnimDead = false;

protected:
	void PhaseShift();
	void SpawnStruct(int type);
	template<int Index>
	void SpawnStruct()
	{
		SpawnStruct(Index);
	}
	void ZoomScroll(float Rate);
	void CameraUpdate(float DeltaTime);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void MeleeAtk();
	void AOEAtk();
	void UpdateActivesStructs();
	void ApplyCD();
	void GameDone();

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION()
		void OnMyOverlapBegin(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnMyOverlapEnd(class UPrimitiveComponent* oComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	TArray<AActor*> EnemiesInZone;
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetActivesStruct)
		TArray<AActor*> ActiveStructures;
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetNumberOfActivesStruct)
		float NumOfStructs;

	FTimerHandle psTimer;
	float atkTimer;
	int psCDleft = 0;
	bool canMeleeAtk = true;
	float TotalTimeSurvived = 0.f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
