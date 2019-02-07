// Fill out your copyright notice in the Description page of Project Settings.

#include "Umir.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SkeletalMeshComponent.h"
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
#include "Public/TimerManager.h"


AUmir::AUmir()
{
	// Enable this actor to tick
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->bReceivesDecals = false;

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
	//Inventory->AddItems(EItem::E_Item_2, 35);
	//Inventory->AddItems(EItem::E_Item_3, 35);
	//Inventory->AddItems(EItem::E_Item_4, 35);
}

void AUmir::BeginPlay()
{
	Super::BeginPlay();

	// Find game instance
	GameInstance = Cast<UTakeMeHomeGameInstance>(GetGameInstance());

	// Set camera init rotation
	CameraBoom->SetRelativeRotation(GetControlRotation());

	// Testing stuff
	CurrentHealth = 20.0f;
	CurrentMana = 40.0f;
	Decal->Activate();
	Decal->SetDecalMaterial(SpellArrowMaterial);

	// Adding spells
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::E_Tornado));
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::E_Starfall));
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::E_Force_Push));
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::E_Lightning_Bolt));

	// Add normal attacks
	AquiredNormalAttacks.Add(*GameInstance->NormalAttacks.Find(ENormalAttack::E_Melee_Attack_1));
	AquiredNormalAttacks.Add(*GameInstance->NormalAttacks.Find(ENormalAttack::E_Melee_Attack_2));
	AquiredNormalAttacks.Add(*GameInstance->NormalAttacks.Find(ENormalAttack::E_Melee_Attack_3));

	// Add defensive spells
	AquiredDefensiveSpells.Add(*GameInstance->DefensiveSpells.Find(EDefensiveSpell::E_Counter_Strike));
	AquiredDefensiveSpells.Add(*GameInstance->DefensiveSpells.Find(EDefensiveSpell::E_Star_Shield));
	AquiredDefensiveSpells.Add(*GameInstance->DefensiveSpells.Find(EDefensiveSpell::E_Spirit_Walk));

	// Add potions
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::E_Healing_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::E_Mana_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::E_Fire_Elemental_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::E_Nature_Elemental_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::E_Water_Elemental_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::E_Lightning_Elemental_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::E_Earth_Elemental_Potion));

	// Add items
	//Inventory->Items.Init(*GameInstance->Items.Find(EItem::E_Empty), 100);
	Inventory->AddItems(EItem::E_Item_2, 20);
	Inventory->AddItems(EItem::E_Item_1, 555);
	Inventory->RemoveItems(EItem::E_Item_1, 400);
	Inventory->RemoveItems(EItem::E_Item_2, 4);

}

void AUmir::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ensure(Decal)) { return; }

	// Activate correct decal type
	switch (ActiveDecal)
	{
	case EDecalType::E_None:
		Decal->SetVisibility(false);
		break;
	case EDecalType::E_Spell_Circle:
		Decal->SetVisibility(true);
		MoveDecalToMouseHitLocation();
		break;
	case EDecalType::E_Arrow:
		RotateDecalAroundPlayer();
		Decal->SetVisibility(true);
		break;
	case EDecalType::E_Box_Indicator:
		Decal->SetVisibility(true);
		break;
	default:
		break;
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
	PlayerInputComponent->BindAction("ActivateOffensiveSpell1", IE_Pressed, this, &AUmir::ActivateOffensiveSlot1);
	PlayerInputComponent->BindAction("ActivateOffensiveSpell2", IE_Pressed, this, &AUmir::ActivateOffensiveSlot2);
	PlayerInputComponent->BindAction("ActivateOffensiveSpell3", IE_Pressed, this, &AUmir::ActivateOffensiveSlot3);
	PlayerInputComponent->BindAction("ActivateDefensiveSpell", IE_Pressed, this, &AUmir::ActivateDefensiveSlot);
	PlayerInputComponent->BindAction("ActivatePotion", IE_Pressed, this, &AUmir::ActivatePotionSlot);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUmir::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUmir::MoveRight);
	PlayerInputComponent->BindAxis("LookRight", this, &AUmir::LookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AUmir::LookUp);
	PlayerInputComponent->BindAxis("Zoom", this, &AUmir::Zoom);
}

