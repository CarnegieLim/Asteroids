// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Asteroids.h"
#include "AsteroidsGameMode.h"
#include "SpaceShip.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AAsteroidsGameMode::BeginPlay()
{
	Super::BeginPlay();

	ASpaceShip* MySpaceShip = Cast<ASpaceShip>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerHUDClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);

		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}


}

AAsteroidsGameMode::AAsteroidsGameMode()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BluePrints/BP_SpaceShip"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	static ConstructorHelpers::FClassFinder<UUserWidget> BPHUD(TEXT("/Game/BluePrints/BP_HUD"));
	PlayerHUDClass = BPHUD.Class;
}

void AAsteroidsGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
