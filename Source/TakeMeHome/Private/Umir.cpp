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
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::OS_Tornado));
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::OS_Starfall));
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::OS_Force_Push));
	AquiredOffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(EOffensiveSpell::OS_Lightning_Bolt));

	// Add normal attacks
	AquiredNormalAttacks.Add(*GameInstance->NormalAttacks.Find(EPhysicalAttack::PA_Melee_Attack_1));
	AquiredNormalAttacks.Add(*GameInstance->NormalAttacks.Find(EPhysicalAttack::PA_Melee_Attack_2));
	AquiredNormalAttacks.Add(*GameInstance->NormalAttacks.Find(EPhysicalAttack::PA_Melee_Attack_3));

	// Add defensive spells
	AquiredDefensiveSpells.Add(*GameInstance->DefensiveSpells.Find(EDefensiveSpell::DS_Counter_Strike));
	AquiredDefensiveSpells.Add(*GameInstance->DefensiveSpells.Find(EDefensiveSpell::DS_Star_Shield));
	AquiredDefensiveSpells.Add(*GameInstance->DefensiveSpells.Find(EDefensiveSpell::DS_Spirit_Walk));

	// Add potions
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::P_Healing_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::P_Mana_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::P_Fire_Elemental_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::P_Nature_Elemental_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::P_Water_Elemental_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::P_Lightning_Elemental_Potion));
	AquiredPotions.Add(*GameInstance->Potions.Find(EPotion::P_Earth_Elemental_Potion));

	// Add items
	//Inventory->Items.Init(*GameInstance->Items.Find(EItem::E_Empty), 100);
	Inventory->AddItems(EItem::I_Item_1, 2);
	Inventory->AddItems(EItem::I_Item_2, 14);
	Inventory->AddItems(EItem::I_Item_3, 35);
	Inventory->AddItems(EItem::I_Item_4, 551);
}

void AUmir::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ensure(Decal)) { return; }


	// Activate correct decal type
	switch (ActiveDecal)
	{
	case EDecalType::DT_None:
		Decal->SetVisibility(false);
		break;
	case EDecalType::DT_Spell_Circle:
		Decal->SetVisibility(true);
		MoveDecalToMouseHitLocation();
		break;
	case EDecalType::DT_Arrow:
		RotateDecalAroundPlayer();
		Decal->SetVisibility(true);
		break;
	case EDecalType::DT_Box_Indicator:
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
	PlayerInputComponent->BindAction("Physical1/ActivatedSpell", IE_Pressed, this, &AUmir::ActivatePhysical1OrCastSpell);
	PlayerInputComponent->BindAction("Physical2/CancelSpell", IE_Pressed, this, &AUmir::ActivatePhysical2OrCancel);
	PlayerInputComponent->BindAction("ActivateOffensiveSpell1", IE_Pressed, this, &AUmir::ActivateOffensiveSlot1);
	PlayerInputComponent->BindAction("ActivateOffensiveSpell2", IE_Pressed, this, &AUmir::ActivateOffensiveSlot2);
	PlayerInputComponent->BindAction("ActivateOffensiveSpell3", IE_Pressed, this, &AUmir::ActivateOffensiveSlot3);
	PlayerInputComponent->BindAction("ActivateDefensiveSpell", IE_Pressed, this, &AUmir::ActivateDefensiveSlot);
	PlayerInputComponent->BindAction("ActivatePotion", IE_Pressed, this, &AUmir::ActivatePotionSlot);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUmir::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUmir::MoveRight);
	PlayerInputComponent->BindAxis("LookRight", this, &AUmir::LookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AUmir::LookUp);
	PlayerInputComponent->BindAxis("LookRightRate", this, &AUmir::LookRightRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AUmir::LookUpRate);
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
	if (bStopMovingCamera) return;

	AddControllerYawInput(NormalizedRate);
}

void AUmir::LookUp(float NormalizedRate)
{
	if (bStopMovingCamera) return;

	AddControllerPitchInput(-NormalizedRate);
}

