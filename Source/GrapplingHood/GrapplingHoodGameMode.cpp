// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "GrapplingHoodGameMode.h"
#include "GrapplingHoodHUD.h"
#include "GrapplingHoodCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGrapplingHoodGameMode::AGrapplingHoodGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Logic/Character/GH_Character_BP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGrapplingHoodHUD::StaticClass();
}
