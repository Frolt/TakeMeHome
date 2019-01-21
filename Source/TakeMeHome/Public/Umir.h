// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Umir.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AStarfall;
class AForcePush;
class ATornado;

UCLASS()
class TAKEMEHOME_API AUmir : public ACharacter
{
	GENERATED_BODY()

public:
	AUmir();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float NormalizedRate);
	void MoveRight(float NormalizedRate);
	void LookRight(float NormalizedRate);
	void LookUp(float NormalizedRate);
	void Zoom(float NormalizedRate);
	void LeftMouseButtonPressed();
	void LeftMouseButtonReleased();
	void RightMouseButtonPressed();
	void RightMouseButtonReleased();
	void CastSpell1();
	void CastSpell2();
	void CastSpell3();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *CameraBoom = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FollowCamera = nullptr;
	
private:
	float MaxZoom = 1000.0f;
	float MinZoom = 300.0f;
	float ZoomStrength = 50.0f;

	bool bIsLeftMouseButtonPressed = false;
	bool bIsRightMouseButtonPressed = false;
	FVector2D PrevMousePos;

public:
	UPROPERTY(EditAnywhere, Category = "Setup")
	TSubclassOf<ATornado> TornadoBP;
	TSubclassOf<AStarfall> StarfallBP;
	TSubclassOf<AForcePush> ForcePushBP;
};
