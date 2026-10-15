# Review(Claude)

코드 리뷰 기록 문서. 리뷰 내용만 정리하며, 코드는 직접 수정하지 않는다.
각 항목은 `[ ]`(미반영) / `[x]`(반영 완료)로 진행 상태를 표시한다.

---

## 0. 병합 전 필수 확인 사항 (2026-10-14)

`Bae`를 `main`(`b42a975`)에 병합하는 담당자는 아래 항목을 **병합 전에 반드시 확인**한다. 시험 병합(`git merge-tree`) 결과 `CharacterStatComponent.cpp`, `Warrior.cpp`에서 충돌한다. 상세 근거는 섹션 4의 3)과 섹션 5의 2) 참고.

- [ ] **M-1. `FWarriorAttributeData` 기본값의 의미를 정한다.** `main`은 10 → 0으로 바꾸고 `ZZAKTUNGSOUL_API`를 제거했다.
  0이 "초기값 10에서 올린 포인트 수"라면 `StatGrowth`의 MinLevel/MaxLevel도 0부터 세는 증가 포인트 기준이어야 하고, 스탯창은 `10 + 값`을 표시해야 한다.
  `StatusWindowWidget`은 값을 그대로 표시하므로 Vigor가 0으로 보인다. 기존 세이브 파일(값 10)과의 호환도 같이 정한다.
- [ ] **M-2. `UpdateCharacterStat`의 책임 범위를 정한다.** `main` 버전은 `Max*`와 전투 스탯만 계산하고 `Current*`를 갱신하지 않으며 방송도 하지 않는다.
  그대로 합치면 최대치만 250이고 현재값은 0이라 HUD가 빈 채로 시작한다. `Current* = Max*`와 `BroadcastAllStats()`를 함수 끝에 두거나, 계산과 복원을 별도 함수로 분리한다.
- [ ] **M-3. `Bae`의 `CharacterStatComponent` 기능을 잃지 않는다.** `main`은 `TickComponent`, 스태미나 회복, `UseStamina`/`UseFP`, 델리게이트, `CurrentHP`/`CurrentFP`를 삭제했고 `Max*` 기본값을 0으로 바꿨다.
  `main` 쪽을 그대로 받으면 HUD와 구르기가 깨진다. 구조는 `Bae`를 기준으로 하고, `main`에서는 static `FName` 키, `GetStatInitialValue`, `UpdateCharacterStat(Attributes)` 시그니처, `StatGrowthTable` 로딩을 가져온다.
- [ ] **M-4. `Warrior.cpp` / `Warrior.h`의 충돌 해소.** `UpdateCharacterStats()`(인자 없음, `main`)와 `UpdateCharacterStats(int32)`(`Bae`) 시그니처를 하나로 맞춘다.
  `Dodge`의 스태미나 소모, 인터페이스 상속, `PlayerState` 연결(`main`)을 모두 남긴다.
- [ ] **M-5. `PlayerState`가 null일 때의 동작.** `main`의 `Warrior::BeginPlay`는 `PS`가 없으면 조용히 return하므로 **AI가 조종하는 `AWarrior`는 모든 스탯이 0**이 된다. 플레이어가 아닐 때는 기본값 로드로 fallback한다.
  `BeginPlay` 시점에 `PlayerState`가 항상 유효하다는 보장도 없으므로(리스폰/로드) `PossessedBy`/`OnRep_PlayerState` 이후 호출이 안전한지 확인한다.
- [ ] **M-6. 컴포넌트 `BeginPlay`의 `BroadcastAllStats()`를 삭제하지 않는다.** 위젯 바인딩이 폰의 `BeginPlay`보다 먼저 실행될 수 있어, 이 방송이 HUD를 250/50/60으로 맞추는 유일한 경로이다(섹션 4의 4-2).
- [ ] **M-7. `ZZakTungSoul.Build.cs`는 수동으로 합친다.** `Bae`는 `UMG`/`Slate`/`SlateCore`를, `Kang`은 `NavigationSystem`/`GameplayTasks`를 추가해 같은 `PublicDependencyModuleNames` 블록을 둘 다 바꿨다.
- [ ] **M-8. `ZZAKTUNGSOUL_API` 제거의 의도를 `main` 작업자에게 확인한다.** 같은 모듈 안에서는 문제가 없으나 다른 모듈에서 쓰면 링크 오류가 난다.
- [ ] **M-9. 병합 직후 확인 (빌드와 PIE).**
  - 빌드 성공.
  - 시작 시 HUD의 HP/FP/Stamina 바가 가득 찬 상태로 표시되는지, 최대치가 250/50/60인지.
  - 구르기 소모, 스태미나 회복(1초 후 시작), 구르기 횟수.
  - AI가 조종하는 `AWarrior`가 크래시 없이 동작하고 스탯이 0이 아닌지.
  - C키 스탯창과 Attribute 표시값.

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
- [x] **1-2. `LoadCharacterStat()`가 아무 일도 하지 않는다.** (`.cpp:36-43`) (2026-10-14 확인: `48e7420`에서 `BeginPlay`가 호출하고 InitialValue를 반영함. 성장 연결은 섹션 4의 4-3)
  행을 지역 배열에 담고 버리며 어디서도 호출되지 않는다. 그래서 `MaxHP/MaxFP/MaxStamina`는 항상 100이다.
  `StatDefine.csv`의 초기값(HP 250, FP 50, Stamina 60)과 어긋난다.
- [x] **1-3. (2026-10-14 확인: `FindRow`로 변경됨) `GetAllRows`로는 어느 행이 HP인지 알 수 없다.**
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

## 4. 신규 커밋 리뷰 (2026-10-14)

**리뷰 대상**
- `Bae` `48e7420` (로컬 HEAD. 섹션 3의 미커밋 변경분이 커밋됨 + `StatusWindowWidget`, C키 토글, `LoadCharacterStat` 구현)
- `origin/main` `b42a975` (ahnw00, "Actor Component Save/Load": `CharacterStatComponent`, `Warrior`, `WarriorSaveData`)
- `origin/Kang` `289d384` 외 4개 (적 AI: `EnemyBase`, `EnemyAIController`, `BTTask_*`, `EnemyAIData`)
- `origin/Ahn` `cc56451` ("Layered blend per bone")는 `Source` 변경이 없어 코드 리뷰는 생략.

빌드와 실행은 하지 않았고 코드 읽기만 했다. `.uasset`/`.umap`은 검토하지 못했다.

**총평**: Bae 쪽은 섹션 3의 지적이 대부분 그대로 남아 있고 새 위험은 적다. 가장 큰 문제는 **`Bae`와 `main`이 `CharacterStatComponent`와 `Warrior`를 서로 다르게 바꿔서 병합 시 충돌과 빌드 실패가 확실**하다는 점이다. 적 AI(Kang)는 구조가 깔끔하지만 크래시 가능성이 있는 곳이 있고 Stat 시스템과 연결이 없다.

### 1) Bae `48e7420` 확인 결과

