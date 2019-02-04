// Copyright J&J.

#include "GCCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"


//////////////////////////////////////////////////////////////////////////
// AImmortalCharacter constructor

AGCCharacter::AGCCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->GravityScale = 8.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 2500.f;
	GetCharacterMovement()->GroundFriction = 10.0f;
	GetCharacterMovement()->MaxWalkSpeed = 800.0f;
	GetCharacterMovement()->MaxFlySpeed = 1000.0f;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f);
	CameraBoom->bUsePawnControlRotation = true;

	// Create an orthographic camera (no perspective) and attach it to the boom
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->bAutoActivate = false;
}

//////////////////////////////////////////////////////////////////////////
// Setup Player Input


void AGCCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// follow camera control
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AGCCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AGCCharacter::LookUpAtRate);

	// action control
	InputComponent->BindAxis("MoveForward", this, &AGCCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGCCharacter::MoveRight);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAction("Browse", IE_Pressed, this, &AGCCharacter::StartBrowsing);
}

void AGCCharacter::StartBrowsing()
{
	FHitResult HitResult;
	if (IsSweepActorBrowseable(HitResult))
	{
		OnBrowse.Broadcast(HitResult.GetActor());
	}
}

bool AGCCharacter::IsSweepActorBrowseable(FHitResult &HitResult)
{
	HitResult = FHitResult();
	if (!FPCamera || !FPCamera->IsActive()) { return false; }
	FVector CameraWorldLocation;
	FVector LookDirection;
	if (!GetLookDirection(CameraWorldLocation, LookDirection)) { return false; }
	//FString ObjectName = HitResult.GetActor()->GetName();
	//UE_LOG(LogTemp, Warning, TEXT("Sweep Name: %s"), *ObjectName);
	//DrawDebugSphere(
	//	GetWorld(),
	//	EndLocation,
	//	SphereSweepRadius,
	//	32,
	//	FColor(255, 0, 255),
	//	true
	//);
	const FVector StartLocation = CameraWorldLocation;
	const FVector EndLocation = StartLocation + LookDirection * SphereSweepRange;
	if (!GetWorld()->
		SweepSingleByChannel(
			HitResult,
			StartLocation,
			EndLocation,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeSphere(SphereSweepRadius),
			FCollisionQueryParams::DefaultQueryParam,
			FCollisionResponseParams::DefaultResponseParam)
		)
	{
		return false;
	}

	if (!(HitResult.Actor->ActorHasTag("2D") || HitResult.Actor->ActorHasTag("3D"))) { return false; }

	return true;
}

void AGCCharacter::FindComponents(UCameraComponent* FPCameraToSet)
{
	FPCamera = FPCameraToSet;

}

bool AGCCharacter::GetLookDirection(FVector &CameraWorldLocation, FVector &LookDirection) const
{
	// Find the crosshair position in pixel coordinates
	int32 ViewportSizeX, ViewportSizeY;
	GetWorld()->GetFirstPlayerController()->GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX /2, ViewportSizeY /2);
	// "De-project" the screen position of the crosshair to a world direction
	return  GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(
		ScreenLocation.X,
		ScreenLocation.Y,
		CameraWorldLocation,
		LookDirection
	);
}


////////////////////////////////////////////////////////////////////////////
// BeginPlay() and Tick()

void AGCCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGCCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}



//////////////////////////////////////////////////////////////////////////
// Player callable character actions

void AGCCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGCCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void AGCCharacter::MoveForward(float Value)
{
	if ((Controller) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AGCCharacter::MoveRight(float Value)
{
	if ((Controller) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}

}