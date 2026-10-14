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

- [x] **1-1. 컴포넌트가 생성되지 않는다.** (2026-10-14 확인: `Warrior.cpp` 생성자에서 생성, `Dodge`와 HUD 연동까지 반영됨)
  `Warrior.h:67-68`에 포인터만 선언되어 있고 `Warrior.cpp` 생성자에 `CreateDefaultSubobject<UCharacterStatComponent>`가 없다.
  프로젝트 전체에서 `ApplyDamage`, `UseStamina` 등을 호출하는 곳도 없다. BP에서 따로 추가하지 않았다면 포인터는 항상 `nullptr`.
- [ ] **1-2. `LoadCharacterStat()`가 아무 일도 하지 않는다.** (`.cpp:36-43`)
  행을 지역 배열에 담고 버리며 어디서도 호출되지 않는다. 그래서 `MaxHP/MaxFP/MaxStamina`는 항상 100이다.
  `StatDefine.csv`의 초기값(HP 250, FP 50, Stamina 60)과 어긋난다.
- [ ] **1-3. `GetAllRows`로는 어느 행이 HP인지 알 수 없다.**
  행 이름(`HP`, `FP` 등)이 키인데 `GetAllRows`는 순서 있는 배열만 돌려준다.
  `FindRow<FStatDefine>(FName("HP"), ...)`처럼 이름으로 찾고, 이름 문자열은 상수/enum으로 한 곳에 모은다.
- [x] **1-4. `UseFP`와 `UseStamina`의 비교가 다르다.** (2026-10-14 코드 확인: 둘 다 `< Amount`로 통일됨. 2-1 참고.)
  `UseFP`는 `<=`, `UseStamina`는 `<`로 달랐던 문제. FP가 필요량과 정확히 같으면 FP만 실패했다.
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

---

## 2. HUD 연동 + PROJECT_GUIDE.md 리뷰 (2026-10-14)

**리뷰 대상 (커밋 `bb9e0ac` 이후 변경분)**
- `CharacterStatComponent.h/.cpp` (수정), `Warrior.h/.cpp` (컴포넌트 생성, `Dodge` 스태미나 소모)
- `UI/PlayerHUDWidget.h/.cpp` (신규), `WarriorController.h/.cpp` (HUD 생성), `ZZakTungSoul.Build.cs`
- `PROJECT_GUIDE.md`, `리뷰 참고용.txt`

**총평**
컴포넌트 생성 → `Dodge` 스태미나 소모 → HUD 델리게이트 바인딩까지 한 줄로 이어졌고, `BindWidget`과 Build.cs 의존성(UMG, Slate)도 올바르다.
`FOnStatChangedDelegate`로 이름도 통일되었다. 남은 문제는 **스태미나 경계값 버그**, **HUD 바인딩이 Pawn 교체에 취약한 점**, **섹션 1의 미반영 항목**이다.

### 1) 버그 / 동작 문제

- [x] **2-1. 스태미나 경계값 버그 (1-4의 방향 오류)** (2026-10-14 확인: `UseFP`와 `UseStamina` 모두 `< Amount`로 수정 완료)
  `UseStamina`와 `UseFP`가 모두 `<= Amount`였으나 `< Amount`로 변경되어 100 기준 4회 구르기가 정상 발동됨.
  (`RegenerateStamina`는 `>=`로 되어 있어 문제 없음. 섹션 3-2에서 이전 경고를 삭제함.)
- [x] **2-2. HUD 바인딩이 `NativeConstruct` 한 번에 의존한다.** (2026-10-14 반영 완료: `AWarriorController::OnPossess`에서 빙의 시점마다 주입, `PlayerHUDWidget::BindStatComponent`에서 이전 연결 해제(`RemoveDynamic`) 및 `AddUniqueDynamic` 연결로 Pawn 재생성/리스폰 대응 완료)
- [ ] **2-3. 초기 방송이 두 곳에 있다.** `CharacterStatComponent::BeginPlay`와 `PlayerHUDWidget::NativeConstruct` 둘 다 `BroadcastAllStats()`를 호출한다.
  컴포넌트 `BeginPlay` 시점에 위젯이 아직 없으면 허공에 방송하는 호출이 된다. 순서에 의존하지 않도록 하나만 남긴다(위젯이 연결 직후 현재값을 읽어 가는 쪽을 권장).
