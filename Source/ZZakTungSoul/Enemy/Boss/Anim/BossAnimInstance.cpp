#include <Enemy/Boss/Anim/BossAnimInstance.h>

UBossAnimInstance::UBossAnimInstance()
{
}

void UBossAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//---------------------------------------------------------------
	// 애니메이션을 사용하는 보스 확인.
	//
	//---------------------------------------------------------------
	Owner = Cast<ABoss>(TryGetPawnOwner());

	Velocity = FVector::ZeroVector;
	Speed = 0.0f;
	Direction = 0.0f;
	BossPhase = EBossPhase::Intro;
	EnemyState = EEnemyState::Idle;
}

void UBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//---------------------------------------------------------------
	// 초기화 시 소유자를 얻지 못했다면 다시 확인.
	//
	//---------------------------------------------------------------
	if (!IsValid(Owner.Get()))
	{
		Owner = Cast<ABoss>(TryGetPawnOwner());
	}

	//---------------------------------------------------------------
	// 프리뷰 등 보스 소유자가 없는 경우 기본값 유지.
	//
	//---------------------------------------------------------------
	if (!IsValid(Owner.Get()))
	{
		Velocity = FVector::ZeroVector;
		Speed = 0.0f;
		Direction = 0.0f;
		BossPhase = EBossPhase::Intro;
		EnemyState = EEnemyState::Idle;

		return;
	}

	Velocity = Owner->GetVelocity();

	//---------------------------------------------------------------
	// 정지 상태에서도 속도와 방향을 갱신하여 이전 값이 남지 않도록 처리.
	//
	//---------------------------------------------------------------
	Speed = FMath::FInterpTo(Speed, Velocity.Size2D(), DeltaSeconds, 10.0f);
	float DesiredDirection = Direction;

	if (!Velocity.IsNearlyZero())
	{
		FRotator MoveRot = Velocity.Rotation();
		FRotator PawnRot = Owner->GetActorRotation();

		DesiredDirection = FMath::FindDeltaAngleDegrees(PawnRot.Yaw, MoveRot.Yaw);
	}

	Direction = FMath::FInterpTo( Direction, Direction + FMath::FindDeltaAngleDegrees(Direction, DesiredDirection), DeltaSeconds, 10.0f);
	Direction = FRotator::NormalizeAxis(Direction);

	//---------------------------------------------------------------
	// 페이즈 변경은 보스가 담당하고 애니메이션에서는 조회만 수행.
	//
	//---------------------------------------------------------------
	BossPhase = Owner->GetBossPhase();
	EnemyState = Owner->GetEnemyState();
}
