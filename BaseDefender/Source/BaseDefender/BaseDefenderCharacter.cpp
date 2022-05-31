// Copyright Epic Games, Inc. All Rights Reserved.

#include "BaseDefenderCharacter.h"

#include <openexr/Deploy/OpenEXR-2.3.0/OpenEXR/include/ImathMath.h>

#include "TurretPH.h"
#include "HPComponent.h"
#include "Minion.h"
#include "Raider.h"
#include "Turret.h"
#include "Laserbeam.h"
#include "SlowField.h"

#include "Components/SphereComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include <Runtime\Engine\Classes\Kismet\KismetMathLibrary.h>

//////////////////////////////////////////////////////////////////////////
// ABaseDefenderCharacter

ABaseDefenderCharacter::ABaseDefenderCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	HPComp = CreateDefaultSubobject<UHPComponent>(FName("HPComponent"));

	SphereTrigger = CreateDefaultSubobject<USphereComponent>(FName("SphereTrigger"));
	if (SphereTrigger != nullptr)
	{
		SphereTrigger->SetupAttachment(this->GetRootComponent());
		SphereTrigger->SetSphereRadius(AtkRange);
		SphereTrigger->SetGenerateOverlapEvents(true);
		SphereTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABaseDefenderCharacter::OnMyOverlapBegin);
		SphereTrigger->OnComponentEndOverlap.AddDynamic(this, &ABaseDefenderCharacter::OnMyOverlapEnd);
	}

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	CameraRotator = { -45, 0, 0 };
	CameraTranslator = { 0, 0, 1250 };
	FollowCamera->SetRelativeLocation(CameraTranslator);
	FollowCamera->SetRelativeRotationExact(CameraRotator);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	atkTimer = AtkCooldown;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABaseDefenderCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("BasicAtk", IE_Pressed, this, &ABaseDefenderCharacter::MeleeAtk);
	PlayerInputComponent->BindAction("RocketLauncher", IE_Pressed, this, &ABaseDefenderCharacter::SpawnStruct<1>);
	PlayerInputComponent->BindAction("GravityField", IE_Pressed, this, &ABaseDefenderCharacter::SpawnStruct<2>);
	PlayerInputComponent->BindAction("LaserBeam", IE_Pressed, this, &ABaseDefenderCharacter::SpawnStruct<3>);
	//PlayerInputComponent->BindAction("PhaseShift", IE_Pressed, this, &ABaseDefenderCharacter::PhaseShift);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseDefenderCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseDefenderCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &ABaseDefenderCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABaseDefenderCharacter::LookUpAtRate);
	PlayerInputComponent->BindAxis("MouseScroll", this, &ABaseDefenderCharacter::ZoomScroll);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABaseDefenderCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABaseDefenderCharacter::TouchStopped);
}

void ABaseDefenderCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	//Begin;
}

void ABaseDefenderCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	//Stop;
}

void ABaseDefenderCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABaseDefenderCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABaseDefenderCharacter::ZoomScroll(float Rate)
{
	float scrollRate = Rate * ScrollMult;
	CameraTranslator.Z -= scrollRate;
}

void ABaseDefenderCharacter::CameraUpdate(float DeltaTime)
{
	FVector playerPos;
	playerPos = this->GetActorLocation();

	if (CameraTranslator.Z < MinCamHeight)
	{
		CameraTranslator.Z = MinCamHeight;
	}
	if (CameraTranslator.Z > MaxCamHeight)
	{
		CameraTranslator.Z = MaxCamHeight;
	}

	FollowCamera->SetRelativeLocation({ CameraTranslator.X, CameraTranslator.Y, FMath::Clamp(CameraTranslator.Z, MinCamHeight, MaxCamHeight) });
	FRotator CamRot = UKismetMathLibrary::FindLookAtRotation(FollowCamera->GetComponentLocation(), playerPos);
	//FRotator NewRot = FMath::RInterpTo(FollowCamera->GetComponentRotation(), CamRot, DeltaTime, 3.f);
	FollowCamera->SetRelativeRotationExact({ CamRot.Pitch, 0, 0 });
}

void ABaseDefenderCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABaseDefenderCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

