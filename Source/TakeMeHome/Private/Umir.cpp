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
#include "Abilities.h"
#include "SpellBase.h"
#include "TakeMeHomeGameInstance.h"
#include "Inventory.h"
#include "Public/TimerManager.h"
#include "DefensiveSpellBase.h"


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

	// Create inventory and abilities
	Inventory = CreateDefaultSubobject<UInventory>(FName("Inventory"));
	Abilities = CreateDefaultSubobject<UAbilities>(FName("Abilities"));
}

void AUmir::BeginPlay()
{
	Super::BeginPlay();

	// Subscribe to death event
	OnDeathDelegate.AddUniqueDynamic(this, &AUmir::OnDeath);

	// Set camera init rotation
	CameraBoom->SetRelativeRotation(GetControlRotation());

	// Adding spells
	Abilities->AddOffensive(EOffensiveSpell::OS_Tornado);
	Abilities->AddOffensive(EOffensiveSpell::OS_Starfall);
	Abilities->AddOffensive(EOffensiveSpell::OS_Force_Push);
	Abilities->AddOffensive(EOffensiveSpell::OS_Lightning_Bolt);

	// Add physical attacks
	Abilities->AddPhysical(EPhysicalAttack::PA_Fast_Attack);
	Abilities->AddPhysical(EPhysicalAttack::PA_Slow_Attack);

	// Add defensive spells
	Abilities->AddDefensive(EDefensiveSpell::DS_Counter_Strike);
	Abilities->AddDefensive(EDefensiveSpell::DS_Star_Shield);
	Abilities->AddDefensive(EDefensiveSpell::DS_Spirit_Walk);

	// Add potions
	Abilities->AddPotion(EPotion::P_Healing_Potion);
	Abilities->AddPotion(EPotion::P_Mana_Potion);
	Abilities->AddPotion(EPotion::P_Fire_Elemental_Potion);
	Abilities->AddPotion(EPotion::P_Nature_Elemental_Potion);
	Abilities->AddPotion(EPotion::P_Water_Elemental_Potion);
	Abilities->AddPotion(EPotion::P_Lightning_Elemental_Potion);
	Abilities->AddPotion(EPotion::P_Earth_Elemental_Potion);

	// Add items
	Inventory->AddItems(EItem::I_Item_1, 2);
	Inventory->AddItems(EItem::I_Item_2, 14);
	Inventory->AddItems(EItem::I_Item_3, 35);
	Inventory->AddItems(EItem::I_Item_4, 551);
}

void AUmir::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ensure(Decal)) return;

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

}

void AUmir::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AUmir::StartJumping);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Physical1/ActivatedSpell", IE_Pressed, this, &AUmir::UseFastAttackOrCastSpell);
	PlayerInputComponent->BindAction("Physical2/CancelSpell", IE_Pressed, this, &AUmir::UseSlowAttackOrCancel);
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

void AUmir::StartJumping()
{
	if (bCanMove)
	{
		Jump();
		InterruptCasting();
	}
}

