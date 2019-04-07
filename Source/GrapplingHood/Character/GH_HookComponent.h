// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GH_Hook.h"
#include "GH_HookComponent.generated.h"


UCLASS(hidecategories = Object, config = Engine, editinlinenew, abstract)
class GRAPPLINGHOOD_API UGH_HookComponent : public USceneComponent
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, Category="Collision")
	class USphereComponent* SphereCollider;

	/** Hook tip mesh component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Projectile, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
public:	
	// Sets default values for this component's properties
	UGH_HookComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
