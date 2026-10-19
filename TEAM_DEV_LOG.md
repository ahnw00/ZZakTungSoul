# 🧭 ZZakTungSoul 팀 개발 로그

> **기준**: `git log`(2026-10-01 ~ 2026-10-16, 마지막 갱신 2026-10-19)  
> **브랜치와 담당**: `main`(통합, 병합 담당 ahnw00) · `Ahn`(ahnw00) · `Bae`(dino3208) · `Kang`(koo1805)  
> **작성 규칙**: 날짜순으로 **중요한 줄기(기능 추가, 구조 변경, 병합)**만 적는다. 문서(md) 갱신 커밋, 오타·미세 조정, 같은 작업의 중간 저장 커밋은 생략하거나 한 줄로 묶었다. 커밋 번호는 근거 확인용이다.  
> **보는 법**: 브랜치마다 접는 토글이 있고, 그 안에서 **작업 줄기별로 한 번 더** 접을 수 있다.  
> **함께 볼 문서**: 현재 구현 상태와 설계 설명은 `PROJECT_GUIDE.md`, 코드 리뷰와 열린 이슈는 `Review(Claude).md`.

---

## 📌 한눈에 보기

| 날짜 | 사건 |
|---|---|
| 10-01 | 프로젝트 시작(UE 3인칭 템플릿), Kang이 `CharacterBase` 추가 → PR #1로 `main` 병합 |
| 10-02 ~ 10-03 | 전사 캐릭터 에셋과 플레이어 기본 구조(`Warrior`, `WarriorController`, `ZTSGameMode`) |
| 10-06 ~ 10-07 | 스탯 데이터 구조와 PlayerState, 세이브/로드 |
| 10-08 ~ 10-16 | Kang: 적 `EnemyBase`와 AI(Perception, Behavior Tree, Strafe) 구축 |
| 10-12 ~ 10-15 | Bae: 스탯 컴포넌트, HUD, 스탯창, `main` 병합과 정리 |
| 10-13 ~ 10-16 | Ahn: 스탯 컴포넌트 저장/로드 기반, 애니메이션(Layered blend, 이동 블렌드 스페이스, 양손 리타게팅) |
| 10-16 | PR #2(Bae)와 Kang 브랜치가 `main`에 병합됨 → `main`·`Bae`·`Ahn`이 `2fabce4`로 일치 |

**브랜치 현재 위치**: `main` = `Bae` = `Ahn` = `2fabce4`, `Kang`은 `main`을 받아 1커밋 앞선 `0308a1e`.

---

<details>
<summary><b>🌿 main</b> — 통합 브랜치 (병합 담당: ahnw00)</summary>

<details>
<summary>🔀 병합 타임라인</summary>

| 날짜 | 병합/사건 | 내용 |
|---|---|---|
| 10-01 | PR #1 `Kang` → `main` (`9ae504f`) | Kang의 `CharacterBase` 추가분이 첫 병합으로 들어옴 |
| 10-02 | `main` 병합 (`6b8b6fa`) | 초기 세팅(`f10568f`)과 Kang 작업을 합침 |
| 10-13 | `b42a975` | 스탯 컴포넌트 저장/로드 기반 (`Ahn` 토글 참고) |
| 10-16 | PR #2 `Bae` → `main` (`e97fc83`) | Bae의 스탯 컴포넌트, `ICharacterStatInterface`, HUD, 스탯창, 병합 정리가 들어옴 |
| 10-16 | `Merge ... 'main'` (`a50d54e`) | Ahn의 애니메이션 작업(`bc21e43`)과 PR #2를 합침 |
| 10-16 | `merge Kang to main` (`2fabce4`) | Kang의 적 AI 전체가 `main`에 들어옴. 이 시점에 `Bae`, `Ahn`도 `main`과 일치 |

</details>

<details>
<summary>✅ 현재 main의 상태</summary>

- **있는 것**: 플레이어(이동, 락온, 구르기, 달리기, 방어 상태), 세이브/로드, 스탯 컴포넌트와 HUD, 스탯창, 적 AI 골격, 애니메이션 팩 연동
- **아직 없는 것**: 피격/데미지 경로, 사망/리스폰, 적의 스탯/체력, 성장 계산 (`PROJECT_GUIDE.md`의 0장 참고)

</details>

</details>

<details>
<summary><b>🌿 Ahn</b> — ahnw00 (프로젝트 구조, 데이터, 세이브, 애니메이션)</summary>

<details>
<summary>🏗️ 프로젝트 시작과 플레이어 기본 구조 (10-01 ~ 10-03)</summary>