// Called every frame
void ABaseDefenderCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GODMODE)
		HPComp->InitHP();

	if (HPComp->GetCurrentHP() <= 0)
	{
		APlayerController* playerCont = Cast<APlayerController>(this->GetController());
		if (playerCont != nullptr)
		{
			FTransform camTransform = FollowCamera->GetRelativeTransform();
			FInputModeUIOnly InputMode;
			playerCont->SetInputMode(InputMode);
			//playerCont->bAutoManageActiveCameraTarget = false;
			//playerCont->UnPossess();
			//FollowCamera->SetRelativeTransform(camTransform);
		}
		AnimDead = true;
		TotalTimeSurvived = TimeSpent;
		UE_LOG(LogTemp, Warning, TEXT("Total time : %f"), TotalTimeSurvived);

		this->SetLifeSpan(3.f);
		SetActorTickEnabled(false);
	}

	if (!canMeleeAtk)
		atkTimer -= DeltaTime;

	if (atkTimer <= 0)
	{
		AnimAtk = false;
		canMeleeAtk = true;
	}

	GameDone();

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	//Camera
	CameraUpdate(DeltaTime);

	if (pShifted && !canShift)
	{
		//UE_LOG(LogTemp, Warning, TEXT("JUST SHIFTED"));
		AOEAtk();
	}

	UpdateActivesStructs();
}

//void ABaseDefenderCharacter::PhaseShift()
//{
//	float mouseX;
//	float mouseY;
//	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetMousePosition(mouseX, mouseY);
//	FVector2D vpSize;
//	this->GetWorld()->GetGameViewport()->GetViewportSize(vpSize);
//	FMath::Clamp(mouseX, 0.f, vpSize.X);
//	FMath::Clamp(mouseX, 0.f, vpSize.Y);
//	float halfX = vpSize.X / 2;
//	float halfY = vpSize.Y / 2;
//	mouseX = mouseX > halfX ? (mouseX - halfX) : -(halfX - mouseX);
//	mouseY = mouseY > halfY ? (mouseY - halfY) : -(halfY - mouseY);
//
//	mouseX = mouseX / halfX;
//	mouseY = mouseY / halfY;
//
//	float moveX = mouseX * PhaseShiftMult;
//	float moveY = mouseY * PhaseShiftMult;
//
//	this->SetActorLocation(this->GetActorLocation() + FVector{ -moveY , moveX, 0 });
//}

void ABaseDefenderCharacter::SpawnStruct(int type)
{
	ATurretPH::TurretType tType;
	switch (type)
	{
	case 1:
		tType = ATurretPH::TurretType::ROCKETLAUNCHER;
		break;
	case 2:
		tType = ATurretPH::TurretType::GRAVITYFIELD;
		break;
	case 3:
		tType = ATurretPH::TurretType::LASERBEAM;
		break;
	default:
		return;
	}
	TArray<AActor*> turretsPH;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurretPH::StaticClass(), turretsPH);

	if (turretsPH.Num() != 0)
	{
		for (auto turrSpawn : turretsPH)
		{
			Cast<ATurretPH>(turrSpawn)->CallSpawn(tType);
		}
	}
}

float ABaseDefenderCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	IsDamaged = true;

	HPComp->RemoveHP(DamageAmount);

	return DamageAmount;
}