void AUmir::MoveForward(float NormalizedRate)
{
	if (Controller && (NormalizedRate != 0.0f) && !bStopMovement)
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
	if (Controller && (NormalizedRate != 0.0f) && !bStopMovement)
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
	if (!bStopZooming)
	{
		CameraBoom->TargetArmLength += NormalizedRate * ZoomStrength;
		CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, MinZoom, MaxZoom);
	}
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

	// Cast spell if one is activated
	if (ActivatedSpell != EOffensiveSpell::E_None)
	{
		CastOffensiveSpell(ActivatedSpell);
		ActivatedSpell = EOffensiveSpell::E_None;
		ActiveDecal = EDecalType::E_None;
	}
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

	// Cancel activated spell
	ActivatedSpell = EOffensiveSpell::E_None;
	ActiveDecal = EDecalType::E_None;
}

void AUmir::RightMouseButtonReleased()
{
	bIsRightMouseButtonPressed = false;
}

void AUmir::ActivateOffensiveSlot1()
{
	// Check if on cooldown or spell is not bound
	if (OffensiveSpellActive1 == EOffensiveSpell::E_None) return;
	if (!FMath::IsNearlyEqual(GetOffensiveSlot1Cooldown(), 0.0f)) { IsOnCooldown(); return; }

	FOffensiveSpell *Spell = GameInstance->OffensiveSpells.Find(OffensiveSpellActive1);
	if (!ensure(Spell)) return;

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedSpell = OffensiveSpellActive1;
		switch (Spell->DecalType)
		{
		case EDecalType::E_None:
			ActiveDecal = EDecalType::E_None;
			break;
		case EDecalType::E_Spell_Circle:
			Decal->SetDecalMaterial(SpellCircleMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::E_Arrow:
			Decal->SetDecalMaterial(SpellArrowMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::E_Box_Indicator:
			Decal->SetDecalMaterial(SpellBoxMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		default:
			break;
		}
	}
	else
	{
		LastTimeActivatedOffensiveSpell1 = GetWorld()->GetTimeSeconds();
		CastOffensiveSpell(OffensiveSpellActive1);
	}
}

void AUmir::ActivateOffensiveSlot2()
{
	// Check if on cooldown or spell is not bound
	if (OffensiveSpellActive2 == EOffensiveSpell::E_None) return;
	if (!FMath::IsNearlyEqual(GetOffensiveSlot2Cooldown(), 0.0f)) { IsOnCooldown(); return; }

	FOffensiveSpell *Spell = GameInstance->OffensiveSpells.Find(OffensiveSpellActive2);
	if (!ensure(Spell)) return;

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedSpell = OffensiveSpellActive2;
		switch (Spell->DecalType)
		{
		case EDecalType::E_None:
			ActiveDecal = EDecalType::E_None;
			break;
		case EDecalType::E_Spell_Circle:
			Decal->SetDecalMaterial(SpellCircleMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::E_Arrow:
			Decal->SetDecalMaterial(SpellArrowMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::E_Box_Indicator:
			Decal->SetDecalMaterial(SpellBoxMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		default:
			break;
		}
	}
	else
	{
		LastTimeActivatedOffensiveSpell2 = GetWorld()->GetTimeSeconds();
		CastOffensiveSpell(OffensiveSpellActive2);
	}
}

void AUmir::ActivateOffensiveSlot3()
{
	// Check if on cooldown or spell is not bound
	if (OffensiveSpellActive3 == EOffensiveSpell::E_None) return;
	if (!FMath::IsNearlyEqual(GetOffensiveSlot3Cooldown(), 0.0f)) { IsOnCooldown(); return; }


	FOffensiveSpell *Spell = GameInstance->OffensiveSpells.Find(OffensiveSpellActive3);
	if (!ensure(Spell)) return;

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedSpell = OffensiveSpellActive3;
		switch (Spell->DecalType)
		{
		case EDecalType::E_None:
			ActiveDecal = EDecalType::E_None;
			break;
		case EDecalType::E_Spell_Circle:
			Decal->SetDecalMaterial(SpellCircleMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::E_Arrow:
			Decal->SetDecalMaterial(SpellArrowMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::E_Box_Indicator:
			Decal->SetDecalMaterial(SpellBoxMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		default:
			break;
		}
	}
	else
	{
		LastTimeActivatedOffensiveSpell3 = GetWorld()->GetTimeSeconds();
		CastOffensiveSpell(OffensiveSpellActive3);
	}
}

void AUmir::ActivateDefensiveSlot()
{
	// Check if on cooldown or spell is not bound
	if (DefensiveSpellActive == EDefensiveSpell::E_None) return;
	if (!FMath::IsNearlyEqual(GetDefensiveSlotCooldown(), 0.0f)) { IsOnCooldown(); return; }

	LastTimeActivatedDefensiveSpell = GetWorld()->GetTimeSeconds();
	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Casting defensive spell"));
}

void AUmir::ActivatePotionSlot()
{
	// Check if on cooldown or spell is not bound
	if (PotionActive == EPotion::E_None) return;
	if (!FMath::IsNearlyEqual(GetPotionSlotCooldown(), 0.0f)) { IsOnCooldown(); return; }

	LastTimeActivatedPotion = GetWorld()->GetTimeSeconds();
	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Using potion"));
}

void AUmir::MoveDecalToMouseHitLocation()
{
	if (!ensure(Decal)) return;
	if (ActivatedSpell == EOffensiveSpell::E_None) return;

	// Reset decal scale
	Decal->RelativeScale3D.Y = 1.0f;
	Decal->RelativeScale3D.Z = 1.0f;

	// Find mouse world pos
	FVector MouseLocation;
	FVector MouseDirection;
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

	// linetrace to find ground first attempt
	FVector EndLocation = MouseLocation + (MouseDirection.GetSafeNormal() * MaxTraceDistance);
	FHitResult HitResult;
	bool bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, MouseLocation, EndLocation, ECC_Visibility);
	
	if (bFoundGround == false)
	{
		// linetrace to find ground second attempt
		UE_LOG(LogTemp, Warning, TEXT("Line trace second try"));
		FVector NewStartPos = MouseLocation + (MouseDirection * 10000.0f);
		FVector NewEndPos = FVector::UpVector * -MaxTraceDistance;
		bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, NewStartPos, NewEndPos, ECC_Visibility);
		if (bFoundGround == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find ground on second attempt"));
			return;
		}
	}

	float SpellRange = GameInstance->OffensiveSpells.Find(ActivatedSpell)->Range;
	FVector DirectionFromPlayerToHitLocation = HitResult.Location - GetActorLocation();
	if (DirectionFromPlayerToHitLocation.Size() > SpellRange)
	{
		// Find new location
		DirectionFromPlayerToHitLocation.Z = GetActorLocation().Z;
		FVector NewDirection = DirectionFromPlayerToHitLocation.GetSafeNormal();
		FVector NewPos = GetActorLocation() + (NewDirection * SpellRange);
		FVector StartTrace(NewPos.X, NewPos.Y, NewPos.Z + MaxTraceDistance);
		FVector EndTrace(NewPos.X, NewPos.Y, NewPos.Z - MaxTraceDistance);
		GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_Visibility); // TODO this might not work in roof environments like caves
		Decal->SetWorldLocationAndRotation(HitResult.Location, FRotator(-90.0f, 0.0f, 0.0f));
	}
	else
	{
		Decal->SetWorldLocationAndRotation(HitResult.Location, FRotator(-90.0f, 0.0f, 0.0f));
	}
}

void AUmir::RotateDecalAroundPlayer()
{
	if (!ensure(Decal)) return;
	if (ActivatedSpell == EOffensiveSpell::E_None) return;

	// Find mouse world pos
	FVector MouseLocation;
	FVector MouseDirection;
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

	// linetrace to find ground first attempt
	FVector EndLocation = MouseLocation + (MouseDirection.GetSafeNormal() * MaxTraceDistance);
	FHitResult HitResult;
	bool bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, MouseLocation, EndLocation, ECC_Visibility);

	if (bFoundGround == false)
	{
		// linetrace to find ground second attempt
		FVector NewStartPos = MouseLocation + (MouseDirection * 10000.0f);
		FVector NewEndPos = FVector::UpVector * -MaxTraceDistance;
		bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, NewStartPos, NewEndPos, ECC_Visibility);
		if (bFoundGround == false)
		{
			return;
		}
	}

	// Calculate new decal position/scale/rotation
	FVector DirectionFromPlayerToHitLocation = (HitResult.Location - GetActorLocation()).GetSafeNormal();
	DirectionFromPlayerToHitLocation.Z = 0.0f;
	DirectionFromPlayerToHitLocation.Normalize();
	FRotator NewDecalRotation = FRotator(90.0f, 90.0f + DirectionFromPlayerToHitLocation.Rotation().Yaw, 0.0f);

	Decal->SetWorldRotation(NewDecalRotation);
	Decal->SetWorldLocation(GetActorLocation() + (DirectionFromPlayerToHitLocation * 500.0f));
	Decal->RelativeScale3D.Y = 2.0f;
	Decal->RelativeScale3D.Z = 0.5f;

}

void AUmir::RestoreMovement()
{
	bStopMovement = false;
	bUseControllerRotationYaw = false;
}

void AUmir::CastOffensiveSpell(EOffensiveSpell SpellKey)
{
	auto *Spell = GameInstance->OffensiveSpells.Find(SpellKey);

	// Check for mana
	if (CurrentMana < Spell->ManaCost)
	{
		NotEnoughMana();
	}
	else
	{
		// Set cooldown
		if (SpellKey == OffensiveSpellActive1)
		{
			LastTimeActivatedOffensiveSpell1 = GetWorld()->GetTimeSeconds();
		}
		else if (SpellKey == OffensiveSpellActive2)
		{
			LastTimeActivatedOffensiveSpell2 = GetWorld()->GetTimeSeconds();
		}
		else if (SpellKey == OffensiveSpellActive3)
		{
			LastTimeActivatedOffensiveSpell3 = GetWorld()->GetTimeSeconds();
		}

		CurrentMana -= Spell->ManaCost;

		// Check for cast time
		if (Spell->CastTime > 0.00001f)
		{
			bStopMovement = true;
			bUseControllerRotationYaw = false;

			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AUmir::RestoreMovement, Spell->CastTime);
		}

		// Find correct spawn location/rotation
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();
		if (Spell->DecalType == EDecalType::E_Spell_Circle)
		{
			SpawnLocation = Decal->GetComponentLocation();
		}
		else if (Spell->DecalType == EDecalType::E_Arrow)
		{
			// TODO find decal arrow direction
			SpawnRotation = (-Decal->GetRightVector()).Rotation();
		}
		auto SpawnedActor = GetWorld()->SpawnActor<ASpellBase>(Spell->ClassRef, SpawnLocation, SpawnRotation);

		// Set spell element according to Umir's active element
		if (Spell->ElementType == EElement::E_Neutral)
		{
			SpawnedActor->ElementType = ActiveElement;
		}
		else
		{
			SpawnedActor->ElementType = Spell->ElementType;
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

float AUmir::GetDefensiveSlotCooldown() const
{
	if (DefensiveSpellActive == EDefensiveSpell::E_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->DefensiveSpells.Find(DefensiveSpellActive)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedDefensiveSpell), 0.0f);
}

float AUmir::GetDefensiveSlotCooldownPercentage() const
{
	if (DefensiveSpellActive == EDefensiveSpell::E_None)
		return 0.0f;
	else
		return GetDefensiveSlotCooldown() / GameInstance->DefensiveSpells.Find(DefensiveSpellActive)->Cooldown;
}

float AUmir::GetOffensiveSlot1Cooldown() const
{
	if (OffensiveSpellActive1 == EOffensiveSpell::E_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->OffensiveSpells.Find(OffensiveSpellActive1)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedOffensiveSpell1), 0.0f);
}

float AUmir::GetOffensiveSlot1CooldownPercentage() const
{
	if (OffensiveSpellActive1 == EOffensiveSpell::E_None)
		return 0.0f;
	else
		return GetOffensiveSlot1Cooldown() / GameInstance->OffensiveSpells.Find(OffensiveSpellActive1)->Cooldown;
}

float AUmir::GetOffensiveSlot2Cooldown() const
{
	if (OffensiveSpellActive2 == EOffensiveSpell::E_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->OffensiveSpells.Find(OffensiveSpellActive2)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedOffensiveSpell2), 0.0f);
}

