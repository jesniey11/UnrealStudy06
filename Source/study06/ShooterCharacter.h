// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

//Input Class
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

class AGun;

UCLASS()
class STUDY06_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	void Shoot();

private:
	UPROPERTY(EditAnywhere)
	float RotationRate = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditAnywhere)
	TArray<AGun*> Weapon;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AGun>> WeaponList;

	UPROPERTY(EditAnywhere)
	int32 ActiveWeaponIdx = 0;

	/*
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	AGun* Gun;
	*/

	//Input
	UPROPERTY(EditDefaultsOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* ShootAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* ReloadAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* SwitchWeaponAction1;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* SwitchWeaponAction2;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* SwitchWeaponAction3;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* SwitchWeaponAction4;

	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* InputMappingContext;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	void Reload();

	void ChangeWeapon(int32 Index);
	void ChangeWeaponIdx1();
	void ChangeWeaponIdx2();
	void ChangeWeaponIdx3();
	void ChangeWeaponIdx4();

	
};
