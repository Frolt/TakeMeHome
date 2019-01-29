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
#include "TakeMeHomeGameInstance.h"
#include "Inventory.h"


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

	// Create inventory
	Inventory = CreateDefaultSubobject<UInventory>(FName("Inventory"));
}

void AUmir::BeginPlay()
{
	Super::BeginPlay();

	// Find game instance
	GameInstance = Cast<UTakeMeHomeGameInstance>(GetGameInstance());

	// Testing stuff
	CurrentHealth = 20.0f;
	CurrentMana = 40.0f;
	SpellCircle->Activate();

	// Adding spells
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::E_Tornado));
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::E_Starfall));
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::E_Force_Push));
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::E_Lightning_Bolt));
	// TODO add normal attacks
	// TODO add defensive spells
	// TODO add potions
}

void AUmir::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ensure(SpellCircle)) { return; }

	if (SpellCircle->IsActive())
	{
		ShowDecalAtMousePosInWorld();
	}
	// Passive regen TODO check if out of combat
	if (true)
	{
		CurrentMana = FMath::Min(CurrentMana + PassiveManaRegenPerSecond * DeltaSeconds, MaxMana);
		CurrentHealth = FMath::Min(CurrentHealth + PassiveHealthRegenPerSecond * DeltaSeconds, MaxHealth);
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
	PlayerInputComponent->BindAction("ActivateOffensiveSpell1", IE_Pressed, this, &AUmir::CastOffensiveSpell1);
	PlayerInputComponent->BindAction("ActivateOffensiveSpell2", IE_Pressed, this, &AUmir::CastOffensiveSpell2);
	PlayerInputComponent->BindAction("ActivateOffensiveSpell3", IE_Pressed, this, &AUmir::CastOffensiveSpell3);
	PlayerInputComponent->BindAction("ActivateDefensiveSpell", IE_Pressed, this, &AUmir::CastDefensiveSpell);
	PlayerInputComponent->BindAction("ActivatePotion", IE_Pressed, this, &AUmir::UsePotion);

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
	if (bStopMovingCamera) return;

	// Left button
	if (bIsRightMouseButtonPressed)
	{
		if (bShouldLockMouse)
		{
			UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);
		}

		AddControllerYawInput(NormalizedRate);
		CameraBoom->SetRelativeRotation(Controller->GetControlRotation());
	}
	// Right button
	else if (bIsLeftMouseButtonPressed)
	{
		if (bShouldLockMouse)
		{
			UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);
		}

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
	if (bStopMovingCamera) return;

	// Left button
	if (bIsRightMouseButtonPressed)
	{
		if (bShouldLockMouse)
		{
			UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);
		}

		AddControllerPitchInput(-NormalizedRate);
		CameraBoom->SetRelativeRotation(Controller->GetControlRotation());
	}
	// Right button
	else if (bIsLeftMouseButtonPressed)
	{
		if (bShouldLockMouse)
		{
			UmirPC->SetMouseLocation(PrevMousePos.X, PrevMousePos.Y);
		}

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
	if (bShouldLockMouse)
	{
		UmirPC->bShowMouseCursor = false;
	}
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
	if (bShouldLockMouse)
	{
		UmirPC->bShowMouseCursor = false;
	}
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

void AUmir::CastOffensiveSpell1()
{
	if (OffensiveSpellActive1 != EOffensiveSpell::E_None)
	{
		FOffensiveSpell *Spell = GameInstance->OffensiveSpells.Find(OffensiveSpellActive1);
		if (!ensure(Spell)) return;

		auto SpawnedActor = GetWorld()->SpawnActor<ASpellBase>(Spell->ClassRef, GetActorLocation(), GetActorRotation());
	}
}

void AUmir::CastOffensiveSpell2()
{
	if (OffensiveSpellActive2 != EOffensiveSpell::E_None)
	{
		FOffensiveSpell *Spell = GameInstance->OffensiveSpells.Find(OffensiveSpellActive2);
		if (!ensure(Spell)) return;
		auto SpawnedActor = GetWorld()->SpawnActor<ASpellBase>(*Spell->ClassRef, GetActorLocation(), GetActorRotation());
	}
}

void AUmir::CastOffensiveSpell3()
{
	if (OffensiveSpellActive3 != EOffensiveSpell::E_None)
	{
		FOffensiveSpell *Spell = GameInstance->OffensiveSpells.Find(OffensiveSpellActive3);
		if (!ensure(Spell)) return;
		auto SpawnedActor = GetWorld()->SpawnActor<ASpellBase>(*Spell->ClassRef, GetActorLocation(), GetActorRotation());
	}
}

void AUmir::CastDefensiveSpell()
{
	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Casting defensive spell"));
}

void AUmir::UsePotion()
{
	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Using potion"));
}

void AUmir::ShowDecalAtMousePosInWorld()
{
	if (!ensure(SpellCircle)) { return; }

	// Find mouse world pos
	FVector MouseLocation;
	FVector MouseDirection;
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

	// linetrace to find ground
	FVector EndLocation = MouseLocation + (MouseDirection.GetSafeNormal() * MaxTargetRange);
	FHitResult HitResult;
	auto bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, MouseLocation, EndLocation, ECC_Visibility);

	// Set decal component pos here

	if (bFoundGround)
	{
		// TODO read range from active spell
		float SpellRange = 2000.0f;
		FVector DirectionToHitLocation = HitResult.Location - GetActorLocation();
		if (DirectionToHitLocation.Size() > SpellRange)
		{
			// Find new location
			DirectionToHitLocation.Z = GetActorLocation().Z;
			FVector NewDirection = DirectionToHitLocation.GetSafeNormal();
			FVector NewPos = GetActorLocation() + (NewDirection * SpellRange);
			FVector StartTrace(NewPos.X, NewPos.Y, NewPos.Z + 100000.0f);
			FVector EndTrace(NewPos.X, NewPos.Y, NewPos.Z - 100000.0f);
			GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility);
			SpellCircle->SetWorldLocationAndRotation(HitResult.Location, FRotator(-90.0f, 0.0f, 0.0f));
		}
		else
		{
			SpellCircle->SetWorldLocationAndRotation(HitResult.Location, FRotator(-90.0f, 0.0f, 0.0f));
		}
	}
}

float AUmir::GetHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

float AUmir::GetManaPercentage() const
{
	return CurrentMana / MaxMana;
}

void AUmir::AddDefensiveSpell(EDefensiveSpell DefensiveSpell)
{
	AquiredDefensiveSpells.Add(*GameInstance->DefensiveSpells.Find(DefensiveSpell));
}

void AUmir::AddOffensiveSpell(EOffensiveSpell OffensiveSpell)
{
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(OffensiveSpell));
}

void AUmir::AddPotion(EPotion Potion)
{
	AquiredPotions.Add(*GameInstance->Potions.Find(Potion));
}

void AUmir::BindDefensiveSpell(EDefensiveSpell DefensiveSpell)
{
	DefensiveSpellActive = DefensiveSpell;
}

void AUmir::BindOffensiveSpell1(EOffensiveSpell OffensiveSpell)
{
	OffensiveSpellActive1 = OffensiveSpell;
}

void AUmir::BindOffensiveSpell2(EOffensiveSpell OffensiveSpell)
{
	OffensiveSpellActive2 = OffensiveSpell;
}

void AUmir::BindOffensiveSpell3(EOffensiveSpell OffensiveSpell)
{
	OffensiveSpellActive3 = OffensiveSpell;
}

void AUmir::BindPotion(EPotion Potion)
{
	PotionActive = Potion;
}