float AUmir::GetOffensiveSlot2CooldownPercentage() const
{
	if (OffensiveSpellActive2 == EOffensiveSpell::E_None)
		return 0.0f;
	else
		return GetOffensiveSlot2Cooldown() / GameInstance->OffensiveSpells.Find(OffensiveSpellActive2)->Cooldown;
}

float AUmir::GetOffensiveSlot3Cooldown() const
{
	if (OffensiveSpellActive3 == EOffensiveSpell::E_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->OffensiveSpells.Find(OffensiveSpellActive3)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedOffensiveSpell3), 0.0f);
}

float AUmir::GetOffensiveSlot3CooldownPercentage() const
{
	if (OffensiveSpellActive3 == EOffensiveSpell::E_None)
		return 0.0f;
	else
		return GetOffensiveSlot3Cooldown() / GameInstance->OffensiveSpells.Find(OffensiveSpellActive3)->Cooldown;
}

float AUmir::GetPotionSlotCooldown() const
{
	if (PotionActive == EPotion::E_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->Potions.Find(PotionActive)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedPotion), 0.0f);
}

float AUmir::GetPotionSlotCooldownPercentage() const
{
	if (PotionActive == EPotion::E_None)
		return 0.0f;
	else
		return GetPotionSlotCooldown() / GameInstance->Potions.Find(PotionActive)->Cooldown;
}