| 항목 | 결과 | 근거 |
|---|---|---|
| 1-2 최대치 로딩 | 반영됨 | `LoadCharacterStat`이 `BeginPlay`에서 호출되어 HP 250, FP 50, Stamina 60이 적용됨 |
| 1-3 행 이름 조회 | 반영됨 | `FindRow`로 이름 조회. 이름은 아직 문자열 리터럴(`TEXT("HP")`)로 흩어져 있음 |
| 1-4 비교 연산자 | 반영됨 | `UseFP`/`UseStamina` 모두 `< Amount` |
| 1-5 `Use*` 음수 가드 | 미반영 | |
| 1-6 주석 처리된 코드 | 미반영 | `UseStamina` 아래 주석 블록이 그대로 있음 |
| 2-4 구르기 비용 매직 넘버 | 미반영 | 25.f 그대로. Stamina 60이 적용되어 구르기는 2번만 가능 |
| 3-1 ~ 3-7 | 미반영 | 주석 코드, 빈 줄, include 각괄호, 전방 선언 혼재 등 |
| 3-8 (`BeginPlay` 방송 삭제) | **권고 철회** | 아래 4-2 참고 |

### 2) 새로 발견한 문제 (Bae)

- [ ] **4-1. `StatusWindowWidgetClass`의 경로가 빈 문자열이다.** (`WarriorController.cpp` 생성자)
  `FClassFinder<UStatusWindowWidget>(TEXT(""))`는 생성자 실행 시마다 "Failed to find" 오류 로그를 남기고, 클래스가 없으므로 C키를 눌러도 아무것도 열리지 않는다.
  경로가 정해질 때까지 `FClassFinder` 블록을 지우고 `UPROPERTY(EditDefaultsOnly)`만 두어 BP에서 지정하게 한다.
- [ ] **4-2. 섹션 3의 3-8("`BeginPlay`의 `BroadcastAllStats()` 삭제")은 현재 구조에서는 틀린 권고이다.**
  컨트롤러 `OnPossess`나 위젯 `NativeConstruct`는 폰의 `BeginPlay`보다 먼저 실행될 수 있다. 그 시점의 `Max*`는 헤더 기본값 100이고,
  `LoadCharacterStat`이 `BeginPlay`에서 250/50/60으로 바꾼 뒤에는 `BroadcastAllStats()`가 HUD를 고쳐 주는 유일한 경로이다. 삭제하면 HUD가 100/100 기준으로 남을 수 있다.
  2-3은 삭제가 아니라 **방송을 `LoadCharacterStat` 끝에서 한 번만** 하도록 정리한다. 호출 순서(Pawn `BeginPlay` ↔ HUD 바인딩)는 로그로 한 번 확인할 것.
- [ ] **4-3. 성장 데이터가 여전히 연결되지 않았다.** `LoadCharacterStat`은 `StatDefine`의 InitialValue만 읽는다.
  `FWarriorAttributeData`(Vigor 등)와 `DT_StatGrowth`는 컴포넌트가 모른다. 레벨을 올려도 최대치는 바뀌지 않는다.
- [ ] **4-4. 로그 서식 오류와 중복.** `LoadCharacterStat`의 `%1.f`는 `%.1f`의 오타이다. 첫 `UE_LOG`의 `TEXT("{StatComponent] ...")`는 `{`가 `[`의 오타이고 줄 끝 세미콜론도 없다.
  두 람다(`LoadPoolStat`, `LoadStat`)가 거의 같은 코드이므로 하나로 합친다.
- [ ] **4-5. 피격 경로가 없다.** `ApplyDamage`와 `UseFP`를 호출하는 곳이 없고 `UseStamina`는 `Dodge`에서만 쓰인다. HP 바가 움직일 수 없고 사망 신호(`OnDeath`)도 없다.
- [ ] **4-6. `AWarrior`가 AI 적에도 쓰이는 구조이면** 컴포넌트와 Tick이 적마다 돈다. Kang 브랜치가 `EnemyBase`로 분리 중이므로 이 시점에 컴포넌트를 `ACharacterBase`로 올릴지 정해야 한다 (3-2).
- [ ] **4-7. 스탯창이 "최대값"만 보여준다.** `StatusWindowWidget`은 HP/FP/Stamina에 `GetMax*`를 표시한다. 라벨이 "HP"라면 현재값처럼 보일 수 있으므로 라벨 또는 "현재/최대" 표기를 정한다.
- [ ] **4-8. C키가 `BindKey`로 바인딩되어 있다.** 나머지 입력은 모두 Enhanced Input(`IA_*`)이므로 `IA_StatusWindow`로 맞추면 리매핑과 일관성에 유리하다.
  창을 열 때 `FInputModeGameAndUI()`만 쓰므로 카메라 입력과 이동은 계속 받는다. 의도인지 확인.
- 좋은 점: `ICharacterStatInterface` 분리, `BindStatComponent`의 `RemoveDynamic`/`AddUniqueDynamic`, `TWeakObjectPtr` 사용은 올바르다.

### 3) `origin/main` `b42a975`와 병합 충돌 (중요)

`git merge-tree` 결과 `Bae`와 `main`은 아래 파일을 둘 다 수정해서 충돌한다.

| 파일 | main 쪽 | Bae 쪽 |
|---|---|---|
| `CharacterStatComponent.h/.cpp` | `LoadCharacterStat`/`TickComponent` 삭제, `CurrentHP`/`CurrentFP` 삭제, `UpdateCharacterStat(const FWarriorAttributeData&)` + `GetStatInitialValue` + static `FName` 키 추가, `Max*`/`CurrentStamina` 기본값 0 | `LoadCharacterStat` 구현, 델리게이트/`Use*`/스태미나 회복/`Tick` 추가, `CurrentHP`/`CurrentFP` 사용 |
| `Warrior.h/.cpp` | `UpdateCharacterStats()` (인자 없음), `BeginPlay`에서 PlayerState의 Attributes로 호출 | `UpdateCharacterStats(int32)` 유지, `Dodge` 소모, 인터페이스 상속 |
| `WarriorSaveData.h` | Attribute 기본값 10 → 0, `ZZAKTUNGSOUL_API` 제거 | 변경 없음 (`StatusWindowWidget`이 이 값을 표시) |

그대로 합치면 `Tick`과 `CurrentHP` 삭제 때문에 Bae의 스태미나 회복과 HP 바가 컴파일 오류가 되거나 동작하지 않는다.

- [ ] **4-9. 합칠 때의 기준 제안.**
  - 구조는 Bae(델리게이트, `Use*`, 회복 Tick, 인터페이스)를 기준으로 하고, main에서는 **static `FName` 키 + `GetStatInitialValue` + `UpdateCharacterStat(Attributes)` 시그니처**를 가져온다. 이는 4-4와 1-3의 해결책이기도 하다.
  - main의 `UpdateCharacterStat`은 `Max*`만 갱신하고 **`Current*`는 갱신하지 않으며 방송도 하지 않는다**. 그대로 합치면 최대치만 250이고 현재값은 0이라 HUD가 빈 채로 시작한다. `Current* = Max*`와 `BroadcastAllStats()`를 함수 끝에 둔다 (로드 시 저장된 현재값 복원은 별도 과제).
  - `AWarrior::BeginPlay`에서 `GetPlayerState<AWarriorPlayerState>()`가 없으면 조용히 return하므로 **AI가 조종하는 `AWarrior`는 모든 스탯이 0**이다. Max 0이면 `Get*Ratio`가 0을 돌려준다. 플레이어가 아닐 때는 기본값 로드로 fallback한다.
  - `PlayerState`가 `BeginPlay` 시점에 항상 유효한지 보장되지 않는다(리스폰/로드). `PossessedBy`/`OnRep_PlayerState` 이후 호출이 안전하다.
  - Attribute 기본값이 0으로 바뀐 의미는 `Claude.md`의 "레벨의 기준" 쟁점과 직접 연결된다. 0이 "초기값 10에서 올린 포인트 수"라면 `StatGrowth`의 MinLevel/MaxLevel도 0부터 세는 증가 포인트 기준이어야 하고, 스탯창에는 `10 + 값`을 표시해야 한다. 현재 `StatusWindowWidget`은 값을 그대로 표시하므로 Vigor가 0으로 보인다.
