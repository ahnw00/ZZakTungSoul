#include <Enemy/Boss/Clown/Anim/AnimNotify_SetClownAttackDamageInfo.h>
#include <Enemy/Boss/Clown/BossClown.h>
#include <Components/SkeletalMeshComponent.h>

void UAnimNotify_SetClownAttackDamageInfo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	ABossClown* Clown = Cast<ABossClown>(MeshComp->GetOwner());
	if (Clown && MeshComp == Clown->GetMesh())
	{
		Clown->SetClownAttackDamage(DamageIndex, Animation);
	}
}