| 날짜 | 커밋 | 내용 |
|---|---|---|
| 10-01 | `053eadc` first commit | UE 3인칭 템플릿 프로젝트로 저장소 시작 |
| 10-02 | `f10568f` initial setting | 전사 캐릭터 에셋(InfinityBlade) 도입, 템플릿 변형(Combat/SideScrolling) 소스 |
| 10-03 | `3582a12` Base work | **플레이어 기본 구조**: `Warrior`, `WarriorController`, `ZTSGameMode`, `WarriorAIController` |

</details>

<details>
<summary>📊 스탯 데이터와 세이브/로드 (10-06 ~ 10-13)</summary>

| 날짜 | 커밋 | 내용 |
|---|---|---|
| 10-06 | `49958cd` Character Stat Settings | **스탯 데이터 구조**: `StatDefine`/`StatGrowth`/`LevelRequiredSouls` 구조체, 상태 enum, `WarriorPlayerState`, DataTable 3종 |
| 10-07 | `bb9e0ac` SaveLoad | **세이브/로드**: `FWarriorSaveData`, `UWarriorPlayerSaveGame`, GameMode 저장/로드, 입력 연결 |
| 10-13 | `b42a975` Actor Component Save/Load | 스탯 컴포넌트가 `DataTable`에서 초기값을 읽도록 변경(정적 키, `GetStatInitialValue`, `UpdateCharacterStat`), `Warrior`가 PlayerState의 `Attributes`를 컴포넌트에 전달, Attribute 기본값을 0으로 변경 |

</details>

<details>
<summary>🎬 애니메이션 (10-14 ~ 10-16)</summary>

| 날짜 | 커밋 | 내용 |
|---|---|---|
| 10-14 | `cc56451` Layered blend per bone | 애니메이션 블루프린트에 본 단위 레이어드 블렌드 적용, `DT_StatDefine` 에셋 수정 |
| 10-15 | `dbf964d` anim pack | 마켓 애니메이션 팩(Frank Slash Pack, 약 957MB) 추가 |
| 10-15 | `4442ac6` Movement Animation | **검과 방패 이동 애니메이션**: 블렌드 스페이스(`BS_IdleWalkRun`), 구르기/백스텝 몽타주, `Warrior`와 `WarriorAnimInstance` 개편(구르기 수동 이동 제거 등) |
| 10-15 | `6e19919` Blend Space (Two Handed) | 양손 무기 블렌드 스페이스와 검방패/양손 리타게팅 애니메이션 |
| 10-16 | `bc21e43` Guard Walk Animations | 양손 방어 걷기 애니메이션 리타게팅 |

</details>

*생략: 병합 커밋.*

</details>

<details>
<summary><b>🌿 Bae</b> — dino3208 (스탯 컴포넌트, HUD, 스탯창)</summary>

<details>
<summary>🖥️ HUD와 스탯 컴포넌트 (10-12 ~ 10-13)</summary>

| 날짜 | 커밋 | 내용 |
|---|---|---|
| 10-12 | `5e493b9`, `843c157` | **HUD 설계**: `PROJECT_GUIDE.md` 작성 시작, UMG HUD 레이아웃(`WBP_PlayerHUD`)과 스탯 변경 델리게이트(`FOnStatChangedDelegate`) 정의 |
| 10-13 | `75908e9`, `deff90e` | **`CharacterStatComponent` 구현과 부착**: HP/FP/Stamina, 델리게이트 방송, 스태미나 자연 회복, `UseStamina`/`UseFP`. `Warrior`가 컴포넌트를 갖고 **구르기가 스태미나를 소모**하도록 연결 |
| 10-13 | `4ba5941` 좌측 상단 스탯 바 구동 완료 | **HUD 완성**: `PlayerHUDWidget`을 C++로 만들어 게이지 3종(HP/FP/Stamina)이 컴포넌트 값에 연동. 컨트롤러가 HUD를 생성 |

</details>

<details>
<summary>🧩 인터페이스 구조와 스탯창 (10-14)</summary>

| 날짜 | 커밋 | 내용 |
|---|---|---|
| 10-14 | `48e7420` 1014 오후 | **인터페이스 구조**: `ICharacterStatInterface` 도입(HUD가 `AWarrior`에 의존하지 않음), `OnPossess` 바인딩, **스탯창(`StatusWindowWidget`)** 과 `C`키 토글, `LoadCharacterStat`로 초기값 로딩 |
| 10-14 | `87eb171`, `2324bab` | 로딩을 `FindRow` 기반으로 정리, HUD 바인딩 안전장치(`RemoveDynamic`, `AddUniqueDynamic`, 약한 참조) |

</details>

<details>
<summary>🔀 main 병합과 정리 (10-15 ~ 10-16)</summary>

