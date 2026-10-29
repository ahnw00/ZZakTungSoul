// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Boss/EnemyBoss.h"
#include <Enemy/EnemyAIController.h>
#include <Enemy/Data/EnemyAIData.h>
#include <Enemy/Components/AttackTraceComponent.h>
#include <Enemy/Components/EnemyStatComponent.h>
#include <Character/CharacterStatComponent.h>
#include <Interface/CombatDamageInterface.h>
#include <Data/DamageInfo.h>

#include <GameFramework/CharacterMovementComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/StaticMesh.h>

AEnemyBoss::AEnemyBoss()
{
	// Mesh setting
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletonMeshRef(
		TEXT("/Game/Assets/Clown_monster/Meshes/base_mesh/SKM_base_mesh.SKM_base_mesh")
	);

	if (SkeletonMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletonMeshRef.Object);

		//GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		static ConstructorHelpers::FClassFinder<UAnimInstance> AnimClassRef(
			TEXT("")
		);

		if (AnimClassRef.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(AnimClassRef.Class);
		}
		else
		{

		}
	}
}

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();


}

void AEnemyBoss::ReceiveDamage(const FDamageInfo& DamageInfo, const FHitResult& Hit)
{

}

void AEnemyBoss::UpdateMovementSpeed()
{

}

void AEnemyBoss::UpdateRotationMode()
{}

void AEnemyBoss::HandleAttackTraceHit(const FHitResult & Hit)
{}

void AEnemyBoss::InitializeWeapon()
{

}
