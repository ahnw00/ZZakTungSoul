#include "UI/BossHUDWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Enemy/Boss/Boss.h"
#include "Enemy/Components/EnemyStatComponent.h"

void UBossHUDWidget::SetBossName(FText InBossName)
{
	if (Text_BossName)
	{
		Text_BossName->SetText(InBossName);
	}
}

void UBossHUDWidget::UpdateHealth(float CurrentHp, float MaxHp)
{
	const float Ratio = MaxHp > 0.f
		? FMath::Clamp(CurrentHp / MaxHp, 0.f, 1.f)
		: 0.f;

	const bool bMaxHpChanged =
		!FMath::IsNearlyEqual(MaxHp, PreviousMaxHp);

	const bool bHealed = CurrentHp > PreviousHp;

	// 최초 연결, 연결 해제, 회복, 최대 HP 변경은 즉시 맞추기
	if (!bHasInitialHealth || MaxHp <= 0.f ||
		bMaxHpChanged || bHealed)
	{
		DelayedHpPercent = Ratio;
		HpDelayRemaining = 0.f;
		ResetDamageDisplay();
	}
	else if (CurrentHp < PreviousHp)
	{
		// 피격: 노란 바는 유지하고 대기 시간 갱신
		HpDelayRemaining = HpDelayDuration;
		AddDamageDisplay(PreviousHp - CurrentHp);
	}

	TargetHpPercent = Ratio;

	PreviousHp = CurrentHp;
	PreviousMaxHp = MaxHp;
	bHasInitialHealth = MaxHp > 0.f;

	// 빨간 바는 실제 HP로 즉시 변경
	if (ProgressBar_HP)
	{
		ProgressBar_HP->SetPercent(TargetHpPercent);
	}

	if (ProgressBar_Delay)
	{
		ProgressBar_Delay->SetPercent(DelayedHpPercent);
	}
}

void UBossHUDWidget::BindBoss(ABoss* InBoss)
{
	// 이전 보스의 HP 변경 알림 연결 해제
	if (UEnemyStatComponent* PreviousStat = BoundStatComponent.Get())
	{
		PreviousStat->OnEnemyHpChanged.RemoveAll(this);
	}

	BoundStatComponent.Reset();

	// 이전 표시 초기화
	SetBossName(FText::GetEmpty());
	UpdateHealth(0.f, 0.f);

	if (Text_Damage)
	{
		Text_Damage->SetText(FText::GetEmpty());
		Text_Damage->SetVisibility(ESlateVisibility::Hidden);
	}

	if (!IsValid(InBoss))
	{
		return;
	}

	UEnemyStatComponent* Stat = InBoss->GetEnemyStatComponent();

	if (!IsValid(Stat))
	{
		return;
	}

	BoundStatComponent = Stat;

	// 실제 보스에 설정된 이름 읽기
	SetBossName(InBoss->GetBossDisplayName());

	// 피격 등으로 HP가 변경될 때 자동 갱신
	Stat->OnEnemyHpChanged.AddUObject(
		this,
		&UBossHUDWidget::UpdateHealth
	);

	// 연결 시점의 실제 HP로 최초 표시
	UpdateHealth(Stat->GetCurrentHp(), Stat->GetMaxHp());
}

void UBossHUDWidget::NativeDestruct()
{
	if (UEnemyStatComponent* Stat = BoundStatComponent.Get())
	{
		Stat->OnEnemyHpChanged.RemoveAll(this);
	}

	BoundStatComponent.Reset();
	ResetDamageDisplay();
	Super::NativeDestruct();
}

void UBossHUDWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 데미지 숫자 표시 시간 갱신
	if (DamageDisplayRemaining > 0.f)
	{
		DamageDisplayRemaining = FMath::Max(
			DamageDisplayRemaining - InDeltaTime,
			0.f
		);

		if (DamageDisplayRemaining <= 0.f)
		{
			ResetDamageDisplay();
		}
	}

	if (DelayedHpPercent <= TargetHpPercent)
	{
		return;
	}

	float AnimationDelta = InDeltaTime;

	// 피격 후 잠시 대기
	if (HpDelayRemaining > 0.f)
	{
		const float WaitDelta =
			FMath::Min(HpDelayRemaining, AnimationDelta);

		HpDelayRemaining -= WaitDelta;
		AnimationDelta -= WaitDelta;
	}

	// 대기가 끝나면 빨간 바까지 감소
	if (AnimationDelta > 0.f)
	{
		DelayedHpPercent = FMath::FInterpConstantTo(
			DelayedHpPercent,
			TargetHpPercent,
			AnimationDelta,
			FMath::Max(HpDecreaseSpeed, 0.01f)
		);

		if (ProgressBar_Delay)
		{
			ProgressBar_Delay->SetPercent(DelayedHpPercent);
		}
	}
}

void UBossHUDWidget::AddDamageDisplay(float Damage)
{
	if (Damage <= 0.f)
	{
		return;
	}

	// 표시 시간이 끝났다면 새로운 누적 시작
	if (DamageDisplayRemaining <= 0.f)
	{
		AccumulatedDamage = 0.f;
	}

	AccumulatedDamage += Damage;
	DamageDisplayRemaining = FMath::Max(
		DamageDisplayDuration,
		0.1f
	);

	if (Text_Damage)
	{
		Text_Damage->SetText(
			FText::AsNumber(FMath::RoundToInt(AccumulatedDamage))
		);

		Text_Damage->SetVisibility(
			ESlateVisibility::HitTestInvisible
		);
	}
}

void UBossHUDWidget::ResetDamageDisplay()
{
	AccumulatedDamage = 0.f;
	DamageDisplayRemaining = 0.f;

	if (Text_Damage)
	{
		Text_Damage->SetText(FText::GetEmpty());
		Text_Damage->SetVisibility(ESlateVisibility::Hidden);
	}
}