void AUmir::MoveForward(float NormalizedRate)
{
	if (!FMath::IsNearlyZero(NormalizedRate) && bCanMove && !bIsCasting)
	{
		InterruptCasting();

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
	if (!FMath::IsNearlyZero(NormalizedRate) && bCanMove && !bIsCasting)
	{
		InterruptCasting();

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
	if (bCanMoveCamera)
	{
		AddControllerYawInput(NormalizedRate);
	}
}

void AUmir::LookUp(float NormalizedRate)
{
	if (bCanMoveCamera)
	{
		AddControllerPitchInput(-NormalizedRate);
	}
}

void AUmir::LookRightRate(float Rate)
{
	if (bCanMoveCamera)
	{
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AUmir::LookUpRate(float Rate)
{
	if (bCanMoveCamera)
	{
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void AUmir::Zoom(float NormalizedRate)
{
	if (bCanZoom)
	{
		CameraBoom->TargetArmLength += NormalizedRate * ZoomStrength;
		CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength, MinZoom, MaxZoom);
	}
}

void AUmir::UseFastAttackOrCastSpell()
{
	// Cast activated offensive spell
	if (ActivatedOffensiveSpell != EOffensiveSpell::OS_None)
	{
		UseOffensiveSpell(ActivatedOffensiveSpell, FTransform());
		OnRemoveHighlight.Broadcast(false);

		ActivatedOffensiveSpell = EOffensiveSpell::OS_None;
		ActiveDecal = EDecalType::DT_None;
	}
	// Cast activated defensive spell
	else if (ActivatedDefensiveSpell != EDefensiveSpell::DS_None)
	{
		UseDefensiveSpell(ActivatedDefensiveSpell, FTransform());
		OnRemoveHighlight.Broadcast(false);
		ActivatedDefensiveSpell = EDefensiveSpell::DS_None;
		ActiveDecal = EDecalType::DT_None;
	}
	// Else use fast physical attack
	else
	{
		UsePhysicalAttack(EPhysicalAttack::PA_Fast_Attack);
	}
}

void AUmir::UseSlowAttackOrCancel()
{
	// Cancel spell
	bool bDidCancel = CancelActivatedSpell();

	// Else use slow physical attack
	if (!bDidCancel)
	{
		UsePhysicalAttack(EPhysicalAttack::PA_Slow_Attack);
		OnRemoveHighlight.Broadcast(false);
	}
}

void AUmir::ActivateOffensiveSlot1()
{
	// Check if on cooldown or spell is not bound
	if (OffensiveSpell1Bound == EOffensiveSpell::OS_None) return;
	if (!FMath::IsNearlyZero(GetOffensiveSlot1Cooldown())) { IsOnCooldown(); return; }
	if (!bCanUseSpell) return;

	auto *Spell = GameInstance->GetOffensiveSpell(OffensiveSpell1Bound);

	// Restore action bar hightlight
	OnRemoveHighlight.Broadcast(false);

	// Check for mana
	if (CurrentMana < Spell->ManaCost) { NotEnoughMana(); return; }

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
		OnOffensive1Cast.Broadcast(false);
	}
	else
	{
		OnOffensive1Cast.Broadcast(true);
		UseOffensiveSpell(OffensiveSpell1Bound, FTransform());
	}
}

void AUmir::ActivateOffensiveSlot2()
{
	// Check if on cooldown or spell is not bound
	if (OffensiveSpell2Bound == EOffensiveSpell::OS_None) return;
	if (!FMath::IsNearlyZero(GetOffensiveSlot2Cooldown())) { IsOnCooldown(); return; }
	if (!bCanUseSpell) return;

	FOffensiveSpell *Spell = GameInstance->GetOffensiveSpell(OffensiveSpell2Bound);

	// Restore action bar hightlight
	OnRemoveHighlight.Broadcast(false);

	// Check for mana
	if (CurrentMana < Spell->ManaCost) { NotEnoughMana(); return; }

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
		OnOffensive2Cast.Broadcast(false);
	}
	else
	{
		OnOffensive2Cast.Broadcast(true);
		UseOffensiveSpell(OffensiveSpell2Bound, FTransform());
	}
}

void AUmir::ActivateOffensiveSlot3()
{
	// Check if on cooldown or spell is not bound
	if (OffensiveSpell3Bound == EOffensiveSpell::OS_None) return;
	if (!FMath::IsNearlyZero(GetOffensiveSlot3Cooldown())) { IsOnCooldown(); return; }
	if (!bCanUseSpell) return;

	FOffensiveSpell *Spell = GameInstance->GetOffensiveSpell(OffensiveSpell3Bound);

	// Restore action bar hightlight
	OnRemoveHighlight.Broadcast(false);

	// Check for mana
	if (CurrentMana < Spell->ManaCost) { NotEnoughMana(); return; }

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
		OnOffensive3Cast.Broadcast(false);
	}
	else
	{
		OnOffensive3Cast.Broadcast(true);
		UseOffensiveSpell(OffensiveSpell3Bound, FTransform());
	}
}

void AUmir::ActivateDefensiveSlot()
{
	UE_LOG(LogTemp, Warning, TEXT("activate defensive slot"));

	// Check if on cooldown or spell is not bound
	if (DefensiveSpellBound == EDefensiveSpell::DS_None) return;
	if (!FMath::IsNearlyZero(GetDefensiveSlotCooldown())) { IsOnCooldown(); return; }
	if (!bCanUseSpell) return;

	FDefensiveSpell *Spell = GameInstance->GetDefensiveSpell(DefensiveSpellBound);

	// Restore action bar hightlight
	OnRemoveHighlight.Broadcast(false);

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
		OnDefensiveCast.Broadcast(false);
	}
	else
	{
		OnDefensiveCast.Broadcast(true);
		UseDefensiveSpell(DefensiveSpellBound, FTransform());
	}
}

void AUmir::ActivatePotionSlot()
{
	// Check if allowed to use potion
	if (PotionBound == EPotion::P_None) return;
	if (!FMath::IsNearlyZero(GetPotionSlotCooldown())) { IsOnCooldown(); return; }
	if (!bCanUseSpell) return;

	// Set cooldown and remove potion
	if (Abilities->RemovePotion(PotionBound))
	{
		OnRemoveHighlight.Broadcast(true);
		OnPotionCast.Broadcast(true);
		LastTimeActivatedPotion = GetWorld()->GetTimeSeconds();
		UsePotion(PotionBound);
	}
}

void AUmir::MoveDecalToMouseHitLocation()
{
	if (!ensure(Decal)) return;
	if (ActivatedOffensiveSpell == EOffensiveSpell::OS_None) return;

	// Get spell
	auto Spell = GameInstance->GetOffensiveSpell(ActivatedOffensiveSpell);

	ResetDecalSize(Spell->DecalRadius);

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

	float SpellRange = Spell->Range;
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

	ResetDecalSize();

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

void AUmir::ResetDecalSize(float Radius)
{
	// Reset decal scale
	Decal->RelativeScale3D.Y = 1.0f;
	Decal->RelativeScale3D.Z = 1.0f;
	Decal->DecalSize.Z = Radius;
	Decal->DecalSize.Y = Radius;
}

void AUmir::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s died (Umir)"), *GetName());
	GetWorld()->GetFirstPlayerController()->StartSpectatingOnly();
}

void AUmir::ResetMousePos()
{
	auto PC = GetWorld()->GetFirstPlayerController();
	if (!PC->ShouldShowMouseCursor())
	{
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		GetWorld()->GetFirstPlayerController()->SetMouseLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	}
	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameOnly().SetConsumeCaptureMouseDown(false));
	bCanMoveCamera = false;
}