void ABaseDefenderCharacter::MeleeAtk()
{
	if (!canMeleeAtk)
		return;

	AnimAtk = true;
	TArray<AActor*> MinionsInZone;
	TArray<AActor*> RaidersInZone;

	SphereTrigger->GetOverlappingActors(MinionsInZone, AMinion::StaticClass());
	SphereTrigger->GetOverlappingActors(RaidersInZone, ARaider::StaticClass());

	EnemiesInZone.Empty();
	if (MinionsInZone.Num() > 0)
	{
		for (auto enemy : MinionsInZone)
		{
			EnemiesInZone.Emplace(enemy);
		}
	}
	if (RaidersInZone.Num() > 0)
	{
		for (auto enemy : RaidersInZone)
		{
			EnemiesInZone.Emplace(enemy);
		}
	}

	if (EnemiesInZone.Num() > 0)
	{
		FVector toTarg;
		AActor* target = EnemiesInZone[0];
		float closest = FVector::Distance(GetActorLocation(), target->GetActorLocation());

		//Changed to 0 for the angle check
		for (int i = 0; i < EnemiesInZone.Num(); i++)
		{
			toTarg = EnemiesInZone[i]->GetActorLocation() - GetActorLocation();

			float angle = ((acosf(FVector::DotProduct(toTarg.GetSafeNormal(), GetActorForwardVector()))) * (180.f / 3.1415926f));
			//UE_LOG(LogTemp, Warning, TEXT("Angle : %f"), angle)
			if (angle <= 45.f)
			{
				if (float distance = FVector::Distance(GetActorLocation(), EnemiesInZone[i]->GetActorLocation()) < closest)
				{
					target = EnemiesInZone[i];
					closest = distance;
				}
			}
		}

		//float checkAngle = ((acosf(FVector::DotProduct(target->GetActorLocation().GetSafeNormal(), GetActorForwardVector()))) * (180.f / 3.1415926f));

		//UE_LOG(LogTemp, Warning, TEXT("CheckAngle : %f"), checkAngle)
		//if (checkAngle > 45.f)
		//	return;

		FDamageEvent dmgEvent;
		target->TakeDamage(AtkDamage, dmgEvent, GetInstigatorController(), this);
	}
	canMeleeAtk = false;
	atkTimer = AtkCooldown;
}

void ABaseDefenderCharacter::AOEAtk()
{
	CameraBoom->SetupAttachment(RootComponent);
	pShifted = false;

	FDamageEvent dmgEvent;

	if (EnemiesInZone.Num() > 0)
	{
		for (int i = 0; i < EnemiesInZone.Num(); i++)
		{
			EnemiesInZone[i]->TakeDamage(AOEDamage, dmgEvent, GetInstigatorController(), this);
		}
	}
	psCDleft = PSCooldown;
	GetWorldTimerManager().SetTimer(psTimer, this, &ABaseDefenderCharacter::ApplyCD, 1, true, 0.f);
}

void ABaseDefenderCharacter::UpdateActivesStructs()
{
	TArray<AActor*> turrets;
	TArray<AActor*> gField;
	TArray<AActor*> lBeam;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATurret::StaticClass(), turrets);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASlowField::StaticClass(), gField);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALaserbeam::StaticClass(), lBeam);

	ActiveStructures.Empty();
	if (turrets.Num() > 0)
	{
		for (auto turr : turrets)
		{
			ActiveStructures.Emplace(turr);
		}
	}
	if (gField.Num() > 0)
	{
		for (auto turr : gField)
		{
			ActiveStructures.Emplace(turr);
		}
	}
	if (lBeam.Num() > 0)
	{
		for (auto turr : lBeam)
		{
			ActiveStructures.Emplace(turr);
		}
	}
}

void ABaseDefenderCharacter::ApplyCD()
{
	psCDleft -= 1;

	if (psCDleft <= 0)
	{
		canShift = true;
		GetWorldTimerManager().ClearTimer(psTimer);
	}
}

void ABaseDefenderCharacter::GameDone()
{
	if (!WinCondition)
	{
		if (TimeSpent >= SecondsToVictory)
		{
			TotalTimeSurvived = TimeSpent;
			WinCondition = true;
		}
	}
}

FString ABaseDefenderCharacter::ConvertTime(float time)
{
	float tSeconds = time;
	std::string ss = std::to_string((static_cast<int>(tSeconds) % 60));
	std::string mm = std::to_string(static_cast<int>(floor(tSeconds / 60.f)) % 60);
	std::string hh = std::to_string(static_cast<int>(floor(tSeconds / 60.f / 60.f)) % 60);
	std::string result = hh + ":" + mm + ":" + ss;

	return FString(result.c_str());
}

void ABaseDefenderCharacter::OnMyOverlapBegin(class UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AMinion>() || OtherActor->IsA<ARaider>())
	{
		EnemiesInZone.Add(OtherActor);
	}
}

void ABaseDefenderCharacter::OnMyOverlapEnd(class UPrimitiveComponent* oComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EnemiesInZone.RemoveSwap(OtherActor);
}