- [ ] **2-4. `Dodge`의 스태미나 비용 25.f가 매직 넘버이다.** (`Warrior.cpp`)
  `UPROPERTY(EditDefaultsOnly)` 멤버나 데이터로 분리. `StatDefine.csv`의 Stamina 초기값이 60이라 `LoadCharacterStat`이 연결되면 구르기는 2번뿐이다. 비용과 최대치를 같이 조정해야 한다.
  `CharacterStatComponent`가 null이면 검사 없이 구르기가 통과하는 점도 의도인지 확인.

### 2) 일관성

- [x] **2-5. HUD 위젯이 구체 클래스 `AWarrior`에 의존한다.** (2026-10-14 반영 완료: `ICharacterStatInterface` 도입 및 `AWarrior` 상속을 통해 구체 클래스 의존성 완전 분리)
- [x] **2-6. 비율 계산이 4곳에 중복된다.** (2026-10-14 반영 완료: `PlayerHUDWidget::UpdateProgressBar` 공통 헬퍼 함수로 중복 연산 단일화)
- [x] **2-7. 컨트롤러가 위젯을 `UUserWidget`으로 들고 있다.** (2026-10-14 반영 완료: `TSubclassOf<UPlayerHUDWidget>`, `TObjectPtr<UPlayerHUDWidget>`로 타입 구체화 및 `OnPossess`에서 `BindStatComponent` 호출 연동)
- [ ] **2-8. include 스타일이 아직 섞여 있다.**
  `Warrior.cpp`에 추가된 `#include <Character/CharacterStatComponent.h>`는 각괄호이고, 같은 파일의 나머지 프로젝트 헤더는 따옴표이다.
  `//DataTable` 주석 아래에 있어 위치도 맞지 않는다. `PlayerHUDWidget.cpp`도 따옴표와 각괄호가 섞여 있다. 프로젝트 헤더는 따옴표, 엔진 헤더는 각괄호로 규칙을 정해 통일.
- [ ] **2-9. 위젯 변수명 `ProgressBar_HP`는 언리얼 PascalCase 관례와 다르다.** (가이드는 "파스칼 케이스 준수"라고 서술)
  `BindWidget`은 WBP의 위젯 이름과 일치해야 하므로 이름을 바꿀 때는 WBP와 같이 바꾼다. 우선순위 낮음.
- [ ] **2-10. `BindWidget`은 WBP에 해당 위젯이 없으면 WBP 컴파일 오류가 나므로 런타임 null이 되지 않는다.**
  `if (ProgressBar_HP && ...)`의 null 검사는 중복이다. 반대로 `NativeConstruct`의 실패 경로(`return`)는 로그가 없어 원인 추적이 어렵다. 프로젝트의 다른 곳처럼 `UE_LOG`를 남길 것.
- [ ] **2-11. Build.cs**: `UMG`는 Public, `Slate`/`SlateCore`는 Private이다. 단일 게임 모듈이라 문제는 없다. 파일에 남은
  "Uncomment if you are using Slate UI" 주석은 이제 낡았으므로 정리 대상.

### 3) 섹션 1에서 아직 코드에 반영되지 않은 항목

`리뷰 참고용.txt`에 옮겨 둔 항목들이 있으나 코드에는 반영 전이다. (※ 이 단락은 작성 시점 기준이다. 이후 `LoadCharacterStat`/`GetStatInitialValue`가 `FindRow` 방식으로 구현되어 1-2/1-3은 부분 해결되었다. 최신 상태는 섹션 4 참고.)
1-2/1-3(`LoadCharacterStat` 미연결, `MaxHP` 100 vs CSV 250), 1-5(`Use*` 음수 가드), 1-6(`.cpp:119-146` 주석 코드, `SetCurrentHP` 위 주석 들여쓰기),
`EquipLoad`/`Poise` 이름, "스테미너/스텟" 표기와 `"CurrentStat"` 카테고리, HP/FP/Stamina 3벌 중복, `Set*` 공개 범위, 사망 신호.
추가로 `AWarrior`는 적 AI에도 쓰이므로 적마다 컴포넌트와 Tick이 하나씩 돈다. 스태미나가 가득 차면 Tick을 끄는 방안은 여전히 유효하다.
`리뷰 참고용.txt`는 이 문서와 내용이 겹치므로, 리뷰는 이 파일 한 곳을 기준으로 두고 루트의 txt는 정리할 것.

