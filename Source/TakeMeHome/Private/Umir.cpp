// Fill out your copyright notice in the Description page of Project Settings.

#include "Umir.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"


AUmir::AUmir()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Possessing
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoPossessAI = EAutoPossessAI::Disabled;

	// Eye height
	BaseEyeHeight = 90.0f;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->RelativeLocation.Z = BaseEyeHeight;

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
	PlayerInputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &AUmir::LeftMouseButtonPressed);
	PlayerInputComponent->BindAction("LeftMouseButton", IE_Released, this, &AUmir::LeftMouseButtonReleased);
	PlayerInputComponent->BindAction("RightMouseButton", IE_Pressed, this, &AUmir::RightMouseButtonPressed);
	PlayerInputComponent->BindAction("RightMouseButton", IE_Released, this, &AUmir::RightMouseButtonReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUmir::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUmir::MoveRight);
	PlayerInputComponent->BindAxis("LookRight", this, &AUmir::LookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AUmir::LookUp);
	PlayerInputComponent->BindAxis("Zoom", this, &AUmir::Zoom);
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
	auto UmirPC = GetWorld()->GetFirstPlayerController();
	check(UmirPC);

	if (bIsRightMouseButtonPressed)
	{
		UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);

		AddControllerYawInput(NormalizedRate);
		CameraBoom->SetRelativeRotation(Controller->GetControlRotation());
	}
	else if (bIsLeftMouseButtonPressed)
	{
		UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);

		CameraBoom->RelativeRotation.Yaw += NormalizedRate * -UmirPC->InputPitchScale;
	}
	else
	{
		UmirPC->bShowMouseCursor = true;
	}
}

void AUmir::LookUp(float NormalizedRate)
{
	auto UmirPC = GetWorld()->GetFirstPlayerController();
	check(UmirPC);

	if (bIsRightMouseButtonPressed)
	{
		UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);

		AddControllerPitchInput(-NormalizedRate);
		CameraBoom->SetRelativeRotation(Controller->GetControlRotation());
	}
	else if (bIsLeftMouseButtonPressed)
	{
		UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);

		CameraBoom->RelativeRotation.Pitch = FMath::Clamp(CameraBoom->RelativeRotation.Pitch + NormalizedRate * -UmirPC->InputPitchScale, -89.0f, 89.0f);
	}
	else 
	{
		UmirPC->bShowMouseCursor = true;
	}
}

void AUmir::Zoom(float NormalizedRate)
{
	CameraBoom->TargetArmLength += NormalizedRate * ZoomStrength;
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, MinZoom, MaxZoom);
}

void AUmir::LeftMouseButtonPressed()
{
	bIsLeftMouseButtonPressed = true;

	auto UmirPC = GetWorld()->GetFirstPlayerController();
	check(UmirPC);
	UmirPC->bShowMouseCursor = false;
	UmirPC->GetMousePosition(PrevMousePos.X, PrevMousePos.Y);
}

void AUmir::LeftMouseButtonReleased()
{
	bIsLeftMouseButtonPressed = false;
}

void AUmir::RightMouseButtonPressed()
{
	bIsRightMouseButtonPressed = true;

	auto UmirPC = GetWorld()->GetFirstPlayerController();
	check(UmirPC);
	UmirPC->bShowMouseCursor = false;
	UmirPC->GetMousePosition(PrevMousePos.X, PrevMousePos.Y);

	auto ControllerRot = Controller->GetControlRotation();
	auto CamRot = CameraBoom->RelativeRotation;
	Controller->SetControlRotation(FRotator(CamRot.Pitch, CamRot.Yaw, ControllerRot.Roll));
}

void AUmir::RightMouseButtonReleased()
{
	bIsRightMouseButtonPressed = false;
}
