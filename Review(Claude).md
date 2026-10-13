# Review(Claude)

코드 리뷰 기록 문서. 리뷰 내용만 정리하며, 코드는 직접 수정하지 않는다.
각 항목은 `[ ]`(미반영) / `[x]`(반영 완료)로 진행 상태를 표시한다.

---

## 1. CharacterStatComponent (2026-10-13)

**리뷰 대상**
- `Source/ZZakTungSoul/Character/CharacterStatComponent.h`
- `Source/ZZakTungSoul/Character/CharacterStatComponent.cpp`

**비교 대상(일관성 확인)**: `Warrior`, `WarriorPlayerState`, `WarriorSaveData`, `StatDefine`/`StatGrowth`, `ZTSGameMode`

**총평**
델리게이트 + 비율 Getter + `Clamp` 구조는 UI 연동 방향이 분명하고, `World` null 체크와 `MaxX > 0` 나눗셈 방어도 잘 되어 있다.
다만 **데이터 로딩이 아직 연결되지 않았고, 컴포넌트가 게임 어디에서도 쓰이지 않는 상태**이다.

### 1) 먼저 고쳐야 할 것

- [ ] **1-1. 컴포넌트가 생성되지 않는다.**
  `Warrior.h:67-68`에 포인터만 선언되어 있고 `Warrior.cpp` 생성자에 `CreateDefaultSubobject<UCharacterStatComponent>`가 없다.
  프로젝트 전체에서 `ApplyDamage`, `UseStamina` 등을 호출하는 곳도 없다. BP에서 따로 추가하지 않았다면 포인터는 항상 `nullptr`.
- [ ] **1-2. `LoadCharacterStat()`가 아무 일도 하지 않는다.** (`.cpp:36-43`)
  행을 지역 배열에 담고 버리며 어디서도 호출되지 않는다. 그래서 `MaxHP/MaxFP/MaxStamina`는 항상 100이다.
  `StatDefine.csv`의 초기값(HP 250, FP 50, Stamina 60)과 어긋난다.
- [ ] **1-3. `GetAllRows`로는 어느 행이 HP인지 알 수 없다.**
  행 이름(`HP`, `FP` 등)이 키인데 `GetAllRows`는 순서 있는 배열만 돌려준다.
  `FindRow<FStatDefine>(FName("HP"), ...)`처럼 이름으로 찾고, 이름 문자열은 상수/enum으로 한 곳에 모은다.
- [ ] **1-4. `UseFP`와 `UseStamina`의 비교가 다르다.**
  `UseFP`(`.cpp:93`)는 `CurrentFP <= Amount`, `UseStamina`(`.cpp:107`)는 `CurrentStamina < Amount`.
  FP가 필요량과 정확히 같으면 FP만 실패한다. 둘 다 `<`로 통일.
- [ ] **1-5. `Use*`에 음수 방어가 없다.**
  `ApplyDamage`에는 `InDamage <= 0` 가드가 있으나 `UseFP`/`UseStamina`에는 없다.
  음수를 넘기면 스탯이 회복되고, 스태미나는 회복 지연 시각도 갱신된다. 세 함수에 같은 가드 필요.
- [ ] **1-6. `.cpp:122-129` 주석 처리된 "어려운 버전"은 살리면 컴파일되지 않는다.**
  파라미터 이름이 `NewStamina`로 잘못되어 있고 `return`이 없다. 구현 시 `Clamp` 하한이 -50이 되므로
  `GetStaminaRatio`/UI 쪽 처리도 함께 확인해야 한다. 구현 전까지는 코드 대신 TODO 주석으로 의도만 남기는 편이 안전하다.

### 2) 다른 파일과의 일관성

- [ ] **이름 불일치**: 컴포넌트의 `EquipLoad`는 현재/최대 하중이 불분명하고, `StatDefine`의 `EquipLoad`는 최대 하중 성장 수치.
  `MaxEquipLoad` / `CurrentEquipLoad`로 구분. `Poise`(방어구에서 오는 값)도 같은 문제.
- [ ] **이동 속도 값이 세 곳에 따로 있다.**
  컴포넌트 `MaxWalkSpeed/OverloadedSpeed`(선언만, 미사용, Getter 없음) / `Warrior.cpp:289-291`과 `SprintStart/End`의 400/600 직접 대입 /
  `StatDefine.csv` 이동 속도 초기값 300. 출처를 한 곳으로 정하고, `Warrior::UpdateCharacterStats()`가 컴포넌트 값을 받아 적용하는 구조가 자연스럽다.
- [ ] **데이터 흐름이 끊겨 있다.**
  `WarriorSaveData`의 `Attributes`(Vigor~Dexterity), `CurrentHP`, `CurrentFP`와 `StatGrowth` 테이블이 컴포넌트에 연결되지 않았다.
  `SaveData`를 받아 최대치를 계산하고 현재값을 복원하는 `Initialize`류 함수가 필요하다.
  (저장 데이터는 PlayerState, 컴포넌트는 Pawn에 있으므로 Pawn 재생성 후에도 이어지게 하려면 이 경로가 필수.)
  `CurrentStamina`는 저장 항목에 없으므로 로드 시 최대치로 시작하는 것이 의도인지 확정할 것.
