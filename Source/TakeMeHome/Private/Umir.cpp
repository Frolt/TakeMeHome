// Fill out your copyright notice in the Description page of Project Settings.

#include "Umir.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


AUmir::AUmir()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Custom
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AUmir::BeginPlay()
{
	Super::BeginPlay();


}

void AUmir::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}

void AUmir::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUmir::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUmir::MoveRight);

	PlayerInputComponent->BindAxis("LookRight", this, &AUmir::LookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AUmir::LookUp);

	PlayerInputComponent->BindAxis("Zoom", this, &AUmir::Zoom);


	//PlayerInputComponent->BindAxis("TurnRate", this, &AUmir::TurnAtRate); // Slow camera movement is diabled
	//PlayerInputComponent->BindAxis("LookUpRate", this, &AUmir::LookUpAtRate); // Slow camera movement is diabled
}

void AUmir::MoveForward(float NormalizedRate)
{
	if (Controller && (NormalizedRate != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, NormalizedRate);
	}
}

void AUmir::MoveRight(float NormalizedRate)
{
	if (Controller && (NormalizedRate != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, NormalizedRate);
	}
}

void AUmir::LookRight(float NormalizedRate)
{
	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(FKey("RightMouseButton"))) {
		AddControllerYawInput(NormalizedRate);
	}
}

void AUmir::LookUp(float NormalizedRate)
{
	if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(FKey("RightMouseButton"))) {
		AddControllerPitchInput(NormalizedRate);
	}
}

void AUmir::Zoom(float NormalizedRate)
{
	UE_LOG(LogTemp, Warning, TEXT("Zooming value is: %f"), NormalizedRate);
	CameraBoom->TargetArmLength += NormalizedRate * ZoomStrength;
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, MinZoom, MaxZoom);
}

//  void AUmir::LookRightAtRate(float NormalizedRate)
//  {
//  	// calculate delta for this frame from the rate information
//  	AddControllerYawInput(NormalizedRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
//  }
//  
//  void AUmir::LookUpAtRate(float NormalizedRate)
//  {
//  	// calculate delta for this frame from the rate information
//  	AddControllerPitchInput(NormalizedRate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
//  }

USpringArmComponent *AUmir::GetCameraBoom() const
{
	return CameraBoom;
}

UCameraComponent *AUmir::GetFollowCamera() const
{
	return FollowCamera;
}