bool AUmir::CancelActivatedSpell()
{
	if (ActivatedOffensiveSpell != EOffensiveSpell::OS_None || ActivatedDefensiveSpell != EDefensiveSpell::DS_None)
	{
		ActivatedOffensiveSpell = EOffensiveSpell::OS_None;
		ActivatedDefensiveSpell = EDefensiveSpell::DS_None;
		ActiveDecal = EDecalType::DT_None;
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		bCanMoveCamera = true;
		return true;
	}
	else
	{
		return false;
	}
}

void AUmir::UseOffensiveSpell(EOffensiveSpell Key, FTransform SpawnTransform)
{
	if (Key == EOffensiveSpell::OS_None) return;

	// Disable mouse
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	bCanMoveCamera = true;

	// Get offensive spell
	auto *Spell = GameInstance->GetOffensiveSpell(Key);

	// Check for mana
	if (CurrentMana < Spell->ManaCost)
	{
		NotEnoughMana();
		CancelActivatedSpell();
		return;
	}

	// Drain mana
	DrainMana(Spell->ManaCost);

	// Set cooldown
	if (Key == OffensiveSpell1Bound)
	{
		LastTimeActivatedOffensiveSpell1 = GetWorld()->GetTimeSeconds();
	}
	else if (Key == OffensiveSpell2Bound)
	{
		LastTimeActivatedOffensiveSpell2 = GetWorld()->GetTimeSeconds();
	}
	else if (Key == OffensiveSpell3Bound)
	{
		LastTimeActivatedOffensiveSpell3 = GetWorld()->GetTimeSeconds();
	}

	// Find correct spawn location/rotation
	SpawnTransform.SetLocation(GetActorLocation());
	SpawnTransform.SetRotation(GetActorRotation().Quaternion());
	if (Spell->DecalType == EDecalType::DT_Spell_Circle)
	{
		SpawnTransform.SetLocation(Decal->GetComponentLocation());
	}
	else if (Spell->DecalType == EDecalType::DT_Arrow)
	{
		// TODO find decal arrow direction
		SpawnTransform.SetRotation((-Decal->GetRightVector()).Rotation().Quaternion());
	}

	// Check for cast time
	if (Spell->CastTime > 0.01f)
	{
		StartCasting(Spell->CastTime);
	}

	// Spawning offensive spell
	auto SpawnedActor = GetWorld()->SpawnActorDeferred<ASpellBase>(Spell->ClassRef, SpawnTransform);
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->Damage = Spell->Damage;
	SpawnedActor->CastTime = Spell->CastTime;
	SpawnedActor->StunDuration = Spell->StunDuration;
	SpawnedActor->DecalRadius = Spell->DecalRadius;
	Spell->ElementType == EElement::E_Neutral ? SpawnedActor->ElementType = ActiveElement : SpawnedActor->ElementType = Spell->ElementType;
	SpawnedActor->FinishSpawning(SpawnTransform);
}