void AUmir::AddDefensiveSpell(EDefensiveSpell DefensiveSpell)
{
	AquiredDefensiveSpells.Add(*GameInstance->DefensiveSpells.Find(DefensiveSpell));
}

void AUmir::AddOffensiveSpell(EOffensiveSpell OffensiveSpell)
{
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(OffensiveSpell));
}

bool AUmir::AddPotion(EPotion Potion)
{
	int32 Index = AquiredPotions.IndexOfByPredicate([Potion](const FPotion &A) {
		return A.Key == Potion;
	});

	if (Index == INDEX_NONE)
	{
		AquiredPotions.Add(*GameInstance->Potions.Find(Potion));
		return true;
	}
	else
	{
		if (AquiredPotions[Index].Quantity < AquiredPotions[Index].MaxQuantity)
		{
			AquiredPotions[Index].Quantity++;
			return true;
		}
		else
		{
			PotionsAreFull();
			return false;
		}
	}
}

bool AUmir::RemovePotion(EPotion Potion)
{
	int32 Index = AquiredPotions.IndexOfByPredicate([Potion](const FPotion &A) {
		return A.Key == Potion;
	});

	if (Index == INDEX_NONE)
	{
		return false;
	}
	else
	{
		if (AquiredPotions[Index].Quantity >= 2)
		{
			AquiredPotions[Index].Quantity--;
		}
		else
		{
			AquiredPotions.RemoveAt(Index);
		}
		return true;
	}
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