- [ ] **4-10. main의 `// Todo` (성장 계산)는 구조 확정 후에 구현.** 예시로 적힌 `MaxHp += Vigor * 10.f`는 고정 곱셈이라 소프트캡 모델과 맞지 않는다. 구간 누적 합산이어야 하며 공식은 `Claude.md` 3-2 표를 기준으로 한다.
- main에서 구조체의 `ZZAKTUNGSOUL_API`를 제거한 것은 같은 모듈 안에서는 문제가 없으나 다른 모듈에서 쓰면 링크 오류가 난다. 의도라면 이유를 남길 것.

### 4) `origin/Kang` 적 AI 리뷰

**총평**: `EnemyBase` → `EnemyAIController` → BT Task의 책임 분리가 명확하고 `EnemyAIData`(DataAsset)로 BT와 이동 속도를 데이터화한 점, 블랙보드 키를 `EnemyAIBlackboardKey.h`로 모은 점이 좋다. 아래는 위험도 순이다.

- [ ] **K-1. `EnemyBase::UpdateMovementSpeed`가 `EnemyAIData`를 null 검사 없이 역참조한다.** (`EnemyBase.cpp`)
  `EnemyAIData`는 `EditDefaultsOnly`라 BP에서 지정하지 않으면 `nullptr`이다. `Patrol`/`Chase`/`Strafe` 분기에서 즉시 크래시한다. 초기 상태가 `Idle`이라 `BeginPlay`는 통과하지만 BT가 상태를 바꾸는 순간 크래시한다. 함수 앞에서 null 검사 + `UE_LOG`.
- [ ] **K-2. `bIsCombat`, `bIsStrafeMode` 비트필드가 초기화되지 않는다.** `uint32 bIsCombat : 1;`처럼 선언만 되어 있다. 명시적으로 `= false`를 주거나 `bool`로 선언한다. `BTTask_SetStrafeMode.h`는 `= true`로 초기화하고 있어 일관성도 없다.
- [ ] **K-3. 타겟 판별이 `GetPlayerPawn(0)` 비교이다.** (`OnTargetPerceptionUpdated`) 플레이어 폰이 교체되면 비교 대상이 바뀐다. 태그나 팀(`IGenericTeamAgentInterface`)이 확장에 안전하다. `DetectionByAffiliation`을 적/우호/중립 모두 `true`로 둔 의도도 주석으로 남긴다.
- [ ] **K-4. `OnPossess`의 실패 경로가 조용하다.** `Enemy`/`AIData`/`BehaviorTreeAsset`이 없으면 `return`만 하고 로그가 없다. 적이 서 있을 때 원인을 찾기 어려우므로 `UE_LOG(Warning)` 추가. `BehaviorTreeAsset`을 `AIData->MainBT`로 덮어쓰므로 멤버가 `EditDefaultsOnly`일 의미가 없다.
- [ ] **K-5. `BlackboardAsset`이 사용되지 않는 것으로 보인다.** `FObjectFinder`로 로드하지만 `RunBehaviorTree`는 BT가 가진 블랙보드를 쓴다. 안 쓰면 삭제.
- [ ] **K-6. `SetEnemyState`에서 `Dead`는 되돌릴 수 없다.** 리스폰/풀링이 필요하면 초기화 경로가 필요하다. `Attack`/`Hit` 분기는 속도를 바꾸지 않는다(의도인지 확인).
- [ ] **K-7. `bIsCombat`이 Strafe와 같은 값으로 묶여 있다.** 전투 중이지만 Strafe가 아닌 상태(공격 중 등)를 표현할 수 없다. `EEnemyState::Attack`과의 관계를 정리할 것.
- [ ] **K-8. `BTTask_SetStrafeMode`가 해제 때도 `TargetActor`가 없으면 `Failed`를 반환한다.** Perception이 타겟을 `ClearValue`한 직후에 해제 Task가 실패해서 `bIsStrafeMode`가 `true`로 남을 수 있다. 해제 경로는 타겟 검사 없이 처리한다.
- [ ] **K-9. 테스트 자산 정리.** `Source/Test/DA_Test`, `TestKangMap`, `BP_TestEnemyBase`는 병합 전에 정리 여부를 정한다.
- `Build.cs`는 Bae가 `UMG`/`Slate`/`SlateCore`를, Kang이 `NavigationSystem`/`GameplayTasks`를 추가해 같은 `PublicDependencyModuleNames` 블록을 둘 다 바꿨다. 병합 시 수동으로 합쳐야 한다.
- Stat 연결: `EnemyBase`가 `ACharacterBase`를 상속하므로 컴포넌트를 `CharacterBase`로 올리면(3-2) 적도 같은 HP/데미지 경로를 쓸 수 있다.

### 5) CSV 확장 계획(`Claude.md`)과 코드의 접점

- 코드는 `StatDefine`을 `FName` 행 키로 읽는 구조(`FindRow`)를 갖췄으므로 `StatDefine.csv`는 그대로 확장해도 된다.
- `StatGrowth.csv` 설계 전에 **Attributes가 "값(10~)"인지 "증가 포인트(0~)"인지**를 확정해야 한다 (4-9). 이 결정이 `MinLevel`/`MaxLevel`의 의미, 스탯창 표시, 세이브 호환성을 모두 정한다.
- 적도 같은 테이블을 쓰려면 `UpdateCharacterStat`이 `FWarriorAttributeData` 대신 공용 속성 구조체를 받는 쪽이 낫다.

### 6) 다음 순서 제안

1. `Bae`와 `main` 병합 방식 확정 (4-9). 그 전에는 `CharacterStatComponent`를 두 사람이 동시에 수정하지 않도록 조율.
2. 4-1(빈 경로 오류), 4-2/2-3(방송 정리), 4-4, 1-5, 1-6.
3. Attributes 의미 확정 → `StatGrowth.csv` 확장 → 성장 계산 (4-10).
4. 피격 경로와 `OnDeath` (4-5), 컴포넌트를 `CharacterBase`로 이동 (3-2).
5. Kang 브랜치 K-1, K-8부터 수정 요청.

---

## 5. 현재 코드 점검 + main 병합 비교표 (2026-10-14, 섹션 4와 병행)

> 이 섹션은 섹션 4(Kang/Ahn/main 포함 전체 리뷰)와 같은 날 병행해 쓴 것이다. 번호는 `5-n`으로 구분했고, 겹치는 항목(빈 경로 `FClassFinder` = 4-1/5-3 등)은 섹션 4를 기준으로 본다.

**대상**: `Bae` 작업 트리의 현재 상태와 `origin/main`(`b42a975`, "Actor Component Save/Load").
빌드와 실행은 확인하지 않았고 코드 읽기와 `git merge-tree` 시험 병합만 했다. 문서 갱신만 하고 코드는 수정하지 않았다.

