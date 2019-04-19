// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GH_Hook.h"
#include "Engine/SkeletalMeshSocket.h"

#include "GH_Character.generated.h"

class AStaticMeshActor;
class APhysicsConstraintActor;

UCLASS(config = Game)
class GRAPPLINGHOOD_API AGH_Character : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* BodyMesh;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* GunMesh;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

public:
	// Sets default values for this character's properties
	AGH_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Hook)
	TSubclassOf<class AGH_Hook> HookClass;

	/** Hook projectile reference */
	AGH_Hook* Hook;

	AStaticMeshActor* Rope;

	///** Projectile class to spawn */
	//UPROPERTY(EditDefaultsOnly, Category = Hook)
	//TSubclassOf<class AGH_HookRope> RopeClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

protected:

	AGH_Hook* HookInstance = nullptr;

	UStaticMesh* RopeMesh = nullptr;
	TArray<APhysicsConstraintActor*> PhysicsConstraints;

	bool RopeLocked = false;

	float SwingRopeLength;
	float SwingAngle;
	float SwingAngleVelocity;
	FVector SwingPlaneNormal;
	float SwingZRotation;
	FVector SwingLastDelta;

	/** Fires a projectile. */
	void OnFire();

	/** Fires a projectile. */
	void UpdateRope();

	/** Fires a projectile. */
	void LockRope();

	void SwingCharacter(float DeltaSeconds);

	/** Fires a projectile. */
	void UnlockRope();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetBodyMesh() const { return BodyMesh; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
	/** Returns WorldLocation at the tip of the gun subobject */
	FORCEINLINE FVector GetMuzzleWorldLocation() const { return GunMesh->GetSocketByName("Muzzle")->GetSocketLocation(GunMesh); }
	/** Returns WorldLocation at the tip of the gun subobject */
	FORCEINLINE FVector GetMuzzleLocalLocation() const { return GunMesh->GetSocketByName("Muzzle")->RelativeLocation; }
};