### 4) PROJECT_GUIDE.md

- [x] **G-1. 이미 끝난 일이 예정으로 남아 있다.** (2026-10-14 반영 완료: 7-3의 `FOnStatChangedDelegate` 통일 및 10장 로드맵 `[완료]` 처리)
- [x] **G-2. 8-2 Q1의 `UseStamina` 설명이 현재 코드와 다르다.** (2026-10-14 반영 완료: 현재 `< Amount` 구현 및 소울라이크 방식 상세 비교 추가)
- [ ] **G-3. 스탯 데이터 모델 설명이 서로 충돌한다.** 4장 팁(146-151행)은 레벨별 **최종값 기준표**를 CSV에 두라고 하고,
  143행과 `FStatGrowth`는 구간별 **증가량** 모델이다. 레벨 1~40을 레벨당 1행 증가량으로 바꾸는 작업(`StatGrowth.new.csv`)도 아직 반영 전이다.
  스탯창이 읽을 모델을 하나로 정하고 문서에 적어야 한다.
- [x] **G-4. 절대 경로 링크** (2026-10-14 반영 완료: 상대 경로 `Source/...` 링크로 수정)
- [ ] **G-5. 문서에 아키텍처 설명, 진행 로그, Q&A 노트, 로드맵이 한 파일에 섞여 있다.** 작성일은 10-12인데 10-13 내용이 들어 있고,
  코드 스니펫을 복사해 두어서 코드가 바뀌면 바로 낡는다(G-1이 그 사례). "현재 상태 요약"과 "변경 이력"을 분리하고, 코드는 파일 링크로 대체.
- [ ] **G-6. 9-2의 "컴파일 타임 1:1 결합"은 정확히는 WBP 컴파일 타임의 검증이다.** "MVC" 표현도, 위젯이 `AWarrior`에 직접 의존하는 지금 구조(2-5)와는 거리가 있다.
- [x] **G-7. 로드맵 순서** (2026-10-14 반영 완료: 스탯창 선결 과제로 '데이터 파이프라인 정립(DT 로드 및 PlayerState 연동)'을 2단계로 명시)
- [ ] **G-8. 14행 "StateTree 포함"** (2026-10-14 재오픈: 가이드는 "Behavior Tree 주력, StateTree 비활성"이라 적었으나 `.uproject`에서 `StateTree`, `GameplayStateTree`는 `Enabled: true`. "활성이지만 미사용"으로 고칠 것.)
- [x] **G-9. 협업 규칙(256-258행)이 특정 도구 기준으로 쓰여 있다.** (2026-10-14 반영 완료: Claude 코드 리뷰어 역할 및 `Review(Claude).md` 기반 협업 체계 추가)

### 5) 권장 작업 순서

2-1(경계값) → 2-2/2-3(바인딩 구조) → 섹션 1의 1-2/1-3(데이터 로딩) → G-3(데이터 모델 확정) → 스탯창 작업.

---

## 3. 수정본 검증 (2026-10-14, Gemini 기반 수정 후)

**대상**: 커밋 `4ba5941` 이후 미커밋 변경분 (`CharacterStatComponent`, `PlayerHUDWidget`, `WarriorController`, `Warrior.h`, 신규 `Interface/CharacterStatInterface`, `PROJECT_GUIDE.md`).
빌드와 실행은 확인하지 않았고 코드 읽기만 했다. `WBP_PlayerHUD.uasset` 변경은 바이너리라 검토하지 못했다.

**총평**: 섹션 2의 핵심 항목(2-1, 2-2, 2-5, 2-6, 2-7)은 코드로 확인되었다. 설계 방향은 맞고 구현도 안정적이다.
다만 **리뷰 문서에 사실과 다른 기록이 있고**, 새로 생긴 정리 대상이 몇 가지 있다.