**총평**: 섹션 3 이후 `LoadCharacterStat`(`FindRow` 기반), `UpdateCharacterStat`, `StatusWindowWidget`이 추가되어 1-2/1-3은 부분 해결되었다.
다만 새 코드에 **복사 붙여넣기 버그**와 **목적을 달성하지 못하는 `UpdateCharacterStat`**이 있고, `main`과는 같은 함수를 서로 다르게 구현해 두 파일에서 충돌한다.

### 1) 새로 발견된 버그 / 동작 문제

- [ ] **5-1. FP 분기가 HP에 대입한다.** (`CharacterStatComponent.cpp:44`) `CurrentHP = FMath::Clamp(CurrentFP, 0.f, MaxFP);` → `CurrentFP = ...`여야 한다.
- [ ] **5-2. `UpdateCharacterStat`이 의도한 일을 하지 않는다.**
  - 매개변수 `Attributes`를 쓰지 않는다. (`StatGrowth` 계산 없음)
  - 내부에서 `LoadCharacterStat()`을 호출하는데, 이 함수가 끝에서 Current를 Max로 덮어쓴다(`.cpp:106-108`). 따라서 뒤따르는 "저장된 현재값 복원/Clamp" 분기는 의미가 없다.
  - 호출하는 곳이 `Bae`에는 없다. `BeginPlay`의 `LoadCharacterStat()`과 중복 로드되는 구조이기도 하다.
  - 정리 방향: 최대치/스탯 계산(`main`의 `UpdateCharacterStat` 정의)과 현재값 복원(Initialize류)을 분리한다.
- [ ] **5-3. `WarriorController` 생성자에 빈 경로 `FClassFinder`.** (`.cpp:68-70`, `TEXT("")`) 로드 경고/오류 가능, 스탯창도 열리지 않는다.
  WBP 제작 전이라면 `FClassFinder`를 빼고 `UPROPERTY(EditDefaultsOnly)`로 BP에서 지정하게 한다.
- [ ] **5-4. 스탯창 갱신 경로가 없다.** `StatusWindowWidget`은 Max 값만 읽고, 레벨/능력치가 바뀔 때 `RefreshStatus`를 부르는 경로가 없다. 데이터 흐름을 한 줄로 확정할 것(1-2/G-3과 연결).
- [ ] **5-5. `UpdateCharacterStats(int32)`(Warrior)와 `UpdateCharacterStat`(컴포넌트)이 이름만 비슷하고 책임이 겹친다.** `main`은 전자를 `UpdateCharacterStats()`로 바꿔 후자를 호출한다.
- [ ] **5-6. 로그 오타**: `.cpp:97` `{StatComponent]`. 닫는 괄호 종류가 다르다.
- 열려 있는 기존 항목: 1-5(`Use*` 음수 가드), 3-1, 3-2, 3-3~3-8, 2-3(방송 정리. 3-8 삭제 권고는 섹션 4의 4-2에서 철회됨), `Set*` 공개 범위, `ApplyDamage` 이름, 사망 신호.

### 2) main 병합 시 확인할 것

시험 병합 결과: `CharacterStatComponent.cpp`, `Warrior.cpp`에서 **충돌**, `.h` 두 개는 자동 병합.
공통 조상은 `bb9e0ac`이며 `main`에는 그 이후 커밋 1개(`b42a975`)만 있다.

| 항목 | `main` | `Bae` | 병합 시 판단 |
|---|---|---|---|
| `UpdateCharacterStat(Attributes)` | 최대치/스탯을 Define 초기값으로 계산, Growth 계산은 Todo | 같은 이름이지만 Current까지 덮어씀(5-2) | `main`의 "계산만 담당" 정의에 맞추고 복원은 분리 |
| `StatGrowthTable` 로딩 | 있음 | 없음 | `main` 것을 받는다 |
| `Warrior::UpdateCharacterStats()` | `PlayerState`의 `SaveData.Attributes`를 컴포넌트에 전달 | `int32 CharacterLevel` 시그니처, PlayerState 연결 없음 | `main` 연결을 먼저 받고 `Bae` 코드 얹기 |
| Tick, 스태미나 회복, `UseStamina`, 델리게이트 | **삭제됨** | 있음 | 그대로 `main`을 받으면 HUD/구르기가 깨진다. `Bae` 쪽 유지 필수 |
| `FWarriorAttributeData` 기본값 | 10 → **0**, `ZZAKTUNGSOUL_API` 제거 | 10, API 있음 | 0은 "추가로 올린 값"이라는 의미. Growth 계산식 확정 후 결정 |
| `Warrior::BeginPlay` | `UpdateCharacterStats()` 호출, `PS`가 없으면 조용히 return | `UpdateCharacterStats(1)` | 빙의 시점 순서에 따라 PS가 null일 수 있다. `OnPossess` 흐름과 함께 확인 |

### 3) 통합 작업 순서 (섹션 1~3, 4를 보완. 섹션 4의 6) 순서와 충돌하면 섹션 4 우선)

1. `main` 병합 방향 결정 (위 표). 병합 전에 5-2의 책임 분리 방침을 `main` 담당자와 맞춘다.
2. 병합 후 충돌 해소: `CharacterStatComponent` (Tick/델리게이트/Use* 유지 + `main`의 Growth 로딩), `Warrior.cpp` (`PlayerState` 연결 + Dodge/HUD).
3. 한 줄 버그: 5-1, 1-5, 3-7, 5-6.
4. 데이터 로딩: 5-2(계산/복원 분리), `StatGrowth` 계산식, G-3(데이터 모델 확정), 5-5.
5. UI: 5-3, 3-1, 5-4, 2-3.
6. 구조: 3-2(`ACharacterBase`로 이동), `ApplyDamage` 이름, `Set*` 공개 범위, 사망 신호.
7. 정리: include 스타일(2-8, 3-3), 주석 코드(3-4), 공백(3-5), 전방 선언(3-6), G-3/G-5/G-6/G-8.

※ 3번의 5-1은 병합 방향과 무관한 한 줄 오류이지만, 5-2와 같은 함수 안이라 함께 고치는 편이 충돌이 적다.

---

## 6. 1015 변경분 점검 + 가이드 12장 검증 (2026-10-15)

**대상**: 커밋 `2324bab`("1014 최종") 이후 미커밋 변경(컴포넌트·HUD·스탯창 주석/include 정리, `PROJECT_GUIDE.md` 12장 신규) + 현재 작업 트리 전체.
빌드와 실행은 확인하지 않았다. 마지막 빌드 산출물(`DebugGame.dll`, 10-15 07:57)보다 늦은 수정(08:24~08:28)은 주석과 include 따옴표 정리라 영향은 작아 보인다.

**총평**: 이전 리뷰의 한 줄 버그들이 대부분 정리되었다. 남은 큰 문제는 **스탯창이 구조상 열릴 수 없다는 점(6-1)**, **`UpdateCharacterStat`의 책임 미정(5-2)**, **`main` 병합(섹션 0)**이다.

### 1) 이전 항목 검증

