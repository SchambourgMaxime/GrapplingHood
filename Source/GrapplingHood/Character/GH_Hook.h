// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GH_Hook.generated.h"

UCLASS()
class GRAPPLINGHOOD_API AGH_Hook : public AActor
{
	GENERATED_BODY()
	
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* SphereCollider;

	/** Hook tip mesh component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;

	/** Hook movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	/** Hook impulse intensity when fired */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float FireSpeed = 10000.f;

	/** Hook retracting speed (in m/s) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	float RetractSpeed = 10000.f;

public:

	enum State {
		DOCKED = 0,
		FIRING,
		HOOKED,
		RETRACTING,
		HOOKSTATE_NUM
	};

	// Sets default values for this actor's properties
	AGH_Hook();

	// Sets default values for this actor's properties
    ~AGH_Hook();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void StopAllMovement();

	UFUNCTION()
	void Fire(FVector direction = FVector::ZeroVector);

	UFUNCTION()
	void Retract(FVector destination, float deltaTime);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return SphereCollider; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	/** Returns State of the Hook **/
	FORCEINLINE State GetState() const { return HookState; }

private :
	State HookState;

};