### 1) 검증 결과

| 항목 | 결과 | 근거 |
|---|---|---|
| 2-1 경계값 | 확인됨 | `UseFP`/`UseStamina` 모두 `< Amount` (`.cpp:94`, `.cpp:103`) |
| 2-2 바인딩 | 확인됨 (아래 보완 있음) | `OnPossess` 주입 + `BindStatComponent`의 `RemoveDynamic` → `AddUniqueDynamic` + `NativeDestruct` 해제 |
| 2-5 구체 클래스 의존 | 확인됨 | 위젯이 `AWarrior` 대신 `ICharacterStatInterface`만 사용 |
| 2-6 비율 중복 | 확인됨 | `UpdateProgressBar` 헬퍼로 통합 (컴포넌트의 `Get*Ratio` 3개는 그대로 남아 있음) |
| 2-7 위젯 타입 | 확인됨 | `TSubclassOf/TObjectPtr<UPlayerHUDWidget>`, `FClassFinder<UPlayerHUDWidget>` |
| G-1, G-2, G-4, G-7, G-9 | 확인됨 | 가이드에 반영됨 |
| G-8 | **부정확** | 가이드가 "StateTree 플러그인은 비활성"이라 적었으나 `.uproject`에서 `StateTree`, `GameplayStateTree`는 `Enabled: true`이다. "활성이지만 미사용"이 맞다. |
| 2-3, 2-4, 2-8, 2-9~2-11, G-3, G-5, G-6 | 미반영 (그대로 열림) | |

### 2) 문서 기록 오류 (확인 필요)

- **섹션 2의 2-1 항목에 있는 주의 문구는 사실과 다르다.** "`RegenerateStamina`에서 `CurrentStamina == MaxStamina`로 변경된 부분"이라고 되어 있으나,
  실제 코드는 `if (CurrentStamina >= MaxStamina)`이고 변경 내역에도 이 함수는 없다. 근거 없는 경고이므로 삭제해야 한다. (2026-10-14 삭제 완료)
- 이 파일의 `[x]` 표시와 반영 완료 문구가 작성자 도구에 의해 직접 수정되어 있다. 이번에는 위 표대로 코드에서 확인되어 문제가 없었지만,
  리뷰 문서의 완료 표시는 리뷰어가 코드를 확인한 뒤에 닫는 것이 원칙이다.

### 3) 새로 발견된 문제 / 개선 제안

- [ ] **3-1. `OnPossess`에서 인터페이스가 없는 Pawn에는 이전 바인딩이 그대로 남는다.** (`WarriorController.cpp` 하단)
  `else`가 없어 `BindStatComponent(nullptr)`이 호출되지 않는다. 또한 `OnUnPossess`에서 해제하지 않아, Pawn이 사라진 뒤 새 Pawn이 오기 전까지 이전 값이 화면에 남는다.
- [ ] **3-2. 스탯 컴포넌트와 인터페이스를 `AWarrior`가 아니라 `ACharacterBase`에 두는 편이 맞다.**
  지금은 보스·일반 몬스터가 `AWarrior`가 아니면 인터페이스를 따로 구현하고 컴포넌트도 다시 만들어야 한다. `CharacterBase`는 이미 그 용도의 공통 부모이다.
- [ ] **3-3. 새 코드에서 include 스타일이 오히려 늘었다.**
  `Warrior.h`의 `#include <Interface/CharacterStatInterface.h>`는 같은 파일의 나머지 프로젝트 헤더(`"Character/CharacterBase.h"` 등)와 달리 각괄호이다.
  `PlayerHUDWidget.cpp`, `WarriorController.cpp`의 새 include도 마찬가지이다. 가이드에는 "따옴표 통일"이 예정되어 있으므로 새로 추가한 것부터 맞춘다.
