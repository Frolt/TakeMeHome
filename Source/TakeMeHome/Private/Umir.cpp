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
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "Tornado.h"
#include "Starfall.h"
#include "ForcePush.h"
#include "LiquidDeath.h"
#include "Abilities.h"
#include "OffensiveSpellBase.h"
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

	//// Add offensive spells
	//Abilities->AddOffensive(EOffensiveSpell::OS_Tornado);
	//Abilities->AddOffensive(EOffensiveSpell::OS_Starfall);
	//Abilities->AddOffensive(EOffensiveSpell::OS_Force_Push);
	//Abilities->AddOffensive(EOffensiveSpell::OS_Liquid_Death);
	//Abilities->AddOffensive(EOffensiveSpell::OS_Vaccum);
	//Abilities->AddOffensive(EOffensiveSpell::OS_Death_Plant);
	//Abilities->AddOffensive(EOffensiveSpell::OS_Meteor);

	// Add physical attacks
	Abilities->AddPhysical(EPhysicalAttack::PA_Fast_Attack);
	Abilities->AddPhysical(EPhysicalAttack::PA_Slow_Attack);

	// Add defensive spells
	//Abilities->AddDefensive(EDefensiveSpell::DS_Counter_Strike);
	//Abilities->AddDefensive(EDefensiveSpell::DS_Star_Shield);
	Abilities->AddDefensive(EDefensiveSpell::DS_Spirit_Walk);

	//// Add potions
	//Abilities->AddPotion(EPotion::P_Healing_Potion, 10);
	//Abilities->AddPotion(EPotion::P_Mana_Potion, 10);
	//Abilities->AddPotion(EPotion::P_Fire_Elemental_Potion, 10);
	//Abilities->AddPotion(EPotion::P_Nature_Elemental_Potion, 10);
	//Abilities->AddPotion(EPotion::P_Water_Elemental_Potion, 10);

	//// Add items
	//Inventory->AddItems(EItem::I_Item_1, 2);
	//Inventory->AddItems(EItem::I_Item_2, 14);
	//Inventory->AddItems(EItem::I_Item_3, 35);
	//Inventory->AddItems(EItem::I_Item_4, 551);
}

void AUmir::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ensure(Decal)) return;

	// Activate correct decal type
	FindDecalMode();

	// Enter spell placing mode
	TogglePlaceSpellMode();
}

void AUmir::TogglePlaceSpellMode()
{
	if (ActiveDecal == EDecalType::DT_Spell_Circle && bCanPlaceSpell)
	{
		bCanPlaceSpell = false;

		// Place spell event
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
		PlaceSpell();
	}
	else if (ActiveDecal != EDecalType::DT_Spell_Circle && !bCanPlaceSpell)
	{
		bCanPlaceSpell = true;

		// Restore variables
		bCanMoveCamera = true;
		bCanPitchCamera = true;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		
		CustomTimeDilation = 1.0f;
		GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
		FinishedPlacingSpell();
	}
}