| 항목 | 결과 | 근거 |
|---|---|---|
| 1-5 `Use*` 음수 가드 | 반영됨 | `UseFP`/`UseStamina` 모두 `Amount <= 0.f`이면 `false` |
| 5-1 FP 분기가 HP에 대입 | 반영됨 | `CharacterStatComponent.cpp:38-45`가 `CurrentFP`로 수정 |
| 3-4 HUD의 주석 처리된 이전 코드 | 반영됨 | `PlayerHUDWidget.cpp` |
| 3-6 전방 선언 혼재 | 반영됨 | `PlayerHUDWidget.h`에 `class UProgressBar;` 추가 |
| 3-7 `Max <= 0`일 때 바 갱신 | 반영됨 | `UpdateProgressBar`가 0%로 설정 |
| 3-3 include 스타일 | **부분** | 컴포넌트·HUD·스탯창 `.cpp`는 따옴표로 통일됨. 남은 곳: `Warrior.cpp:15,20`, `WarriorController.cpp:7-22`(프로젝트 헤더 각괄호) |
| 4-1 빈 경로 `FClassFinder` | 코드는 해소 | 주석 처리됨. 그러나 아래 6-1 |
| 5-2 `UpdateCharacterStat` | 미반영 | `Attributes`를 쓰지 않고, 내부 `LoadCharacterStat()`이 `Current = Max`로 덮어써 뒤따르는 복원 분기가 무의미. 호출하는 곳도 없음 |
| 4-4 로그 오타 | 미반영 | `CharacterStatComponent.cpp:96` `{StatComponent]` |
| 2-3/4-2, 2-4(25.f), 3-1, 3-2, 3-5, 4-5~4-8, 섹션 0의 M-1~M-9 | 미반영 | 컨트롤러의 연속 빈 줄도 그대로 |

### 2) 새로 발견한 문제

- [ ] **6-1. (중요) 스탯창이 열리지 않는 구조이다.**
  - `StatusWindowWidgetClass`는 `EditDefaultsOnly`이다. 그런데 `ZTSGameMode.cpp:12`가 `PlayerControllerClass = AWarriorController::StaticClass()`로 **C++ 클래스를 직접 지정**하므로 BP에서 값을 채울 컨트롤러 BP가 없다.
  - `FClassFinder`는 주석 처리되어 있고 `WBP_StatusWindow`도 아직 없다(`Content/UI`에는 `WBP_PlayerHUD`만 있음).
  - 따라서 C키(`WarriorController.cpp:150`)를 눌러도 `StatusWindowWidgetClass`가 null이라 아무 일도 일어나지 않는다. 가이드 10장의 "[완료] 스탯창 토글"은 "코드 작성 완료, 위젯 에셋 없음"으로 정정할 것.
  - 해결 방향: `WBP_StatusWindow`를 만든 뒤 경로를 지정하거나, `BP_WarriorController`를 만들어 GameMode에서 그 클래스를 쓰게 한다.
- [ ] **6-2. 스탯창이 열릴 때 `RefreshStatus()`가 두 번 불린다.** `NativeConstruct`와 `ToggleStatusWindow` 둘 다 호출한다. `AddToViewport`마다 `NativeConstruct`가 다시 실행되므로 하나만 남긴다.
  `NativeConstruct`의 `// OnPossess는 Controller에만 있는 함수.` 주석은 이 함수와 관계가 없어 읽는 사람이 혼동한다.
- [ ] **6-3. 스탯창은 열려 있는 동안 갱신되지 않는다.** 값이 바뀌는 순간에 `RefreshStatus`를 부르는 경로가 없다(5-4). HP/FP/Stamina는 현재값이 아니라 `GetMax*`를 보여 준다(4-7).
- 좋은 점: `ICharacterStatInterface::GetSaveData()`의 기본 `nullptr`과 `AWarrior`의 오버라이드 분리, `BindWidgetOptional`과 람다 헬퍼의 null 처리는 올바르다.

### 3) PROJECT_GUIDE.md 12장(핵심 복습) 검증

| 항목 | 평가 |
|---|---|
| Q1 인터페이스 | 정확. 순수 가상 vs 기본 구현의 구분이 코드와 일치 |
| Q2 `OnPossess` 주입 | **프레이밍 보완 필요.** 실제 코드는 `NativeConstruct`와 `OnPossess`를 **병행**한다. `NativeConstruct`는 위젯이 Pawn보다 늦게 만들어진 경우, `OnPossess`는 위젯이 먼저 있거나 Pawn이 바뀌는 경우를 맡는다. "크래시"는 과장이다. 죽은 컴포넌트의 델리게이트는 더 이상 호출되지 않으므로 증상은 HUD가 갱신되지 않는 것이 주이다. 크래시는 일반 포인터를 보관해 역참조할 때 생긴다 |
| Q3 초기값 Getter | 정확. 컴포넌트 `BeginPlay`의 `BroadcastAllStats()`를 남겨 둔 이유(호출 순서가 보장되지 않아 이 방송이 `LoadCharacterStat` 이후 250/50/60을 HUD에 반영하는 경로, 4-2)를 한 줄 추가하면 완성 |
| Q4 `FindRow`와 정적 키 | **보완 필요.** (a) "컴파일 타임에 안전성 확보"는 과장이다. 코드 쪽 오타만 막고, CSV 행 이름과의 불일치는 런타임 경고와 0 반환이다. (b) `GetAllRows`의 문제는 순서가 아니라 **행 이름(키)을 알 수 없다**는 점이다. (c) 3번(스태미나 회복 타이머)은 주제가 달라 별도 문항으로 분리 |
| Q5 스탯창 | 설명은 정확하나 6-1 때문에 지금은 실제로 열리지 않는다. "구조 설명"이라는 점을 밝혀 둘 것 |
| 기타 낡은 서술 | 8-3(`FORCEINLINE` Getter → 이제 인터페이스 오버라이드), 9-1/9-2(`UUserWidget`, `NativeConstruct`만으로 바인딩), 14행(StateTree "비활성"; 실제로는 활성이지만 미사용, G-8), 5장 가데이터 100.f |

### 4) 12장에 추가하면 좋은 항목 (제안)

1. **초기화 순서**: 컨트롤러 `BeginPlay` → `OnPossess` → Pawn `BeginPlay` → 컴포넌트 `BeginPlay`의 순서 관련 질문. 실제 로그로 한 번 찍어 보고 그 결과를 적을 것. 이 프로젝트의 HUD 버그 가능성이 모두 여기에서 나온다.
2. **Dynamic 델리게이트가 필요한 이유**: 콜백이 `UFUNCTION()`이어야 하고, `AddDynamic`/`AddUniqueDynamic`/`RemoveDynamic`을 쓰며, BP에서도 바인딩 가능하다는 점.
3. **`UINTERFACE`의 U/I 쌍**: `Cast<ICharacterStatInterface>(Pawn)`이 되는 이유와 null 안전성, BP 구현이 필요하면 `BlueprintNativeEvent`가 필요하다는 차이.
4. **`BindWidget` vs `BindWidgetOptional`**: 이름 불일치 시 WBP 컴파일 오류 vs null. Optional은 사용처마다 null 검사가 필요(그래서 람다 헬퍼가 있음).
5. **포인터 종류**: `TObjectPtr`(소유/GC 보호, `UPROPERTY` 필요), `TWeakObjectPtr`(비소유, 소멸 감지), raw 포인터의 위험, 그리고 이 프로젝트에서 각각을 쓴 곳.
6. **`CreateDefaultSubobject`와 `FObjectFinder`/`FClassFinder`**: 생성자에서만 가능한 이유, 클래스 경로의 `_C` 접미사, 생성자 경로가 틀리면 어떤 증상이 나는지. 6-1이 좋은 사례.
7. **영속 데이터 vs 휘발 데이터**: `PlayerState`/`SaveGame`(Pawn이 죽어도 유지)과 컴포넌트(Pawn과 함께 사라짐)의 분리 근거, 그리고 리스폰 시 컴포넌트를 어떻게 초기화할지(5-2).
8. **입력 모드 3종 표**: 지금은 `ToggleStatusWindow` 주석에만 있다. `GameOnly`/`UIOnly`/`GameAndUI`와 `bShowMouseCursor`의 조합을 표로.
9. **"실수에서 배운 것" 목록**: `CurrentHP = Clamp(CurrentFP …)` 복사-붙여넣기 오타, `<=`와 `<` 경계값(구르기 4번이 3번이 된 사례), 빈 경로 `FClassFinder`, `EditDefaultsOnly`인데 BP 컨트롤러가 없는 경우(6-1).

