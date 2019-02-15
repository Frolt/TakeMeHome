// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilityBase.generated.h"

class ABaseCharacter;

UCLASS()
class TAKEMEHOME_API AAbilityBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AAbilityBase();

public:
	UPROPERTY(BlueprintReadWrite, Category = "Setup", meta = (ExposeOnSpawn = true))
	ABaseCharacter *AbilityOwner = nullptr;
};
