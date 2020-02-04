// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AsteroidEnemy.h"
#include "SpaceShip.generated.h"

UCLASS()
class ASTEROIDS_API ASpaceShip : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* PlaneMesh;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;
public:
	ASpaceShip();

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float FireRate;

	// Begin AActor overrides
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides

	void Fire();

	/* Fire a shot in the specified direction (returns if the fire can continue)*/
	bool FireShot(FVector FireDirection, bool canFire);

	/* Handler for the fire timer expiry */
	UFUNCTION()
		void ShotTimerExpired();

	float GetCurrentForwardSpeed();

protected:

	virtual void BeginPlay() override;

	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	// End APawn overrides

	/** Bound to the thrust axis */
	void ThrustInput(float Val);

	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

	/** Bound to the roll */
	void Roll(float);

private:
	/** Health Percentage */
	UPROPERTY(Category = Plane, EditAnywhere)
		float CurrentHP;

	UPROPERTY(Category = Plane, EditAnywhere)
		float InitialHP;

	/** Time Left */
	UPROPERTY(Category = Plane, EditAnywhere)
		float CurrentTimeLeft;

	UPROPERTY(Category = Plane, EditAnywhere)
		float InitialTimeLeft;

	/** Progress, Percentage */
	UPROPERTY(Category = Plane, EditAnywhere)
		float CurrentProgress;

	UPROPERTY(Category = Plane, EditAnywhere)
		float InitialProgress;

	/** How quickly forward speed changes */
	UPROPERTY(Category = Plane, EditAnywhere)
		float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category = Plane, EditAnywhere)
		float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
		float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = Yaw, EditAnywhere)
		float MinSpeed;

	/*Blueprint Reference of AAsteroidEnemy class*/
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawning")
		TSubclassOf<AAsteroidEnemy> EnemyBP;

	/*Delay after the Blueprint of the UsefulActor will get spawned*/
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawning")
		float TimeToSpawn = 2.f;

	/*Spawns the UsefulActor Blueprint*/
	UFUNCTION()
		void SpawnEnemy();


	/** Current forward speed */
	float CurrentForwardSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	bool IsFirePushed;

public:

	/* Check if this player is death. */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		bool IsDeath;

	/** Returns PlaneMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetPlaneMesh() const { return PlaneMesh; }
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }

	/** Returns CurrentHP **/
	UFUNCTION(BlueprintPure, Category = "Game Rules")
	FORCEINLINE float GetCurrentHP() const { return CurrentHP; }
	/** Returns InitialHP **/
	UFUNCTION(BlueprintPure, Category = "Game Rules")
	FORCEINLINE float GetInitialHP() const { return InitialHP; }
	/** Update CurrentHP **/
	FORCEINLINE void UpdateCurrentHP(float dHP) { CurrentHP += dHP; }
	/** Returns CurrentTimeLeft **/
	UFUNCTION(BlueprintPure, Category = "Game Rules")
	FORCEINLINE float GetCurrentTimeLeft() const { return CurrentTimeLeft; }
	/** Returns InitialTimeLeft **/
	UFUNCTION(BlueprintPure, Category = "Game Rules")
	FORCEINLINE float GetInitialTimeLeft() const { return InitialTimeLeft; }
	/** Returns CurrentProgress **/
	UFUNCTION(BlueprintPure, Category = "Game Rules")
	FORCEINLINE float GetCurrentProgress() const { return CurrentProgress; }
	/** Returns InitialProgress **/
	UFUNCTION(BlueprintPure, Category = "Game Rules")
	FORCEINLINE float GetInitialProgress() const { return InitialProgress; }
	/** Update Current Progress **/
	FORCEINLINE void UpdateCurrentProgress(float dProgress) { CurrentProgress += dProgress; }

	UFUNCTION()
		void OnSpaceShipHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

};