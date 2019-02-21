// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "TakeMeHomeGameInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"


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
	float DamageMultiplier = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	float Size;
	FLinearColor Color;
	if (DamageMultiplier > 1.1f)
	{
		Size = 2.0f;
		Color = GameInstance->SuperEffectiveColor;
	}
	else if (DamageMultiplier < 0.9f)
	{
		Size = 0.5f;
		Color = GameInstance->NotVeryEffectiveColor;
	}
	else
	{
		Size = 1.0f;
		Color = GameInstance->NormalColor;
	}
	SpawnCombatText(Damage * DamageMultiplier, Size, Color);

	return Damage;
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
	case EElement::E_Earth:
		ElementColor = UKismetMathLibrary::Conv_LinearColorToVector(GameInstance->EarthColor);
		GetMesh()->SetVectorParameterValueOnMaterials(FName("BodyColor"), ElementColor);
		break;
	case EElement::E_Fire:
		ElementColor = UKismetMathLibrary::Conv_LinearColorToVector(GameInstance->FireColor	);
		GetMesh()->SetVectorParameterValueOnMaterials(FName("BodyColor"), ElementColor);
		break;
	case EElement::E_Lightning:
		ElementColor = UKismetMathLibrary::Conv_LinearColorToVector(GameInstance->LightningColor);
		GetMesh()->SetVectorParameterValueOnMaterials(FName("BodyColor"), ElementColor);
		break;
	case EElement::E_Nature:
		ElementColor = UKismetMathLibrary::Conv_LinearColorToVector(GameInstance->NatureColor);
		GetMesh()->SetVectorParameterValueOnMaterials(FName("BodyColor"), ElementColor);
		break;
	case EElement::E_Water:
		ElementColor = UKismetMathLibrary::Conv_LinearColorToVector(GameInstance->WaterColor);
		GetMesh()->SetVectorParameterValueOnMaterials(FName("BodyColor"), ElementColor);
		break;
	}
}