### 5) 다음 순서 제안

6-1(스탯창 클래스 연결 방식 결정) → 6-2 → 5-2(`UpdateCharacterStat` 책임 분리) → 섹션 0(병합) → 나머지 정리(3-3, 3-5).

---

## 7. `main` 병합 충돌 정리 (2026-10-15)

**방법**: `git fetch` 후 `git merge-tree --write-tree`로 시험 병합했다(작업 트리와 브랜치는 변경하지 않음). 비교 대상은 `Bae` HEAD(`2324bab`)와 `origin/main`(`b42a975`).
공통 조상은 `bb9e0ac`이고 `main`에는 그 이후 커밋이 `b42a975` 1개뿐이다(`main`은 이후 변하지 않았다). 빌드는 하지 않았다.
이 섹션은 섹션 0(M-1~M-9)의 지시를 대체하지 않고 **정확한 충돌 지점과 자동 병합이 만드는 숨은 문제**를 보강한다.

### 1) 텍스트 충돌 (Git이 표시하는 것)

| 파일 | 충돌 수 | 내용 |
|---|---|---|
| `Character/CharacterStatComponent.cpp` | 3 | 상단 include / `GetStatInitialValue`·`UpdateCharacterStat` 정의 부분 / `TickComponent` 이후 본문 전체 |
| `Character/CharacterStatComponent.h` | 2 | `LoadCharacterStat`·`GetStatInitialValue` 선언부 / `StatGrowthTable`과 정적 키 선언부 |
| `Warrior/Warrior.cpp` | 1 | `#include <Warrior/WarriorPlayerState.h>` 중복 |
| `Warrior/Warrior.h` | 0 | 자동 병합됨 (`UpdateCharacterStats()` 선언과 인터페이스 상속이 함께 들어감) |

섹션 0은 "`.cpp` 두 개만 충돌"이라고 했으나, `Bae`도 `GetStatInitialValue`와 정적 키를 같은 이름으로 추가했기 때문에 `CharacterStatComponent.h`도 충돌한다.
충돌 구간은 Git이 서로 다른 함수를 섞어서 표시하므로 **"ours/theirs" 한쪽을 통째로 고르면 안 된다.** 아래 2)의 숨은 문제 때문에 특히 그렇다.

### 2) 자동 병합이 만드는 숨은 문제 (충돌 표시 없음)

- [ ] **7-1. (치명) 스태미나가 회복되지 않는다.** `main`이 컴포넌트 생성자의 `PrimaryComponentTick.bCanEverTick = true;`를 삭제했고, `Bae`는 그 줄을 건드리지 않아 **삭제가 자동 반영된다.**
  충돌을 `Bae` 쪽으로 해소해도 Tick이 꺼진 채라 `RegenerateStamina`가 호출되지 않는다. 빌드는 통과하므로 발견이 늦다. 해소 후 `bCanEverTick = true`가 있는지 확인한다.
- [ ] **7-2. `TickComponent` 선언이 헤더에서 사라진다.** `main`의 삭제가 `.h`에 자동 반영되고, `.cpp`의 `Bae` 쪽 `TickComponent` 정의만 남으면 선언이 없어 **컴파일 오류**이다. 헤더에 선언을 되살린다.
- [ ] **7-3. `Warrior.cpp`에 `CreateDefaultSubobject`가 두 번 들어간다.** `main`은 이름 `"CharacterStat"`, `Bae`는 `"CharacterStatComponent"`이다. 둘 다 남으면 두 번째가 포인터를 덮어쓰고 첫 번째 컴포넌트는 Pawn에 붙은 채 남는다(Tick과 스탯 로드가 두 번).
  하나만 남기고, `BP_Warrior`에 저장된 컴포넌트 이름과 맞는 쪽을 쓴다.
- [ ] **7-4. 헤더에 같은 멤버 함수가 중복 선언된다.** `UpdateCharacterStat`(UFUNCTION 버전과 일반 버전), `GetStatInitialValue`(protected와 private)이 각각 두 번 선언되어 **컴파일 오류**이다.
  정의도 `.cpp`에서 두 곳에 생긴다. 하나로 합친다(7-6 참고).
- [ ] **7-5. `UpdateCharacterStats()`의 `PlayerState` 조기 return.** `main` 버전은 `PS`가 없으면 이동 속도 설정(`MaxAcceleration`/`MaxWalkSpeed`/`BrakingDecelerationWalking`)보다 **먼저** return한다.
  PlayerState가 없는 AI `AWarrior`는 이동 설정을 못 받는다(`Bae` 버전은 무조건 설정). 이동 설정을 return 앞으로 옮기거나 `PS`가 없을 때의 대체 경로를 둔다(M-5).

### 3) 의미 충돌 (둘 다 자동으로 합쳐져도 결정이 필요한 것)

- [ ] **7-6. `UpdateCharacterStat`의 책임.** `main`은 Max와 전투 스탯만 계산하고 `Current*`를 건드리지 않으며 방송도 하지 않는다. `Bae`는 `LoadCharacterStat()`을 호출해 `Current = Max`로 덮어쓰고 `BroadcastAllStats()`까지 한다(M-2, 5-2).
  결정할 것: (a) 함수 하나로 "계산 + 복원"을 할지 (b) 계산과 복원을 분리할지. 둘 중 어느 쪽이든 `Attributes` 인자를 아직 쓰지 않는 점은 그대로이다(`main`의 Todo).
- [ ] **7-7. 초기화 호출이 두 번 일어난다.** 컴포넌트는 자기 `BeginPlay`에서 `LoadCharacterStat()`을 호출하고, `AWarrior::BeginPlay`가 `UpdateCharacterStats()` → `UpdateCharacterStat()`을 통해 다시 호출한다.
  컴포넌트의 `BeginPlay`는 Pawn의 `Super::BeginPlay()` 안에서 실행되므로 Pawn 쪽 호출이 나중에 와서 `Current`를 한 번 더 `Max`로 되돌린다. 나중에 저장된 현재값을 복원하려면 이 순서가 문제가 되므로 호출 경로를 하나로 정한다.
- [ ] **7-8. `FWarriorAttributeData` 기본값 10 → 0 (M-1).** `main`이 Vigor 등을 0으로 바꿨다(자동 병합됨). 스탯창은 값을 그대로 표시하므로 **Vigor 0**으로 보이고, 기존 세이브 파일에는 10이 저장되어 있다.
  의미를 "초기값 10에서 올린 포인트"로 확정하면 `StatGrowth`의 MinLevel/MaxLevel도 0부터 세는 기준이어야 하고 스탯창은 `10 + 값`을 보여 준다.
