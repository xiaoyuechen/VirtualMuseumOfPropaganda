// Fill out your copyright notice in the Description page of Project Settings.

#include "GCPlayerController.h"
#include "GCCharacter.h"

void AGCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GCCharacter = Cast<AGCCharacter>(GetPawn());
	if (ensure(GCCharacter))
	{
		GCCharacter->OnBrowse.AddUniqueDynamic(this, &AGCPlayerController::SwapToCamPawn);
	}

	bIsBrowsing = false;
}

void AGCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Cancel", IE_Pressed, this, &AGCPlayerController::StopBrowsing);
}

void AGCPlayerController::SwapToCamPawn(AActor* HitActor)
{
	FVector SpawnLocation = HitActor->GetActorLocation();
	FRotator SpawnRotation = PlayerCameraManager->GetCameraRotation();
	UnPossess();
	APawn* CamPawn = GetWorld()->SpawnActor<APawn>(CamPawnBlueprint,SpawnLocation, SpawnRotation);
	Possess(CamPawn);
	bIsBrowsing = true;

	OnCamPawnSpawned(CamPawn, HitActor);
}

void AGCPlayerController::StopBrowsing()
{
	if (!bIsBrowsing) { return; }
	APawn* CamPawn = GetPawn();
	UnPossess();
	Possess(GCCharacter);
	CamPawn->Destroy();
	bIsBrowsing = false;
}