void AUmir::LookRightRate(float Rate)
{
	if (bStopMovingCamera) return;

	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUmir::LookUpRate(float Rate)
{
	if (bStopMovingCamera) return;

	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AUmir::Zoom(float NormalizedRate)
{
	if (!bStopZooming)
	{
		CameraBoom->TargetArmLength += NormalizedRate * ZoomStrength;
		CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, MinZoom, MaxZoom);
	}
}

void AUmir::ActivatePhysical1OrCastSpell()
{
	// Cast activated offensive spell
	if (ActivatedOffensiveSpell != EOffensiveSpell::OS_None)
	{
		CastOffensiveSpell(ActivatedOffensiveSpell);
		ActivatedOffensiveSpell = EOffensiveSpell::OS_None;
		ActiveDecal = EDecalType::DT_None;
	}
	// Cast activated defensive spell
	else if (ActivatedDefensiveSpell != EDefensiveSpell::DS_None)
	{
		CastDefensiveSpell(ActivatedDefensiveSpell);
		ActivatedDefensiveSpell = EDefensiveSpell::DS_None;
		ActiveDecal = EDecalType::DT_None;
	}
	else
	{
		UsePhysicalAttack1();
	}
}

void AUmir::ActivatePhysical2OrCancel()
{
	bool bDidCancel = CancelActivatedSpell();

	if (!bDidCancel)
	{
		UsePhysicalAttack2();
	}
}

void AUmir::ActivateOffensiveSlot1()
{
	// Check if on cooldown or spell is not bound
	if (OffensiveSpell1Bound == EOffensiveSpell::OS_None) return;
	if (!FMath::IsNearlyEqual(GetOffensiveSlot1Cooldown(), 0.0f)) { IsOnCooldown(); return; }
	if (!bCanCastSpell) return;

	FOffensiveSpell *Spell = GameInstance->OffensiveSpells.Find(OffensiveSpell1Bound);
	if (!ensure(Spell)) return;

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedOffensiveSpell = OffensiveSpell1Bound;
		ResetMousePos();
		switch (Spell->DecalType)
		{
		case EDecalType::DT_Spell_Circle:
			Decal->SetDecalMaterial(SpellCircleMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::DT_Arrow:
			Decal->SetDecalMaterial(SpellArrowMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::DT_Box_Indicator:
			Decal->SetDecalMaterial(SpellBoxMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Spell has no valid decal type"));
			break;
		}
	}
	else
	{
		LastTimeActivatedOffensiveSpell1 = GetWorld()->GetTimeSeconds();
		CastOffensiveSpell(OffensiveSpell1Bound);
	}
}

void AUmir::ActivateOffensiveSlot2()
{
	// Check if on cooldown or spell is not bound
	if (OffensiveSpell2Bound == EOffensiveSpell::OS_None) return;
	if (!FMath::IsNearlyEqual(GetOffensiveSlot2Cooldown(), 0.0f)) { IsOnCooldown(); return; }
	if (!bCanCastSpell) return;

	FOffensiveSpell *Spell = GameInstance->OffensiveSpells.Find(OffensiveSpell2Bound);
	if (!ensure(Spell)) return;

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedOffensiveSpell = OffensiveSpell2Bound;
		ResetMousePos();
		switch (Spell->DecalType)
		{
		case EDecalType::DT_Spell_Circle:
			Decal->SetDecalMaterial(SpellCircleMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::DT_Arrow:
			Decal->SetDecalMaterial(SpellArrowMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::DT_Box_Indicator:
			Decal->SetDecalMaterial(SpellBoxMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Spell has no valid decal type"));
			break;
		}
	}
	else
	{
		LastTimeActivatedOffensiveSpell2 = GetWorld()->GetTimeSeconds();
		CastOffensiveSpell(OffensiveSpell2Bound);
	}
}

void AUmir::ActivateOffensiveSlot3()
{
	// Check if on cooldown or spell is not bound
	if (OffensiveSpell3Bound == EOffensiveSpell::OS_None) return;
	if (!FMath::IsNearlyEqual(GetOffensiveSlot3Cooldown(), 0.0f)) { IsOnCooldown(); return; }
	if (!bCanCastSpell) return;

	FOffensiveSpell *Spell = GameInstance->OffensiveSpells.Find(OffensiveSpell3Bound);
	if (!ensure(Spell)) return;

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedOffensiveSpell = OffensiveSpell3Bound;
		ResetMousePos();
		switch (Spell->DecalType)
		{
		case EDecalType::DT_Spell_Circle:
			Decal->SetDecalMaterial(SpellCircleMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::DT_Arrow:
			Decal->SetDecalMaterial(SpellArrowMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::DT_Box_Indicator:
			Decal->SetDecalMaterial(SpellBoxMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Spell has no valid decal type"));
			break;
		}
	}
	else
	{
		LastTimeActivatedOffensiveSpell3 = GetWorld()->GetTimeSeconds();
		CastOffensiveSpell(OffensiveSpell3Bound);
	}
}

void AUmir::ActivateDefensiveSlot()
{
	UE_LOG(LogTemp, Warning, TEXT("activate defensive slot"));

	// Check if on cooldown or spell is not bound
	if (DefensiveSpellBound == EDefensiveSpell::DS_None) return;
	if (!FMath::IsNearlyEqual(GetDefensiveSlotCooldown(), 0.0f)) { IsOnCooldown(); return; }
	if (!bCanCastSpell) return;

	FDefensiveSpell *Spell = GameInstance->DefensiveSpells.Find(DefensiveSpellBound);
	if (!ensure(Spell)) return;

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedDefensiveSpell = DefensiveSpellBound;
		ResetMousePos();

		switch (Spell->DecalType)
		{
		case EDecalType::DT_Spell_Circle:
			Decal->SetDecalMaterial(SpellCircleMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::DT_Arrow:
			Decal->SetDecalMaterial(SpellArrowMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		case EDecalType::DT_Box_Indicator:
			Decal->SetDecalMaterial(SpellBoxMaterial);
			ActiveDecal = Spell->DecalType;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Spell has no valid decal type"));
			break;
		}
	}
	else
	{
		LastTimeActivatedDefensiveSpell = GetWorld()->GetTimeSeconds();
		CastDefensiveSpell(DefensiveSpellBound);
	}
}

void AUmir::ActivatePotionSlot()
{
	UE_LOG(LogTemp, Warning, TEXT("activate potion slot"));

	// Check if on cooldown or spell is not bound
	if (PotionBound == EPotion::P_None) return;
	if (!FMath::IsNearlyEqual(GetPotionSlotCooldown(), 0.0f)) { IsOnCooldown(); return; }
	if (!bCanCastSpell) return;

	LastTimeActivatedPotion = GetWorld()->GetTimeSeconds();
	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Using potion"));
}

void AUmir::MoveDecalToMouseHitLocation()
{
	if (!ensure(Decal)) return;
	if (ActivatedOffensiveSpell == EOffensiveSpell::OS_None) return;

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
		FVector NewStartPos = MouseLocation + (MouseDirection * 10000.0f);
		FVector NewEndPos = FVector::UpVector * -MaxTraceDistance;
		bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, NewStartPos, NewEndPos, ECC_Visibility);
		if (bFoundGround == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find ground on second attempt"));
			return;
		}
	}

	float SpellRange = GameInstance->OffensiveSpells.Find(ActivatedOffensiveSpell)->Range;
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
	if (ActivatedOffensiveSpell == EOffensiveSpell::OS_None) return;

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

void AUmir::ResetMousePos()
{
	if (!GetWorld()->GetFirstPlayerController()->ShouldShowMouseCursor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Mouse is showing"));

		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		GetWorld()->GetFirstPlayerController()->SetMouseLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	}
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly().SetConsumeCaptureMouseDown(false));
	bStopMovingCamera = true;
}

bool AUmir::CancelActivatedSpell()
{
	if (ActivatedOffensiveSpell != EOffensiveSpell::OS_None || ActivatedDefensiveSpell != EDefensiveSpell::DS_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("cancelled activated spell"));

		ActivatedOffensiveSpell = EOffensiveSpell::OS_None;
		ActivatedDefensiveSpell = EDefensiveSpell::DS_None;
		ActiveDecal = EDecalType::DT_None;
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		bStopMovingCamera = false;
		return true;
	}
	else
	{
		return false;
	}
}

void AUmir::CastOffensiveSpell(EOffensiveSpell SpellKey)
{
	if (SpellKey == EOffensiveSpell::OS_None) return;

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	bStopMovingCamera = false;

	auto *Spell = GameInstance->OffensiveSpells.Find(SpellKey);
	// Check for mana
	if (CurrentMana < Spell->ManaCost)
	{
		NotEnoughMana();
	}
	else
	{
		// Set cooldown
		if (SpellKey == OffensiveSpell1Bound)
		{
			LastTimeActivatedOffensiveSpell1 = GetWorld()->GetTimeSeconds();
		}
		else if (SpellKey == OffensiveSpell2Bound)
		{
			LastTimeActivatedOffensiveSpell2 = GetWorld()->GetTimeSeconds();
		}
		else if (SpellKey == OffensiveSpell3Bound)
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
		if (Spell->DecalType == EDecalType::DT_Spell_Circle)
		{
			SpawnLocation = Decal->GetComponentLocation();
		}
		else if (Spell->DecalType == EDecalType::DT_Arrow)
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

void AUmir::CastDefensiveSpell(EDefensiveSpell SpellKey)
{
	if (SpellKey == EDefensiveSpell::DS_None) return;

	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	bStopMovingCamera = false;

	auto *Spell = GameInstance->DefensiveSpells.Find(SpellKey);

	// Set cooldown
	if (SpellKey == DefensiveSpellBound)
	{
		LastTimeActivatedDefensiveSpell = GetWorld()->GetTimeSeconds();
	}
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
	if (Spell->DecalType == EDecalType::DT_Spell_Circle)
	{
		SpawnLocation = Decal->GetComponentLocation();
	}
	else if (Spell->DecalType == EDecalType::DT_Arrow)
	{
		SpawnRotation = (-Decal->GetRightVector()).Rotation();
	}
	auto SpawnedActor = GetWorld()->SpawnActor<ASpellBase>(Spell->ClassRef, SpawnLocation, SpawnRotation);


	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Cast Defensive spell"));
}

void AUmir::UsePhysicalAttack1()
{
	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Used physical attack 1"));
}

void AUmir::UsePhysicalAttack2()
{
	// TODO
	UE_LOG(LogTemp, Warning, TEXT("Used physical attack 2"));
}

float AUmir::GetHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

float AUmir::GetManaPercentage() const
{
	return CurrentMana / MaxMana;
}

FPotion AUmir::GetPotion(EPotion Key) const
{
	if (Key != EPotion::P_None)
	{
		return *AquiredPotions.FindByPredicate([Key](const FPotion &A) {
			return A.Key == Key;
		});
	}
	else
	{
		return FPotion();
	}
}

float AUmir::GetDefensiveSlotCooldown() const
{
	if (DefensiveSpellBound == EDefensiveSpell::DS_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->DefensiveSpells.Find(DefensiveSpellBound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedDefensiveSpell), 0.0f);
}

float AUmir::GetDefensiveSlotCooldownPercentage() const
{
	if (DefensiveSpellBound == EDefensiveSpell::DS_None)
		return 0.0f;
	else
		return GetDefensiveSlotCooldown() / GameInstance->DefensiveSpells.Find(DefensiveSpellBound)->Cooldown;
}

float AUmir::GetOffensiveSlot1Cooldown() const
{
	if (OffensiveSpell1Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->OffensiveSpells.Find(OffensiveSpell1Bound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedOffensiveSpell1), 0.0f);
}

float AUmir::GetOffensiveSlot1CooldownPercentage() const
{
	if (OffensiveSpell1Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return GetOffensiveSlot1Cooldown() / GameInstance->OffensiveSpells.Find(OffensiveSpell1Bound)->Cooldown;
}

float AUmir::GetOffensiveSlot2Cooldown() const
{
	if (OffensiveSpell2Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->OffensiveSpells.Find(OffensiveSpell2Bound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedOffensiveSpell2), 0.0f);
}

float AUmir::GetOffensiveSlot2CooldownPercentage() const
{
	if (OffensiveSpell2Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return GetOffensiveSlot2Cooldown() / GameInstance->OffensiveSpells.Find(OffensiveSpell2Bound)->Cooldown;
}

float AUmir::GetOffensiveSlot3Cooldown() const
{
	if (OffensiveSpell3Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->OffensiveSpells.Find(OffensiveSpell3Bound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedOffensiveSpell3), 0.0f);
}

float AUmir::GetOffensiveSlot3CooldownPercentage() const
{
	if (OffensiveSpell3Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return GetOffensiveSlot3Cooldown() / GameInstance->OffensiveSpells.Find(OffensiveSpell3Bound)->Cooldown;
}

float AUmir::GetPotionSlotCooldown() const
{
	if (PotionBound == EPotion::P_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->Potions.Find(PotionBound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedPotion), 0.0f);
}

float AUmir::GetPotionSlotCooldownPercentage() const
{
	if (PotionBound == EPotion::P_None)
		return 0.0f;
	else
		return GetPotionSlotCooldown() / GameInstance->Potions.Find(PotionBound)->Cooldown;
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
	DefensiveSpellBound = DefensiveSpell;
}

void AUmir::BindOffensiveSpell1(EOffensiveSpell OffensiveSpell)
{
	OffensiveSpell1Bound = OffensiveSpell;
}

void AUmir::BindOffensiveSpell2(EOffensiveSpell OffensiveSpell)
{
	OffensiveSpell2Bound = OffensiveSpell;
}

void AUmir::BindOffensiveSpell3(EOffensiveSpell OffensiveSpell)
{
	OffensiveSpell3Bound = OffensiveSpell;
}

void AUmir::BindPotion(EPotion Potion)
{
	PotionBound = Potion;
}
