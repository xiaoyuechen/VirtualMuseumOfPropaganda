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
	bool bUseOrbitCam;
	if (HitActor->ActorHasTag("3D")) { bUseOrbitCam = true; }
	else if (HitActor->ActorHasTag("2D")) { bUseOrbitCam = false; }
	else { return; }
	USceneComponent* Centre = Cast<USceneComponent>(HitActor->GetComponentsByTag(USceneComponent::StaticClass(), "Centre")[0]);
	if (!Centre) { return; }
	FVector SpawnLocation = Centre->GetComponentLocation();
	FRotator SpawnRotation = PlayerCameraManager->GetCameraRotation();
	APawn* CamPawn = nullptr;
	if (bUseOrbitCam) { CamPawn = GetWorld()->SpawnActor<APawn>(OrbitCamPawnBlueprint, SpawnLocation, SpawnRotation); }
	else { CamPawn = GetWorld()->SpawnActor<APawn>(PanCamPawnBlueprint, SpawnLocation, SpawnRotation); }
	if (!CamPawn) { return; }
	GCCharacter->SetActorHiddenInGame(true);
	UnPossess();
	Possess(CamPawn);
	bIsBrowsing = true;

	if (bUseOrbitCam) { OnOrbitCamPawnSpawned(CamPawn, HitActor); }
	else { OnPanCamPawnSpawned(CamPawn, HitActor); }
}

void AGCPlayerController::StopBrowsing()
{
	if (!bIsBrowsing) { return; }
	APawn* CamPawn = GetPawn();
	UnPossess();
	Possess(GCCharacter);
	GCCharacter->SetActorHiddenInGame(false);
	CamPawn->Destroy();
	bIsBrowsing = false;
}