- [ ] **3-4. 주석 처리된 이전 코드가 16줄 남아 있다.** (`PlayerHUDWidget.cpp` `NativeConstruct`) 이력은 git에 있으므로 삭제한다.
- [ ] **3-5. 공백과 배치 정리.**
  - `WarriorController.h/.cpp`에 연속 빈 줄이 5~6줄씩 있다 (`BeginPlay`와 생성자 사이 등).
  - `OnPossess`는 UI 전용이 아닌 컨트롤러 오버라이드인데 `// [UI]` 블록 안, `private` 뒤의 `protected:`에 들어가 있다. `BeginPlay`/`Tick` 선언 옆으로 옮기고 접근 지정자를 한 번씩만 둔다.
  - `PlayerHUDWidget.h`의 "캐릭터 사망시 안전하게 nullptr 만든다." 주석이 `CurrentStatComponent` 선언 아래에 떨어져 있다. 위로 옮긴다.
  - `Warrior.h` 끝부분 빈 줄과 줄 끝 탭.
- [ ] **3-6. 전방 선언이 혼재한다.** `PlayerHUDWidget.h`는 `UCharacterStatComponent`는 파일 상단에서 전방 선언하고 `UProgressBar`는 `class UProgressBar*`로 인라인 선언한다. 상단 전방 선언으로 통일.
- [ ] **3-7. `UpdateProgressBar`는 `Max <= 0`이면 바를 갱신하지 않아 이전 값이 남는다.** 0%로 맞추는 쪽이 안전하다.
- [ ] **3-8. 컴포넌트 `BeginPlay`의 `BroadcastAllStats()`는 이제 HUD에서 쓰이지 않는다.** 위젯이 연결 시점에 Getter로 값을 읽기 때문이다. 다른 구독자가 없다면 2-3 항목을 닫기 위해 삭제한다.
- 인터페이스 자체는 `UINTERFACE(MinimalAPI)` + 순수 가상 함수로 올바르게 선언되어 있다. `InterfaceCpp`는 비어 있으므로 유지해도 되고 삭제해도 된다.

### 4) 다음 순서 제안

3-1, 3-2 → 2-3(`BeginPlay` 방송 삭제와 3-8 통합) → 3-3~3-7(정리) → 데이터 로딩(1-2, 1-3) → G-3.

---

## 4. 현재 코드 점검 + main 병합 대비 (2026-10-14)

**대상**: `Bae` 작업 트리의 현재 상태와 `origin/main`(`b42a975`, "Actor Component Save/Load").
빌드와 실행은 확인하지 않았고 코드 읽기와 `git merge-tree` 시험 병합만 했다. 문서 갱신만 하고 코드는 수정하지 않았다.

**총평**: 섹션 3 이후 `LoadCharacterStat`(`FindRow` 기반), `UpdateCharacterStat`, `StatusWindowWidget`이 추가되어 1-2/1-3은 부분 해결되었다.
다만 새 코드에 **복사 붙여넣기 버그**와 **목적을 달성하지 못하는 `UpdateCharacterStat`**이 있고, `main`과는 같은 함수를 서로 다르게 구현해 두 파일에서 충돌한다.

### 1) 새로 발견된 버그 / 동작 문제

- [ ] **4-1. FP 분기가 HP에 대입한다.** (`CharacterStatComponent.cpp:44`) `CurrentHP = FMath::Clamp(CurrentFP, 0.f, MaxFP);` → `CurrentFP = ...`여야 한다.
- [ ] **4-2. `UpdateCharacterStat`이 의도한 일을 하지 않는다.**
  - 매개변수 `Attributes`를 쓰지 않는다. (`StatGrowth` 계산 없음)
  - 내부에서 `LoadCharacterStat()`을 호출하는데, 이 함수가 끝에서 Current를 Max로 덮어쓴다(`.cpp:106-108`). 따라서 뒤따르는 "저장된 현재값 복원/Clamp" 분기는 의미가 없다.
  - 호출하는 곳이 `Bae`에는 없다. `BeginPlay`의 `LoadCharacterStat()`과 중복 로드되는 구조이기도 하다.
  - 정리 방향: 최대치/스탯 계산(`main`의 `UpdateCharacterStat` 정의)과 현재값 복원(Initialize류)을 분리한다.
- [ ] **4-3. `WarriorController` 생성자에 빈 경로 `FClassFinder`.** (`.cpp:68-70`, `TEXT("")`) 로드 경고/오류 가능, 스탯창도 열리지 않는다.
  WBP 제작 전이라면 `FClassFinder`를 빼고 `UPROPERTY(EditDefaultsOnly)`로 BP에서 지정하게 한다.
