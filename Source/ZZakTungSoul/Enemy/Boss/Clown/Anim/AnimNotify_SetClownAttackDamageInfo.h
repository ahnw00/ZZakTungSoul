#pragma once

#include <CoreMinimal.h>
#include <Animation/AnimNotifies/AnimNotify.h>
#include "AnimNotify_SetClownAttackDamageInfo.generated.h"

UCLASS()
class ZZAKTUNGSOUL_API UAnimNotify_SetClownAttackDamageInfo : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	//---------------------------------------------------------------
	// 현재 광대 공격의 DamageInfo 배열 인덱스.
	// 몽타주 자체의 Notify 트랙에 배치.
	//
	//---------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Clown|Damage", meta = (ClampMin = "0"))
	int32 DamageIndex = 0;
};