void AUmir::UseDefensiveSpell(EDefensiveSpell Key, FTransform SpawnTransform)
{
	if (Key == EDefensiveSpell::DS_None) return;

	// Disable mouse
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	bCanMoveCamera = true;

	// Set cooldown
	LastTimeActivatedDefensiveSpell = GetWorld()->GetTimeSeconds();

	// Find defensive spell
	auto *Spell = GameInstance->GetDefensiveSpell(Key);

	// Find spawn transform
	SpawnTransform.SetLocation(GetActorLocation());
	SpawnTransform.SetRotation(GetActorRotation().Quaternion());
	if (Spell->DecalType == EDecalType::DT_Spell_Circle)
	{
		SpawnTransform.SetLocation(Decal->GetComponentLocation());
	}
	else if (Spell->DecalType == EDecalType::DT_Arrow)
	{
		SpawnTransform.SetRotation((-Decal->GetRightVector()).Rotation().Quaternion());
	}

	// Call the base version
	Super::UseDefensiveSpell(Key, SpawnTransform);
}

float AUmir::GetDefensiveSlotCooldown() const
{
	if (DefensiveSpellBound == EDefensiveSpell::DS_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->GetDefensiveSpell(DefensiveSpellBound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedDefensiveSpell), 0.0f);
}

float AUmir::GetDefensiveSlotCooldownPercentage() const
{
	if (DefensiveSpellBound == EDefensiveSpell::DS_None)
		return 0.0f;
	else
		return GetDefensiveSlotCooldown() / GameInstance->GetDefensiveSpell(DefensiveSpellBound)->Cooldown;
}

float AUmir::GetOffensiveSlot1Cooldown() const
{
	if (OffensiveSpell1Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->GetOffensiveSpell(OffensiveSpell1Bound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedOffensiveSpell1), 0.0f);
}

float AUmir::GetOffensiveSlot1CooldownPercentage() const
{
	if (OffensiveSpell1Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return GetOffensiveSlot1Cooldown() / GameInstance->GetOffensiveSpell(OffensiveSpell1Bound)->Cooldown;
}

float AUmir::GetOffensiveSlot2Cooldown() const
{
	if (OffensiveSpell2Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->GetOffensiveSpell(OffensiveSpell2Bound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedOffensiveSpell2), 0.0f);
}

float AUmir::GetOffensiveSlot2CooldownPercentage() const
{
	if (OffensiveSpell2Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return GetOffensiveSlot2Cooldown() / GameInstance->GetOffensiveSpell(OffensiveSpell2Bound)->Cooldown;
}

float AUmir::GetOffensiveSlot3Cooldown() const
{
	if (OffensiveSpell3Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->GetOffensiveSpell(OffensiveSpell3Bound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedOffensiveSpell3), 0.0f);
}

float AUmir::GetOffensiveSlot3CooldownPercentage() const
{
	if (OffensiveSpell3Bound == EOffensiveSpell::OS_None)
		return 0.0f;
	else
		return GetOffensiveSlot3Cooldown() / GameInstance->GetOffensiveSpell(OffensiveSpell3Bound)->Cooldown;
}

float AUmir::GetPotionSlotCooldown() const
{
	if (PotionBound == EPotion::P_None)
		return 0.0f;
	else
		return FMath::Max(GameInstance->GetPotion(PotionBound)->Cooldown - (GetWorld()->GetTimeSeconds() - LastTimeActivatedPotion), 0.0f);
}

float AUmir::GetPotionSlotCooldownPercentage() const
{
	if (PotionBound == EPotion::P_None)
		return 0.0f;
	else
		return GetPotionSlotCooldown() / GameInstance->GetPotion(PotionBound)->Cooldown;
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

void AUmir::StartCasting(float CastDuration)
{
	Super::StartCasting(CastDuration);

	// Umir also needs to handle casting bar
	CastingBarActivated();
}

bool AUmir::InterruptCasting()
{
	bool bWasCasting = Super::InterruptCasting();

	// Umir also needs to handle casting bar
	if (bWasCasting)
	{
		CastingBarInterrupted();
	}
	return bWasCasting;
}

void AUmir::CastSuccesfull()
{
	Super::CastSuccesfull();

	// Umir also needs to handle casting bar
	CastingBarSucceded();
}

float AUmir::GetCastingPercentage() const
{
	auto CurrentCastTime = GetWorld()->GetTimeSeconds() - TimeCastingBegan;
	auto TotalCastTime = TimeCastingEnds - TimeCastingBegan;

	return CurrentCastTime / TotalCastTime;
}