- [ ] **4-4. 스탯창 갱신 경로가 없다.** `StatusWindowWidget`은 Max 값만 읽고, 레벨/능력치가 바뀔 때 `RefreshStatus`를 부르는 경로가 없다. 데이터 흐름을 한 줄로 확정할 것(1-2/G-3과 연결).
- [ ] **4-5. `UpdateCharacterStats(int32)`(Warrior)와 `UpdateCharacterStat`(컴포넌트)이 이름만 비슷하고 책임이 겹친다.** `main`은 전자를 `UpdateCharacterStats()`로 바꿔 후자를 호출한다.
- [ ] **4-6. 로그 오타**: `.cpp:97` `{StatComponent]`. 닫는 괄호 종류가 다르다.
- 열려 있는 기존 항목: 1-5(`Use*` 음수 가드), 3-1, 3-2, 3-3~3-8, 2-3/3-8(`BeginPlay` 방송 삭제), `Set*` 공개 범위, `ApplyDamage` 이름, 사망 신호.

### 2) main 병합 시 확인할 것

시험 병합 결과: `CharacterStatComponent.cpp`, `Warrior.cpp`에서 **충돌**, `.h` 두 개는 자동 병합.
공통 조상은 `bb9e0ac`이며 `main`에는 그 이후 커밋 1개(`b42a975`)만 있다.

| 항목 | `main` | `Bae` | 병합 시 판단 |
|---|---|---|---|
| `UpdateCharacterStat(Attributes)` | 최대치/스탯을 Define 초기값으로 계산, Growth 계산은 Todo | 같은 이름이지만 Current까지 덮어씀(4-2) | `main`의 "계산만 담당" 정의에 맞추고 복원은 분리 |
| `StatGrowthTable` 로딩 | 있음 | 없음 | `main` 것을 받는다 |
| `Warrior::UpdateCharacterStats()` | `PlayerState`의 `SaveData.Attributes`를 컴포넌트에 전달 | `int32 CharacterLevel` 시그니처, PlayerState 연결 없음 | `main` 연결을 먼저 받고 `Bae` 코드 얹기 |
| Tick, 스태미나 회복, `UseStamina`, 델리게이트 | **삭제됨** | 있음 | 그대로 `main`을 받으면 HUD/구르기가 깨진다. `Bae` 쪽 유지 필수 |
| `FWarriorAttributeData` 기본값 | 10 → **0**, `ZZAKTUNGSOUL_API` 제거 | 10, API 있음 | 0은 "추가로 올린 값"이라는 의미. Growth 계산식 확정 후 결정 |
| `Warrior::BeginPlay` | `UpdateCharacterStats()` 호출, `PS`가 없으면 조용히 return | `UpdateCharacterStats(1)` | 빙의 시점 순서에 따라 PS가 null일 수 있다. `OnPossess` 흐름과 함께 확인 |

### 3) 통합 작업 순서 (섹션 1, 2, 3, 4를 대체)

1. `main` 병합 방향 결정 (위 표). 병합 전에 4-2의 책임 분리 방침을 `main` 담당자와 맞춘다.
2. 병합 후 충돌 해소: `CharacterStatComponent` (Tick/델리게이트/Use* 유지 + `main`의 Growth 로딩), `Warrior.cpp` (`PlayerState` 연결 + Dodge/HUD).
3. 한 줄 버그: 4-1, 1-5, 3-7, 4-6.
4. 데이터 로딩: 4-2(계산/복원 분리), `StatGrowth` 계산식, G-3(데이터 모델 확정), 4-5.
5. UI: 4-3, 3-1, 4-4, 2-3/3-8.
6. 구조: 3-2(`ACharacterBase`로 이동), `ApplyDamage` 이름, `Set*` 공개 범위, 사망 신호.
7. 정리: include 스타일(2-8, 3-3), 주석 코드(3-4), 공백(3-5), 전방 선언(3-6), G-3/G-5/G-6/G-8.

※ 3번의 4-1은 병합 방향과 무관한 한 줄 오류이지만, 4-2와 같은 함수 안이라 함께 고치는 편이 충돌이 적다.
