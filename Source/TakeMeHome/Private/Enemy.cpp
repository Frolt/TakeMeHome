// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "TakeMeHomeGameInstance.h"
#include "Components/SkeletalMeshComponent.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// Subscribe to death event
	OnDeathDelegate.AddUniqueDynamic(this, &ABaseCharacter::OnNPCDeath);

	// Set material
	SetMaterialAccordingToElement();
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
