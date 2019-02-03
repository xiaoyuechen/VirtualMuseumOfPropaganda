// Copyright J&J.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GCCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGCCharacterDelegate, AActor*, HitActor);

/**
* This class is the character class for Immortal, and it is responsible for all physical interaction between the player and the world
* It also handles the damage the character takes in
*
* The CapsuleComponent (inherited from ACharacter) handles collision with the world
* The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
*/

UCLASS(config = Game)
class AGCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGCCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Called for forwards/backward input */
	UFUNCTION(BlueprintCallable, Category = "CharacterActions")
		void MoveForward(float Value);

	/** Called for right/left input */
	UFUNCTION(BlueprintCallable, Category = "CharacterActions")
		virtual void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	void StartBrowsing();

	UFUNCTION(BlueprintCallable, Category = Initialization)
	void FindComponents(class UCameraComponent* FPCameraToSet);

	bool GetLookDirection(FVector &CameraWorldLocation, FVector &LookDirection) const;

	FGCCharacterDelegate OnBrowse;

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	class UCameraComponent* FPCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = "CharacterSetup")
		float SphereSweepRange = 500.f;

	UPROPERTY(EditAnywhere, Category = "CharacterSetup")
		float SphereSweepRadius = 100.f;

};