- [ ] **7-9. `ZZAKTUNGSOUL_API` 제거 (M-8).** 자동 병합으로 적용된다. 같은 모듈 안에서는 문제가 없고, 다른 모듈에서 `FWarriorSaveData`를 쓰면 링크 오류가 난다. `main` 작업자에게 의도를 확인한다.
- [ ] **7-10. `DT_LevelRequiredSouls.uasset`만 `main`에서 바뀌었다.** `Content/Data/LevelRequiredSouls.csv`는 그대로이다. 에셋과 CSV가 어긋난 채로 남지 않도록 `main` 작업자에게 CSV의 최신본을 확인한다. 바이너리라 충돌은 없지만 내용 확인은 에디터에서만 가능하다.

### 4) 병합 전 준비

- 작업 트리에 **미커밋 변경**이 있다: `CharacterStatComponent.cpp`(충돌 대상 파일), `PlayerHUDWidget.cpp`, `StatusWindowWidget.*`, `WarriorController.cpp`, `WBP_PlayerHUD.uasset`, 그리고 **미추적 `WBP_StatusWindow.uasset`**. 먼저 커밋하거나 stash하지 않으면 병합이 거부되거나 섞인다. 새 위젯 에셋은 이를 참조하는 코드와 같은 커밋에 넣는다.
- `origin/Ahn`도 `main`과 같은 파일에서 `Bae`와 충돌한다(`Ahn`이 `main`의 변경을 포함). `main`에 먼저 합치고 나서 `Ahn`/`Kang`을 정리하는 편이 중복 해소를 줄인다.
- `origin/Kang`은 `ZZakTungSoul.Build.cs` 1개 파일에서만 `Bae`와 충돌한다(`UMG`/`Slate`/`SlateCore` ↔ `NavigationSystem`/`GameplayTasks`). 두 의존성 목록을 합치면 된다.
- `main`의 컴포넌트 파일들은 UTF-8 BOM이 새로 붙었다. 내용에는 영향이 없으나 diff가 파일 전체로 보이므로 편집기 인코딩을 맞춘다.

### 5) 해소 후 확인 목록

1. 모든 파일에 충돌 표식(`<<<<<<<`, `=======`, `>>>>>>>`)이 남지 않았는지 확인.
2. `CharacterStatComponent.h/.cpp`: `bCanEverTick = true` 있음(7-1), `TickComponent` 선언·정의 각 1개(7-2), `UpdateCharacterStat`/`GetStatInitialValue` 선언·정의 각 1개(7-4), `StatGrowthTable` 로딩(`main`의 추가분)과 `Data/StatGrowth.h` include 유지.
3. `Warrior.cpp`: `CreateDefaultSubobject<UCharacterStatComponent>` 1개(7-3), `Dodge`의 스태미나 소모 유지, `GetSaveData()` 오버라이드 유지, 이동 설정이 `PS`와 무관하게 적용(7-5).
4. 프로젝트 헤더 include는 따옴표로 통일(`main`이 가져온 각괄호 include 포함).
5. 빌드 후 PIE에서: HUD 시작값(HP 250 / FP 50 / Stamina 60 가득 참), 구르기 2회 후 **1초 뒤 스태미나가 회복되는지**, AI Warrior 이동 속도, C키 스탯창 표시값, 세이브/로드(F키 바인딩) 후 값이 유지되는지.

### 6) 권장 해소 방식

`CharacterStatComponent.h/.cpp`는 `Bae` 버전을 기준으로 삼고 `main`에서 아래 세 가지만 옮긴다: ① `StatGrowthTable` 멤버·로딩·`Data/StatGrowth.h` include, ② `UpdateCharacterStat`의 정의 위치(7-6 결정 후), ③ 정적 키 주석 정도.
`Warrior.cpp`는 컴포넌트 생성 1개, `UpdateCharacterStats()` 시그니처(`main`)와 `Dodge` 소모(`Bae`)를 모두 남긴다. `WarriorSaveData.h`는 7-8 결정이 나기 전에는 `main`의 변경을 그대로 받되 스탯창의 표시 방식을 같이 정한다.
`main` 작업자(ahnw00)와 `CharacterStatComponent`를 동시에 수정하지 않도록, 해소 완료 전까지 해당 파일의 수정을 멈추도록 알린다.

### 7-A. 재점검 (2026-10-15, 미커밋 변경 포함)

**방법**: 작업 트리 전체(미커밋 수정과 미추적 파일 포함)를 임시 인덱스에서 스냅샷 커밋(어떤 브랜치에도 연결되지 않음)으로 만든 뒤 `git merge-tree`로 `origin/main`(`b42a975`, 변동 없음)과 시험 병합했다.
작업 트리, 브랜치, 인덱스는 건드리지 않았고(점검 전후 `git status`와 HEAD가 동일함) 코드 수정과 실제 병합은 하지 않았다.

**결과**
- 충돌 파일과 개수는 이전과 같다: `CharacterStatComponent.cpp` 3, `CharacterStatComponent.h` 2, `Warrior.cpp` 1. `Warrior.h`, `WarriorController.cpp`, `StatusWindowWidget.cpp`는 충돌 없음.
- 7-1 ~ 7-5의 숨은 문제가 스냅샷 기준으로도 그대로 재현된다: 병합 결과에 `bCanEverTick`이 없고, `TickComponent` 선언이 헤더에 없으며(정의만 `.cpp`에 있음), `CreateDefaultSubobject<UCharacterStatComponent>`가 `"CharacterStat"`과 `"CharacterStatComponent"`로 2번 들어가고, `UpdateCharacterStat`/`GetStatInitialValue` 선언이 헤더에 각각 2개이며, `if (!PS) return;`이 이동 설정(`MaxWalkSpeed = 400`)보다 앞에 있다.
- 미커밋 `CharacterStatComponent.cpp` 수정은 주석 정리뿐이라 충돌 구성에 영향이 없다.

**이번에 새로 확인된 것**
- 6-1이 작업 트리에서 해소되었다. `WarriorController.cpp`에 `FClassFinder`(`/Game/UI/WBP_StatusWindow.WBP_StatusWindow_C`)가 활성화되었고 `WBP_StatusWindow.uasset`이 생겼다. 에셋 문자열에서 `StatusWindowWidget` 부모 참조와 C++이 요구하는 `TextBlock_*` 16개 이름이 모두 보인다(문자열 검색일 뿐이므로 에디터에서 부모 클래스와 이름 일치는 따로 확인).
- 이 에셋과 `WBP_PlayerHUD.uasset`은 `main`이 건드리지 않아 충돌이 없다. **단, `WBP_StatusWindow.uasset`은 아직 Git이 추적하지 않는다.** 커밋하지 않으면 병합·푸시에서 빠져 `FClassFinder`가 실패한다.
- 6-1은 이 변경을 반영하면 `[x]`로 닫아도 된다(에디터 확인 후).

**여전히 확인하지 못한 것**: 컴파일 결과, `BP_Warrior`에 저장된 컴포넌트 이름, 에셋의 실제 내용, 컴포넌트 `BeginPlay` 호출 순서의 실측, `main` 작업자의 의도.

---

## 8. main 병합 진행 기록과 결과 (2026-10-15)

**결과 요약**: `origin/main`(`b42a975`)이 `Bae`에 병합되었다(병합 커밋 `95665c7`, 부모 `fbb250e` + `b42a975`). 이후 `Warrior.cpp` 정리 커밋 `62022e8`까지 포함해 작성 시점에 `Bae`와 `origin/Bae`가 같다.
빌드와 PIE(HUD, 스탯창)는 사용자가 직접 확인했다. 저는 빌드와 실행을 하지 않았고 코드 읽기와 `git merge-tree` 시험 병합으로만 점검했다.

