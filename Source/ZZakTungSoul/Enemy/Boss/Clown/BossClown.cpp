// Fill out your copyright notice in the Description page of Project Settings.


#include <Enemy/Boss/Clown/BossClown.h>
#include <Animation/AnimInstance.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <UObject/ConstructorHelpers.h>
#include <Enemy/Boss/Clown/Anim/BossClownAnimationInstance.h>

ABossClown::ABossClown()
{
	//---------------------------------------------------------------
	// 광대 SkeletalMesh 설정.
	//
	//---------------------------------------------------------------
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletonMeshRef(
		TEXT("/Game/Assets/Clown_monster/Meshes/base_mesh/SKM_base_mesh.SKM_base_mesh")
	);

	if (SkeletonMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SkeletonMeshRef.Object);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

		static ConstructorHelpers::FClassFinder<UBossClownAnimationInstance> AnimClassRef(
			TEXT("/Game/Blueprints/Enemy/Boss/Clown/Animation/ABP_ClownBoss.ABP_ClownBoss_C")
		);

		if (AnimClassRef.Succeeded())
		{
			GetMesh()->SetAnimInstanceClass(AnimClassRef.Class);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BossClown: failed to load animation blueprint class."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BossClown: failed to load skeleton mesh."));
	}


}