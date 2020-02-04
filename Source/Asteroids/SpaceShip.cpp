// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip.h"
#include "SpaceshipProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "TimerManager.h"



ASpaceShip::ASpaceShip()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());	// Set static mesh
	RootComponent = PlaneMesh;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);	// Attach SpringArm to RootComponent
	SpringArm->SocketOffset = FVector(0.f, 0.f, 150.f);
	SpringArm->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	SpringArm->bEnableCameraLag = true;	// Do not allow camera to lag
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach the camera
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 200.f;
	TurnSpeed = 150.f;
	MaxSpeed = 5000.f;
	MinSpeed = 200.f;
	CurrentForwardSpeed = MinSpeed;
	
	FireRate = 0.1f;
	bCanFire = true;

	InitialHP = 1.0;
	CurrentHP = InitialHP;

	InitialTimeLeft = 100.f;
	CurrentTimeLeft = InitialTimeLeft;

	InitialProgress = 0.0f;
	CurrentProgress = InitialProgress;

	
}

// Called when the game starts or when spawned
void ASpaceShip::BeginPlay()
{
	Super::BeginPlay();
	IsDeath = false;

	OnActorHit.AddDynamic(this, &ASpaceShip::OnSpaceShipHit);

	FTimerHandle OutHandle;

	//Will call SpawnUsefulActor after the specified time
	GetWorld()->GetTimerManager().SetTimer(OutHandle, this, &ASpaceShip::SpawnEnemy, TimeToSpawn, true);
}

void ASpaceShip::Tick(float DeltaSeconds)
{
	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);

	CurrentTimeLeft -= DeltaSeconds * 0.01f * InitialTimeLeft;
	if (CurrentTimeLeft <= 0.01f) {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, "No Time Left. Game Over!");
		Destroy();
	}

	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	// Create fire direction vector
	float FireForwardValue = IsFirePushed ? 1 : 0;
	const FVector FireDirection = FVector(FireForwardValue, FireForwardValue, 0.f);

	if (IsFirePushed) IsFirePushed = false;
	
	bCanFire = FireShot(FireDirection, bCanFire);
}

void ASpaceShip::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}


void ASpaceShip::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Check if PlayerInputComponent is valid (not NULL)
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("Thrust", this, &ASpaceShip::ThrustInput);
	PlayerInputComponent->BindAxis("MoveUp", this, &ASpaceShip::MoveUpInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpaceShip::MoveRightInput);
	PlayerInputComponent->BindAxis("Roll", this, &ASpaceShip::Roll);

	// Bind our action to callback functions
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ASpaceShip::Fire);
}

void ASpaceShip::ThrustInput(float Val)
{
	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void ASpaceShip::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	// TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void ASpaceShip::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	// const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value.
	// float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	// CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void ASpaceShip::Roll(float Val)
{
	// Target roll speed is based on input
	float TargetRollSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void ASpaceShip::SpawnEnemy()
{
	//If the EnemyBP is valid
	if (EnemyBP)
	{
		// move to SpaceShip 
		const FRotator SpawnRotation = GetActorRotation().Add(180.f, 0.f, 0.f);

		// Spawn location
		const FVector SpawnLocation = GetActorLocation() + GetActorRotation().RotateVector(FVector(5000.f, 0.f, -100.f));

		TWeakObjectPtr<UWorld> World = GetWorld();

		if (World != NULL)
		{
			// spawn the projectile
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.bNoFail = true;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this;

			AAsteroidEnemy* Enemy = World->SpawnActor<AAsteroidEnemy>(EnemyBP, SpawnLocation, SpawnRotation, SpawnParams);

		}

	}
}

void ASpaceShip::OnSpaceShipHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	
	if (AAsteroidEnemy* Enemy = Cast<AAsteroidEnemy>(OtherActor))
	{

		if (!Enemy->IsDestroyed)
		{
			UpdateCurrentHP(-.2f);
			if (GetCurrentHP() <= 0.01f) {
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "No HP. Game Over!");
				Destroy();
			}
		}

		Enemy->IsDestroyed = true;
	}
}

void ASpaceShip::Fire()
{
	IsFirePushed = true;
}

bool ASpaceShip::FireShot(FVector FireDirection, bool canFire)
{
	bool canFireToReturn = canFire;
	// If it's ok to fire again
	if (canFireToReturn == true)
	{
		
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			// const FRotator FireRotation = FireDirection.Rotation();
			const FRotator FireRotation = GetActorRotation();
			// Spawn projectile at an offset from this pawn
			
			// const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(FVector(300.f, 0.f, 0.f));
			const FVector SpawnLocation = GetActorLocation() + GetActorRotation().RotateVector(FVector(200.f, 0.f, 0.f));
			TWeakObjectPtr<UWorld> World = GetWorld();

			if (World != NULL)
			{
				// spawn the projectile
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.bNoFail = true;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = this;

				TWeakObjectPtr<ASpaceshipProjectile> SpaceshipProjectile = World->SpawnActor<ASpaceshipProjectile>(SpawnLocation, FireRotation, SpawnParams);
				SpaceshipProjectile->SetSpaceShip(this);

				World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ASpaceShip::ShotTimerExpired, FireRate);
			}

			canFireToReturn = false;

			// try and play the sound if specified
			//if (FireSound != nullptr)
			//{
			//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			//}

			canFireToReturn = false;
		}
	}
	return canFireToReturn;
}

void ASpaceShip::ShotTimerExpired()
{
	bCanFire = true;
}

float ASpaceShip::GetCurrentForwardSpeed()
{
	return CurrentForwardSpeed;
}
