// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GrapplingHoodHUD.generated.h"

UCLASS()
class AGrapplingHoodHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGrapplingHoodHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