| 날짜 | 커밋 | 내용 |
|---|---|---|
| 10-15 | `8805b11` ~ `62022e8` (4개) | **병합 준비**: `Use*` 경계값/음수 검사, 스탯창 클래스 연결(`WBP_StatusWindow`), `main`의 의도(성장 테이블 로딩, `UpdateCharacterStats()` 시그니처, PlayerState 연결)를 먼저 옮김 |
| 10-15 | `95665c7` | **`main`을 `Bae`에 병합**: 충돌 3개 파일은 `Bae` 버전으로 정리하고 `Warrior.cpp`의 중복 코드를 제거. 빌드와 PIE(HUD, 스탯창) 확인 |
| 10-16 | PR #2 | `main`에 병합됨 |

</details>

*생략: md/txt 갱신 커밋(`4d4767b`, `34250e3`).*  
*로컬 진행 중(미커밋, 10-19 기준): DataTable 에셋을 `Content/Data/`로 옮기고 경로를 `/Game/Data/...`로 바꾸는 작업.*

</details>

<details>
<summary><b>🌿 Kang</b> — koo1805 (적 AI)</summary>

<details>
<summary>🧱 CharacterBase와 적 기초 구조 (10-01 ~ 10-12)</summary>

| 날짜 | 커밋 | 내용 |
|---|---|---|
| 10-01 | `0be9b88` | `CharacterBase`(플레이어/적 공용 부모) 추가 → PR #1로 `main` 병합 |
| 10-08 | `50c75f0` | `EnemyBase`가 `CharacterBase`를 상속하도록 뼈대 생성 |
| 10-12 | `642891b` EnemyBase 기초 구축 | **적 기본 구조**: `EnemyBase`, `EnemyAIController`, 적 상태 enum(`EEnemyState`), 테스트 맵 |

</details>

<details>
<summary>🤖 AI 컨트롤러와 Behavior Tree (10-12 ~ 10-13)</summary>

| 날짜 | 커밋 | 내용 |
|---|---|---|
| 10-12 | `3674e82` | **AI 컨트롤러와 Behavior Tree**: Perception 기반 타겟 인식, BT Task 시작, `Build.cs`에 AI 의존성 추가 |
| 10-13 | `5f6d489` | **상태 변경 Task**(`BTTask_SetEnemyState`)와 **`EnemyAIData`(DataAsset)** 로 BT와 이동 속도를 데이터화 |

</details>

<details>
<summary>🏃 Strafe(간보기)와 전투 정보 (10-14 ~ 10-16)</summary>

| 날짜 | 커밋 | 내용 |
|---|---|---|
| 10-14 ~ 10-15 | `289d384`, `3f23959` | **Strafe Task**: `BTTask_SetStrafeMode`, `BT_EnemyBase`/`BB_EnemyBase` 구성, 타겟 유효성 검사 수정 |
| 10-15 ~ 10-16 | `fce1351`, `9f2f400` | **`BTTask_FindStrafeLocation`**(가중치 기반 방향 선택, NavMesh 보정)과 **`BTService_UpdateCombatInfo`**(거리/범위 플래그), `BT_NormalCombat`. *작업 메모: Strafe 모드에서 캐릭터 회전과 컨트롤러 Focus를 일치시켜야 함* |
| 10-16 | `0308a1e` Main Merge | `main`을 `Kang`으로 가져와 동기화(애니메이션 팩 포함). `Kang`의 작업은 `2fabce4`로 `main`에 병합 완료 |

</details>

*생략: 같은 작업의 중간 저장과 사소한 수정.*  
*아직 없는 것: 적의 공격/피격, 적 스탯/체력, `Hit`/`Dead` 상태 진입, 테스트 자산(`DA_Test`, `TestKangMap`) 정리.*

</details>

---

## 🔁 이 로그를 갱신하는 방법

1. 새 **주요 기능 커밋**이 생기면 해당 브랜치 토글의 알맞은 작업 줄기에 한 줄을 추가하고 날짜를 적는다. 새 줄기가 생기면 토글을 하나 더 만든다. md 갱신이나 사소한 수정은 적지 않는다.
2. **병합**은 `main` 토글의 "병합 타임라인"에 한 줄 추가하고, `한눈에 보기`의 "브랜치 현재 위치"를 갱신한다.
3. 내용을 쓰기 전에 `git log --date=short --format='%h | %ad | %an | %s'`와 `git show --stat <커밋>`으로 **실제 변경 파일을 확인**한다. 커밋 메시지만 보고 추정하지 않는다.
4. 구현 상태의 상세는 이 문서가 아니라 `PROJECT_GUIDE.md`의 0장 "현재 구현 상태 요약"을 고친다.