### 1) 진행 방식과 이유

- 처음 시험 병합에서 충돌 구간이 서로 다른 함수끼리 뒤섞여 해소가 어려웠다. 병합을 한 번 취소(Abort)하고 **`main`의 의도를 `Bae`에 먼저 손으로 옮긴 뒤** 다시 병합했다.
  옮긴 것: `StatGrowthTable` 로딩(`CharacterStatComponent.h/.cpp`), `Data/StatGrowth.h` include, `UpdateCharacterStats()` 시그니처(인자 제거), PlayerState 저장 데이터를 컴포넌트에 넘기는 연결(`GetSaveData()` 경유).
- 그 결과 충돌 3개(`CharacterStatComponent.cpp/.h`, `Warrior.h`)는 **"Bae 파일 통째로"** 처리해도 `main`의 의미 있는 내용을 잃지 않았다. 병합 후 이 3개 파일이 병합 전 `Bae`(`fbb250e`)와 동일함을 확인했다.
- 병합 방식은 **Create a merge commit**을 사용했다(rebase는 강제 푸시와 반복 충돌, squash는 병합 기록 소실 때문에 제외).

### 2) 병합에서 발견한 함정 (다음 병합 때 같은 확인이 필요)

| 함정 | 설명 |
|---|---|
| 충돌 표시 없이 사라지는 줄 | `main`이 컴포넌트 생성자의 `bCanEverTick = true`와 `TickComponent` 선언을 지웠고, 이 삭제가 자동 병합된다. 충돌을 `Bae`로 풀어도 Tick이 꺼진 채라 스태미나가 회복되지 않는다. 빌드는 통과한다 |
| 충돌 구간 밖의 중복 | `Warrior.cpp`는 충돌 표시가 없었지만 `main`의 `CreateDefaultSubobject`(이름 `"CharacterStat"`)와 `if (!PS) return;` 블록이 `Bae`의 같은 기능 위에 얹혔다. 컴포넌트가 2번 생성되고, 이동 설정이 건너뛰어질 수 있었다 |
| 구간만 `Current`로 체크했을 때 | `Warrior.h`에 `UpdateCharacterStats()` 선언이 2번, `CharacterStatComponent.h`에 `main`의 `private:` 라벨이 들어와 델리게이트와 Getter가 private이 되는 문제가 구간 밖에서 생김. 충돌 파일은 구간이 아니라 **파일 전체**를 한쪽으로 골랐다 |
| 일부만 지웠을 때의 크래시 | `Warrior.cpp`에서 `main`의 PS 블록 중 `if (!PS) return;`만 지우고 `PS->GetSaveData()` 호출이 남았을 때, PlayerState가 없는 AI 적에서 null 역참조 크래시 위험이 있었다. 관련 블록 전체를 지워 해소 |
| Abort 실패 | 파일이 스테이지된 버전과 그 뒤의 수정 두 가지를 동시에 갖는 `MM` 상태에서는 Abort가 거부된다. 해당 파일의 수정을 먼저 Discard 한 뒤 Abort 한다 |
| 자동 커밋 | GitHub Desktop은 충돌을 모두 해소하면 병합 커밋을 자동으로 만들 수 있다. 충돌 표시가 없는 파일(`Warrior.cpp`)의 점검은 **충돌 해소 직후, 커밋 전에** 해야 하므로 시험 병합으로 미리 확인해 두는 것이 유효했다 |

### 3) 섹션 0(병합 전 필수 확인)과 섹션 7의 항목 상태

| 항목 | 상태 | 비고 |
|---|---|---|
| M-3 `Bae` 컴포넌트 기능 유지 | 해결 | `Tick`, 회복, `Use*`, 델리게이트 모두 유지 |
| M-4 `Warrior.cpp/.h` 충돌 해소 | 해결 | 시그니처 `()` 통일, `Dodge` 소모, 인터페이스 유지 |
| M-5 PlayerState가 null일 때 | 해결 (일부 확인 필요) | 이동 설정을 먼저 적용하고 `GetSaveData()`가 null이면 연결만 건너뜀. `BeginPlay` 시점에 PlayerState가 항상 유효한지는 아직 미확인 |
| M-6 컴포넌트 `BeginPlay`의 `BroadcastAllStats()` 유지 | 해결 | 삭제하지 않음 |
| M-7 `Build.cs` | 해당 없음 | `main`은 이 파일을 바꾸지 않았다. `Kang` 병합 때 `NavigationSystem`/`GameplayTasks`와 합칠 것 |
| M-9 병합 직후 확인 | 일부 | 빌드와 PIE에서 HUD/스탯창 동작은 사용자가 확인함. 구르기 후 스태미나 회복, AI `AWarrior`의 동작, 세이브/로드는 미확인 |
| M-1 Attribute 기본값 10 → 0 | **미해결** | 자동 병합으로 `main`의 0이 들어왔다. 스탯창이 Vigor 0으로 보일 수 있다. ahn과 의미 확정 필요 |
| M-2 / 7-6 `UpdateCharacterStat` 책임 | **미해결** | `Bae` 버전(`LoadCharacterStat` + 현재값 복원 + 방송)을 유지했고 `Attributes` 인자는 아직 쓰지 않는다 |
| 7-7 초기화 이중 호출 | **미해결** | 컴포넌트 `BeginPlay`와 `Warrior::BeginPlay` 경로 모두에서 로드된다 |
| M-8 / 7-9 `ZZAKTUNGSOUL_API` 제거 | 미확인 | 자동 병합으로 반영됨. 의도 확인 필요 |
| 7-10 `DT_LevelRequiredSouls.uasset` | 미확인 | `main`만 에셋을 바꿨고 `LevelRequiredSouls.csv`는 그대로다. 둘이 어긋나지 않는지 확인 필요 |
| 7-1 ~ 7-5 (숨은 문제) | 해결 | `bCanEverTick`, `TickComponent` 선언, 컴포넌트 생성 1회, 선언 중복 없음, PS 조기 return 제거 |
| 6-1 스탯창 클래스 연결 | 해결 | `FClassFinder`로 `WBP_StatusWindow` 연결, 사용자 PIE 확인 |

### 4) 아직 남은 일

1. **`Bae → main` PR**: 올리기 전에 문서 3개(`PROJECT_GUIDE.md`, `Review(Claude).md`, `리뷰 참고용.txt`)를 `main`에 포함할지 ahn과 먼저 정한다. 현재는 `Bae`에서 추적 중이므로 PR을 올리면 그대로 포함된다. 합칠 때는 **Create a merge commit**을 쓴다(squash 금지).
2. ahn과 정할 것: Attribute 기본값 0의 의미(M-1), `UpdateCharacterStat`과 성장 계산의 책임(7-6), `ZZAKTUNGSOUL_API` 제거 의도.
3. 이후 개선 과제(리뷰 앞 섹션): 컴포넌트와 인터페이스를 `ACharacterBase`로 이동(3-2), 구르기 비용 25 데이터화(2-4), 스탯창 갱신/현재값 표시(4-7), 피격 경로와 `OnDeath`(4-5), `Kang` 브랜치의 `Build.cs` 합치기와 K-1, K-8.
