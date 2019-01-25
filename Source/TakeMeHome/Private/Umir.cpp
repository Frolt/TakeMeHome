// Fill out your copyright notice in the Description page of Project Settings.

#include "Umir.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Tornado.h"
#include "Starfall.h"
#include "ForcePush.h"
#include "LightningBolt.h"
#include "Spellbook.h"
#include "SpellBase.h"


AUmir::AUmir()
{
	// Enable this actor to tick
	PrimaryActorTick.bCanEverTick = true;

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
	GetCharacterMovement()->AirControl = 0.2f;

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

	// Create spellbook
	SpellBook = CreateDefaultSubobject<USpellBook>(FName("Spell Book"));

}

void AUmir::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = 20;
	SpellCircle->Activate();
}

void AUmir::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ensure(SpellCircle)) { return; }

	if (SpellCircle->IsActive())
	{
		ShowDecalAtMousePosInWorld();
	}
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
	PlayerInputComponent->BindAction("Spell1", IE_Pressed, this, &AUmir::CastSpell1);
	PlayerInputComponent->BindAction("Spell2", IE_Pressed, this, &AUmir::CastSpell2);
	PlayerInputComponent->BindAction("Spell3", IE_Pressed, this, &AUmir::CastSpell3);
	PlayerInputComponent->BindAction("Spell4", IE_Pressed, this, &AUmir::CastSpell4);
	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &AUmir::HandleEscape);

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

	// Left button
	if (bIsRightMouseButtonPressed)
	{
		// Lock mouse
		UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);

		AddControllerYawInput(NormalizedRate);
		CameraBoom->SetRelativeRotation(Controller->GetControlRotation());
	}
	// Right button
	else if (bIsLeftMouseButtonPressed)
	{
		// Lock mouse
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

	// Left button
	if (bIsRightMouseButtonPressed)
	{
		// Lock mouse
		UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);

		AddControllerPitchInput(-NormalizedRate);
		CameraBoom->SetRelativeRotation(Controller->GetControlRotation());
	}
	// Right button
	else if (bIsLeftMouseButtonPressed)
	{
		// Lock mouse
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
	auto UmirPC = GetWorld()->GetFirstPlayerController();
	check(UmirPC);

	bIsLeftMouseButtonPressed = true;
	UmirPC->bShowMouseCursor = false;
	// Stores the mouse position
	UmirPC->GetMousePosition(PrevMousePos.X, PrevMousePos.Y);
}

void AUmir::LeftMouseButtonReleased()
{
	bIsLeftMouseButtonPressed = false;
}

void AUmir::RightMouseButtonPressed()
{
	auto UmirPC = GetWorld()->GetFirstPlayerController();
	check(UmirPC);

	bIsRightMouseButtonPressed = true;
	UmirPC->bShowMouseCursor = false;
	// Stores the mouse position
	UmirPC->GetMousePosition(PrevMousePos.X, PrevMousePos.Y);

	// Sets player rotation to the camera, the camera rotation resets in LookRight/Left method
	auto ControllerRot = Controller->GetControlRotation();
	auto CamRot = CameraBoom->RelativeRotation;
	Controller->SetControlRotation(FRotator(CamRot.Pitch, CamRot.Yaw, ControllerRot.Roll));
}

void AUmir::RightMouseButtonReleased()
{
	bIsRightMouseButtonPressed = false;
}

void AUmir::CastSpell1()
{
	if (SpellBook->OffensiveSpellClasses.Find(EOffensiveSpell::E_Tornado))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Tornado"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tornado not found!"));
	}
	auto SpawnedActor = GetWorld()->SpawnActor<ASpellBase>(TornadoBP, GetActorLocation(), GetActorRotation());
}

void AUmir::CastSpell2()
{
	auto SpawnedActor = GetWorld()->SpawnActor<ASpellBase>(StarfallBP, GetActorLocation(), GetActorRotation());
}

void AUmir::CastSpell3()
{
	auto SpawnedActor = GetWorld()->SpawnActor<ASpellBase>(ForcePushBP, GetActorLocation(), GetActorRotation());
}

void AUmir::CastSpell4()
{
	auto SpawnedActor = GetWorld()->SpawnActor<ASpellBase>(LightningBoltBP, GetActorLocation(), GetActorRotation());
}

void AUmir::HandleEscape()
{
	// Find out if a widget has focus
	// Close it
	// Repeat until all focused widgets are closed
	// If no widget has focus check if player has active spell
	// If so cancel the active state
	// If none of above is the case open pause menu
	
}

void AUmir::ShowDecalAtMousePosInWorld()
{
	if (!ensure(SpellCircle)) { return; }

	// Find mouse world pos
	FVector MouseLocation;
	FVector MouseDirection;
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

	// linetrace to find ground
	FVector EndLocation = MouseLocation + (MouseDirection.GetSafeNormal() * TargetRange);
	FHitResult HitResult;
	auto bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, MouseLocation, EndLocation, ECC_Visibility);

	// Set decal component pos here

	if (bFoundGround)
	{
		SpellCircle->SetWorldLocation(HitResult.Location);
	}
	else
	{
		auto NewEndLocation = EndLocation + (FVector::UpVector * -1000000.0f);
		auto bFoundGroundSecondTry = GetWorld()->LineTraceSingleByChannel(HitResult, EndLocation, NewEndLocation, ECC_Visibility);
		if (bFoundGroundSecondTry)
		{
			SpellCircle->SetWorldLocation(HitResult.Location);
		}
	}
}

float AUmir::GetHealthPercentage() const
{
	return static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
}
