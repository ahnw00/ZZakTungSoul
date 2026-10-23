// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LevelUpWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Warrior/WarriorPlayerState.h"
#include "Interface/CharacterStatInterface.h"
#include "Character/CharacterStatComponent.h"

// 캐싱 객체 접근 함수.
AWarriorPlayerState* ULevelUpWidget::GetWarriorPlayerState() const
{
	return GetOwningPlayerState<AWarriorPlayerState>();
}
UCharacterStatComponent* ULevelUpWidget::GetCharacterStatComponent() const
{
	if (ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(GetOwningPlayerPawn()))
	{
		return StatOwner->GetCharacterStatComponent();
	}
	return nullptr;
}


void ULevelUpWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 결정/취소 버튼 바인딩.

	// 모달 팔업 버튼 바인딩.
}

void ULevelUpWidget::NativeConstruct()
{
	// 창이 열릴 때마다 최신 세이브데이터로 동기화.

	// 모달 팝업은 기본적으로 숨긴다.
}

// 초기화: PlayerState 원본 복사 & 장바구니 리셋.
void ULevelUpWidget::InitializedLevelUp()
{
	// 원본 데이터 복사.

	// 가상 시뮬레이션 장바구니 0 리셋.

	// 화면 갱신.
}

// 가상 투자 사전 검사.
bool ULevelUpWidget::CanAddPoint() const
{
	// 다음 레벨 상승시 필요한 소울 조회.

	// 누적 소울 + 다음 비용이 보유 소울 이내 여부 검사.
	return false;
}
// 가상 투자 포인트 증가.
void ULevelUpWidget::AddPoint(FName StatName)
{
	// 소울 부족 시 차단.

	// 해당 스탯 가상 포인트 1 증가.

	// 가상 레벨 증가.

	// 누적 소울 필요 재계산.

	// 화면 즉시 갱신.
}

void ULevelUpWidget::SubPoint(FName StatName)
{
	// 해당 스탯에 가상 포인트 있는 지 검사.

	// 포인트 차감 및 가상 레벨 감소.

	// 누적 필요 소울 재계산.

	// 화면 즉시 갱신.
}

// 모달 팝업 열림.
void ULevelUpWidget::OpenConfirmModal()
{
}
// 모달 팝업 닫힘.
void ULevelUpWidget::CloseConfirmModal()
{
}
// 레벨업 확정.
void ULevelUpWidget::CommitLevelUp()
{
}
// 레벨업 취소.
void ULevelUpWidget::CancelLevelUp()
{
}

// 화면 텍스트 갱신.
void ULevelUpWidget::RefreshUI()
{
	// 텍스트 출력용 람다.

	// 레벨 & 소울 갱신.

	// 가상 속성 합성.

	// 주 스탯 화면 출력.

	// 파생 전투 스탯 실시간 시뮬레이션.
}