void AUmir::FindDecalMode()
{
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
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AUmir::StartJumping);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
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
	if (bCanMoveCamera && bCanPitchCamera)
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
	}
	// Cast activated defensive spell
	else if (ActivatedDefensiveSpell != EDefensiveSpell::DS_None)
	{
		UseDefensiveSpell(ActivatedDefensiveSpell, FTransform());
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
	OnRemoveHighlight.Broadcast();

	// Check for mana
	if (CurrentMana < Spell->ManaCost) { NotEnoughMana(); return; }

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedOffensiveSpell = OffensiveSpell1Bound;
		EnterCastMode();

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

	// Reset mouse pos if timer is up
	if (bShouldResetMouse)
	{
		bShouldResetMouse = false;
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		GetWorld()->GetFirstPlayerController()->SetMouseLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
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
	OnRemoveHighlight.Broadcast();

	// Check for mana
	if (CurrentMana < Spell->ManaCost) { NotEnoughMana(); return; }

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedOffensiveSpell = OffensiveSpell2Bound;
		EnterCastMode();

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

	// Reset mouse pos if timer is up
	if (bShouldResetMouse)
	{
		bShouldResetMouse = false;
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		GetWorld()->GetFirstPlayerController()->SetMouseLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
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
	OnRemoveHighlight.Broadcast();

	// Check for mana
	if (CurrentMana < Spell->ManaCost) { NotEnoughMana(); return; }

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedOffensiveSpell = OffensiveSpell3Bound;
		EnterCastMode();

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

	// Reset mouse pos if timer is up
	if (bShouldResetMouse)
	{
		bShouldResetMouse = false;
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		GetWorld()->GetFirstPlayerController()->SetMouseLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
	}
}

void AUmir::ActivateDefensiveSlot()
{
	// Check if on cooldown or spell is not bound
	if (DefensiveSpellBound == EDefensiveSpell::DS_None) return;
	if (!FMath::IsNearlyZero(GetDefensiveSlotCooldown())) { IsOnCooldown(); return; }
	if (!bCanUseSpell) return;

	FDefensiveSpell *Spell = GameInstance->GetDefensiveSpell(DefensiveSpellBound);

	// Restore action bar hightlight
	OnRemoveHighlight.Broadcast();

	if (Spell->bMustActivate)
	{
		// Activate spell
		ActivatedDefensiveSpell = DefensiveSpellBound;
		EnterCastMode();

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
	auto PC = GetWorld()->GetFirstPlayerController();

	// Reset mouse pos if timer is up
	if (bShouldResetMouse)
	{
		bShouldResetMouse = false;
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		GetWorld()->GetFirstPlayerController()->SetMouseLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
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
		OnRemoveHighlight.Broadcast();
		OnPotionCast.Broadcast(true);
		LastTimeActivatedPotion = GetWorld()->GetTimeSeconds();
		UsePotion(PotionBound);
	}
	else
	{
		NoMorePotions();
	}
}

void AUmir::MoveDecalToMouseHitLocation()
{
	if (!ensure(Decal)) return;
	if (ActivatedOffensiveSpell == EOffensiveSpell::OS_None) return;

	// Get spell
	auto Spell = GameInstance->GetOffensiveSpell(ActivatedOffensiveSpell);

	// Set scale and size
	SetDecalSize(Spell->DecalRadius);
	SetDecalScale();

	// Find screen location and direction
	FVector ScreenLocation;
	FVector ScreenDirection;
	FVector2D ViewportMaxSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportMaxSize);
	FVector2D MousePos;
	GetWorld()->GetFirstPlayerController()->GetMousePosition(MousePos.X, MousePos.Y);
	GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(ViewportMaxSize.X / 2.0f, MousePos.Y, ScreenLocation, ScreenDirection);

	GetWorld()->GetFirstPlayerController()->SetMouseLocation(ViewportMaxSize.X / 2.0f, MousePos.Y);

	// linetrace to find ground first attempt
	FVector EndLocation = ScreenLocation + (ScreenDirection.GetSafeNormal() * MaxTraceDistance);
	FHitResult HitResult;
	bool bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, ScreenLocation, EndLocation, ECC_Visibility);
	if (bFoundGround == false)
	{
		// linetrace to find ground second attempt
		FVector NewStartPos = ScreenLocation + (ScreenDirection * 10000.0f);
		FVector NewEndPos = FVector::UpVector * -MaxTraceDistance;
		bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, NewStartPos, NewEndPos, ECC_Visibility);
		if (bFoundGround == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find ground on second attempt"));
			return;
		}
	}

	// Take spell range into account
	float SpellRange = Spell->Range;
	FVector DirectionFromPlayerToHitLocation = HitResult.Location - GetActorLocation();
	if (DirectionFromPlayerToHitLocation.Size() > SpellRange)
	{
		DirectionFromPlayerToHitLocation.Z = 0.0f;
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

	// Find screen location and direction
	FVector MiddleScreenLocation;
	FVector MiddleScreenDirection;
	FVector2D ViewportMaxSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportMaxSize);
	GetWorld()->GetFirstPlayerController()->DeprojectScreenPositionToWorld(ViewportMaxSize.X / 2.0f, ViewportMaxSize.Y / 3.0f, MiddleScreenLocation, MiddleScreenDirection);

	// linetrace to find ground first attempt
	FVector EndLocation = MiddleScreenLocation + (MiddleScreenDirection.GetSafeNormal() * MaxTraceDistance);
	FHitResult HitResult;
	bool bFoundGround = GetWorld()->LineTraceSingleByChannel(HitResult, MiddleScreenLocation, EndLocation, ECC_Visibility);

	if (bFoundGround == false)
	{
		// linetrace to find ground second attempt
		FVector NewStartPos = MiddleScreenLocation + (MiddleScreenDirection * 10000.0f);
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

	// Set scale and size
	SetDecalSize();
	SetDecalScale(2.0f, 0.5f);
}

void AUmir::SetDecalSize(float Size /*= 256.0f*/)
{
	Decal->DecalSize.Y = Size;
	Decal->DecalSize.Z = Size;
}

void AUmir::SetDecalScale(float Y /*= 1.0f*/, float Z /*= 1.0f*/)
{
	Decal->RelativeScale3D.Y = Y;
	Decal->RelativeScale3D.Z = Z;
}

void AUmir::OnDeath()
{
	//GetWorld()->GetFirstPlayerController()->StartSpectatingOnly();
	bCanMoveCamera = false;
	bCanMove = false;
	bCanZoom = false;
	bCanBeDamaged = false;
	bIsDead = true;
	bCanUseSpell = false;

	PlayDeathCamera();
}

void AUmir::EnterCastMode()
{
	//auto PC = GetWorld()->GetFirstPlayerController();
	//PC->SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways));
	//PC->bShowMouseCursor = true;
	//bCanMoveCamera = false;
}

void AUmir::ResetMousePos()
{
	bShouldResetMouse = true;
}

bool AUmir::CancelActivatedSpell()
{
	// Remove action bar highlight
	OnRemoveHighlight.Broadcast();

	// Set mouse reset timer
	GetWorldTimerManager().ClearTimer(ResetMouseTimer);
	GetWorldTimerManager().SetTimer(ResetMouseTimer, this, &AUmir::ResetMousePos, 2.0f);

	// Set input mode game only
	auto PC = GetWorld()->GetFirstPlayerController();
	if (PC)	PC->SetInputMode(FInputModeGameOnly());

	// Cancel spell if one is activated
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
	if (!bCanUseSpell) { CancelActivatedSpell(); return; }

	// Get offensive spell
	auto *Spell = GameInstance->GetOffensiveSpell(Key);

	// Check for mana
	if (CurrentMana < Spell->ManaCost)
	{
		NotEnoughMana();
		CancelActivatedSpell();
		return;
	}

	// Check if casting is possible
	if (Spell->CastTime > 0.1f && GetMovementComponent()->IsFalling())
	{
		NeedToBeOnGround();
		return;
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
		SpawnTransform.SetRotation((-Decal->GetRightVector()).Rotation().Quaternion());
	}

	Super::UseOffensiveSpell(Key, SpawnTransform);

	// Start casting
	if (Spell->CastTime > 0.1f)
	{
		StartCasting(Spell->CastTime);
	}
	else
	{
		OnInstaCast.Broadcast();
	}

	// Disable mouse
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	bCanMoveCamera = true;

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

	// Cancel spell
	CancelActivatedSpell();
}

void AUmir::UseDefensiveSpell(EDefensiveSpell Key, FTransform SpawnTransform)
{
	if (Key == EDefensiveSpell::DS_None) return;
	if (!bCanUseSpell) { CancelActivatedSpell(); return; }

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

	// Disable mouse
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	bCanMoveCamera = true;

	// Set cooldown
	LastTimeActivatedDefensiveSpell = GetWorld()->GetTimeSeconds();

	// Cancel spell
	CancelActivatedSpell();
}

bool AUmir::UsePhysicalAttack(EPhysicalAttack Key)
{
	// Check for stamina
	auto *PhysicalAttack = GameInstance->GetPhysicalAttack(Key);
	if (CurrentStamina < PhysicalAttack->StaminaCost)
	{
		StaminaBarFail();
		return false;
	}

	auto bDidAttack = Super::UsePhysicalAttack(Key);

	// Handle stamina bar
	if (bIsStaminaFull && bDidAttack)
	{
		bIsStaminaFull = false;
		OnStaminaStatusChange.Broadcast(false);
	}
	return bDidAttack;
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

float AUmir::GetActiveElementDuration() const
{
	return TimeActiveElementExpires - GetWorld()->GetTimeSeconds();
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

void AUmir::StartCasting(float CastDuration, bool bCanInterrupt /*= true*/)
{
	Super::StartCasting(CastDuration, bCanInterrupt);

	CastingBarActivated();
}

bool AUmir::InterruptCasting()
{
	bool bWasCasting = Super::InterruptCasting();

	if (bWasCasting)
	{
		CastingBarInterrupted();
	}
	return bWasCasting;
}

void AUmir::CastSuccesfull()
{
	Super::CastSuccesfull();

	CastingBarSucceded();
}

void AUmir::Stun(float StunDuration)
{
	Super::Stun(StunDuration);

	CancelActivatedSpell();
}

float AUmir::GetCastingPercentage() const
{
	auto CurrentCastTime = GetWorld()->GetTimeSeconds() - TimeCastingBegan;
	auto TotalCastTime = TimeCastingEnds - TimeCastingBegan;

	return CurrentCastTime / TotalCastTime;
}

void AUmir::RegisterAggro()
{
	AggroCount++;
	if (AggroCount == 1)
	{
		EnteredCombat();
	}
}

void AUmir::DeRegisterAggro()
{
	AggroCount = FMath::Max(--AggroCount, 0);
	if (AggroCount == 0)
	{
		LeftCombat();
	}
}

bool AUmir::IsInCombat()
{
	if (AggroCount == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void AUmir::PassiveRegen(float DeltaTime)
{
	if (!IsInCombat())
	{
		Super::PassiveRegen(DeltaTime);
	}
}

void AUmir::RestoreStamina()
{
	Super::RestoreStamina();

	if (CurrentStamina == MaxStamina && bIsStaminaFull == false)
	{
		bIsStaminaFull = true;
		OnStaminaStatusChange.Broadcast(true);
	}
}
