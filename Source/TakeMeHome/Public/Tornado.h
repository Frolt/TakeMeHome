// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBase.h"
#include "Tornado.generated.h"

/**
 * 
 */
UCLASS()
class TAKEMEHOME_API ATornado : public ASpellBase
{
	GENERATED_BODY()
	
public:
	ATornado();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void MoveAlongGround(float DeltaTime);

private:
	UPROPERTY(EditAnywhere, Category = "Settings")
	float TraceHeight = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Settings")
	float Speed = 1000.0f;
};
