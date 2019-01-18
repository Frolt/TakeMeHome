// Fill out your copyright notice in the Description page of Project Settings.

#include "TakeMeHomeGameModeBase.h"
#include "UObject/ConstructorHelpers.h"


ATakeMeHomeGameModeBase::ATakeMeHomeGameModeBase()
{
	// Sets default pawn
	static ConstructorHelpers::FClassFinder<APawn> UmirBPClass(TEXT("/Game/Blueprints/Umir_BP"));
	if (UmirBPClass.Succeeded()) {
		DefaultPawnClass = UmirBPClass.Class;
	}
	// Sets default player controller
	//static ConstructorHelpers::FClassFinder<APawn> UmirBPClass(TEXT("/Game/Blueprints/Umir_BP"));
	//if (UmirBPClass.Succeeded()) {
	//	DefaultPawnClass = UmirBPClass.Class;
	//}
}
