// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GRAPHICSCOURSE_API AGCPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UFUNCTION()
	void SwapToCamPawn(AActor* HitActor);

	void StopBrowsing();

	void TryQuitGame();

	UFUNCTION(BlueprintImplementableEvent, Category = "CamPawn")
	void OnOrbitCamPawnSpawned(APawn* CamPawn, AActor* HitActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "CamPawn")
	void OnPanCamPawnSpawned(APawn* CamPawn, AActor* HitActor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<APawn> OrbitCamPawnBlueprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<APawn> PanCamPawnBlueprint;

private:
	
	class AGCCharacter* GCCharacter;
	
	bool bIsBrowsing = false;
};