- [ ] **타입 정책 부재**: `StatDefine`/`StatGrowth`/`FWarriorAttributeData`는 `int32`, 컴포넌트는 `float`.
  "기본 능력치는 int, 파생 수치는 float"처럼 규칙을 정해 변환 지점을 분명히 한다.
- [ ] **include 스타일 혼재**: 이 파일의 `#include <Data/StatDefine.h>`는 각괄호, 나머지는 따옴표.
  `Warrior.cpp`는 따옴표, `ZTSGameMode.cpp`/`WarriorPlayerState.h`는 각괄호가 섞여 있다. 프로젝트 헤더는 따옴표로 통일 권장.
- [ ] **표기 혼재**: 코드 주석 "스테미너/스텟" vs `StatDefine.csv` "스태미나/스탯". 통일.
  카테고리명도 `"Stat"`, `"CurrentStat"`, `"Stat|Stamina"`, `"Stat|Events"`가 섞여 있어 `"Stat|Current"`처럼 계층으로 맞춘다.
- [ ] **에셋 경로 하드코딩**: 생성자의 `FObjectFinder`는 `Warrior`/`GameMode`와 같은 방식이지만,
  이 컴포넌트는 적/보스에도 쓸 계획이므로 `UPROPERTY(EditDefaultsOnly)` 또는 `TSoftObjectPtr`로 BP에서 지정하게 하는 편이 낫다.
  현재 테이블 멤버는 `private`이고 `EditAnywhere`도 아니라 BP에서 바꿀 수 없다.

### 3) 가독성

- [ ] 헤더의 `public`/`protected`/`private`이 여러 번 번갈아 나온다(템플릿 구조 잔재). 공개 API → protected → private 데이터 순으로 한 번씩 모은다.
- [ ] `TObjectPtr<UDataTable>`을 쓰는데 `class UDataTable;` 전방 선언이 없다. 지금은 컴파일되지만 다른 include에 의존하므로 명시.
- [ ] Getter 스타일이 둘로 나뉜다: 현재/최대값은 `FORCEINLINE`(C++ 전용), 비율은 `UFUNCTION(BlueprintPure)`.
  멤버가 `private VisibleAnywhere`라 BP는 비율만 읽을 수 있다. UI에서 "현재/최대" 숫자를 표시하려면 `BlueprintPure` Getter 필요.
- [ ] `SetCurrentHP`의 메모형 주석(`.cpp:58-60`)을 설명 문장으로 정리. 엔진 템플릿 주석("Sets default values…")도 정리 대상.
- [ ] `Use*` 함수 주석이 `UseStamina`에만 있다. 공개 API에는 일관되게 달 것.

### 4) 확장성

- [ ] **HP/FP/Stamina 코드 3벌 반복**: Current, Max, Delegate, `Set`, `Use`, `Ratio`가 각각 세 벌. 스탯 하나 추가에 6곳 이상 수정.
  - 가볍게: `Clamp` + 대입 + `Broadcast`를 하는 private 헬퍼 하나로 `SetCurrent*` 중복 제거.
  - 데이터 기반: `EResourceType`(또는 `StatDefine` 행 이름과 같은 `FName`) 키와 `{Current, Max}` 구조체의 맵으로 통합.
    델리게이트는 타입 인자를 받는 하나로. `StatDefine`/`StatGrowth`가 이미 `FName` 키 기반이라 방향이 맞다.
  - 적/보스처럼 FP/Stamina가 없는 개체도 현재는 불필요한 멤버를 갖는다.
- [ ] **`Set*`이 모두 `BlueprintCallable`**: 어떤 BP에서든 HP를 직접 쓸 수 있다.
  `ApplyDamage`/`Heal`/`Use*`만 공개하고 `Set*`은 `protected`/`private`으로 내린다. `Heal`은 현재 없음.
- [ ] **사망/변화 감지 부재**: HP 0이 되어도 알리는 신호가 없다(`Warrior`의 `EActionState::Hit`와도 연결 없음).
  `OnDeath` 델리게이트 또는 `ApplyDamage` 반환값 고려. 값이 바뀌지 않았는데 방송하는 경우(HP 0에서 재피격 등)는 이전 값과 비교 후 방송.
- [ ] **`ApplyDamage` 이름**: 엔진의 `UGameplayStatics::ApplyDamage`(주는 쪽 호출 → `AActor::TakeDamage`)와 혼동된다.
  이 컴포넌트는 `TakeDamage`를 받은 `Warrior`가 넘겨주는 쪽이므로 `ReceiveDamage`/`TakeDamage` 계열이 더 맞다.
- [ ] **Tick 상시 활성**: 스태미나가 가득 차 있어도 매 프레임 호출 후 즉시 return. 적이 많아지면 비용이 쌓인다.
  소모 시에만 Tick을 켜고 가득 차면 끄거나 타이머로 대체.

### 5) 권장 작업 순서

1-1(생성) → 1-2/1-3(최대치 로딩 연결) → 1-4~1-6(나머지 버그) → 확장성 개선.
확장성 항목은 로딩이 연결된 뒤에 구조를 정해야 헛수고가 줄어든다.
