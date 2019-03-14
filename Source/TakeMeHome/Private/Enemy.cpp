// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "TakeMeHomeGameInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;


}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Subscribe to death event
	OnDeathDelegate.AddUniqueDynamic(this, &ABaseCharacter::OnDeath);

	// Set material
	SetMaterialAccordingToElement();
}

float AEnemy::TakeDamage(float Damage, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	// Check if attacked by player

	if (EventInstigator)
	{
		if (EventInstigator->IsA<APlayerController>())
		{
			bWasAttacked = true;
		}
	}

	// Calculate damage
	float FinalDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	float Size;
	FLinearColor Color;
	if (FinalDamage > Damage)
	{
		Size = 2.0f;
		Color = GameInstance->SuperEffectiveColor;
	}
	else if (FinalDamage < Damage)
	{
		Size = 0.5f;
		Color = GameInstance->NotVeryEffectiveColor;
	}
	else
	{
		Size = 1.0f;
		Color = GameInstance->NormalColor;
	}
	SpawnCombatText(FinalDamage, Size, Color);

	return FinalDamage;
}

void AEnemy::OnDeath()
{
	DetachFromControllerPendingDestroy();
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetCapsuleComponent()->SetCollisionProfileName(FName("NoCollision"));
	DeathEvent();
}

void AEnemy::SetMaterialAccordingToElement()
{
	FVector ElementColor;
	switch (ElementType)
	{
	case EElement::E_Fire:
		ElementColor = UKismetMathLibrary::Conv_LinearColorToVector(GameInstance->FireColor	);
		GetMesh()->SetVectorParameterValueOnMaterials(FName("BodyColor"), ElementColor);
		break;
	case EElement::E_Water:
		ElementColor = UKismetMathLibrary::Conv_LinearColorToVector(GameInstance->WaterColor);
		GetMesh()->SetVectorParameterValueOnMaterials(FName("BodyColor"), ElementColor);
		break;
	case EElement::E_Nature:
		ElementColor = UKismetMathLibrary::Conv_LinearColorToVector(GameInstance->NatureColor);
		GetMesh()->SetVectorParameterValueOnMaterials(FName("BodyColor"), ElementColor);
		break;
	}
}
