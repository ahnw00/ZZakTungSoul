#pragma once

#include <CoreMinimal.h>
#include <BehaviorTree/BTTaskNode.h>
#include "BTTask_ClownEvade.generated.h"

class ABossClown;
class UAnimMontage;

UENUM(BlueprintType)
enum class EClownEvadeAction : uint8
{
	Backstep, Backjump, SideEvade, Feint UMETA(Hidden), Orbit
};

//---------------------------------------------------------------
// BT가 선택한 회피 / 주변 이동의 물리 이동과 몽타주만 실행.
//
//---------------------------------------------------------------
UCLASS()
class ZZAKTUNGSOUL_API UBTTask_ClownEvade : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ClownEvade();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UPROPERTY(EditAnywhere, Category = "Clown|Evade")
	EClownEvadeAction Action = EClownEvadeAction::Backstep;

	UPROPERTY(EditAnywhere, Category = "Clown|Target")
	FName TargetActorKey = TEXT("TargetActor");

private:
	bool BeginTactical(UBehaviorTreeComponent& OwnerComp, ABossClown* Clown, AActor* Target);
	bool FindSafeGoal(UBehaviorTreeComponent& OwnerComp, ABossClown* Clown, AActor* Target, const FVector& Desired, FVector& OutGoal) const;
	bool BuildJumpVelocity(ABossClown* Clown, AActor* Target, FVector& OutVelocity);
	void FinishTactical(bool bAbort);

	TWeakObjectPtr<AActor> TacticalTarget;
	TWeakObjectPtr<UAnimMontage> TacticalMontage;
	FVector MoveGoal = FVector::ZeroVector;
	FVector TacticalStart = FVector::ZeroVector;
	float Elapsed = 0.0f;
	float Duration = 1.0f;
	float MoveDuration = 0.55f;
	float SavedSpeed = 0.0f;
	bool bSavedOrient = false;
	bool bSavedDesired = false;
	bool bSavedYaw = false;
	bool bTacticalActive = false;
	float OrbitSign = 1.0f;

	TWeakObjectPtr<ABossClown> AttackingClown;
};
