# Review(Claude)

코드 리뷰 기록 문서. 리뷰 내용만 정리하며, 코드는 직접 수정하지 않는다.
각 항목은 `[ ]`(미반영) / `[x]`(반영 완료)로 진행 상태를 표시한다.

---

## 열린 항목 한눈에 (2026-10-27 기준, 맨 위에서 이것만 보면 된다)

> **2026-10-29 갱신**: 기준 코드는 `Bae` = `origin/Bae` = `8855464`(`main` 병합) + 미커밋 작업 트리(맵 옵션 기반 이어하기, 로딩 화면 폴더 스캔, 마지막 화톳불 이동, 메뉴 열 때 이동 매핑 해제). 이 문서와 `PROJECT_GUIDE.md`는 저장소 밖 `Desktop\깃허브사생호라\`로 옮겨졌다. 섹션 26이 이번 점검이고, 25-1은 해소되었다.
>
> **2026-10-28 갱신**: 기준 코드는 `Bae` = `origin/Bae` = `422c8dc` + 미커밋 작업 트리(자동 저장, `MainMenuWidget.*` 새 파일). 섹션 22~24(10-27)는 이 날 오전·오후 리뷰이고 섹션 25가 10-28 점검이다. `origin/Ahn` = `681830f`, `origin/Kang` = `75d7530`, `origin/main`은 `Bae`에 PR #7 병합 커밋만 앞섬.
>
> **2026-10-27 갱신**: 기준 코드는 `Bae` = `b61f76b` + 미커밋 작업 트리(`BonfireMenuButtonWidget.*` 새 파일, `TeleportWidget.*` 구현, `BonfireMenuWidget` 서브메뉴 임베드 등). 오늘 점검은 **섹션 22**이다. 21-2, 21-3, 21-4, 20-7, 16-8, 16-9가 해소되었고, 다음 병목은 코드가 아니라 **위젯 에셋**(레벨업 창, 텔레포트 창, 텔레포트 항목)이 없는 것(22-1)이다. 락온 null(19-2), 장착 크래시(19-3, 19-4), `ApplyLevelUp` 검증(17-1)은 이번에도 그대로다. 아래 표에서 이미 해소된 항목(위 6개)은 해당 섹션의 `[x]`를 기준으로 본다. 10-27 신규 항목(22-x)은 표의 맨 앞 세 줄이다.

> 이 아래의 번호 섹션(0~9)은 **시간순 기록**이다. 현재 무엇이 남았는지는 이 표를 기준으로 하고, 항목 번호로 해당 섹션의 상세 근거를 찾는다. 항목이 해결되면 이 표에서 지우고 해당 섹션에 `[x]`를 단다(체크 표시는 리뷰어가 코드를 확인한 뒤에만 한다).
> 기준 코드(2026-10-26 갱신): `Bae` = `origin/Bae` = `main` = `origin/main` = `bb4211c`("머지 전 커밋. 화톳불."), `origin/Ahn` = `233d87d`(앞서 있음, 시험 병합 결과 텍스트 충돌 없음). 10-23 통합 머지 후 전체 점검은 **섹션 19**, 10-26 작업(화톳불 마이그레이션 + 레벨업 위젯 본문 + 컴포넌트 정리)은 **섹션 20**, 이후 미커밋으로 추가된 점화 목록 저장·텔레포트 이동은 **섹션 21**이다. 19-1(화톳불 `E`)은 해소되었다. 20-1(현재 소울 표시)과 20-3(`CancelLevelUp`)은 수정이 확인되었다. 20-2(레벨 표 밖 구간)는 보류이고, 남은 높은 항목은 `ApplyLevelUp`의 음수 포인트 검사(17-1)와 Ahn에게 전달할 19-2 ~ 19-4이다. 미커밋 작업 트리(`CharacterStatComponent.*`, `LevelUpWidget.*`, `StatusWindowWidget.cpp`)를 포함해서 읽었다. 빌드/실행은 리뷰어가 하지 않았다.

| 우선순위 | 항목 | 내용 | 상세 |
|---|---|---|---|
| **높음 (10-29 새로 발견)** | 26-1 | 텔레포트 후 화톳불 메인 메뉴가 닫히지 않고 `IMC_Warrior`가 제거된 UI 모드로 남음: `CloseBonfireMenu`가 서브메뉴를 먼저 닫고 반환하는데 `TeleportToBonfire`/`떠나기`도 같은 함수를 부름 → `Esc`용과 전체 닫기용으로 분리 | 섹션 26 |
| 중간 (10-29) | 26-2, 26-3, 26-4 | 에디터에서 인게임 맵을 직접 열면 자동 저장이 기존 세이브를 덮음(`bLoadedThisSession` 가드), 로딩 화면의 선행 로딩이 레벨 전환 때 풀릴 가능성(핸들이 위젯 소유), `TakeStanceDamage`가 `CurrentStamina`로 그로기를 덮고 스태미나 회복을 지연(복사 오류) | 섹션 26 |
| 낮음 (10-29) | 26-5 ~ 26-8 | 마지막 화톳불 이동 반환값 무시, 옵션 문자열 중복, 로딩 위젯 `static` 변수·타임아웃·오타, BOM | 섹션 26 |
| 해소 (10-29) | 25-1, 7-6, 15-2 | 이어하기(맵 옵션 `LoadGame=1` + `GameMode::BeginPlay` 불러오기), 불러온 뒤 스탯 갱신(`LoadSaveDataFrom`), 메뉴 중 이동·공격 입력(`IMC_Warrior` 제거) | 섹션 26 |
| **높음 (10-28 새로 발견)** | 25-1 | 메인 메뉴의 "이어하기"가 세이브를 적용하지 않음(`LoadGame` 호출처가 `IA_Load` 키 하나뿐). 새 자동 저장(화톳불·레벨업)이 기존 세이브를 기본 상태로 덮어씀. 폰 생성 전에 자동으로 불러오게 할 것 | 섹션 25 |
| **높음 (병합 전, 10-28)** | 25-3 | `origin/Ahn` 병합: `WarriorController.cpp` 충돌 1건(양쪽이 파일 끝에 함수 추가, 이어 붙이면 됨), Ahn이 장착 UI를 연결해서 19-3, 19-4(장착 어서션·null)가 이제 실제로 닿음, `origin/Lee`와 `TestMap.umap` 바이너리 충돌, `origin/Kang`은 충돌 없음(`EnemyStatComponent` 미리뷰) | 섹션 25 |
| 중간 (10-28) | 25-2 | 자동 저장 호출 4줄이 세 곳(`BonFire.cpp` 2, `LevelUpWidget.cpp` 1)에 복사됨 → 컨트롤러 `AutoSave()` 한 개로. 메시지 문구 정리 | 섹션 25 |
| 낮음 (10-28) | 25-4 | `MainMenuWidget`: 슬롯 이름이 `GameMode`와 중복, 시작 맵이 개인 시험 맵, 메뉴 입력 모드 확인, `GEngine` 검사 | 섹션 25 |
| 중간 (10-27 오후 점검) | 24-1, 24-2 | 텔레포트 창에 `Button_Cancel`이 없는 것으로 보임, 화톳불 메뉴의 스탯창 임베딩이 없어졌는데 가이드는 계속 있다고 서술 | 섹션 24 |
| 낮음 (10-27 오후 점검) | 24-3 ~ 24-5 | 디버그 키 바인딩이 출시 빌드 가드 밖, 음수 검사 도우미, 텔레포트 목록 22-4 그대로 | 섹션 24 |
| 해소 (10-27 오후) | 22-1, 22-2, 22-3, 22-6, 17-1, 20-4, 19-12, 21-1 | 서브메뉴 초기화·`ShowSubMenu`, 위젯 에셋 3개와 이름 일치, `BonfireMenuButtonWidget.*` 커밋, 음수 포인트 검사, 화톳불 `BonfireID` 둘 배치 | 섹션 24 |
| 결정 기록 (10-27 오후) | 섹션 23 | 오늘 오후 블루프린트 작업 전 결정: 16-4/22-5/15-8 보류, 22-2·22-3·19-5(디버그 소울)·22-6 오늘 작업, 19-14는 Ahn 담당 | 섹션 23 |
| **높음 (확인 필요, 10-27)** | 22-1 | `LevelUpWidget`/`TeleportWidget`/텔레포트 항목 위젯 블루프린트가 `Content/UI/`에 없어 화톳불 메뉴의 버튼이 조용히 무반응(`BindWidgetOptional`) | 섹션 22 |
| 중간 (10-27) | 22-2, 22-3, 22-4, 22-6 | 메뉴 재오픈 시 서브메뉴 상태 유지, 보이기/숨기기 3함수 복사(`ShowSubMenu`), 텔레포트 목록에 현재 화톳불 포함·순서, `BonfireMenuButtonWidget.*` 미추적 | 섹션 22 |
| 낮음 (10-27) | 22-5, 22-7 ~ 22-10 | 버튼 위젯 이름(`TeleportEntry`), `AttackTraceComponent` Tick 시작 상태, 적 상태 이중화, 단순화 후보(`UpdateAIFocus` switch, `MenuWidget` 11줄, `WarriorAnimInstance` 생성자 3블록, 죽은 해시 함수, `LoadGame` 중복 검사), 작은 null 검사 | 섹션 22 |
| 높음 (확인 필요) | 15-1, 15-2 | 화톳불 메뉴 → 스탯창 → 닫기에서 입력 모드/마우스 커서가 어긋남, 메뉴 중 이동·공격 입력이 계속 들어감(PIE 재현 필요) | 섹션 15 |
| 높음 | 17-1, 17-2 | `ApplyLevelUp`: `PendingPoints`에 음수가 들어가도 막지 않아 한 스탯을 깎으면서 총합만 맞추는 입력이 통과함. 레벨 표(`Level1~10`)에 없는 11레벨 이상은 비용 0이라 소울 없이 레벨업됨 (CSV 확장이 선행 조건) | 섹션 17 |
| 중간 | 17-3, 17-4 | `ApplyLevelUp`이 `MaxValue`(99)를 확인하지 않음, `UpgradeStat`이 호출자가 준 비용을 믿는 채로 남아 `ApplyLevelUp`과 두 갈래가 됨 (= 13-3) | 섹션 17, 13 |
| 중간 | 17-6 (13-2 잔여) | 레벨업 시 현재 HP/FP 유지로 바뀜(13-2 해소). 남은 문제: 로드/시작 시 현재 HP가 초기 최대치로 시작해 `MaxHP`보다 낮고, `CurrentHP <= 0`(사망)에서 레벨업하면 부활함 | 섹션 17 |
| 중간 (확인 필요, 통합 머지로 구조가 바뀜) | 9-1 → 19-11 | 공격 상태 고착: 콤보 시스템(Lee)이 병합되어 `PlayAttackMontage()` 경로는 안 쓰이지만 `OnActionMontageEnded`가 `Attack`을 무시해서 노티파이 배치(에셋)에 의존 | 섹션 9, 19 |
| 높음 | 9-2, 9-3 → 19-2 | 락온의 `GetController()` null 역참조(`Warrior.cpp`), `LockOnTarget` 일반 포인터 (자세한 위치와 수정은 19-2) | 섹션 9, 19 |
| 중간 | 9-4, 3-2, 4-5 → 19-6 | 전투 루프: `ApplyDamage` 호출처는 생겼으나(적 → 플레이어, 50 고정) 방어/무적/사망이 없고 플레이어 → 적 경로 없음, 적에게 스탯 컴포넌트/인터페이스 없음, 컴포넌트를 `ACharacterBase`로 이동 | 섹션 9, 3, 19 |
| 중간 | 9-5, 9-6, 9-7 | 이동 속도 출처 4곳, `EnemyAIData` 속도 기본값 없음, AI 경로 2개(`WarriorAIController`/`EnemyAIController`) | 섹션 9 |
| 중간 | 13-3, 13-4, 13-5, 12-3, 7-6, 7-7 | `UpgradeStat`이 호출자가 준 비용을 믿음, 레벨 상한이 우연히 걸림(레벨 표 10 / 성장 표 40 / `MaxValue` 99), 업그레이드 후 UI 미갱신, 비용 0의 의미, `UpdateCharacterStat` 책임/이중 호출 | 섹션 13, 12, 7 |
| 중간 | 16-3, 16-4, 16-6 | 구현 전에 정할 설계: 위젯의 파생값 저장, 스탯 이름 `FName` 3번째 사용(= 13-6 격상, `EAttributeType`, `ApplyLevelUp`이 구조체 필드로 받아 `AddPoint`/`SubPoint`/`UpgradeStat`만 남음), 스탯 텍스트/헬퍼 중복 (16-1, 16-2, 16-5는 10-22 작업 트리에서 해소, 섹션 17) | 섹션 16 |
| 중간 | 15-3, 15-4, 15-5 | UI 모드 전환 코드 4곳 복사(위젯이 `CloseBonfireMenu()`를 안 씀), `CurrentInteractable` 가공되지 않은 포인터, 파일명 대소문자(Git `BonFire.*` ↔ 디스크 `Bonfire.*`) | 섹션 15 |
| 중간 | 16-11 | 가이드 "임베딩한 스탯창이 실시간 자동 갱신" 서술이 코드와 맞지 않음(`RefreshStatus`는 열 때만 호출) | 섹션 16 |
| 중간 | 13-10, 13-12 | `Interact`가 `AWarrior`에 묶임, 적 리셋 구독 구조 | 섹션 13 |
| 중간 | 9-15, 9-17 | CSV ↔ DataTable 에셋 동기화 규칙(에셋 3개는 10-19에 CSV와 같은 `Content/Data/`로 이동됨, 원본 규칙은 미정), 저장소 크기(`Frank_Slash_Pack` 약 957MB, LFS 없음) | 섹션 9, 12 (팀 합의 필요) |
| 중간 | 2-4, 4-7, 5-4 | 구르기 비용 25 하드코딩, 스탯창이 최대값만 표시/열 때만 갱신 | 섹션 2, 4, 5 |
| 낮음 | K-2, K-3, K-4, K-6, K-7, K-9 | 적 AI 정리 항목(비트필드 초기화, `GetPlayerPawn(0)`, 조용한 실패 경로, `Dead` 복구, `bIsCombat`, 테스트 자산) | 섹션 4, 9 |
| 낮음 | 2-8, 3-3, 9-12 | 프로젝트 헤더 include 스타일 혼재(각괄호/따옴표) | 섹션 2, 3, 9 |
| 낮음 | 16-7 ~ 16-10, 16-12 | `LevelUpWidget` 초기화 시점/이름, `TeleportWidget` 구현 전 결정(점화 목록 저장), 서브메뉴 미연결, 헤더 서식, 가이드의 남은 오타·낡은 문구 | 섹션 16 |
| 낮음 | 15-7 ~ 15-11 | 화면 디버그 메시지(`GEngine` null), 필수 버튼이 `BindWidgetOptional`, 필요 소울 0 표시, `E`/`C` 키 `BindKey`, 가이드의 확인 못한 "검증 완료" 문구 | 섹션 15 |
| 중간 | 14-1, 14-2, 14-3, 14-4 | 불필요한 복잡도: 인터페이스→컴포넌트 2단 조회 4곳 복사(헬퍼 1개로), 비율 계산 중복(+ 미사용 getter), `Bonfire`의 불필요한 분기와 `RestoreToFull()`, 실행되지 않는 현재값 분기 | 섹션 14 |
| 낮음 | 14-5 ~ 14-9 | 주석 처리된 코드 28줄, 쓰지 않는 `OverloadedSpeed`/화톳불 getter, 생성자 테이블 로딩 3블록, 인터페이스의 두 책임, `SetNumberText`(= 13-8) | 섹션 14 |
| 낮음 | 13-1, 13-7, 13-8, 13-14 ~ 13-16 | `int32` 함수 안의 `float` 캐스트 잔재, Pawn 없을 때 `true`, 스탯창 캐스트 14회, 사용 불가 피드백, 점화 상태 미저장, 정리 | 섹션 13 |
| 낮음 | 1-6, 2-9~2-11, 11-7, 11-8, 12-6, 12-7, 이름·표기, 9-8~9-11, 9-13, 9-14 | 주석 처리된 코드, 변수명/표기 통일, 낡은 주석, 들여쓰기, `DA_Test` | 섹션 1, 2, 9 |
| 중간 | 18-1 ~ 18-4 | 가이드가 코드보다 앞서 서술됨: 레벨업 "완비"(17-1~17-3 미해결), `LevelUpWidget` 동작을 구현된 것처럼 서술(본문은 전부 빈 함수), 임베딩한 스탯창 "실시간 자동 갱신"(실제로는 열 때만), 에셋에서 확인되지 않는 "검증 완료" 문구 | 섹션 18 |
| 낮음 | 18-5 ~ 18-9 | 코드에 없는 이름 `GetPreviewStats`, 낡은 헤더 문구(브랜치/커밋), 리뷰 번호 직접 인용, `CanInteracBonfire` 오타, "Safe Fallback" 교훈이 17-2 문제를 장점으로 서술, Git 파일명 통일 서술 | 섹션 18 |
| **높음 (보류)** | 20-2 (= 17-1 ~ 17-3 연장) | 위젯 `CanAddPoint`가 비용 행이 없으면(0) 항상 통과해 레벨 10 이후 무한 추가, `MaxValue` 미확인. 레벨 표 밖 구간은 사용자가 나중에 다루기로 함(확정 버튼 조건 `비용 > 0`은 문제 없음으로 정정). `ApplyLevelUp`의 음수 포인트 검사(17-1)는 별개로 남음 | 섹션 20, 17 |
| 중간 | 20-4, 20-5, 20-11 | 위젯을 만들고 여는 곳이 없음(`WBP_LevelUpWidget` 없음), 확정 후 임베드 스탯창이 갱신되지 않음, `origin/Ahn` 병합 시 `Escape` 이중 바인딩과 `SetInventory`가 `BeginPlay`의 폰 없음에 걸릴 가능성 | 섹션 20 |
| **높음 (10-26 점화 목록/텔레포트)** | 21-1, 21-2 | (21-1은 배치 규칙: 인스턴스마다 `BonfireID`를 다르게 적으면 됨, 로그는 선택) `BonfireID`가 전부 `Bonfire_Default`라 점화 목록·텔레포트가 첫 화톳불에만 맞음, 점화 상태가 복원되지 않아(`IsBonfireLit` 호출처 없음) 불러오기 후 점화한 화톳불이 `Unlit`으로 시작해 `E`를 두 번 눌러야 쉴 수 있음 | 섹션 21 |
| 중간 | 21-3, 21-4 | `TeleportToBonfire`가 점화 여부 검증·`TeleportTo` 결과 확인 없음, 착지점이 바닥 높이(캡슐 반높이 필요), 컴포넌트 포인터 `EditAnywhere`(서브오브젝트 이름 오타 `TeleportPOint`는 수정 확인됨) | 섹션 21 |
| 낮음 | 21-5 | 주석·공백 정리, 쓰지 않는 getter, `Category` 이름, 텔레포트 목록 UI 미연결 | 섹션 21 |
| 해소 (10-26) | 20-1, 20-3 | 현재 소울 칸이 `BaseSouls`로 수정됨, `CancelLevelUp`이 리셋만 하도록 수정됨 | 섹션 20 |
| 낮음 | 20-6 ~ 20-10, 20-12, 20-13 | `FName` 분기 중복(도우미 제안), 주 스탯 `= 10.f`, `using namespace` 위치와 불필요한 캐스트, 컴포넌트 `.cpp` 함수 순서, `Warrior.h`의 죽은 `CurrentInteractable`, 저장소 기록에 남은 60MB pptx와 문서 추적 해제, 가이드 불일치 | 섹션 20 |
| 해소 | 19-1, 19-8 | 병합 후 `E`로 화톳불이 동작하지 않던 문제(`0a9a3ac`로 해결), 상호작용 인터페이스 둘 병존 | 섹션 19, 20 |
| 높음 | 19-2 (= 9-2, 9-3 격상) | 락온 후보 `GetController()` null 역참조(`Warrior.cpp:504,543`), `UpdateLockOnCamera`의 `&&` 조건(`:165`). `origin/main`의 `ECC_Pawn` 변경으로 후보가 넓어져 확률 증가 | 섹션 19 |
| 높음 (Ahn 코드) | 19-3, 19-4 | `EquipmentComponent`의 `TMap::operator[]`(처음 장착 시 어서션), `EquipWeapon`이 `InItem`을 두 번 검사하고 `WeaponData`는 검사하지 않음 | 섹션 19 |
| 중간 | 19-5, 19-6 | 소울을 얻는 코드 없음(레벨업 시험 불가), 피격 경로는 생겼으나 50 고정·방어/무적/사망 없음(9-4 일부 해소) | 섹션 19 |
| 중간 | 19-7, 19-8 | UI 체계 셋(`WBP_Menu`/스탯창/화톳불 메뉴)의 소유자·입력 모드 규칙 없음(15-1, 15-3과 연결), 상호작용 인터페이스 둘 병존 정리 | 섹션 19 |
| 중간 | 19-9 ~ 19-12 | 인벤토리가 공유 데이터 애셋에 `InstanceID`를 씀, 충돌 채널 기준(`Pawn` vs 커스텀 `Enemy`/`Player`), 공격 상태 고착이 노티파이 배치(에셋)에 의존(9-1 후속), `WBP_BonfireMenuWidget`에 버튼 이름 없음(임베딩은 확인됨) | 섹션 19 |
| 낮음 | 19-13 ~ 19-19 | `Bonfire` include 대소문자, `IA_Interact` 미매핑 + 이중 바인딩 가능성, `ItemListWidget` `_C` 누락, `AItemObject` 빈 목록 크래시, 작은 정리들, `LevelUpWidget::NativeConstruct`의 `Super::` 누락과 `UIHelpers.h` 미사용, `.git` 임시 팩 | 섹션 19 |
| 중간 | 19-20 | 가이드가 병합 이후 코드와 달라진 곳: 피격 "미구현"(이제 호출처 있음), 장비·인벤토리 "✅ 구현"(장착 크래시), `GetMind()`/`GetIntelligence()`(없는 함수) 등 | 섹션 19 |

**2026-10-22 (후속 점검)**
- 해소: 15-6(`ef6c899`에서 코드와 에셋이 함께 커밋됨). 새 항목은 섹션 16(16-1 ~ 16-12). 새로 추가된 `GetPreviewStats`, `LevelUpWidget`, `TeleportWidget`은 모두 골격 단계(본문이 비어 있음)라서 구현 전에 정할 설계를 중심으로 리뷰했다.

**2026-10-22에 정리된 것**
- 코드 반영 확인: 12-8(`TEAM_DEV_LOG.md` 삭제), 13-9(화톳불 `UPROPERTY`를 `EditAnywhere`로), 13-11(오버랩 바인딩 + `E` 키 + `Warrior::Interact`로 호출 경로 연결), 13-13(`Bonfire` 표기 통일, 단 Git 파일명은 15-5), 13-16 일부. 새 항목은 섹션 15(15-1 ~ 15-11).

**2026-10-19에 정리된 것**
- 가이드 정정: G-10(Q19 오류), G-11(사망/리스폰 과장), G-12(Q2↔Q7), G-13(Q16), G-14(적 AI 반영), G-15(5장 코드 조각), G-17(번호 참조), G-18(병합 교훈)을 `PROJECT_GUIDE.md`에 반영했다. G-16(문서 분리)은 맨 위에 "현재 구현 상태 요약" 표와 맨 끝에 "알려진 이슈/체크리스트"를 추가해 부분 해결했다. 구체적인 변경은 섹션 10 참고.
- 이 문서: R-1(이 표 추가), R-2(존재하지 않는 파일 참조 정리), R-3(Kang 항목 상태 표시)을 처리했다.
- 문서 3개(`PROJECT_GUIDE.md`, `Review(Claude).md`, `리뷰 참고용.txt`)는 Bae PR #2를 통해 이미 `main`에 포함되어 있다.
**2026-10-20에 정리된 것**
- 코드 반영 확인(작업 트리): 12-1(`GetRequiredSouls` 조건 수정), 12-4(스탯창이 `GetVigor()` 등 최종값 사용). `UpgradeStat` 본문이 구현되어 12-5는 설계 문제로 좁혀짐. 새 항목은 섹션 13(13-1 ~ 13-16).
- 기준 코드가 `1d98fd8` + 미커밋 변경(수정 4개, 새 파일 `Interactable/`, `Interface/InteractionInterface.*`)으로 바뀌었다.

**2026-10-19에 정리된 것(이어서)**
- 코드 반영 확인(커밋 `9fc0efe`): 11-1 ~ 11-6(`CalculateGrowthValue` 수정), 9-16(`StatGrowthTable`을 이제 읽음), M-1(`Attributes`는 추가 포인트, 최종 = 초기값 + 추가). 새로 발견한 문제는 섹션 12.

---

## 0. 병합 전 필수 확인 사항 (2026-10-14) — 2026-10-15에 병합 완료, 참고용 기록

`Bae`를 `main`(`b42a975`)에 병합하는 담당자는 아래 항목을 **병합 전에 반드시 확인**한다. 시험 병합(`git merge-tree`) 결과 `CharacterStatComponent.cpp`, `Warrior.cpp`에서 충돌한다. 상세 근거는 섹션 4의 3)과 섹션 5의 2) 참고.

- [x] **M-1. `FWarriorAttributeData` 기본값의 의미를 정한다.** (2026-10-19 확인: `9fc0efe`에서 정해짐 — `Attributes`는 추가 포인트이고 최종 = `StatDefine` 초기값 + 추가. 스탯창 표시는 12-4) `main`은 10 → 0으로 바꾸고 `ZZAKTUNGSOUL_API`를 제거했다.
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
  - Attribute 기본값이 0으로 바뀐 의미는 별도 CSV 확장 계획 메모(저장소에는 없음)의 "레벨의 기준" 쟁점과 직접 연결된다. 0이 "초기값 10에서 올린 포인트 수"라면 `StatGrowth`의 MinLevel/MaxLevel도 0부터 세는 증가 포인트 기준이어야 하고, 스탯창에는 `10 + 값`을 표시해야 한다. 현재 `StatusWindowWidget`은 값을 그대로 표시하므로 Vigor가 0으로 보인다.
- [ ] **4-10. main의 `// Todo` (성장 계산)는 구조 확정 후에 구현.** 예시로 적힌 `MaxHp += Vigor * 10.f`는 고정 곱셈이라 소프트캡 모델과 맞지 않는다. 구간 누적 합산이어야 하며 공식은 별도 CSV 확장 계획 메모(저장소에는 없음)의 성장 표를 기준으로 한다.
- main에서 구조체의 `ZZAKTUNGSOUL_API`를 제거한 것은 같은 모듈 안에서는 문제가 없으나 다른 모듈에서 쓰면 링크 오류가 난다. 의도라면 이유를 남길 것.

### 4) `origin/Kang` 적 AI 리뷰

**총평**: `EnemyBase` → `EnemyAIController` → BT Task의 책임 분리가 명확하고 `EnemyAIData`(DataAsset)로 BT와 이동 속도를 데이터화한 점, 블랙보드 키를 `EnemyAIBlackboardKey.h`로 모은 점이 좋다. 아래는 위험도 순이다.

- [x] **K-1. `EnemyBase::UpdateMovementSpeed`가 `EnemyAIData`를 null 검사 없이 역참조한다.** (`EnemyBase.cpp`) (2026-10-19 확인: `EnemyBase.cpp:39`에 `if (!Movement || !EnemyAIData) return;` 추가됨. 다만 로그가 없어 원인 추적이 어렵다. 9-6 참고)
  `EnemyAIData`는 `EditDefaultsOnly`라 BP에서 지정하지 않으면 `nullptr`이다. `Patrol`/`Chase`/`Strafe` 분기에서 즉시 크래시한다. 초기 상태가 `Idle`이라 `BeginPlay`는 통과하지만 BT가 상태를 바꾸는 순간 크래시한다. 함수 앞에서 null 검사 + `UE_LOG`.
- [ ] **K-2. `bIsCombat`, `bIsStrafeMode` 비트필드가 초기화되지 않는다.** `uint32 bIsCombat : 1;`처럼 선언만 되어 있다. 명시적으로 `= false`를 주거나 `bool`로 선언한다. `BTTask_SetStrafeMode.h`는 `= true`로 초기화하고 있어 일관성도 없다.
- [ ] **K-3. 타겟 판별이 `GetPlayerPawn(0)` 비교이다.** (`OnTargetPerceptionUpdated`) 플레이어 폰이 교체되면 비교 대상이 바뀐다. 태그나 팀(`IGenericTeamAgentInterface`)이 확장에 안전하다. `DetectionByAffiliation`을 적/우호/중립 모두 `true`로 둔 의도도 주석으로 남긴다.
- [ ] **K-4. `OnPossess`의 실패 경로가 조용하다.** `Enemy`/`AIData`/`BehaviorTreeAsset`이 없으면 `return`만 하고 로그가 없다. 적이 서 있을 때 원인을 찾기 어려우므로 `UE_LOG(Warning)` 추가. `BehaviorTreeAsset`을 `AIData->MainBT`로 덮어쓰므로 멤버가 `EditDefaultsOnly`일 의미가 없다.
- [ ] **K-5. `BlackboardAsset`이 사용되지 않는 것으로 보인다.** `FObjectFinder`로 로드하지만 `RunBehaviorTree`는 BT가 가진 블랙보드를 쓴다. 안 쓰면 삭제.
- [ ] **K-6. `SetEnemyState`에서 `Dead`는 되돌릴 수 없다.** 리스폰/풀링이 필요하면 초기화 경로가 필요하다. `Attack`/`Hit` 분기는 속도를 바꾸지 않는다(의도인지 확인).
- [ ] **K-7. `bIsCombat`이 Strafe와 같은 값으로 묶여 있다.** 전투 중이지만 Strafe가 아닌 상태(공격 중 등)를 표현할 수 없다. `EEnemyState::Attack`과의 관계를 정리할 것.
- [x] **K-8. `BTTask_SetStrafeMode`가 해제 때도 `TargetActor`가 없으면 `Failed`를 반환한다.** (2026-10-19 확인: `BTTask_SetStrafeMode.cpp:52-57`의 해제 경로는 타겟 검사 없이 처리됨) Perception이 타겟을 `ClearValue`한 직후에 해제 Task가 실패해서 `bIsStrafeMode`가 `true`로 남을 수 있다. 해제 경로는 타겟 검사 없이 처리한다.
- [ ] **K-9. 테스트 자산 정리.** `Source/Test/DA_Test`, `TestKangMap`, `BP_TestEnemyBase`는 병합 전에 정리 여부를 정한다.
- `Build.cs`는 Bae가 `UMG`/`Slate`/`SlateCore`를, Kang이 `NavigationSystem`/`GameplayTasks`를 추가해 같은 `PublicDependencyModuleNames` 블록을 둘 다 바꿨다. 병합 시 수동으로 합쳐야 한다.
- Stat 연결: `EnemyBase`가 `ACharacterBase`를 상속하므로 컴포넌트를 `CharacterBase`로 올리면(3-2) 적도 같은 HP/데미지 경로를 쓸 수 있다.

### 5) CSV 확장 계획(별도 메모, 저장소에는 없음)과 코드의 접점

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

---

## 9. 프로젝트 전체 + 문서 점검 (2026-10-19)

**기준 상태**: `main`, `Bae`, `origin/Bae`가 모두 `2fabce4`(Bae PR #2 + Kang 병합 포함)로 같다. `origin/Kang`은 `main`을 받아 1커밋 앞선다(`0308a1e`). 로컬에 미커밋 변경은 `PROJECT_GUIDE.md`(Q17~Q20 추가)뿐이다.
**점검 범위**: 병합으로 새로 들어온 코드(적 AI 전체, `Warrior.cpp`/`WarriorAnimInstance` 변경, `Build.cs`), 데이터 파일과 저장소 구성, `PROJECT_GUIDE.md` 전체, 이 문서 자체.
빌드와 실행은 하지 않았다. 아래 "확인 필요"는 코드만으로 단정할 수 없는 항목이다.

### 1) 코드

| # | 우선순위 | 위치 | 내용 |
|---|---|---|---|
| 9-1 | 높음 (확인 필요) | `Warrior.cpp:358` → `WarriorAnimInstance.cpp:130-132` | `Attack()`은 `ActionState`를 `Attack`으로 바꾸고 `PlayAttackMontage()`를 부르는데, 이 함수는 **빈 함수**이다. `AttackMontage`는 생성자(`WarriorAnimInstance.cpp:12-26`)에서 로드되지 않고(`Roll`/`BackStep`만 로드), `StopAttackMontage()`(`:139`)와 `PlayRollMontage()`/`PlayBackStepMontage()`(`:122-128`), `PlayMontage()`(`:85`)도 빈 함수이다. `ActionState`를 되돌리는 경로는 `AnimNotify_ActionEnd`(`:60-69`)와 `OnActionMontageEnded`(`Warrior.cpp:252-263`)뿐이라, ABP가 공격 애니메이션을 따로 재생하며 `ActionEnd` 노티파이를 쏘지 않으면 `ActionState`가 `Attack`에 고착되고 `Move()`(`WarriorController.cpp:169`)와 `Dodge()`가 막힌다. Ahn이 작업 중인 구간일 수 있으니 PIE에서 공격 후 이동되는지 확인 |
| 9-2 | 높음 | `Warrior.cpp:410`, `:449` | `Detected->GetController()->IsPlayerController()`가 null 검사 없이 역참조한다. 컨트롤러가 없는 `ACharacter`(`EnemyBase`가 빙의 전/사망 후 등)가 락온 후보에 잡히면 크래시. `GetController()`가 null인지 먼저 확인 |
| 9-3 | 높음 (적 사망 구현 시) | `Warrior.h:47` | `LockOnTarget`이 `UPROPERTY`/`TWeakObjectPtr`이 아닌 일반 포인터라 대상이 파괴되면 dangling이 된다(`AnimInstance` `:62`도 동일). 락온 중인 적을 `Destroy`하면 `UpdateLockOnCamera`가 크래시한다. `TWeakObjectPtr`와 유효성 검사 필요 |
| 9-4 | 중간 | `EnemyBase.h/.cpp`, 전체 | 적에게 **스탯 컴포넌트와 인터페이스가 없다.** `Hit`/`Dead` 상태(`EnemyStates.h:15-16`)는 있지만 진입하는 곳이 없고(`EEnemyState::Dead`는 `EnemyBase.cpp:70` switch에만 등장), 플레이어 쪽도 `ApplyDamage`/`UseFP`의 호출처가 프로젝트 전체에 없다. 그래서 HP 바는 움직일 수 없고 전투 루프가 비어 있다(4-5, 3-2의 연장) |
| 9-5 | 중간 | `Warrior.cpp:288,306,316` 외 | **이동 속도의 출처가 4곳**이다. `StatDefine.csv`(MaxWalkSpeed 300), `Warrior.cpp`의 400/600 하드코딩, 컴포넌트 `MaxWalkSpeed`(`CharacterStatComponent.cpp:129`에서 읽기만 하고 `GetMaxWalkSpeed()`는 아무도 호출하지 않음), `EnemyAIData`의 속도. 가이드 Q16이 말하는 "데이터 단일 출처"가 이 값에는 적용되지 않는다 |
| 9-6 | 중간 | `EnemyAIData.h:38,42,46` | `PatrolMovementSpeed` 등 `float` 멤버에 기본값이 없다. DataAsset에서 값을 안 채우면 0이라 적이 움직이지 못한다. `EnemyBase.cpp:39`는 `EnemyAIData`가 null이면 **로그 없이** return한다(K-1은 크래시는 막았으나 원인 추적은 어렵다) |
| 9-7 | 중간 | `Warrior.cpp:76` ↔ `EnemyBase.cpp:22` | **AI 적 경로가 두 개**이다. `AWarrior`는 여전히 `AWarriorAIController`를 쓰고(`Warrior.cpp:76-77`), 새 적은 `AEnemyBase` + `AEnemyAIController`이다. 어느 쪽이 정식인지 정하고 레거시는 정리 |
| 9-8 | 낮음 | `EnemyBase.h:62,66` | `bIsCombat`, `bIsStrafeMode` 비트필드가 초기화되지 않았고(K-2), `bIsCombat`은 어디서도 쓰이지 않는다 |
| 9-9 | 낮음 | `EnemyAIController.cpp:68-77,81` | 실패 경로가 로그 없이 return(K-4). `:74`에서 `MainBT`가 null이 아님을 확인한 뒤 `:81`에서 다시 검사해 **도달할 수 없는 검사**이다. `:110`은 `GetPlayerPawn(0)` 비교(K-3) |
| 9-10 | 낮음 | `BTService_UpdateCombatInfo.h:30`, `.cpp:30`, `BTTask_FindStrafeLocation.cpp:29` | `Attackrange`는 `AttackRange`가 관례. `OwnerComp.GetAIOwner()->GetPawn()`에서 `GetAIOwner()` null 검사가 없다 |
| 9-11 | 낮음 | `Warrior.cpp:361-362` | "애니메이션 애셋이 이상해서 위치를 직접 조정" 주석이 낡았다. 해당 `AddActorWorldOffset`(롤)과 `LaunchCharacter`(백스텝)가 제거됨 |
| 9-12 | 낮음 | 전체 | 프로젝트 헤더 include 스타일: 각괄호 `<Enemy/…>` 11, `<Warrior/…>` 7, `<Character/…>` 3(따옴표는 각각 9, 9, 2). `Enemy` 폴더는 전부 각괄호이고 `UI`/`Anim`/`Interface`는 따옴표. 규칙을 문서에 한 줄 정하고 맞출 것(2-8, 3-3) |
| 9-13 | 낮음 | `BTTask_SetEnemyState.cpp` | 들여쓰기가 4칸 공백이고 다른 파일은 탭이다 |
| 9-14 | 낮음 | `Source/ZZakTungSoul/Test/DA_Test.*` | K-9. 테스트 에셋(`DA_Test`, `TestKangMap`, `BP_TestEnemyBase`)을 정리할지 정할 것 |

**이전 항목 중 해소된 것**: K-1(`EnemyBase.cpp:39`에 null 검사), K-8(`BTTask_SetStrafeMode.cpp:52-57` 해제 경로에 타겟 검사 없음). 병합 후에도 `Build.cs`는 두 의존성 목록이 합쳐졌다(`AIModule`/`NavigationSystem`/`GameplayTasks`/`UMG`).
**여전히 열린 이전 항목**: 2-4(구르기 비용 25.f, `Warrior.cpp:371`), 3-2(컴포넌트를 `CharacterBase`로), 4-7(스탯창이 최대값만 표시), 7-6/7-7(`UpdateCharacterStat` 책임, 이중 호출), M-1(Attribute 기본값).
Attribute 표시값: `StatusWindowWidget.cpp:46-51`이 `SaveData->Attributes`를 그대로 출력하므로 기본값 0이 화면에도 0으로 보인다(가이드 Q16의 "최종 = 10 + Attributes"는 코드에 아직 없음).

### 2) 데이터와 저장소

- [ ] **9-15. CSV와 DataTable 에셋이 어긋날 수 있다.** `DT_StatDefine.uasset`은 10-14(`cc56451`)에 바뀌었는데 `StatDefine.csv`는 10-07 이후 변경이 없다. `DT_LevelRequiredSouls.uasset`(10-13)도 CSV는 그대로다. 가이드 Q16은 CSV를 단일 출처라고 쓰지만 CSV를 고쳐도 에셋에 반영되는 절차(재임포트)와 어느 쪽이 원본인지가 정해져 있지 않다.
- [x] **9-16. `StatGrowth.csv`는 임시 값(1~20레벨 10, 21~40레벨 7)이고 코드가 읽지 않는다.** (2026-10-19 확인: `9fc0efe`의 `CalculateGrowthValue`가 `StatGrowthTable`을 읽고 `UpdateCharacterStat`이 8곳에서 호출함. CSV 값은 여전히 임시 값) `StatGrowthTable`은 생성자에서 로드만 되고(`CharacterStatComponent.cpp:28-34`) 사용하는 곳이 없다. 가이드 143행은 CSV의 내용만 설명하고 "아직 미사용"임을 적지 않았다.
- [ ] **9-17. 저장소 크기.** Git 팩이 927MiB이고 `Content/Frank_Slash_Pack` 하나가 957MB이다. `.gitattributes`가 없어 Git LFS를 쓰지 않는다. 클론/푸시가 느려지고 GitHub의 파일당 100MB 제한에 걸릴 수 있다. 마켓플레이스 에셋은 라이선스상 공개 저장소 재배포가 제한되는 경우가 있으니 저장소 공개 범위도 확인할 것. LFS 도입은 이력 재작성이 필요할 수 있어 팀 합의가 먼저다.

### 3) PROJECT_GUIDE.md

| # | 종류 | 위치 | 내용 |
|---|---|---|---|
| G-10 | **오류** | `PROJECT_GUIDE.md:630-632` (Q19) | "정적 상수를 쓰면 오타 발생 시 컴파일 타임에 빌드 에러로 잡아준다"는 **틀렸다.** `HpKey = TEXT("Hp")`처럼 상수의 *값*을 잘못 쓰면 컴파일은 통과하고 런타임에 0과 경고가 나온다. 컴파일러가 잡는 것은 `HpKye`처럼 상수 *이름*을 잘못 쓴 경우뿐이다. 같은 문서 Q4(`:452`)의 주의 문구와도 모순된다. Q19 2번을 Q4와 같은 내용으로 고칠 것 |
| G-11 | 과장 | `:346`, `:357`, Q2(`:428-431`), Q13(`:550-558`), Q20(`:636-643`) | "사망/리스폰 대응 완료", "영속적 성장 파이프라인이 완성"이라고 썼지만 **사망·리스폰 흐름은 구현되어 있지 않다**(`ApplyDamage` 호출처, `OnDeath`, 리스폰 로직 없음. 9-4). 구현된 것은 Pawn 교체에 견디는 HUD 바인딩 구조까지다. "설계상 대응 가능"으로 표현을 낮추고 미구현임을 적을 것 |
| G-12 | 상충 | Q2(`:425-427`) ↔ Q7(`:479-489`) | Q2는 실행 순서가 환경에 따라 달라진다고 쓰고, Q7은 5단계 순서를 확정처럼 쓴다. 로그로 실측한 결과가 없다. "일반적 순서(로그로 검증 필요)"로 표기하거나 실측값을 붙일 것 |
| G-13 | 코드와 불일치 | Q16 3번째 항목(`:593-595`) | "최종 스탯 = CSV 기본값(10) + Attributes … 완벽하게 지원"은 아직 코드에 없다(위 `StatusWindowWidget.cpp:46-51`, `LoadCharacterStat`은 `Attributes`를 쓰지 않음). 설계 방향이라고 표시 |
| G-14 | 낡음 | `:38`, `:117-124`, `:26-63` | 아키텍처 그림과 줄기 5가 적 AI를 `AWarrior`+`WarriorAIController`로 설명한다. 이제 `EnemyBase`, `EnemyAIController`, BT Task/Service, `EnemyAIData`, `Interface/`, `UI/`가 있다. 주요 파일 목록도 갱신 필요 |
| G-15 | 낡음 | `:155-184` (5장) | "개선 완료"의 코드 조각이 `MaxHP = 100.f` 등 가데이터 값을 보여 주는데, 실제 값은 `LoadCharacterStat`이 250/50/60으로 덮어쓴다. 코드를 복사하지 말고 파일 링크로 대체(G-5와 같은 이유) |
| G-16 | 구조 | 전체(670행) | 아키텍처, 작업 일지, 학습 Q&A(20개), 실수 기록이 한 파일에 섞여 있다. 헤더 작성일은 10-12인데 내용은 10-19까지이고, 이 문서는 `main`에 올라가 있어서 틀린 내용(G-10~G-13)이 팀에 공유된다. 최소한 "현재 구현 상태" 요약 표를 맨 위에 두고, Q&A는 별도 문서로 분리하는 것을 권장 |
| G-17 | 의존 | `:657` (Troubleshooting #3) | "6-1 현상"이 이 리뷰 문서의 번호를 가리킨다. 문서끼리 번호로 참조하면 한쪽이 정리될 때 깨진다. 설명을 풀어 쓸 것 |
| G-18 | 보강 | `:666-668` (Troubleshooting #6) | 병합 교훈이 `bCanEverTick` 삭제만 다룬다. 실제로 더 위험했던 것은 **충돌 표시 없이 `Warrior.cpp`에 중복 코드가 얹힌 것**이었다. 병합 직전 `git merge-tree`로 시험 병합해서 충돌 밖의 변화를 미리 보는 방법을 추가하면 가치가 크다(이 문서의 섹션 8 참고) |

잘 된 점: Q4의 FName 키 주의 문구, Q10 표, Q14 표, Q18은 정확하다. Q17은 코드(`ActionPressed`/`Tick`/`ActionReleased`)와 일치한다.

### 4) 이 문서(Review(Claude).md)

- [x] **R-1. 길이와 중복.** (2026-10-19 처리: 맨 위에 "열린 항목 한눈에" 표를 추가하고 섹션 0을 참고용 기록으로 표시함. 섹션 4·5·7의 중복 서술은 기록으로 남김) 583줄이고 섹션 4·5·7이 같은 충돌 내용을 세 번 다룬다. 섹션 0(병합 전 필수 확인)은 병합이 끝나서 더는 해당 없다. **현재 열린 항목만 모은 한 장짜리 목록**을 맨 위에 두고, 나머지는 기록으로 취급하는 구조가 좋다.
- [x] **R-2. 존재하지 않는 참조 파일.** 섹션 4의 3)·5)가 저장소에 없는 별도 메모를 가리키던 부분을 "별도 CSV 확장 계획 메모(저장소에는 없음)"로 고쳤다. (2026-10-19)
- [x] **R-3. Kang 항목 상태 불일치.** (2026-10-19 처리: K-1, K-8에 `[x]` 표시) 섹션 4의 K-1, K-8은 이미 해결되었고(위 1)절) K-2/3/4/9는 현재 코드에서도 유효하다. 해결된 항목에 표시를 달 것.

### 5) 지금 우선순위 제안

1. 9-1 확인(PIE에서 공격 후 이동/구르기). 9-2의 null 검사는 한 줄이라 바로 처리 가능.
2. 가이드 G-10(오류)과 G-11(과장)을 `main`에 올라가기 전에 정정. 이 변경은 아직 미커밋인 Q17~Q20 안에 있다.
3. 9-4(적/플레이어의 피격 경로와 `OnDeath`)를 설계하면서 3-2(컴포넌트를 `CharacterBase`로)를 같이 정한다. 9-3, 9-5, 9-6은 그 설계에 포함시키면 된다.
4. 9-15(CSV와 에셋의 원본 규칙)와 9-17(저장소 크기)은 팀 합의 사항이라 ahn, koo와 정할 것.

---

## 10. 문서 갱신 기록 (2026-10-19, 사용자 요청으로 Claude가 문서만 수정함)

코드는 수정하지 않았다. `PROJECT_GUIDE.md`의 사실 오류와 과장을 섹션 9의 G-10~G-18 지적에 맞춰 고쳤다. 이후 가이드를 읽거나 고치는 AI/팀원이 **무엇이 왜 바뀌었는지** 알 수 있도록 정리한다. (행 번호는 편집으로 계속 바뀌므로 절 이름으로 적는다.)

| 지적 | 가이드의 변경 위치 | 변경 내용 |
|---|---|---|
| G-10 오류 | 12장 Q19 2번 | "정적 키가 오타를 컴파일 타임에 잡는다"를 삭제하고 효과와 한계로 다시 씀. 상수 *이름* 오타만 컴파일 오류이고 *값* 오타는 런타임 `nullptr`/0이라는 점을 명시(Q4 3번과 일치) |
| G-11 과장 | 10장 4단계/1번 항목, Q2 2번, Q13 2번, Q20 1·2번 | "사망/리스폰 대응 완료·완성"을 "Pawn이 교체되어도 견디는 구조까지 완료, 사망/리스폰은 미구현"으로 수정. Q2의 "크래시"는 "갱신 정지"로 정정 |
| G-12 상충 | 12장 Q7 | 실행 순서가 일반적인 흐름이며 이 프로젝트에서 로그로 실측하지 않았다고 명시 |
| G-13 설계와 코드 | 12장 Q16 2번 | "최종 = 10 + Attributes"는 설계 방향이고 현재 코드에는 반영되지 않았다고 명시(스탯창은 원본값 출력, `LoadCharacterStat`은 `Attributes` 미사용) |
| G-14 낡음 | 2장 다이어그램, 3장 줄기 5·6, 4장 표 | `AEnemyBase`/`AEnemyAIController`를 다이어그램에 추가하고 줄기 5를 적 AI 구성(파일, 블랙보드 키, 상태, 한계)으로 다시 씀. 줄기 6에 `Interface/`, `SaveData/`, `UI/` 추가. `StatGrowth.csv`가 코드에서 미사용임과 CSV↔에셋 동기화 규칙 부재를 4장에 명시 |
| G-15 코드 조각 | 5장 | `100.f` 조각이 당시 모습이며 현재는 `LoadCharacterStat`이 덮어쓴다는 정정 문구 추가 |
| G-16 구조 | 0장(신규), 13장(신규), 머리말 | 맨 앞에 "현재 구현 상태 요약" 표(구현/부분/미구현)와 읽는 규칙을 추가하고, 맨 끝에 "알려진 이슈"와 갱신 AI용 체크리스트를 추가. 작성일을 최종 갱신 2026-10-19로 표기. Q&A를 별도 문서로 분리하는 일은 하지 않았다(남은 일) |
| G-17 번호 참조 | 12장 문제 해결 기록 3번 | "6-1 현상"이라는 리뷰 번호 인용을 설명 문장으로 바꿈 |
| G-18 보강 | 문제 해결 기록 7, 8번(신규) | 충돌 표시 없는 중복 코드 사례와 `git merge-tree` 시험 병합 방법, Abort 거부(`MM` 상태) 사례를 추가 |

**아직 하지 않은 것**: 가이드의 5~11장은 작업 일지 성격의 낡은 서술(초기 구현 기록)이 남아 있다. 일부는 위 정정 문구로 덮었고, 전체 재구성은 이후 작업으로 남긴다. 12장의 나머지 Q&A(Q1, Q3~Q6, Q8~Q15, Q17, Q18)는 코드와 대조해 큰 오류가 없음을 확인했다.

---

## 11. `CalculateGrowthValue` 리뷰 (2026-10-19, 작성 당시 미커밋 코드 — 커밋 `9fc0efe`에서 11-1 ~ 11-6이 수정된 것을 확인함, 현재 상태는 섹션 12)

대상: `Source/ZZakTungSoul/Character/CharacterStatComponent.cpp:64-92`, 선언 `CharacterStatComponent.h:33`. 코드는 수정하지 않았다. 빌드/실행은 하지 않았고 CSV(`Content/Data/StatGrowth.csv`)와 손으로 대조해서 확인했다.

### 1) 먼저 고쳐야 할 것 (동작 오류)

- [x] **11-1. 항상 0을 반환한다.** (2026-10-19: `return static_cast<float>(TotalBonus)`로 수정됨, `CharacterStatComponent.cpp:90` 근처) `cpp:91`이 `return 0.0f;`이다. 계산한 `TotalBonus`(`:71`, `:84`)를 버린다. `return TotalBonus;`여야 한다. 이 상태로는 `UpdateCharacterStat`에 연결해도 성장 효과가 전혀 없고, 컴파일 경고도 없어서 눈에 띄지 않는다.
- [x] **11-2. null 검사 대상이 틀렸다.** (2026-10-19: `!StatGrowthTable`로 수정됨) `cpp:66`은 `!StatDefineTable`을 검사하지만 `:73`에서 쓰는 것은 `StatGrowthTable`이다. 에셋 경로가 틀리거나(`DT_StatGrowth`는 `Content/Blueprints/Data`에서 `Content/Data`로 옮겨진 미커밋 변경이 있다) 로드에 실패하면 `StatGrowthTable`만 null이고 `StatDefineTable`은 멀쩡해서 검사를 통과한 뒤 **null 역참조로 크래시**한다. `!StatGrowthTable`로 고칠 것.
- [x] **11-3. `reinterpret_cast<const FStatGrowth*>(It.Value)` (`cpp:75`).** (2026-10-19: `GetAllRows<FStatGrowth>`로 바뀜) 행 타입이 `FStatGrowth`인지 확인하지 않고 바이트 포인터를 그대로 해석한다. 에셋의 Row Structure가 다른 구조체로 잘못 지정되어도 경고 없이 쓰레기 값을 읽는다. `ForeachRow<FStatGrowth>` 또는 `GetAllRows<FStatGrowth>`는 타입을 검사하고 불일치 시 로그를 남긴다. `if (const ... = reinterpret_cast...)`의 null 검사도 `GetRowMap()` 값이 null일 일이 없어서 사실상 의미가 없다.

### 2) 효율성 판단

- 매 호출마다 전체 행을 훑는다(O(행 수)). 지금은 14행, 호출은 스탯 갱신 시 8번 안팎이라 **성능 문제가 아니다.** 프레임마다 부르는 함수도 아니다. 캐시(`TMap<키, TArray<행>>`)는 지금 필요 없다. 테이블이 수백 행이 되거나 매 프레임 호출하게 되면 그때 `BeginPlay`에서 한 번 만들 것.
- 계산 로직 자체(구간별 `Min(Added, Max) - Min + 1`)는 맞다. CSV로 확인: 추가 포인트 25이면 1~20구간 20×10=200, 21~40구간 5×7=35, 합 235.

### 3) 번잡한 부분 (가독성)

- [x] **11-4. 들여쓰기가 5단계다.** (2026-10-19: 조건을 `continue`로 일찍 빠지는 형태로 정리됨) `for` > `if(캐스트)` > `if(ID/Target)` > `if(MinLevel)` > `if(Effective)`. 조건을 거꾸로 써서 `return`(람다) 또는 `continue`로 일찍 빠지면 한 단계로 줄어든다.
- [x] **11-5. 중복 조건.** (2026-10-19: `AddedPoints < MinLevel` 한 조건만 남음) `AddedPoints >= MinLevel`(`:79`)이면 `EffectivePoints > 0`(`:82`)이고 그 반대도 성립한다(`Max >= Min`일 때). 둘 중 하나만 있으면 된다. `FMath::Max(0, ...)`로 한 줄에 합치는 방법도 있다.
- [x] **11-6. 자료형이 섞여 있다.** (2026-10-19: `AddedPoints`가 `int32`로 바뀌고 헤더도 맞춰짐) `AddedPoints`는 `float`인데 비교 대상 `MinLevel`/`MaxLevel`/`Value`는 `int32`이고, 호출 쪽 `FWarriorAttributeData`의 값도 `int32`다(`WarriorSaveData.h:14-29`). 레벨은 정수이므로 `int32`로 받으면 `EffectivePoints`도 `int32`가 되어 `static_cast<float>(...)`(`:84`, 이미 `float`이라 불필요)도 사라진다. 헤더 선언(`h:33`)도 같이 바꿔야 한다.
- [ ] **11-7. 일치하는 행이 없을 때 조용히 0을 준다.** `GetStatInitialValue`는 못 찾으면 경고 로그를 남기는데(`cpp:55-60`) 이 함수는 아니다. `StatID` 값의 오타는 컴파일러가 못 잡는다(가이드 G-10과 같은 이유). 일치하는 행이 하나도 없으면 `UE_LOG(Warning)`을 남기는 편이 좋다. 단 `Strength/AttackPower`처럼 값이 0인 임시 행(`StatGrowth.csv:14-15`)은 일치 행이 있으므로 경고 대상이 아니다.
- [ ] **11-8. 사소한 것.** 위 함수와 사이에 빈 줄이 없고(`cpp:63-64`), 아래 `// 증가값.` 주석(`:93`)이 이 함수가 아니라 `UpdateCharacterStat` 것인지 헷갈린다. 헤더와 cpp의 매개변수 이름이 다르다(`StatID`/`InStatID`). 이 파일은 `GetStatInitialValue`도 같은 방식이라 일관성은 있으니 우선순위는 낮다.

### 4) 제안 형태 (참고용, 적용은 작성자가 판단)

```cpp
float UCharacterStatComponent::CalculateGrowthValue(const FName& InStatID, const FName& InTarget, int32 AddedPoints) const
{
	if (!StatGrowthTable || AddedPoints <= 0)
	{
		return 0.f;
	}

	static const FString ContextString(TEXT("CalculateGrowthValue"));
	int32 TotalBonus = 0;

	StatGrowthTable->ForeachRow<FStatGrowth>(ContextString,
		[&](const FName& /*RowName*/, const FStatGrowth& Row)
		{
			if (Row.StatID != InStatID || Row.Target != InTarget)
			{
				return;
			}
			// 이 구간에 속하는 포인트 수 (구간 밖이면 0).
			const int32 Points = FMath::Max(0, FMath::Min(AddedPoints, Row.MaxLevel) - Row.MinLevel + 1);
			TotalBonus += Points * Row.Value;
		});

	return static_cast<float>(TotalBonus);
}
```

### 5) 연결 시 주의

- `UpdateCharacterStat`의 Todo(`cpp:98-100`)와 아직 연결되지 않았다. 연결할 때 `LoadCharacterStat()`이 `CurrentHP = MaxHP`로 되돌리는 동작(`cpp:153`)과 맞물리므로 (이미 열린 항목 7-6, 7-7) 증가값을 더한 뒤에 현재값을 어떻게 다룰지 같이 정할 것.
- 9-16(`StatGrowthTable` 미사용)은 이 함수가 연결되면 해소된다. 현재는 호출처가 없다. (2026-10-19 갱신: `9fc0efe`에서 `UpdateCharacterStat`에 연결되어 해소됨. 위 "연결 시 주의"의 현재값 문제는 12-2 참고)

---

## 12. 오늘 커밋 리뷰: Attribute 관련 코드 (2026-10-19, `Bae` `9fc0efe` "1019 Attribute 관련 코드 작성")

**대상**: `CharacterStatComponent.h/.cpp`, `WarriorPlayerState.h/.cpp`, DataTable 에셋 3개의 이동(`Content/Blueprints/Data/` → `Content/Data/`). 다른 브랜치(`main`/`Ahn`/`Kang`)는 이 커밋 이후 변화가 없다.
빌드와 실행은 하지 않았고 코드 읽기와 CSV 대조만 했다. 코드는 수정하지 않았다.

**총평**: 성장 계산이 `UpdateCharacterStat`에 연결되어 **`Attributes`가 처음으로 실제 스탯에 반영**된다. 이전 리뷰에서 지적한 `CalculateGrowthValue`의 문제(11-1 ~ 11-6)는 모두 수정되었다. 다만 새로 추가한 `GetRequiredSouls`에 **조건이 반전된 버그**가 있고, 성장을 적용한 뒤 **현재값 복원**이 아직 없다.

### 1) 동작 오류

- [x] **12-1. (높음) `GetRequiredSouls`의 조건이 반전되었다.** (2026-10-20: 현재 코드 `CharacterStatComponent.cpp:196`이 `!LevelRequiredSoulsTable || Level <= 0`으로 수정된 것을 확인함) (`CharacterStatComponent.cpp:197-213`, 조건은 `:199`)
  `if (LevelRequiredSoulsTable || Level <= 0) { return 0; }`로 되어 있다. `!LevelRequiredSoulsTable`이어야 한다.
  - 테이블이 정상 로드된 경우: 조건이 항상 참이라 **언제나 0을 반환**한다(필요 소울이 늘 0).
  - 테이블이 null인 경우(경로 오류, 에셋 이동 직후 등): 조건을 통과해 `:207`의 `LevelRequiredSoulsTable->FindRow(...)`에서 **null 역참조로 크래시**한다.
  아직 호출처가 없어 지금은 드러나지 않지만, `UpgradeStat`을 연결하는 순간 소울 소모가 0이 된다.
- [ ] **12-2. (중간 → 2026-10-20 높음으로 상향: `UpgradeStat`이 `UpdateCharacterStat`을 호출하게 되어 실제 경로가 됨, 13-2 참고) 성장 적용 후 현재값이 최대값보다 낮게 시작한다.** (`CharacterStatComponent.cpp:98-130`)
  `UpdateCharacterStat`이 `:100`에서 `LoadCharacterStat()`을 호출하면 `Current* = Max*(초기값)`으로 되돌려진다. 그 뒤 `:110-120`에서 `Max*`에만 성장치를 더하고, `:121-127`의 복원 분기는 `Current`가 이미 초기 최대값이라 `Clamp`만 한다.
  예: Vigor에 25포인트를 투자한 저장 데이터로 시작하면 `MaxHP`는 250 + 235 = 485이고 `CurrentHP`는 250이라 HUD가 52%로 시작한다. 지금은 `Attributes` 기본값이 0이라 보이지 않는다.
  또 업그레이드할 때마다 이 함수를 다시 부르면 현재 HP가 초기 최대값으로 돌아간다. 계산(`Max*` 갱신)과 현재값 처리(비율 유지 또는 전부 회복)를 분리해서 정할 것(7-6과 같은 주제).
- [ ] **12-3. (중간) 존재하지 않는 레벨의 비용이 0이다.** (2026-10-20 부분 완화: `WarriorPlayerState.cpp:17`이 `RequiredSouls <= 0`이면 거부해서 "공짜 업그레이드"는 막힘. 다만 행이 없는 것과 "최대 레벨"이 구분되지 않고 로그도 없음. 13-3 참고) (`CharacterStatComponent.cpp:207-212`)
  `LevelRequiredSouls.csv`는 `Level1`~`Level10`만 있는데 `StatGrowth.csv`의 성장 구간은 40까지다. 11레벨 이상이거나 행이 없으면 `0`을 반환하므로, 12-1을 고친 뒤에도 "비용 0 = 공짜 업그레이드"가 된다. "비용 없음/최대 레벨"을 구분할 수 있는 값(예: `-1`)이나 `bool` 반환으로 바꾸고, 행이 없을 때는 `UE_LOG(Warning)`을 남길 것(`GetStatInitialValue`처럼).
- [x] **12-4. (중간) 스탯창이 계산 결과를 쓰지 않는다.** (2026-10-20: `StatusWindowWidget.cpp:53-58`이 `StatComp->GetVigor()` 등을 쓰도록 수정됨. 남은 `static_cast<int32>`는 13-8) (`StatusWindowWidget.cpp:46-51`)
  주 스탯 6개를 여전히 `SaveData->Attributes`(추가 포인트, 기본값 0)로 출력한다. 이번에 만든 `GetVigor()` 등(`CharacterStatComponent.h:82-92` 근처)은 호출처가 없다. 이 getter는 `초기값 + 추가`(= 10)를 돌려주므로 스탯창이 `StatComp->GetVigor()`를 쓰면 0 대신 10이 보인다. 또 getter가 `float`을 돌려주는데 값은 정수 개념이라(`Attributes`는 `int32`) `FText::AsNumber`로 표시할 때 형식을 맞출 것.

### 2) 설계

- [ ] **12-5. (설계) `UpgradeStat`은 빈 함수이고, 호출자가 비용을 넘기는 구조이다.** (2026-10-20: 함수 본문은 구현됨 `WarriorPlayerState.cpp:14-45`. 비용을 호출자가 넘기는 구조와 나머지 설계 문제는 13-3 ~ 13-6) (`WarriorPlayerState.cpp:11-16`, `WarriorPlayerState.h:25-26`)
  `bool UpgradeStat(const FName& StatName, int32 RequiredSouls)`는 현재 `return false`뿐이다. 구현하기 전에 정할 것:
  - 비용(`RequiredSouls`)을 **호출자가 넘기면** 잘못된 값을 넣어도 막을 수 없다. 스탯 이름과 현재 레벨만 받아 안쪽에서 비용을 구하는 편이 안전하다.
  - 비용 계산(`GetRequiredSouls`)은 컴포넌트에 있는데 `UpgradeStat`은 영속 데이터를 가진 PlayerState에 있다. PlayerState가 Pawn 소속 컴포넌트를 거치면 Pawn이 없을 때(사망 등) 동작하지 않는다. 비용 테이블 조회를 PlayerState 쪽이나 별도 함수로 옮기는 방안도 있다.
  - 업그레이드 후에는 (1) 소울 차감, (2) `Attributes` 증가와 레벨 증가, (3) 컴포넌트의 최대치 재계산(12-2의 현재값 처리 포함), (4) HUD/스탯창 갱신이 모두 이어져야 한다.

### 3) 정리와 일관성

- [ ] **12-6. 주석과 서식.** `CharacterStatComponent.cpp:43-45`의 `// [UI]` 주석이 스탯 계산 함수 위에 붙어 있다(UI 코드가 아님). `CharacterStatComponent.h:153,161`의 `Attunement= 10.f`, `Dexterity= 10.f`는 공백이 빠졌다. `.cpp:7`의 `#include <Data/LevelRequiredSouls.h>`는 각괄호이다(9-12).
  헤더의 `Vigor = 10.f` 같은 기본값은 `StatDefine.csv`의 10과 중복이므로 "로딩 전 임시값"이라는 주석을 달아 두면 단일 출처 원칙(가이드 Q16)과의 관계가 분명해진다.
- [ ] **12-7. DataTable 에셋 이동 후 참조 확인.** 에셋 3개가 CSV와 같은 `Content/Data/`로 옮겨졌고 생성자의 경로 3곳(`CharacterStatComponent.cpp:12-40`)도 `/Game/Data/...`로 바뀌었다(9-15를 일부 완화). 옛 폴더 `Content/Blueprints/Data/`는 디스크에서 사라졌다. 에디터의 Move가 아니라 파일을 직접 옮겼다면 **다른 블루프린트가 옛 경로를 참조하고 있지 않은지** 에디터에서 확인할 것(소스에서는 컴포넌트 3곳이 전부이다). 에셋과 CSV의 내용이 같은지 확인하는 절차와 원본 규칙은 아직 없다.
- [x] **12-8. 쓸모없다고 정한 `TEAM_DEV_LOG.md`가 이 커밋에 포함되어 푸시되었다.** (2026-10-22: 작업 트리에 파일이 없음을 확인함) 팀 개발일지는 Notion 페이지로 정했으므로 저장소의 `TEAM_DEV_LOG.md`는 제거 대상이다(삭제 커밋이 필요). 푸시된 상태라 다음 PR이 `main`에 들어가기 전에 지울 것.

### 4) 확인된 좋은 점 (검증 결과)

- `CalculateGrowthValue`의 구간 계산이 CSV 모델과 맞다. 예) Vigor 25포인트 → HP `1~20 × 10 + 21~25 × 7 = 200 + 35 = 235`, 0포인트 → 0, Strength/Dexterity의 임시 행(`MinLevel 0, MaxLevel 0, Value 0`)은 결과가 0이라 안전하다. 40을 넘는 포인트는 마지막 구간 상한에서 멈춘다.
- `Attributes`를 "추가 포인트"로 보고 `초기값(StatDefine) + 추가`를 최종 속성으로 쓰는 구조가 가이드 Q16의 설계와 일치한다(M-1 해소).
- 스탯 키를 `Vigor`~`Dexterity`까지 정적 상수로 확장했고, 성장 대응(Endurance→Stamina/Defense, Vitality→EquipLoad/Resistance, Strength·Dexterity→AttackPower)이 CSV의 `StatID`/`Target`과 일치한다.
- `GetStatInitialValue`를 `UpdateCharacterStat` 앞으로 옮겨 읽는 순서가 자연스러워졌다.

### 5) 이번 커밋으로 상태가 바뀐 이전 항목

- 해소: 11-1 ~ 11-6, 9-16, M-1.
- 일부 진전: 7-6(`Attributes`는 이제 사용됨, 현재값 처리는 12-2로 남음), 9-15(에셋이 CSV와 같은 폴더로).
- 그대로: 7-7(`BeginPlay`와 `UpdateCharacterStat`의 이중 로드), 2-4, 3-2, 4-5, 9-1~9-7.
- `PROJECT_GUIDE.md`의 0장 상태 표, 4장 `StatGrowth` 행, Q16, 13장 이슈 표를 위 변화에 맞춰 갱신했다.

### 6) 우선순위

1. 12-1(한 줄)을 고치고, 12-3의 반환 규칙을 정한다. `UpgradeStat`을 만들기 전에 해야 한다.
2. 12-2(현재값 처리)와 12-5(업그레이드 흐름)를 같이 설계한다.
3. 12-4(스탯창이 `GetVigor()` 등을 쓰도록)는 작은 변경이다.
4. 12-8(`TEAM_DEV_LOG.md` 삭제)은 `main` PR 전에 처리한다.

---

## 13. 오늘 작업 리뷰 (2026-10-20, 미커밋 변경 + 새 파일)

**대상**: 수정 4개(`CharacterStatComponent.h/.cpp`, `StatusWindowWidget.cpp`, `WarriorPlayerState.cpp`)와 새 파일 3묶음(`Interactable/Bonfire.h/.cpp`, `Interface/InteractionInterface.h/.cpp`). 기준은 `Bae` `1d98fd8` 위의 작업 트리이고, 새 파일은 아직 추적되지 않는다(`git status`의 `??`).
빌드와 실행은 하지 않았고 코드 읽기만 했다. 코드는 수정하지 않았다.

**총평**: 방향이 좋다. `UpgradeStat`이 소울 검사 → 스탯 증가 → 소울 차감과 레벨 증가 → 컴포넌트 재계산 순서로 구현되어 12-5의 4단계 중 앞 3단계가 이어졌고, 스탯창은 계산된 최종값을 읽게 되었다. 화톳불은 "휴식 신호만 방송하고 적 리셋은 구독자가 맡는" 구조(`OnBonfireRest`)로 나뉘어 있어 의도한 설계와 맞다. 다만 **업그레이드 경로가 열리면서 12-2(현재값 초기화)가 실제로 발생하게 되었다.**

**이번 세션에서 정한 규칙(기록)**: 데이터 읽기와 계산은 `int32`(`GetStatInitialValue`, `CalculateGrowthValue`), 게임플레이에서 쓰는 최종 멤버(`MaxHP`, `AttackPower` 등)는 `float`으로 둔다. `float` 멤버에 `int32`를 넣는 대입은 암묵 변환으로 허용한다.

### 1) 스탯 컴포넌트와 업그레이드

- [ ] **13-1. (낮음) `int32` 함수 안에 `float` 리터럴과 캐스트가 남아 있다.** 선언(`CharacterStatComponent.h:32-33`)과 정의 시그니처는 `int32`로 바뀌었지만 본문은 그대로다: `cpp:50`, `:69`, `:79`의 `return 0.f;`, `:59`의 `return static_cast<float>(Row->InitialValue);`, `:104`의 `return static_cast<float>(TotalBonus);`. 암묵 변환으로 컴파일되고 값도 맞지만(작은 정수), `int32` → `float` → `int32` 왕복이 그대로라 반환 타입을 바꾼 목적(11-6)이 절반만 이루어졌고, 컴파일러가 `float` → `int32` 변환 경고를 낼 수 있다. `return 0;`, `return Row->InitialValue;`, `return TotalBonus;`로 정리할 것.
- [x] **13-2. (높음) (2026-10-22: 작업 트리의 `UpdateCharacterStat`이 `LoadCharacterStat()` 호출을 없애고 `CurrentHP/FP`를 유지하도록 바뀐 것을 확인함. 남은 문제는 17-6) 레벨업할 때마다 현재 HP/FP가 초기 최대치로 돌아간다. (12-2의 실제 경로)** `WarriorPlayerState.cpp:40`의 `UpdateCharacterStat(...)` → `CharacterStatComponent.cpp:109` `LoadCharacterStat()`이 `CurrentHP = MaxHP`(초기값 250, `:181`)로 되돌린 뒤 `:119`에서 `MaxHP`만 키운다. `:130-145`는 `Clamp`만 한다.
  - 예: HP 100 상태에서 Vigor를 올리면 현재 HP가 250으로 *회복*되고(전투 중 레벨업이 회복 수단이 됨), 이미 Vigor가 투자된 상태라면 `MaxHP`가 485여도 현재 HP는 250이라 HUD가 52%로 보인다. `CurrentHP <= 0.f` 분기(`:130`, `:138`)는 `LoadCharacterStat`이 항상 `CurrentHP = MaxHP`로 만들어서 **실행되지 않는 코드**다.
  - 먼저 정책을 정한다: (가) 업그레이드 시 전부 회복(`Current* = Max*`이면 `:130-148`을 세 줄로 줄일 수 있음), (나) 현재값 유지(`LoadCharacterStat` 호출 전에 비율이나 값을 저장했다가 복원). 화톳불에서 쉬면 전부 회복되므로(`Bonfire.cpp:106-108`) (나)가 자연스럽다.
- [ ] **13-3. (중간) `UpgradeStat`이 호출자가 준 비용을 그대로 믿는다.** (`WarriorPlayerState.h:26`, `WarriorPlayerState.cpp:14-20`) `BlueprintCallable`이라 `RequiredSouls`에 1을 넣어 부르면 1소울로 올릴 수 있다. 비용은 `GetRequiredSouls(SaveData.Level)`처럼 안쪽에서 구하는 것이 안전하다(12-5). 그때 `Level`이 "현재 레벨"이므로 `Level1` 행(300)이 *레벨 1에서 2로 올리는 비용*인지 CSV(`LevelRequiredSouls.csv`)와 함께 문서로 정해 둘 것. 비용이 `0`일 때 거부하는 현재 방식(`:17`)은 "행 없음/최대 레벨"과 "비용 0"을 구분하지 못하고 조용히 `false`만 돌려준다(12-3).
- [ ] **13-4. (중간) 레벨 상한이 우연히 걸린다.** `StatDefine`에는 `MaxValue = 99`(`StatDefine.h:25`)가 있지만 어디서도 쓰지 않는다. 지금은 `LevelRequiredSouls.csv`가 `Level10`까지라서 11레벨부터 비용 0으로 거부되어 상한이 생기는 것뿐이고, `StatGrowth.csv`의 구간은 40까지다. 세 숫자(레벨 표 10, 성장 표 40, `MaxValue` 99)가 서로 맞지 않는다. 상한을 명시적으로 검사하고(`Attributes.X + 초기값 >= MaxValue`이면 거부) 세 표의 범위를 맞출 것.
- [ ] **13-5. (중간) 업그레이드 후 화면이 갱신되지 않는다.** `UpdateCharacterStat`은 `BroadcastAllStats()`로 HP/FP/Stamina만 방송한다(`cpp:150`). 스탯창은 열 때 한 번 `RefreshStatus`를 부르는 구조라(4-7, 5-4) 창이 열려 있는 동안 올리면 레벨, 소울, 주 스탯, 전투 스탯이 그대로다. `UpgradeStat`이 `true`를 돌려준 뒤 창을 다시 그리거나(`OnStatsUpdated` 같은 델리게이트) 창 쪽이 구독하게 할 것.
- [ ] **13-6. (낮음) 스탯 이름을 문자열 `if-else`로 6번 비교한다.** (`WarriorPlayerState.cpp:23-29`) `FName == TEXT("Vigor")`는 매번 문자열 비교이고 오타는 컴파일이 아니라 `return false`로 끝난다. 같은 이름이 `CharacterStatComponent.h:187-192`의 `VigorKey` 등에도 있어 이중 관리다. `UENUM EAttributeType` + `switch`가 가장 안전하다(호출부도 같이 바꿔야 함). 6개 규모에서는 현재 형태도 읽을 만하므로 낮은 우선순위.
- [ ] **13-7. (낮음) Pawn이 없으면 재계산 없이 `true`를 돌려준다.** (`WarriorPlayerState.cpp:36-42`) 사망이나 Pawn 교체 중이면 `SaveData`만 바뀌고 `UpdateCharacterStat`은 건너뛴다. 이후 Pawn이 생길 때 `Warrior.cpp`의 `UpdateCharacterStats()`(`Warrior.cpp:84`)가 다시 계산하므로 결과는 맞게 되지만, 호출자가 "반영됨"으로 오해할 수 있어 주석으로 남겨 둘 것.

### 2) UI

- [ ] **13-8. (낮음) 스탯창에 `static_cast<int32>(float getter)`가 14번 반복된다.** (`StatusWindowWidget.cpp:53-70`) **(2026-10-21 보강)** 원인은 헬퍼 `SetNumberText`(`cpp:28-34`)가 `auto Value`를 받는 캡처 없는 람다라서 호출하는 쪽이 타입을 맞춰야 하기 때문이다. 해결은 (1) 람다를 이 파일의 익명 `namespace` 안 일반 함수로 옮기고, (2) 매개변수를 `UTextBlock*`로(현재 `TObjectPtr<UTextBlock>&`는 수정 가능한 참조라 의미가 어긋남), (3) `int32` 버전과 `float` 버전(`FMath::RoundToInt`로 반올림) 두 개로 오버로드하면 호출부 캐스트 14개가 모두 사라진다. `static_cast<int32>`는 버림이라 249.9가 249로 보이는 문제도 같이 해소된다. 큰 `Souls`를 `float`으로 넘기지 않도록 `int32` 오버로드를 따로 둘 것(`float`은 약 1,600만부터 정수를 정확히 못 담음). 주 스탯 6개는 정수 개념이므로 `Vigor` 등 멤버와 getter(`CharacterStatComponent.h:79-89`)를 `int32`로 바꾸면 해당 6줄의 캐스트와 `cpp:112-117`의 암묵 변환이 모두 사라진다(별도 작업, 블루프린트 핀 영향 확인 필요). 나머지(`HP`, `AttackPower` 등)는 위 규칙대로 `float` 유지. 오늘 12-4가 해소되어 스탯창에는 이제 `초기값 + 추가`(= 10 + 투자)가 표시된다.

### 3) `Bonfire`와 `InteractionInterface` (새 파일)

- [x] **13-9. (중간) 에디터에서 값을 바꿀 수 없다.** (2026-10-22: `Bonfire.h:69,73,77`이 `EditAnywhere`로 수정됨. 카테고리는 아직 `Bonfire|Components`이고 `BonfireState`는 `VisibleAnywhere` 그대로이며, 둘 다 동작에는 문제없음) (`Bonfire.h:49-67`) `EnemyCheckRadius`, `BonfireID`, `BonfireDisplayName`이 `VisibleAnywhere`라 배치한 인스턴스마다 값을 못 정한다. 특히 `BonfireID`는 주석이 "레벨 내 겹치지 않는 ID"인데 *읽기 전용*이라 모두 `Bonfire_Default`가 된다(전송과 저장의 키가 겹침). `EditAnywhere`(또는 인스턴스별이면 `EditInstanceOnly`)로 바꾸고, 카테고리도 `Components`가 아니라 `Bonfire|Config`처럼 나눌 것. `BonfireState`도 레벨 디자이너가 초기 상태를 정해야 한다면 마찬가지.
- [ ] **13-10. (중간) 인터페이스가 `AWarrior`에 묶여 있다.** (`InteractionInterface.h:26`) `Interact(class AWarrior*)`라서 NPC, AI, 다른 캐릭터 타입이 상호작용할 수 없고, 공통 헤더가 구체 클래스 하나를 알게 된다. `AActor*`나 `APawn*`를 받고 구현 쪽에서 필요하면 `Cast`하는 편이 확장에 유리하다. 이 인터페이스는 `UFUNCTION`이 아닌 순수 가상 함수라 블루프린트에서 구현할 수 없다(C++ 전용). 의도한 것인지 정할 것. 반환값이 없어서 "실패 이유"도 호출자가 알 수 없다(13-14).
- [x] **13-11. (중간) 아직 어디서도 호출하지 않는다.** (2026-10-22: `Bonfire.cpp:39-40` 오버랩 바인딩, `Warrior::Interact`/`SetCurrentInteractable`, `WarriorController`의 `E` 키 → `Interact()`로 연결됨. 연결하면서 생긴 새 문제는 섹션 15) `Interact(` 호출처가 없고 `InteractionSphere`에 오버랩 바인딩도 없다(`Bonfire.cpp:23-29`는 콜리전 설정만). 플레이어가 화톳불을 쓸 수 없는 상태이므로 `Warrior`/`WarriorController` 쪽 입력과 대상 감지(스피어 `BeginOverlap`/`EndOverlap`으로 현재 상호작용 대상 보관 → 입력 시 `IInteractionInterface::Interact`)를 만들어야 완성된다.
- [ ] **13-12. (중간) 적 리셋의 구독 구조.** `OnBonfireRest`는 화톳불 *인스턴스별* 델리게이트(`Bonfire.h:34-36`)라서 게임모드 같은 월드 단위 관리자가 쓰려면 레벨의 모든 화톳불에 일일이 바인딩해야 한다. 화톳불이 월드 단위 객체(게임모드 또는 `UWorldSubsystem`)의 `HandleBonfireRest()`를 직접 한 번 부르게 하고, 적은 `Resettable` 인터페이스(`ResetToSpawn`)를 구현하는 구조가 더 단순하다. 현재 델리게이트는 블루프린트 연출(사운드, 화면 효과) 용도로 남겨도 된다.
- [x] **13-13. (낮음) 이름 표기가 섞여 있다.** (2026-10-22: 코드는 `ABonfire`, `BonfireDisplayName`, 파일 `Bonfire.h/.cpp`로 통일됨. 단 Git이 기록한 파일명은 아직 `BonFire.*`라서 15-5 참고) 클래스/파일은 `ABonfire`/`Bonfire.h`, 나머지는 `EBonfireState`, `BonfireID`, `GetBonfireDisplayName`인데 변수는 `BonfireDisplayName`(`Bonfire.h:40,64`)이다. `Bonfire`로 통일하는 것이 좋다. 블루프린트나 에셋이 아직 이 클래스를 참조하기 전인 지금이 파일 이름을 바꾸기 가장 싸다.
- [ ] **13-14. (낮음) 사용 불가 시 플레이어에게 아무 신호가 없다.** (`Bonfire.cpp:49,77`) 로그만 남기고 조용히 `return`한다. 나중에 UI 메시지를 띄우려면 `CanInteractBonfire`가 이유(락온 중, 주변 적)를 돌려주는 편이 좋다. 또 적 감지는 `OverlapMultiByChannel(ECC_Pawn)`(`:60-67`)이라 적 캡슐이 `Pawn` 채널에 응답하는 설정에 의존한다. 적이 있는데도 사용이 가능해지는지 에디터에서 확인할 것. 락온 여부를 "전투 중"의 대리 지표로 쓰는 것(`:47`)은 락온 없이 교전할 때 빠지므로 2차 검사(반경)가 실제 방어선이다.
- [ ] **13-15. (낮음) 점화 상태가 저장되지 않는다.** `BonfireState`는 런타임 값이고 `FWarriorSaveData`에 점화한 화톳불 목록이 없다. 다시 시작하면 모두 `Unlit`이다. `BonfireID`가 그 키로 쓰이도록 만든 것이므로(13-9를 먼저 고칠 것) 저장 구조는 이후 작업으로 남긴다.
- [ ] **13-16. (낮음) 정리.** (2026-10-22: 빈 `BeginPlay`는 오버랩 바인딩으로 채워져 해소, `Interactable/`/`InteractionInterface.*`는 커밋됨. 각괄호 `#include`와 가이드 반영은 남음) `Bonfire.cpp:33-37`의 빈 `BeginPlay` 오버라이드는 지워도 된다. `Bonfire.cpp:5-11`, `WarriorPlayerState.cpp:5-6`의 각괄호 `#include`(9-12)는 이번에도 늘었다. `Interactable/`와 `Interface/InteractionInterface.*`가 아직 추적되지 않으므로 커밋 때 포함할 것(`.generated.h`는 제외). `PROJECT_GUIDE.md`의 상태 표에 `UpgradeStat` 구현, 스탯창의 최종값 표시, 화톳불/상호작용 인터페이스가 아직 반영되지 않았다.

### 4) 확인된 좋은 점

- `UpgradeStat`의 검사 순서가 안전하다. 이름이 틀리면 `SaveData`를 건드리기 전에 `return false`(`:29`), 소울 차감은 모든 검사를 통과한 뒤(`:32`)라서 실패 시 부분 변경이 없다.
- `GetRequiredSouls`가 `FindRow`(이름 조회)를 쓰고 `ContextString`을 함수 이름으로 둔 것, `CalculateGrowthValue`가 `GetAllRows`로 타입 검사를 받는 것이 이 파일의 다른 함수와 일관된다.
- 화톳불의 휴식이 `SetCurrentHP/FP/Stamina`(`Bonfire.cpp:106-108`)를 통해 `Clamp`와 UI 방송을 거친다. 멤버를 직접 바꾸지 않았다.
- `ICharacterStatInterface`를 통해 컴포넌트에 접근해서 `Bonfire`가 `AWarrior`의 내부 구조에 의존하지 않는다(`Interact`의 시그니처만 `AWarrior`를 알고 있음, 13-10).

### 5) 이번 변경으로 상태가 바뀐 이전 항목

- 해소: 12-1, 12-4.
- 변경: 12-2(13-2로 상향), 12-3(부분 완화), 12-5(구현은 됨, 설계 문제가 13-3 ~ 13-7로 구체화).
- 그대로: 7-7(`BeginPlay`와 `UpdateCharacterStat`의 이중 로드), 11-7, 11-8, 12-6 ~ 12-8, 9-1 ~ 9-7.

### 6) 우선순위

1. 13-2의 정책을 정하고 `UpdateCharacterStat`의 현재값 처리를 고친다(`UpgradeStat`이 이미 이 경로를 쓴다).
2. 13-3(비용을 안쪽에서 구함)과 13-4(상한 명시)를 같이 정한다.
3. 13-9(`BonfireID` 등 `EditAnywhere`)를 화톳불을 레벨에 배치하기 전에 고친다.
4. 13-11(상호작용 호출 경로)과 13-5(UI 갱신)로 기능을 연결한다.
5. 13-1, 13-8, 13-13, 13-16은 정리 작업으로 묶어서 한 번에 한다.

---

## 14. 불필요한 복잡도 전수검사 (2026-10-21, 사용자 요청)

**범위**: `git blame`으로 확인한 사용자 작성 코드. 전부 직접 작성한 파일은 `CharacterStatComponent.h/.cpp`, `StatusWindowWidget.h/.cpp`, `PlayerHUDWidget.h/.cpp`, `WarriorPlayerState.h/.cpp`, `Interactable/Bonfire.h/.cpp`, `Interface/CharacterStatInterface.*`, `Interface/InteractionInterface.*`이다. `WarriorController`는 UI 부분(`.cpp:18-23, 58-74, 342-390`, `.h:9-11, 97-116`)과 `Warrior.cpp`의 `UpdateCharacterStats` 끝 4줄만 해당하고, 나머지는 `ahnw00` 작성이라 제외했다. `Data/*.h`, `SaveData/*.h`는 구조체 선언뿐이라 복잡도 문제가 없다.
기준은 **"같은 일을 더 짧고 단순하게 할 수 있는가"**이다. 버그는 섹션 11~13에 있고 여기서는 다루지 않는다. 빌드는 하지 않았고 제안 코드는 컴파일을 확인하지 않았다.

**총평**: 구조 자체(인터페이스로 컴포넌트 접근, 델리게이트로 UI 갱신, 데이터 테이블 분리)는 과하지 않고 필요한 만큼이다. 복잡도는 대부분 **같은 모양의 코드를 여러 곳에 복사한 것**과 **쓰지 않는 코드를 남겨 둔 것**에서 나온다.

### 1) 효과가 큰 순서

- [ ] **14-1. (중간) "인터페이스 캐스트 → 컴포넌트 얻기 → null 검사" 2단 중첩이 4곳에 복사되어 있다.**
  `WarriorPlayerState.cpp:36-42`, `Bonfire.cpp:102-110`, `PlayerHUDWidget.cpp:16-19`, `WarriorController.cpp:349-355`(`StatusWindowWidget.cpp:37-50`은 `GetSaveData`도 쓰므로 별도). 매번 `Cast<ICharacterStatInterface>(X)` → `GetCharacterStatComponent()` → `if (Comp)` 순서다. 인터페이스에 정적 헬퍼 하나를 두면 4곳이 모두 한 단계로 줄고, 인터페이스를 거친다는 규칙도 한 곳에만 있게 된다.
  ```cpp
  // CharacterStatInterface.h (ICharacterStatInterface 안)
  static UCharacterStatComponent* FindStatComponent(UObject* Object);

  // CharacterStatInterface.cpp (지금 비어 있음)
  UCharacterStatComponent* ICharacterStatInterface::FindStatComponent(UObject* Object)
  {
  	if (ICharacterStatInterface* StatOwner = Cast<ICharacterStatInterface>(Object))
  	{
  		return StatOwner->GetCharacterStatComponent();
  	}
  	return nullptr;
  }
  ```
  ```cpp
  // 사용 예: WarriorPlayerState.cpp:35-42가 이렇게 줄어든다.
  if (UCharacterStatComponent* StatComp = ICharacterStatInterface::FindStatComponent(GetPawn()))
  {
  	StatComp->UpdateCharacterStat(SaveData.Attributes);
  }
  ```
- [ ] **14-2. (중간) 같은 비율 계산이 두 곳에 있고, 한 곳은 아무도 쓰지 않는다.**
  `CharacterStatComponent.h:94-100`의 `GetHPRatio/GetFPRatio/GetStaminaRatio`는 C++ 호출처가 없다(블루프린트 사용 여부는 에디터에서 확인 필요). 같은 `Max > 0 ? Clamp(Current / Max)` 계산을 `PlayerHUDWidget.cpp:61-77`의 `UpdateProgressBar`가 10줄짜리 `if/else`로 다시 한다. 델리게이트가 `Current`/`Max`를 넘기므로 위젯 쪽 계산이 필요하니, 위젯을 한 줄로 줄이고 컴포넌트의 ratio getter 3개는 BP 사용이 없으면 지울 것.
  ```cpp
  void UPlayerHUDWidget::UpdateProgressBar(UProgressBar* Bar, float Current, float Max)
  {
  	if (Bar)
  	{
  		Bar->SetPercent(Max > 0.f ? FMath::Clamp(Current / Max, 0.f, 1.f) : 0.f);
  	}
  }
  ```
- [ ] **14-3. (중간) `Bonfire`의 불필요한 분기와 중복.**
  - `CanInteractBonfire`(`Bonfire.cpp:60-82`): `bHasOverlap`을 받아 `if (bHasOverlap) { for ... }`로 감쌌는데, 결과 배열이 비면 `for`가 아무것도 하지 않으므로 변수와 `if`가 필요 없다. 반환값을 버리고 바로 순회하면 들여쓰기가 한 단계 줄고, 안쪽 `Cast` + `IsDead` 조건도 한 줄로 합쳐진다.
  - `Interact`(`:86-115`): `if (Unlit) {...; return;}` 뒤의 `if (Lit)`(`:100`)는 상태가 두 개뿐이라 항상 참이다. 이 `if`를 없애면 `Lit` 처리가 한 단계 바깥으로 나온다.
  - HP/FP/Stamina를 각각 `SetCurrent*(Max*)`로 채우는 3줄(`:106-108`)은 컴포넌트 안에 `RestoreToFull()`을 만들어 한 번 호출하게 하면, 화톳불이 풀(pool)이 몇 개인지 알 필요가 없다. 이 함수는 13-2의 정책 (가)(업그레이드 시 전부 회복)에서도 그대로 쓸 수 있다.
  ```cpp
  void ABonfire::Interact(AWarrior* Interactor)
  {
  	if (!CanInteractBonfire(Interactor)) { return; }

  	if (BonfireState == EBonfireState::Unlit)
  	{
  		BonfireState = EBonfireState::Lit;
  		return;
  	}

  	if (UCharacterStatComponent* StatComp = ICharacterStatInterface::FindStatComponent(Interactor))
  	{
  		StatComp->RestoreToFull();   // 새 함수: SetCurrentHP/FP/Stamina(Max*)
  	}
  	OnBonfireRest.Broadcast();
  }
  ```
- [ ] **14-4. (중간) 실행되지 않는 분기와 이중 초기화.** (`CharacterStatComponent.cpp:130-148`) `CurrentHP <= 0.f` 분기는 `LoadCharacterStat`이 항상 `CurrentHP = MaxHP`로 만들기 때문에 실행되지 않고, `else`의 `Clamp`는 값을 바꾸지 못한다(13-2). 정책이 정해지면 이 20줄이 `RestoreToFull()` 한 줄(또는 저장해 둔 값 복원 몇 줄)로 줄어든다. 또 `BeginPlay`(`:160`)와 `UpdateCharacterStat`(`:109`)가 둘 다 `LoadCharacterStat()`을 불러 시작할 때 두 번 실행된다(7-7). 읽는 사람이 "어느 쪽이 진짜 초기화인가"를 따라가야 한다.

### 2) 정리하면 좋은 것

- [ ] **14-5. (낮음) 주석 처리된 코드 덩어리.** `CharacterStatComponent.cpp:285-312`는 `UseStamina`의 옛 버전 두 개가 28줄이다(파일 328줄의 약 9%). 아이디어("다크소울은 스태미나가 음수까지 내려감")는 한 줄 TODO 주석이나 `PROJECT_GUIDE.md`에 남기고 코드는 지울 것. 이력은 Git이 보관한다(1-6).
- [ ] **14-6. (낮음) 쓰지 않는 코드(YAGNI).**
  - `OverloadedSpeed`: 멤버(`h:179`), 키(`h:210`), 로드 대입(`cpp:191`)만 있고 getter와 사용처가 없다. `GetMaxWalkSpeed()`도 호출처가 없다(9-5).
  - `Bonfire`의 `GetBonfireState/ID/DisplayName`(`Bonfire.h:38-40`)과 `BonfireID`, `BonfireDisplayName`은 전송 메뉴가 생기기 전까지 쓰는 곳이 없다. 전송/저장 계획이 있다면 유지해도 되지만 13-9와 함께 그때 만드는 편이 단순하다.
- [ ] **14-7. (낮음, 선택) 생성자의 테이블 로딩 3블록이 같은 모양이다.** (`CharacterStatComponent.cpp:18-40`) 경로만 다른 7줄짜리 블록이 세 번 반복된다. 테이블이 하나 더 늘어날 때 익명 `namespace`의 헬퍼(`UDataTable* FindDataTable(const TCHAR* Path)`, 안에서 `ConstructorHelpers::FObjectFinder`를 쓰고 `Succeeded()`면 `Object`를 돌려줌)로 묶으면 블록당 1줄이 된다. 지금은 3개라 그대로 둬도 읽기 어렵지 않다.
- [ ] **14-8. (낮음, 선택) `ICharacterStatInterface`가 두 가지를 맡는다.** (`CharacterStatInterface.h:29,33`) `GetCharacterStatComponent`(모든 캐릭터)와 `GetSaveData`(플레이어만, 기본 `nullptr`)가 한 인터페이스에 있어 소비자가 `GetSaveData()`의 null을 매번 검사해야 한다. 소비자가 `StatusWindowWidget` 하나뿐인 지금은 문제없다. 적에게 이 인터페이스를 붙일 때(9-4, 3-2) 세이브 데이터 쪽을 `GetOwningPlayer()->GetPlayerState<AWarriorPlayerState>()`로 얻거나 인터페이스를 나눌지 다시 판단할 것.
- [ ] **14-9. (낮음) `StatusWindowWidget`의 `SetNumberText`와 14번 캐스트.** 13-8과 같은 항목이다(오버로드 + 익명 `namespace`).

### 3) 검토했고 문제없는 것 (불필요한 복잡도가 아님)

- `PlayerHUDWidget`의 `UpdateHP/FP/Stamina` 세 개: 동적 델리게이트는 함수마다 바인딩 대상이 필요해서 얇은 래퍼 세 개가 필요하다. `BindStatComponent`의 해제 → 저장 → 연결 순서와 `NativeDestruct`의 해제도 올바른 구조이다.
- `CharacterStatComponent.h`의 getter 18쌍(`UFUNCTION(BlueprintPure)` + `FORCEINLINE`): 단순 반복이지만 UE 관례이고 블루프린트 노출에 필요하다. `StatusWindowWidget.h`의 `BindWidgetOptional` 16쌍도 마찬가지다.
- `CalculateGrowthValue`(수정 후), `GetStatInitialValue`, `GetRequiredSouls`: 길이가 짧고 조기 반환이 있어 흐름이 단순하다.
- `UpgradeStat`의 검사 → 변경 → 차감 순서와 `ToggleStatusWindow`의 열고 닫기(`WarriorController.cpp:358-390`)도 적절하다.

### 4) 우선순위

1. 14-1(헬퍼)과 14-3의 `RestoreToFull()`을 같이 만든다. 호출처 5곳이 한꺼번에 단순해지고 13-2의 정책 결정에도 재료가 된다.
2. 14-2, 14-4는 13-2 정책이 정해지면 같이 정리한다.
3. 14-5, 14-6은 지우기만 하면 되는 작업이라 아무 때나 한 번에 한다(블루프린트에서 ratio getter 사용 여부만 먼저 확인).
4. 14-7, 14-8은 필요가 생길 때까지 미룬다.

---

## 15. 화톳불 연동 리뷰 (2026-10-22, 커밋 `434331e` `f05fe14` `4f7ca23` + 미커밋 작업 트리)

**대상**: `Interactable/Bonfire.*`, `UI/BonfireMenuWidget.*`(새 파일, 미추적), `Warrior/Warrior.*`(`Interact`, `CurrentInteractable`), `Warrior/WarriorController.*`(`E` 키, `OpenBonfireMenu`/`CloseBonfireMenu`), `UI/StatusWindowWidget.*`(`TextBlock_RequiredSouls`), 미커밋 `PROJECT_GUIDE.md`.
빌드와 실행은 하지 않았고 코드 읽기만 했다. 에셋(`WBP_*`, `BP_Bonfire`, `TestMap`)의 내용은 읽을 수 없어서 아래 입력 모드 문제는 **코드 흐름으로 추론한 것이며 PIE에서 확인이 필요하다.** 코드는 수정하지 않았다.

**총평**: 13-9, 13-11, 13-13이 한 번에 해결되었다. 화톳불이 오버랩으로 상호작용 대상을 등록하고(`EndOverlap`의 `GetCurrentInteractable() == this` 검사로 다른 화톳불로 덮어쓴 경우를 지키는 점이 좋다), `E` 키 → 컨트롤러 → `Warrior::Interact()` → 대상의 `Interact`로 이어지는 호출 경로가 인터페이스를 통해 `Warrior`와 `Bonfire`가 서로의 구체 클래스를 몰라도 되게 짜였다. `StatusWindowWidget::RefreshStatus`는 널 검사를 앞으로 모아 중첩이 한 단계 줄었다. 새 문제는 **UI 입력 모드를 여러 위젯이 제각각 바꾸는 부분**에 몰려 있다.

### 1) 동작 오류 (실행 확인 필요)

- [ ] **15-1. (높음) 화톳불 메뉴에서 스탯창을 열었다 닫으면 입력 모드가 어긋날 수 있다.**
  `BonfireMenuWidget.cpp:28-35`의 `OnLevelUpClicked`는 `ToggleStatusWindow()`만 부르고 화톳불 메뉴를 닫지 않는다. 그래서 두 위젯이 동시에 화면에 있다(둘 다 `AddToViewport(10)`). 스탯창을 `C` 키나 `ToggleStatusWindow`로 닫을 때(`WarriorController.cpp`의 닫기 경로) `bShowMouseCursor = false` + `FInputModeGameOnly`로 되돌리는데, 이때 화톳불 메뉴는 화면에 남아 있다. 결과적으로 **메뉴는 보이는데 마우스 커서가 사라지고 게임 입력만 받는 상태**가 된다. 반대로 `떠나기`를 먼저 누르면(`BonfireMenuWidget.cpp:42-51`) 스탯창이 열려 있는데도 게임 입력 모드로 돌아간다.
  해결 방향: 입력 모드를 "열려 있는 UI가 하나라도 있으면 UI 모드, 없으면 게임 모드"로 컨트롤러 한 곳에서 계산한다(15-3의 헬퍼). 또는 `레벨업`을 누르면 화톳불 메뉴를 닫고 스탯창을 열고, 스탯창을 닫으면 화톳불 메뉴로 돌아오게 한다.
- [ ] **15-2. (중간) 메뉴가 열려 있는 동안에도 이동·공격 입력이 들어가고, 행동 중에도 메뉴가 열린다.**
  `FInputModeGameAndUI`(`WarriorController.cpp`의 `OpenBonfireMenu`)는 게임 입력을 막지 않는다. 메뉴를 연 채 걸어서 영역을 벗어나면 메뉴가 남는다(`EndOverlap`은 `CurrentInteractable`만 해제하고 메뉴는 닫지 않음). 또 `AWarrior::Interact()`(`Warrior.cpp`, 새 함수)는 `ActionState`를 확인하지 않아서 구르기나 공격 중에도 `E`로 메뉴가 열린다. 메뉴를 여는 동안 `SetIgnoreMoveInput(true)`(닫을 때 해제)나 `FInputModeUIOnly`를 쓰고, `Interact()` 시작에서 `GetActionState() != EActionState::None`이면 반환할 것.

### 2) 설계와 안전성

- [ ] **15-3. (중간) UI 모드 전환 코드가 4곳에 복사되어 있고, 위젯이 컨트롤러의 닫기 함수를 쓰지 않는다.**
  `ToggleStatusWindow`(열기·닫기 2곳), `OpenBonfireMenu`, `CloseBonfireMenu`가 모두 `bShowMouseCursor`와 `SetInputMode`를 직접 바꾸고, `BonfireMenuWidget::OnLeaveClicked`는 같은 일을 위젯 안에서 다시 한다(`:42-51`). 정작 컨트롤러에 있는 `CloseBonfireMenu()`를 부르지 않는다. 컨트롤러에 `UpdateUIInputMode()` 같은 한 함수를 두고(`StatusWindowWidget`과 `BonfireMenuWidget`이 `IsInViewport()`인지 보고 모드 결정), 열고 닫는 함수는 위젯을 넣고 빼기만 한 뒤 그 함수를 부르게 하면 15-1과 15-2가 같이 정리된다. 위젯의 `떠나기`는 `PC->CloseBonfireMenu()` 한 줄이면 된다.
  ```cpp
  void AWarriorController::UpdateUIInputMode()
  {
  	const bool bAnyUIOpen =
  		(StatusWindowWidget && StatusWindowWidget->IsInViewport()) ||
  		(BonfireMenuWidget && BonfireMenuWidget->IsInViewport());

  	bShowMouseCursor = bAnyUIOpen;
  	if (bAnyUIOpen) { SetInputMode(FInputModeGameAndUI()); }
  	else            { SetInputMode(FInputModeGameOnly()); }
  }
  ```
- [ ] **15-4. (중간) `CurrentInteractable`이 가공되지 않은 인터페이스 포인터이다.** (`Warrior.h`의 `class IInteractionInterface* CurrentInteractable`) `UPROPERTY`가 될 수 없어서 대상 액터가 파괴되거나 서브레벨이 내려갈 때 `EndOverlap`이 오지 않으면 매달린 포인터가 된다. `TWeakInterfacePtr<IInteractionInterface>`(UE 5.1 이상)나 `TWeakObjectPtr<AActor>` + `Cast`로 바꾸면 안전하다. `LockOnTarget`(9-3)과 같은 종류의 문제이다.
- [ ] **15-5. (중간) 파일 이름의 대소문자가 Git과 디스크에서 다르다.** 디스크의 파일은 `Interactable/Bonfire.h/.cpp`이고 코드의 `#include "Interactable/Bonfire.h"`, `#include "Bonfire.generated.h"`도 같지만, Git이 추적하는 이름은 `BonFire.h/.cpp`이다(`git ls-files`로 확인). `core.ignorecase=true`라서 Git이 이름 변경을 인식하지 못하고 `M`으로만 표시한다. 팀원이 받으면 Windows에서는 옛 이름 `BonFire.h`가 그대로 남아 UHT가 만드는 `BonFire.generated.h`와 코드의 `Bonfire.generated.h`가 달라진다(UHT나 대소문자를 구분하는 환경에서 오류가 날 수 있음, 미확인). 이름 변경을 Git에 반영하려면 임시 이름을 거쳐 두 번 `git mv`를 하고(예: `BonFire.h` → `tmp.h` → `Bonfire.h`) `.cpp`도 같게 한 뒤 커밋한다. `main`에 올리기 전에 처리할 것.
- [x] **15-6. (중간) 코드와 에셋이 따로 커밋될 위험.** (2026-10-22: 커밋 `ef6c899`에 `BonfireMenuWidget.*`, `WBP_BonfireMenuWidget.uasset`, `BP_Bonfire.uasset`, `TestMap.umap`이 컨트롤러 변경과 함께 들어간 것을 확인함. `FClassFinder` 실패 시 로그를 남기는 것은 선택으로 남음) `BonfireMenuWidget.*`, `Content/UI/WBP_BonfireMenuWidget.uasset`, `Content/Blueprints/Interactable/`가 아직 추적되지 않았고 `TestMap.umap`은 바이너리 변경이다. 컨트롤러 생성자가 `FClassFinder`로 경로를 찾는데 실패하면 로그 없이 `BonfireMenuWidgetClass`가 비어서 **메뉴가 그냥 열리지 않는다**(`WarriorController.cpp`의 `Succeeded()` else 없음, HUD/스탯창 로딩도 같다). 코드와 에셋을 같은 커밋으로 올리고, `Succeeded()`가 아닐 때 `UE_LOG(Error)`를 남기면 원인을 바로 찾는다. `.umap`은 바이너리라 같은 맵을 두 명이 고치면 병합할 수 없으니 맵 편집 담당을 정해 둘 것.

### 3) 정리

- [ ] **15-7. (낮음) 화면 디버그 메시지와 플레이스홀더.** `Bonfire.cpp:97,112,129,146`, `BonfireMenuWidget.cpp:39`의 `GEngine->AddOnScreenDebugMessage`는 `GEngine`이 없는 환경에서 크래시할 수 있고 출시 빌드에는 남기지 않는 것이 좋다. `UE_LOG(LogTemp, Log, ...)`로 바꾸거나 `#if !UE_BUILD_SHIPPING`으로 감쌀 것. `:146`의 "휴식 + 스탯 업그레이드 창" 문구는 이제 실제 동작(메뉴 열기)과 맞지 않는다.
- [ ] **15-8. (낮음) 필수 버튼이 `BindWidgetOptional`이다.** (`BonfireMenuWidget.h:35-40`) 위젯 블루프린트에서 버튼 이름이 틀리면 에러 없이 버튼이 무반응이 된다. 반드시 있어야 하는 버튼이면 `HUD`처럼 `BindWidget`으로 해서 컴파일 때 잡히게 할 것.
- [ ] **15-9. (낮음) 필요 소울이 0으로 표시될 수 있다.** (`StatusWindowWidget.cpp:51`) `GetRequiredSouls`가 행이 없을 때 `0`을 돌려주므로(12-3) 최대 레벨에서 "필요 소울: 0"으로 보인다. `-`나 `MAX`를 보여 주려면 반환 규칙(13-3)부터 정해야 한다.
- [ ] **15-10. (낮음) `E`·`C` 키가 `BindKey`로 직접 묶여 있다.** (`WarriorController.cpp`의 `SetupInputComponent`) 나머지 입력은 모두 Enhanced Input(`IA_*`)인데 UI와 상호작용 키만 방식이 달라서 재배치나 게임패드 지원이 안 된다. `IA_Interact`, `IA_StatusWindow`를 만들어 같은 방식으로 묶으면 일관된다.
- [ ] **15-11. (낮음) 미커밋 `PROJECT_GUIDE.md`의 새 문구 중 리뷰어가 확인할 수 없는 것.** (2026-10-22 후속: 이 문구는 `ef6c899`로 커밋됨. `CanInteractBonfire` 오타는 한 곳(`:190`)만 고쳐졌고 나머지와 헤더의 브랜치 문구는 그대로. 새로 확인한 가이드 문제는 16-11, 16-12) "플레이어 E키 입력 및 오버랩 바인딩 연동"은 코드로 확인했다. "`BP_Bonfire` 레벨 배치 및 인게임 작동 검증 완료", "`WBP_StatusWindow` 임베딩 검증 완료"는 에셋을 읽을 수 없고 실행도 안 해서 확인하지 못했다. 또 15-1, 13-2, 13-3이 열려 있는 상태에서 "✅ 구현 (완료)"로 쓰면 독자가 오해하므로 "알려진 이슈" 표와 같이 갱신할 것. 헤더의 "브랜치 `Bae` (`main`과 같은 커밋 `2fabce4`에서 갱신)"는 틀렸다(`Bae`는 `4f7ca23`이고 `main`보다 앞섬). 본문의 `CanInteracBonfire`는 `CanInteractBonfire` 오타이다. `UpgradeStat`의 C++ 호출처는 없다(블루프린트의 호출 여부는 에셋을 읽지 못해 확인 못함).

### 4) 확인된 좋은 점

- `Bonfire.cpp:107`의 `GetCurrentInteractable() == this` 검사: 겹친 영역에서 나갈 때 다른 화톳불이 등록한 대상을 지우지 않는다.
- `Interactor->GetController<AWarriorController>()`(`:149`)처럼 템플릿 접근으로 한 줄에 캐스트와 널 검사를 처리했다.
- `StatusWindowWidget::RefreshStatus`(`:39-45`)가 `SaveData`와 `StatComp`를 한 번에 검사하고 일찍 반환해서 이전의 중첩 `if` 3단이 한 단계로 줄었다.
- 화톳불이 `AWarrior`가 아닌 `IInteractionInterface`로 불리는 구조라 나중에 NPC나 문 같은 다른 상호작용 대상을 `Warrior`를 고치지 않고 추가할 수 있다.

### 5) 이번 변경으로 상태가 바뀐 이전 항목

- 해소: 12-8, 13-9, 13-11, 13-13. 일부 해소: 13-16.
- 그대로: 13-1 ~ 13-8, 13-10, 13-12, 13-14, 13-15, 14-1 ~ 14-9. `CharacterStatComponent.*`와 `WarriorPlayerState.*`는 10-20 이후 변경이 없다.
- 14-1(인터페이스 → 컴포넌트 조회 복사)은 `Bonfire.cpp:136-144`에 그대로 있다. 14-3의 `bHasOverlap` 분기, `if (Lit)`, 3줄 회복도 그대로이다.

### 6) 우선순위

1. 15-1, 15-2(입력 모드)를 PIE에서 재현해 보고 15-3의 `UpdateUIInputMode()` 하나로 정리한다.
2. 13-2(레벨업 시 현재 HP/FP 초기화)의 정책을 정한다. 화톳불 메뉴로 레벨업이 가능해졌으므로 이제 플레이어가 바로 겪는 문제이다.
3. `main`에 올리기 전에 15-5(파일명 대소문자), 15-6(코드와 에셋을 같이 커밋)을 처리한다.
4. 15-4(약참조), 15-7 ~ 15-10은 정리 작업으로 묶는다.

---

## 16. 레벨업 시뮬레이션과 새 위젯 골격 리뷰 (2026-10-22, 커밋 `ef6c899` 이후 미커밋 작업)

**대상**: `CharacterStatComponent.h/.cpp`의 `GetPreviewStats`(본문 비어 있음), `UI/LevelUpWidget.h/.cpp`(선언만 있고 모든 함수 본문이 비어 있음), `UI/TeleportWidget.h/.cpp`(빈 클래스). 모두 **골격 단계**라서 동작 오류가 아니라 구현 전에 정할 설계를 중심으로 본다. 커밋 `ef6c899`는 앞 섹션에서 본 `BonfireMenuWidget`, `WarriorController`, `Bonfire` 변경을 그대로 담고 있어서 추가 코드 변화는 없었다.
빌드와 실행은 하지 않았다. 코드는 수정하지 않았다.

**총평**: 직전 논의의 결론(시뮬레이션 계산은 컴포넌트에, 임시 선택 상태는 위젯에)이 구조에 반영되어 있다. `PendingPoints`를 위젯이 갖고 `GetPreviewStats`를 컴포넌트에 둔 것이 그렇다. 구현에 들어가기 전에 정할 것은 **(1) 확정(Commit)을 한 번에 처리하는 방법, (2) 출력 값 7개를 넘기는 방식, (3) 공식을 `UpdateCharacterStat`과 공유하는 방법**이다.

### 1) 컴포넌트: `GetPreviewStats`

- [x] **16-1. (중간) (2026-10-22: 작업 트리에서 `FStatPreview` 구조체 반환 `CalculateDerivedStats`로 바뀐 것을 확인함. 이름은 `GetPreviewStats`에서 변경, `Poise`와 이동 속도는 구조체에 없음) 출력 매개변수가 7개이다.** (`CharacterStatComponent.h:84-86`, `.cpp:214-223`) `float& OutMaxHP, OutMaxFP, OutMaxStamina, OutAttackPower, OutDefense, OutEquipLoad, OutResistance`는 스탯이 하나 늘 때마다 선언, 정의, C++ 호출부, 블루프린트 노드가 모두 바뀐다. 호출하는 쪽도 지역 변수 7개가 필요하다. 결과를 담는 구조체 하나(`FStatPreview`)를 반환하면 필드를 추가해도 호출부는 바뀌지 않고, 블루프린트에서도 `Break` 노드로 쓸 수 있다. 본문이 비어 있어 출력이 초기화되지 않으므로, 구현 전에 호출하면 쓰레기 값이 나온다는 점도 알아 둘 것(임시로 0 대입).
  ```cpp
  USTRUCT(BlueprintType)
  struct FStatPreview
  {
  	GENERATED_BODY()
  	UPROPERTY(BlueprintReadOnly) float MaxHP = 0.f;
  	UPROPERTY(BlueprintReadOnly) float MaxFP = 0.f;
  	UPROPERTY(BlueprintReadOnly) float MaxStamina = 0.f;
  	UPROPERTY(BlueprintReadOnly) float AttackPower = 0.f;
  	UPROPERTY(BlueprintReadOnly) float Defense = 0.f;
  	UPROPERTY(BlueprintReadOnly) float EquipLoad = 0.f;
  	UPROPERTY(BlueprintReadOnly) float Resistance = 0.f;
  };
  // UFUNCTION(BlueprintPure) FStatPreview GetPreviewStats(const FWarriorAttributeData& InAttributes) const;
  ```
- [x] **16-2. (중간) (2026-10-22: `UpdateCharacterStat`이 `CalculateDerivedStats` 결과를 대입하도록 바뀌어 공식이 한 곳에 있음을 확인함) 계산식이 `UpdateCharacterStat`(`cpp:112-127`)과 두 벌이 될 수 있다.** 시뮬레이션 숫자와 실제 적용 결과가 어긋나는 것이 이 기능의 가장 큰 위험이다. `GetPreviewStats`를 먼저 만들고 `UpdateCharacterStat`이 그 결과를 멤버에 대입하게 하면 공식이 한 곳에만 남는다. 이렇게 하면 계산(`GetPreviewStats`)과 현재값 처리(`UpdateCharacterStat`)가 갈라져 13-2, 7-6도 같이 풀린다. `InAttributes`는 `SaveData.Attributes`와 같은 **추가 포인트** 의미(기본값 10은 포함하지 않음)이므로, 위젯은 `BaseAttributes + PendingPoints`를 넘긴다는 점을 선언 주석에 적어 둘 것.

### 2) `LevelUpWidget`

- [ ] **16-3. (중간) 파생값을 멤버로 저장하고 있다.** (`LevelUpWidget.h:61-68`) `BaseLevel`, `BaseSouls`, `BaseAttributes`, `VirtualLevel`, `TotalRequiredSouls`, `PendingPoints` 6개 중 진짜 변하는 상태는 `PendingPoints` 하나이다. `VirtualLevel`은 `BaseLevel + (PendingPoints의 합)`이고 `TotalRequiredSouls`는 `BaseLevel`부터 `VirtualLevel` 직전까지 `GetRequiredSouls`의 합이다. 저장해 두면 `AddPoint`/`SubPoint`/초기화 어디선가 갱신을 빼먹는 순간 화면이 틀린다. `RefreshUI`에서 `PendingPoints`로부터 계산하면 필드가 줄고 불일치가 생길 수 없다.
- [ ] **16-4. (중간) 스탯 이름을 `FName`으로 받는 곳이 세 번째가 된다.** (`AddPoint(FName)`, `SubPoint(FName)`, 이미 있는 `UpgradeStat(FName, ...)`, 13-6) 이 위젯에서 이름마다 `PendingPoints.Vigor` 같은 멤버로 분기하면 `if-else`가 하나 더 생긴다. `UENUM EAttributeType`을 하나 만들면 세 곳이 같은 타입을 쓰고, 오타가 컴파일 오류가 되며, 위젯 블루프린트의 버튼 이벤트에서 드롭다운으로 고를 수 있다. 13-6을 낮음에서 중간으로 올린다.
- [x] **16-5. (중간) (2026-10-22: `AWarriorPlayerState::ApplyLevelUp`과 `GetTotalRequiredSouls`가 추가된 것을 확인함. 다만 검증이 빠진 곳이 있어 17-1 ~ 17-3. 위젯의 `CommitLevelUp` 본문은 아직 비어 있음) `CommitLevelUp`을 `UpgradeStat`의 반복 호출로 만들면 안 된다.** `UpgradeStat`은 1포인트 단위(검사 → 증가 → 차감 → `UpdateCharacterStat`)이다. 포인트 N개를 N번 부르면 (1) 매번 `UpdateCharacterStat` → `LoadCharacterStat`이 현재 HP/FP를 초기화하고(13-2), (2) 중간에 소울이 모자라면 일부만 적용된 상태로 끝나며, (3) 레벨이 올라갈 때마다 비용이 바뀌는 계산이 호출마다 퍼진다. `AWarriorPlayerState`에 **일괄 함수**(예: `bool ApplyLevelUp(const FWarriorAttributeData& Pending)`)를 하나 두어 총 비용을 한 번에 검증하고, 전부 차감·증가한 뒤 `UpdateCharacterStat`을 한 번만 부르게 할 것. 총 비용 계산 `GetTotalRequiredSouls(FromLevel, ToLevel)`은 컴포넌트의 `GetRequiredSouls` 옆에 두고, 위젯(표시용)과 PlayerState(검증용)가 같이 쓰게 한다. 위젯이 계산한 `TotalRequiredSouls`는 믿지 말고 PlayerState가 다시 계산해야 한다(13-3).
- [ ] **16-6. (중간) 스탯 텍스트 목록과 숫자 헬퍼가 중복된다.** (`LevelUpWidget.h:71-111`) `TextBlock_*` 20개가 `StatusWindowWidget.h`의 목록과 이름·용도가 거의 같고, 값을 넣는 `SetNumberText` 헬퍼도 세 번째로 필요해진다. 헬퍼는 13-8에서 제안한 오버로드 두 개를 `UI/` 공용 헤더의 `inline` 함수(또는 `UBlueprintFunctionLibrary`)로 한 번만 두고, 가능하면 "스탯 이름 + 값" 한 줄을 서브 위젯으로 만들어 스탯창과 레벨업 창이 같이 쓰게 하면 코드와 `UPROPERTY` 선언이 크게 줄어든다.
- [ ] **16-7. (낮음) 초기화 시점과 이름.** (`LevelUpWidget.h:27-33`) 버튼 바인딩은 한 번만 하는 `NativeOnInitialized`에, 열릴 때마다 필요한 `PendingPoints` 초기화는 `InitializedLevelUp()`에 두고 호출 위치를 정해야 한다. 화톳불 메뉴 안에 임베드하고 `WidgetSwitcher`로 페이지를 전환하면 `NativeConstruct`가 다시 불리지 않으므로 전환할 때 부모가 `InitializedLevelUp()`을 직접 불러야 한다. 이름은 `InitializeLevelUp`이 자연스럽고, `CanAddPoint()`는 `BlueprintPure`로 하면 `+` 버튼의 활성/비활성에 쓸 수 있다. `h:56`의 "캐싱된 컴포넌트 가져오기" 주석과 달리 캐시 멤버는 없다(`UCharacterStatComponent`는 `ICharacterStatInterface` 헬퍼(14-1)로 얻으면 됨).

### 3) 나머지

- [x] **16-8. (낮음) `TeleportWidget` 구현 전에 정할 것.** (2026-10-27: 점화 목록 저장 `LitBonfireIDs`, `TActorIterator`로 화톳불 찾기, 같은 레벨 안 이동으로 정해졌고 `TeleportWidget.cpp`가 구현됨) 빈 클래스(`TeleportWidget.h:12-17`)는 지금은 문제없다. 구현하려면 (1) 점화한 화톳불 목록을 어디에 저장하는지(`FWarriorSaveData`에는 아직 없음, 13-15), (2) `BonfireID`로 월드의 화톳불 위치를 찾는 방법, (3) 같은 레벨 안 이동인지 레벨 전환인지를 먼저 정해야 한다.
- [x] **16-9. (낮음) 아직 서브메뉴가 연결되지 않았다.** (2026-10-27: `BonfireMenuWidget.cpp:43-70`이 임베드한 `LevelUpWidget`/`TeleportWidget`을 보이고 숨기도록 바뀜. 위젯 에셋이 아직 없는 것은 22-1) `BonfireMenuWidget::OnLevelUpClicked`는 여전히 `ToggleStatusWindow()`를 부르고, `OnTeleportClicked`는 디버그 메시지이다(`BonfireMenuWidget.cpp:28-40`). 새 위젯을 화톳불 메뉴 안에 임베드하는 방식이면 15-1, 15-3(UI 입력 모드 정리)과 같이 정리할 것.
- [ ] **16-10. (낮음) 헤더 서식.** `CharacterStatComponent.h:50`의 `public:`이 바로 앞의 `public:` 구획과 중복되고, `h:109-112` 부근의 빈 줄 네 줄, `// ====` 구분선 주석이 추가되었다. `GetPreviewStats` 선언이 getter 목록 한가운데(`GetRequiredSouls`와 Attributes getter 사이)에 끼어 있으니 "시뮬레이션" 구획으로 분리할 것. `LevelUpWidget.h`에도 빈 줄이 여러 줄 있다.

### 4) `PROJECT_GUIDE.md` (커밋 `ef6c899`에 포함된 내용)

- [ ] **16-11. (오류 가능성) "임베딩한 `WBP_StatusWindow`가 최신 수치를 실시간으로 자동 갱신한다"는 서술(가이드 13-6)은 코드와 맞지 않는다.** `UStatusWindowWidget::RefreshStatus()`는 `NativeConstruct`(`StatusWindowWidget.cpp:11-17`)와 컨트롤러의 `ToggleStatusWindow`에서만 불린다. 델리게이트 구독도 없어서 메뉴가 열려 있는 동안 수치가 바뀌어도 갱신되지 않는다(4-7, 5-4, 13-5). 또 같은 클래스가 화톳불 메뉴에 임베드된 인스턴스와 `C`/`레벨업`으로 여는 별도 인스턴스 두 개가 동시에 존재할 수 있다(에셋 내용은 읽지 못해 확인하지 못함). "열릴 때 한 번 갱신"으로 정정할 것.
- [ ] **16-12. (낮음) 남은 불일치.** 가이드 6행의 "`main`과 같은 커밋 `2fabce4`에서 갱신"은 `Bae`가 `ef6c899`이므로 틀리다. `CanInteracBonfire` 오타가 12장 11번 항목(792행 부근)과 13-3 제목(827행 부근)에 남아 있다. 14장 표의 `LevelUpWindowWidget`은 실제 클래스 이름 `ULevelUpWidget`과 다르다. 14장 표에는 15-1(메뉴와 스탯창의 입력 모드)이 없다.

### 5) 이번 변경으로 상태가 바뀐 이전 항목

- 해소: 15-6(코드와 에셋이 `ef6c899`에 함께 커밋됨).
- 격상: 13-6(스탯 이름 `FName` 비교)을 낮음에서 중간으로(16-4).
- 그대로: 13-1 ~ 13-5, 13-7, 13-8, 13-10, 13-12, 13-14, 13-15, 14-1 ~ 14-9, 15-1 ~ 15-5, 15-7 ~ 15-10. 15-5는 `git ls-files`가 여전히 `BonFire.h/.cpp`를 돌려준다.

### 6) 우선순위

1. 16-1, 16-2, 16-5를 먼저 정한다(출력 구조체, 공식 공유, 일괄 확정 함수). 구현에 들어간 뒤에 바꾸면 위젯과 PlayerState를 다시 고쳐야 한다.
2. 16-4(`EAttributeType`)를 같이 도입한다. `UpgradeStat`, `AddPoint`, `SubPoint`가 한 번에 정리된다.
3. 13-2(레벨업 시 현재 HP/FP)는 16-2의 분리와 함께 해결한다.
4. 16-11(가이드의 "실시간 갱신" 정정)과 15-5(파일명 대소문자)는 `main`에 올리기 전에 처리한다.

---

## 17. 레벨업 계산 엔진과 `ApplyLevelUp` 리뷰 (2026-10-22, 커밋 `223d76f` 이후 미커밋 작업 트리)

**대상**: 작업 트리 변경 4개 — `CharacterStatComponent.h/.cpp`(`FStatPreview`, `CalculateDerivedStats`, `GetTotalRequiredSouls`, `UpdateCharacterStat` 재작성), `WarriorPlayerState.h/.cpp`(`ApplyLevelUp`), `PROJECT_GUIDE.md`(문구 갱신). 비교용으로 `LevelUpWidget.*`, `Bonfire.*`, `WarriorController.*`, `Warrior.*`, `StatusWindowWidget.*`, `PlayerHUDWidget.*`와 `Content/Data/*.csv`를 같이 읽었다.
빌드와 실행은 하지 않았고 코드 읽기와 CSV 대조만 했다. 코드는 수정하지 않았다.

**총평**: 섹션 16에서 구현 전에 정하자고 한 세 가지(출력 구조체, 공식 공유, 일괄 확정 함수)가 그대로 구현되었다. `UpdateCharacterStat`이 `CalculateDerivedStats`를 불러 대입하므로 시뮬레이션과 실제 적용이 어긋날 수 없게 되었고, `LoadCharacterStat()` 이중 호출이 사라져 레벨업 때 HP가 초기값으로 돌아가던 문제(13-2)도 해소되었다. 반면 `ApplyLevelUp`은 **입력 검증이 부족해서 값이 잘못 들어오면 그대로 반영**된다. 특히 11레벨 이상의 비용이 0인 점은 CSV 확장 작업과 직결된다.

### 1) 확인된 변경

| 항목 | 결과 | 근거 |
|---|---|---|
| 16-1 출력 구조체 | 해소 | `FStatPreview`(`h:10-29`), `CalculateDerivedStats`(`cpp:226-245`) |
| 16-2 공식 공유 | 해소 | `UpdateCharacterStat`(`cpp:107-148`)이 `CalculateDerivedStats`(`:117`) 결과를 `MaxHP` 등에 대입 |
| 16-5 일괄 확정 | 해소 (검증은 17-1 ~ 17-3) | `ApplyLevelUp`, `GetTotalRequiredSouls`(`cpp:211-224`) |
| 13-2 레벨업 시 HP 초기화 | 해소 (잔여 문제 17-6) | `UpdateCharacterStat`이 더 이상 `LoadCharacterStat()`을 부르지 않음 |
| 14-4 실행되지 않는 현재값 분기 | **오히려 도달 가능해짐** | 17-6 참고 |
| 시뮬레이션 비용 `GetTotalRequiredSouls` | 의미 확인 | `Level1` 행 = 1→2 레벨 비용이고 `[FromLevel, ToLevel)`을 합산해서 CSV와 맞음 |

`CalculateGrowthValue`의 구간 계산도 다시 대입해 보았다. Vigor 추가 25포인트 → `1~20 × 10 + 21~25 × 7 = 235`, 0포인트 → 0이고, 40을 넘는 포인트는 마지막 구간 상한에서 멈춘다.

### 2) 동작 오류 (높음)

- [x] **17-1. (높음) `ApplyLevelUp`이 `PendingPoints`의 음수를 걸러내지 않는다.** (2026-10-27: `WarriorPlayerState.cpp:49-53`에 필드별 음수 검사가 추가됨. 여섯 필드를 한 줄에 나열한 조건이라 `FWarriorAttributeData::HasNegative()` 도우미로 묶으면 `CanAddPoint`와 같은 검사를 쓴다) (`WarriorPlayerState.cpp`의 `ApplyLevelUp`)
  검사는 합계 `AddedLevels <= 0`뿐이다. 필드별 `>= 0`을 보지 않으므로 `Vigor = +5`, `Strength = -3`이면 합계 2로 통과해, 소울은 2레벨분만 내고 `Vigor`는 +5, `Strength`는 -3이 된다. 반대로 `Vigor = +1`, `Strength = -1`처럼 합계 0이 되는 입력은 거부되지만, 합계를 양수로 맞추면서 스탯을 재분배(환급)할 수 있다. 함수가 `BlueprintCallable`이라 위젯 블루프린트에서 구조체를 만들어 넘기면 그대로 들어간다(현재는 C++ 위젯이 비어 있어 드러나지 않을 뿐이다).
  모든 필드가 `0` 이상인지 확인하고, 하나라도 음수면 `false`를 돌려줄 것. `FWarriorAttributeData`에 `HasNegative()`/`Total()` 같은 작은 도우미를 두면 `PlayerState`와 위젯의 `CanAddPoint`가 같은 검사를 쓴다.
- [ ] **17-2. (높음) 레벨 표에 없는 레벨은 비용 0이라 소울 없이 레벨업된다.** (`CharacterStatComponent.cpp:191-209`, `ApplyLevelUp`)
  `LevelRequiredSouls.csv`는 `Level1`~`Level10`만 있고 `GetRequiredSouls`는 행이 없으면 `0`을 돌려준다(12-3). `ApplyLevelUp`은 `GetTotalRequiredSouls`의 합이 `SaveData.Souls`보다 큰지만 보므로, **Level 11 이상으로 올리는 구간은 비용 0**이다. 소울이 0이어도 통과하고, 한 번에 `Vigor +50`처럼 큰 값을 넣어도 막히지 않는다. 기존 `UpgradeStat`에는 `RequiredSouls <= 0` 거부가 있었지만 `ApplyLevelUp`에는 없다.
  `GetRequiredSouls`가 "행 없음/최대 레벨"을 구분해서 돌려주거나(예: `-1` 또는 `bool` 출력), 총비용 계산 중 하나라도 행이 없으면 실패(`false`)로 처리해야 한다. **CSV 쪽에서는 `LevelRequiredSouls.csv`를 목표 최대 레벨까지 채우는 일이 이 기능의 선행 조건**이다.

### 3) 설계와 안전성 (중간)

- [ ] **17-3. (중간) 스탯 상한(`MaxValue` 99)을 확인하지 않는다.** (`ApplyLevelUp`, 13-4 참고)
  `StatDefine.csv`의 `Vigor ... MaxValue = 99`는 어디서도 쓰이지 않는다. 총 레벨 상한이 레벨 표(10)에 우연히 걸리던 것(13-4)이 17-2로 사라지면 `Vigor`가 99를 넘어 올라가는 것을 막는 곳이 없다. 성장 표는 40포인트(= 스탯 값 50)까지만 정의되어 있어서 그 이후의 증가량도 정해야 한다. `ApplyLevelUp`에서 `초기값 + 기존 + 추가 <= MaxValue`를 검사하고, `StatDefine` 행을 읽는 함수는 컴포넌트에 이미 있으니(`GetStatInitialValue`) 최대값을 읽는 짝 함수를 두면 된다.
- [ ] **17-4. (중간) `UpgradeStat`과 `ApplyLevelUp`이 같은 일을 두 갈래로 한다. (= 13-3)**
  `UpgradeStat(FName, int32 RequiredSouls)`는 호출자가 준 비용을 믿고 이름을 문자열로 비교한다. `ApplyLevelUp`은 비용을 안쪽에서 계산한다. 사용처가 없다면 `UpgradeStat`을 지우거나, 구조체 한 개를 만들어 `ApplyLevelUp`을 부르는 얇은 함수로 바꿔서 로직이 한 곳에만 남게 할 것. 지금 남겨 두면 나중에 블루프린트가 `UpgradeStat`을 부르는 순간 검증을 우회한다. `PROJECT_GUIDE.md`도 "`UpgradeStat`(단일) 및 `ApplyLevelUp`(일괄 승인) 완비"라고 적어 두 갈래를 정상으로 서술하고 있다.
- [x] **17-5. (중간) `LevelUpWidget`의 오버라이드가 `Super::`를 부르지 않는다.** (2026-10-26: `LevelUpWidget.cpp:30,46`에서 두 오버라이드가 모두 `Super::`를 호출함을 확인) (`LevelUpWidget.cpp:5-12`)
  `NativeOnInitialized()`와 `NativeConstruct()`가 빈 본문이라 `Super::NativeOnInitialized()`/`Super::NativeConstruct()`가 호출되지 않는다. `UUserWidget`의 `Super`를 건너뛰면 블루프린트의 `Event Construct`/`Event On Initialized`가 실행되지 않고 위젯 초기화 순서가 어긋난다. 골격 단계라 지금은 영향이 없지만, 본문을 채우기 전에 `Super::` 호출을 먼저 넣어 둘 것(`BonfireMenuWidget::NativeOnInitialized`는 이미 올바르게 호출한다).
- [ ] **17-6. (중간) `UpdateCharacterStat`의 현재값 처리에 두 가지 남은 문제가 있다.** (`cpp:127-145`)
  1. **시작/로드 시 현재 HP가 `MaxHP`보다 낮다.** 컴포넌트 `BeginPlay`가 `LoadCharacterStat()`으로 `CurrentHP = 초기값(250)`을 만든 뒤 `AWarrior::BeginPlay` → `UpdateCharacterStat`이 `MaxHP`만 키우고 `Current`는 유지(`Clamp`)한다. 저장 데이터에 투자된 포인트가 있으면 `Vigor +25`일 때 `MaxHP 485` / `CurrentHP 250`으로 시작해 HUD가 52%로 보인다(12-2의 원래 시나리오). `SaveData.CurrentHP/CurrentFP`가 있지만 어디서도 읽거나 쓰지 않는다. 시작/로드 경로에서만 "현재값 = 저장값(없으면 최대)"로 정하는 분기가 필요하다. 그리고 `LoadGame()`이 `PlayerState`만 덮어쓰고 폰의 `UpdateCharacterStat`을 부르지 않으므로 로드해도 수치가 그대로다(7-6).
  2. **`CurrentHP <= 0`(사망 상태)이면 레벨업이 부활이 된다.** `:127-130`의 분기는 이전에는 실행되지 않던 코드(14-4)였는데, `LoadCharacterStat()` 호출이 빠지면서 도달 가능해졌다. 사망 처리(`OnDeath`)가 생기면 사망 중에 `ApplyLevelUp`이 `Current = Max`로 되살린다. `FP`도 같고, 스태미나는 항상 `MaxStamina`로 채운다(`:145`). 의도한 정책이면 주석으로 남기고, 아니라면 `HP <= 0`일 때 `ApplyLevelUp`을 거부(`Interact`도 사망 중에는 막힘)하거나 이 분기를 지울 것.

### 4) 정리와 일관성 (낮음 ~ 중간)

- [ ] **17-7. (낮음) `PROJECT_GUIDE.md`의 13-8 새 문구가 코드와 다르다.**
  "`UpdateCharacterStat`에서 `LoadCharacterStat()` 중복 호출을 제거하여 **레벨업이나 세이브 로드 시** `CurrentHP`가 강제로 풀피가 되는 현상 방지 (Review 13-2, 14-4 해결)"라고 적혀 있다. 레벨업 경로는 맞지만 세이브 로드는 `LoadGame()`이 `UpdateCharacterStat`을 부르지 않아서 해당 없다(17-6의 1번). 14-4는 해소가 아니라 사망 시 부활로 바뀌었다(17-6의 2번). "`ApplyLevelUp`... 원자적(Atomic) 갱신"도 17-1 ~ 17-3의 검증이 빠진 상태에서는 과장이다. 상태 표의 "✅ 구현 (2026-10-22 갱신)"도 "🟡 검증 보강 필요"로 낮추는 편이 맞다.
- [ ] **17-8. (낮음) 블루프린트에서 `FWarriorAttributeData`를 만들 수 없다.** (`WarriorSaveData.h`)
  구조체는 `BlueprintType`이지만 필드가 `UPROPERTY()`뿐이라 블루프린트에는 보이지 않는다. `CalculateDerivedStats(BlueprintPure)`와 `ApplyLevelUp(BlueprintCallable)`의 인자로 쓰이지만 블루프린트가 값을 채울 수 없다. 레벨업 위젯을 C++로 끝낼 거라면 `BlueprintType`/`UFUNCTION`을 빼는 쪽이 의도에 맞고, 블루프린트에서도 쓸 거라면 필드에 `BlueprintReadWrite`가 필요하다. 세이브 구조체에 읽기/쓰기를 열면 외부에서 임의 수정이 쉬워지므로 위젯용 별도 구조체를 두는 방법도 있다.
- [ ] **17-9. (낮음) `int32` 반환 함수 안의 `float` 잔재가 그대로다. (= 13-1)** `GetStatInitialValue`의 `return 0.f;` / `static_cast<float>(Row->InitialValue)`(`cpp:50`, `:59`), `CalculateGrowthValue`의 `return 0.f;` / `static_cast<float>(TotalBonus)`(`:79`, `:104`). 의도한 변환 규칙이 있으니(13장의 기록) 캐스트만 지우면 된다.
- [ ] **17-10. (낮음) 성장 계산이 매 호출마다 테이블 전체를 훑는다.** `CalculateGrowthValue`가 `GetAllRows`로 모든 행을 가져와 스탯마다 반복한다. `CalculateDerivedStats` 한 번에 8회, `+`/`-` 버튼을 누를 때마다 위젯이 부른다면 한 번 클릭에 16행 × 8번이다. 지금 크기(14행)에서는 문제없지만 CSV를 40~99 구간 × 8개 스탯으로 늘리면 수백 행이 된다. `BeginPlay`에서 `StatID/Target`별로 한 번 묶어 두거나, 호출 횟수가 문제가 될 때만 손볼 것.
- [ ] **17-11. (낮음) `FStatPreview` 위치와 범위.** 구조체가 컴포넌트 헤더에 있어 이를 쓰는 위젯이 컴포넌트 헤더를 include해야 한다. `Data/` 아래로 옮기면 위젯은 데이터만 알면 된다. `Poise`, `MaxWalkSpeed`는 성장 대상이 아니라서 빠진 것으로 보이지만(맞음), 미리보기에서 값이 나오지 않으므로 UI에서 "변화 없음"으로 표시할지 정해 둘 것.
- [ ] **17-12. (낮음) `ApplyLevelUp`이 실패 이유를 돌려주지 않는다.** `false` 하나로 (1) 포인트 없음, (2) 폰 없음, (3) 소울 부족이 구분되지 않아 UI에서 안내 문구를 낼 수 없다. 17-1 ~ 17-3의 검증이 늘어나면 `enum`(또는 `FText` 출력)이 필요해진다. 또 `PlayerState`가 `GetPawn()`의 컴포넌트에 비용 조회를 맡기는 구조라 폰이 없으면 레벨업이 불가능하다(12-5).

### 5) CSV 확장 작업(`Claude.md`)에 미치는 영향

코드가 확정한 의미 두 가지를 성장표 설계에 반영해야 한다.

1. **`StatGrowth.csv`의 `MinLevel`/`MaxLevel`은 "투자한 추가 포인트 수"(0부터 세는 증가량)이다.** `CalculateGrowthValue`가 `AddedPoints`로 구간을 계산한다(`cpp:72-105`). `Claude.md` 3-2의 표는 "스탯 값(11부터)" 기준으로 썼으므로 **구간을 포인트 기준으로 바꿔야 한다.** 예: Vigor→HP `11~27: +25`는 `1~17: +25`, `28~49`는 `18~39`, `50~99`는 `40~89`로 쓴다. 이 문서의 "레벨의 기준" 쟁점은 코드로 확정되었다.
2. **`LevelRequiredSouls.csv`를 목표 최대 레벨까지 채우는 일은 레벨업 비용이 새는 것을 막는 일이기도 하다**(17-2). `Level1`~`Level10`만 있으면 11레벨부터 비용 0이다. 행 이름은 `Level%d`(`cpp:198`)이므로 `Level11`, `Level12`, ...로 이어서 써야 한다. 성장 표가 40포인트(레벨 41)까지이므로 최소한 `Level1`~`Level40`이 필요하다.
3. `Strength`/`Dexterity` → `AttackPower`는 0,0,0 행이다(`StatGrowth.csv`의 `Growth_013`, `Growth_014`). `Value`가 0이라 아무 일도 하지 않으므로 스탯창에서 힘/기량을 올려도 공격력이 변하지 않는다. 계획표의 값으로 채우기 전까지는 레벨업 UI에서 "효과 없음"으로 보일 수 있다.
4. 에셋은 `Content/Data/`로 이동되어 CSV와 같은 폴더에 있다(12-7). CSV를 고친 뒤에는 에디터에서 해당 DataTable을 **리임포트**해야 게임에 반영된다(9-15).

### 6) 확인된 좋은 점

- `ApplyLevelUp`이 **모든 검사를 끝낸 뒤에야** `SaveData`를 바꾼다. 실패 시 부분 변경이 없다(검사 항목은 17-1 ~ 17-3에서 보강).
- `UpdateCharacterStat`이 `Attributes`(추가 포인트)를 `초기값 + 추가`로 환산해 `Vigor` 등 getter에 저장하고, 스탯창이 그 getter를 읽는다. 표시값과 계산값이 같은 출처이다.
- `CalculateDerivedStats`가 `const`/`BlueprintPure`이고 상태를 바꾸지 않아 미리보기에 안전하다. 같은 함수를 실제 적용에도 쓰므로 "보이는 값 = 적용되는 값"이 구조로 보장된다.
- `GetTotalRequiredSouls`가 `FromLevel >= ToLevel`이면 0을 돌려줘서 역방향 입력에 안전하다.

### 7) 이번 변경으로 상태가 바뀐 이전 항목

- 해소: 13-2, 16-1, 16-2, 16-5.
- 모습이 바뀜: 14-4(실행되지 않던 분기 → 사망 시 부활, 17-6), 12-3/13-3(비용 0 문제가 `ApplyLevelUp`으로 옮겨감, 17-2/17-4).
- 그대로: 13-1(→ 17-9), 13-3(→ 17-4), 13-4(→ 17-3), 13-5, 13-6, 15-1 ~ 15-5, 16-3, 16-4, 16-6 ~ 16-12.

### 8) 우선순위

1. 17-1, 17-2를 `ApplyLevelUp`에서 먼저 막는다(음수, 비용 0). 둘 다 한 함수 안의 작은 수정이다. 이 검증이 있어야 위젯 `CommitLevelUp`을 안전하게 연결할 수 있다.
2. `LevelRequiredSouls.csv`를 `Level40`까지 확장한다(5-2). 17-2를 코드로 막으면 11레벨부터 올릴 수 없게 되므로 CSV와 같이 진행한다.
3. 17-3(`MaxValue` 상한)과 17-4(`UpgradeStat` 정리)를 같이 정한다.
4. 17-5(`Super::` 호출)는 위젯 본문을 쓰기 전에 넣는다.
5. 17-6의 시작/로드 경로와 사망 정책은 `OnDeath`와 `LoadGame`을 만들 때 같이 정한다.
6. `StatGrowth.csv` 확장은 5-1의 포인트 기준으로 쓴다. 17-7(가이드 문구)은 `main` 병합 전에 정정한다.

---

## 18. 갱신된 md 파일 확인 (2026-10-23, `Bae` `153836a` 기준)

**대상**: 10-20 ~ 10-22에 갱신된 `PROJECT_GUIDE.md`(0장 상태 표, 12장 문제 해결 기록 9~16번, 13장 화톳불, 14장 이슈 표)와 이 문서의 상단 표·섹션 13~17. 서술을 현재 코드와 하나씩 대조했다. 빌드와 실행은 하지 않았고 코드·에셋 문자열 읽기와 `git` 조회만 했다. 코드와 가이드는 수정하지 않았다.

**총평**: 사실로 확인된 서술이 대부분이다. 다만 **구현이 끝나지 않은 부분이 "완료/완비"로 적힌 곳이 있고**, 존재하지 않는 이름과 낡은 문구가 남아 있다.

### 1) 코드와 일치하는 것 (확인됨)

- `GetRequiredSouls`의 조건 반전 수정(`CharacterStatComponent.cpp:193` `!LevelRequiredSoulsTable || Level <= 0`)과 12장 9번의 설명.
- `UpdateCharacterStat`이 `LoadCharacterStat()`을 더 이상 부르지 않고 `CalculateDerivedStats`/`FStatPreview`로 계산(`CharacterStatComponent.cpp:107-148`, `226-245`), `GetStatInitialValue`가 `int32` 반환(`:46`)이며 `UpdateCharacterStat`의 형변환이 사라진 것(12장 10번).
- 화톳불: `InteractionSphere` 반경 200(`Bonfire.cpp:26`), `EnemyCheckRadius = 1500.f`(`Bonfire.h:70`), 락온 중 차단과 `ECC_Pawn` 오버랩으로 주변의 살아 있는 적 차단(`Bonfire.cpp:43-88`), 오버랩 바인딩(`:39-40`), `E` 키 → `Warrior::Interact()` → `CurrentInteractable->Interact(this)`(`WarriorController.cpp:160`, `Warrior.cpp:542-546`), 휴식 시 `SetCurrent*` 전체 회복, `OpenBonfireMenu()`, `OnBonfireRest` 방송(`Bonfire.cpp:136-155`).
- `BonfireMenuWidget`의 `NativeOnInitialized`에서 버튼 바인딩(`BonfireMenuWidget.cpp:8-26`), 12장 12번의 설명.
- `BP_Bonfire` 에셋이 있고(`Content/Blueprints/Interactable/BP_Bonfire.uasset`) `TestMap.umap`에서 참조됨. `WBP_BonfireMenuWidget`의 부모가 `BonfireMenuWidget`.
- 0장·14장의 "적 데미지/피격 미구현"은 `Bae` 기준으로 맞다(`ApplyDamage`/`UseFP` 호출처 없음). 단 아래 18-10.

### 2) 가이드가 코드보다 앞선 곳 (정정 필요)

- [ ] **18-1. 레벨업이 "완비"로 적혀 있다.** (`PROJECT_GUIDE.md:28`, `:879`)
  "`ApplyLevelUp`(일괄 승인) 완비, 소울 부족 검증 및 원자적 갱신". 소울 부족 검증과 한 번에 반영하는 구조는 맞지만, 섹션 17의 문제가 그대로 남아 있다: 음수 `PendingPoints`를 막지 않고 합계만 본다(`WarriorPlayerState.cpp:50-62`), 레벨 표(`Level1~10`)에 없는 11레벨 이상은 비용 0이다(`CharacterStatComponent.cpp:208`, `GetTotalRequiredSouls`가 이를 합산), `MaxValue` 확인이 없다. "✅ 구현" 대신 "🟡 계산 엔진 구현, 입력 검증 보완 필요"로 표시할 것.
- [ ] **18-2. `LevelUpWidget`의 동작이 구현된 것처럼 서술되어 있다.** (`PROJECT_GUIDE.md:880-887`, `:805-807`)
  가상 장바구니, `CanAddPoint` 사전 차단, `[-]` 환불, 확인 모달, `NativeOnInitialized`에서의 영구 바인딩을 현재 시제로 설명한다. 실제 `LevelUpWidget.cpp:6-49`는 **모든 함수 본문이 비어 있고** `CanAddPoint`는 `false`만 반환한다. `NativeOnInitialized`/`NativeConstruct`는 `Super::`도 부르지 않는다(17-5). 0장(`:31`)과 14장(`:911`)의 "진행 중"과도 모순이다. 13장 8절 제목의 "(진행 중)"에 맞춰 "설계 의도(구현 전)"를 항목마다 붙일 것.
- [ ] **18-3. 임베딩한 스탯창이 "실시간으로 자동 갱신"된다고 적혀 있다.** (`PROJECT_GUIDE.md:866`)
  `StatusWindowWidget::RefreshStatus()`는 `NativeConstruct`(`StatusWindowWidget.cpp:11-17`)와 `ToggleStatusWindow()`에서만 불린다. 메뉴 위젯 안에 임베딩한 인스턴스는 메뉴가 처음 생성될 때 한 번만 갱신되고, 이후 레벨업해도 다시 불리지 않는다(16-11과 같은 문제). 그리고 `UBonfireMenuWidget::OnLevelUpClicked`는 임베딩한 창이 아니라 별도의 `ToggleStatusWindow()`를 호출한다(`BonfireMenuWidget.cpp:28-35`). "열 때만 갱신, 임베딩 인스턴스는 자동 갱신되지 않음"으로 고칠 것.
- [ ] **18-4. 에셋에서 확인되지 않는 "검증 완료" 문구.** (`PROJECT_GUIDE.md:29`, `:30`, `:905`, `:909`)
  "`BP_Bonfire` 레벨 배치 및 인게임 작동 검증 완료", "`WBP_StatusWindow` 임베딩 검증 완료"를 사실로 적었다. 에셋 문자열 검색에서 `BP_Bonfire`는 `TestMap`에 있지만, `WBP_BonfireMenuWidget.uasset`(커밋 `ef6c899`)에는 `Button_LevelUp`/`Button_Teleport`/`Button_Leave`와 `WBP_StatusWindow`라는 이름이 **나타나지 않았다**(스탯창 위젯 에셋에서는 같은 방식으로 `TextBlock_*` 이름이 모두 확인된다). 이 버튼들은 `BindWidgetOptional`이라 이름이 다르면 **조용히 null이 되고** 클릭이 동작하지 않는다. 에디터에서 위젯 이름과 임베딩 여부를 확인한 뒤, 확인 전에는 "검증 완료"를 지울 것(15-11과 같은 지적).

### 3) 코드에 없는 이름과 낡은 문구

- [ ] **18-5. `GetPreviewStats`는 코드에 없다.** (`PROJECT_GUIDE.md:808-810`, 12장 16번)
  `Source/`에 이 이름이 없다. 실제 함수는 `CalculateDerivedStats(const FWarriorAttributeData&) const`가 `FStatPreview`를 반환하는 형태이다(`CharacterStatComponent.cpp:226`). 그리고 "UI는 합성된 가상 속성만 넘겨 받아 출력하도록 설계하여 완벽한 책임 분리를 달성함"은 위젯이 비어 있어 아직 달성된 것이 아니다. 이름을 고치고 "달성"을 "설계 방향"으로 낮출 것.
- [x] **18-6. 헤더 문구가 낡았다.** (`PROJECT_GUIDE.md:6`) (2026-10-23 통합 머지 후: 헤더가 `Bae` 최신 커밋 `e97f4ee`로 갱신되어 현재 `Bae`와 일치함을 확인. 단 `origin/main`이 한 커밋 앞서 있어 커밋 번호를 적는 방식은 곧 다시 낡는다)
  "`Bae` (`main`과 같은 커밋 `2fabce4`에서 갱신)"인데 지금 `Bae`는 `main`보다 앞선 `153836a`이다. 커밋 번호를 적지 말고 "`Bae` 브랜치 기준"으로만 쓰거나 최신 번호로 바꿀 것. 같은 이유로 0장 "미구현"의 피격/데미지 항목은 "(`Bae` 기준. `Kang` 브랜치에는 적의 무기 판정과 데미지 임시 구현이 있고 아직 병합되지 않음)"처럼 브랜치 범위를 붙이면 팀원이 읽을 때 혼동이 없다.
- [ ] **18-7. 리뷰 번호를 직접 인용한다.** (`PROJECT_GUIDE.md:874`, `:876`, `:878`, `:879`)
  "Claude Review 16-1, 16-2 해결", "Review 13-2, 14-4 해결", "Review 16-5 전반부/후반부 해결". 이 문서 자신의 체크리스트(14장 5번)와 어긋나고, 리뷰 문서의 번호가 바뀌면 깨진다. 설명 문장으로 풀어 쓸 것.
- [ ] **18-8. `CanInteracBonfire` 오타.** (`PROJECT_GUIDE.md:835`, `:794`) 실제 함수는 `CanInteractBonfire`(`Bonfire.cpp:43`)이다.
- [ ] **18-9. 두 서술이 사실을 흐린다.**
  - 12장 9번의 "존재하지 않는 행이 요청되면 기본값 0으로 흘려보내는 2중 안전장치(Safe Fallback)"는 장점이 아니라 17-2의 원인이다(비용 0 = 무료 레벨업). "행이 없으면 0"이 아니라 "비용 없음/최대 레벨"을 구분해야 한다고 고칠 것.
  - 14장(`:907`)의 "레벨업/로드 시 풀피 초기화 해결"은 레벨업 경로만 맞다. 시작/로드 때는 `LoadCharacterStat()`이 현재값을 초기 최대치로 정한 뒤 `Clamp`만 하므로 성장을 적용한 저장 데이터로 시작하면 현재 HP가 `MaxHP`보다 낮고(`CharacterStatComponent.cpp:127-134`), `CurrentHP <= 0`이면 부활하는 경로가 남아 있다(17-6).
  - 12장 14번의 "`ABonfire`로 완전 일괄 통일"은 소스 이름만 맞다. Git이 추적하는 파일명은 아직 `Interactable/BonFire.cpp/.h`이고 디스크 이름은 `Bonfire.*`이다(15-5). 대소문자를 구분하는 환경에서는 `#include "Interactable/Bonfire.h"`가 실패한다.

### 4) 그 밖의 확인

- [ ] **18-10. 상태 표의 범위가 브랜치마다 다르다.** `Kang`(`4c4eee4` 적 무기 설정·데미지 판정 임시 구현, `fd4de7f` 일반 공격 BTTask)과 `Lee`(`d749a2e` 맨손 콤보 리타게팅·몽타주)는 아직 `main`에 병합되지 않았다. 0장의 "❌ 미구현" 항목은 병합되는 순간 틀리게 된다. 표에 "기준 브랜치"를 한 줄 적어 둘 것.
- 9-1(`PlayAttackMontage()` 빈 함수 `WarriorAnimInstance.cpp:130-132`)은 `Bae` 기준으로 그대로이다. `Lee`의 공격 콤보 작업이 병합되면 다시 확인한다.

### 5) 우선순위

1. 18-1, 18-2, 18-4는 팀원이 가이드를 읽고 판단할 때 오해를 만드는 항목이라 먼저 정정한다. 이 문서의 17-7과 같은 주제이다.
2. 18-3과 18-5는 한 문장씩 고치면 된다. 18-6 ~ 18-8은 오타와 문구 정리이다.
3. 18-4의 에셋 확인(위젯 이름, 임베딩)은 에디터에서만 가능하다. 확인된 뒤에 "검증 완료"를 다시 적는다.

---

## 19. 통합 머지 후 전체 점검 (2026-10-23, `Bae` = `e97f4ee` 기준)

**대상**: `Source/` 전체 약 7,200줄 중 `Warrior/*`, `Character/*`, `Interactable/*`, `Interface/*`, `UI/*`, `Component/*`, `Item/*`, `ItemData/*`, `Anim/*`, `Enemy/*`(`EnemyBase`, `EnemyAIController`, `AttackTraceComponent`, `BTTask`/`BTService`, 애님 인스턴스), `GamePlay/*`, `Data/*`, `SaveData/*`, `Content/Data/*.csv`, `Config/DefaultEngine.ini`/`DefaultGame.ini`의 채널·AssetManager 설정, `PROJECT_GUIDE.md` 전체 구조(0장, 13~14장). 에셋(`.uasset`)은 열 수 없어서 **문자열 검색으로만** 확인했다(결과가 "없음"이어도 에디터 확인이 필요하다). 빌드와 실행은 하지 않았다. 코드는 수정하지 않았다.
**기준 상태**: 로컬 `Bae` = `origin/Bae` = 로컬 `main` = `e97f4ee`("2026.10.23 merge completed", Bae·Kang·Ahn·Lee 통합). `origin/main`은 `f1051b3`("Lockon bug fixed", Ahn)으로 **1커밋 앞서 있고** `Bae`에는 아직 없다. 변경은 `Warrior.cpp`의 `GetOverlaps()` 한 줄(`ECC_GameTraceChannel2` → `ECC_Pawn`)뿐이다. `Review(Claude).md`와 `PROJECT_GUIDE.md`는 `main`에서 삭제된 상태라 작업 트리에서 `A`(스테이지된 새 파일)로 보인다.

**총평**: 병합 자체는 깨끗하다. `WarriorController.cpp`의 생성자 괄호와 `Interact()` 중복 정의가 정리되었고 `Warrior.cpp`에 `CreateDefaultSubobject`/스탯 호출이 중복되지 않는다(가이드 14장의 "머지 충돌 오타 정리"와 일치). 그러나 **병합 직후 화톳불이 동작하지 않는다**(19-1). 두 팀원의 상호작용 인터페이스가 서로 다른데 `Warrior::Interact()`는 Ahn 쪽만 처리하기 때문이다. 이 한 가지를 고치면 이전 작업(스탯·레벨업 엔진)은 그대로 쓸 수 있다. Ahn의 아이템 쪽에는 **처음 장착하는 순간 터질 코드**가 2곳 있다(19-3, 19-4). 이 코드는 아직 호출처가 없어서 드러나지 않았을 뿐이다.

### 1) 동작 오류 (높음)

- [x] **19-1. 병합 후 `E` 키로 화톳불을 쓸 수 없다. (13-11의 회귀)** (2026-10-26: 커밋 `0a9a3ac`에서 `IInteractableInterface` 상속, 오버랩 등록/해제, `Interact(AActor*)`, 구 인터페이스 삭제를 코드로 확인함. 남은 정리는 20-10)
  호출 경로가 중간에서 끊긴다.
  1. `WarriorController.cpp:207`: `E` → `AWarriorController::Interact()`(`:378`) → `AWarrior::Interact()`.
  2. `Warrior.cpp:599-618`: `DetectedActors`가 비어 있으면 반환(`:601`), 있어도 `Cast<IInteractableInterface>(Target)`(`:613`)이 `nullptr`이면 반환.
  3. `BonFire.cpp:90-115`: 오버랩 콜백의 `Warrior->SetCurrentInteractable(this)`가 주석 처리되어 **`AddDetectedObject(this)`가 어디에도 없다.** `DetectedActors`는 `AItemObject`만 채운다(`ItemObject.cpp:84`).
  4. `BonFire.h:27`: `ABonfire`는 `IInteractionInterface`(구)를 상속하고 `IInteractableInterface`(신)가 아니다. 감지 스택에 넣어도 `:613`의 캐스트가 실패한다.
  결과는 **빌드는 성공하고 에러도 없이 `E`가 아무 일도 하지 않는 것**이다. 가이드 13-5, 13-10의 계획이 정확하다. 작업은 세 곳이다.
  ```cpp
  // Interactable/BonFire.h:7  (include 교체)
  #include "Interface/InteractableInterface.h"
  // Interactable/BonFire.h:27 (상속 교체)
  	public IInteractableInterface
  // Interactable/BonFire.h:36 (시그니처 교체, 부모가 AActor*를 받는다)
  	virtual void Interact(AActor* detectedActor) override;
  ```
  ```cpp
  // Interactable/BonFire.cpp:90-100  OnInteractionSphereBeginOverlap
  	if (AWarrior* Warrior = Cast<AWarrior>(OtherActor))
  	{
  		Warrior->AddDetectedObject(this);
  	}
  // Interactable/BonFire.cpp:102-115 OnInteractionSphereEndOverlap
  	if (AWarrior* Warrior = Cast<AWarrior>(OtherActor))
  	{
  		Warrior->RemoveDetectedObject(this);
  	}
  // Interactable/BonFire.cpp:117-123 Interact 앞부분. 아래 본문은 그대로 쓴다(변수 이름이 같다).
  void ABonfire::Interact(AActor* detectedActor)
  {
  	AWarrior* Interactor = Cast<AWarrior>(detectedActor);
  	if (!CanInteractBonfire(Interactor))   // 함수 안에서 nullptr를 이미 걸러낸다(:45).
  	{
  		return;
  	}
  	// ...이하 기존 코드
  ```
  끝난 뒤 `Warrior.h:17`(`class IInteractionInterface;`)과 `:150`(`CurrentInteractable`), `Warrior.cpp:26`의 include, `Interface/InteractionInterface.*`는 쓰는 곳이 없어지므로 지울 수 있다(15-4는 이 삭제로 해소). 그리고 화면 디버그 메시지(`BonFire.cpp:97,112`)는 지금 "영역에 진입. 상호작용 대상 등록"이라고 말하지만 실제로는 등록하지 않는 상태다. 위 수정 후에 문구가 맞게 된다.
  **같이 알아 둘 점(Ahn의 선택 규칙)**: `Warrior::Interact()`는 `DetectedActors`의 **마지막에 들어온 대상 하나**만 상호작용한다(`:604-605`). 아이템 트리거(`ItemObject`)와 화톳불 구체(반경 200)가 겹치는 자리에서는 나중에 들어간 쪽이 우선이다. 또 마지막 대상이 무효이면 `RemoveAt`만 하고 반환해서(`:607-611`) `E`를 한 번 더 눌러야 한다. 기능에는 문제없지만 알고 있어야 한다.
- [ ] **19-2. 락온 후보 탐색의 `GetController()` null 역참조 (= 9-2). `origin/main`의 채널 변경으로 발생 확률이 올라간다.**
  `Warrior.cpp:504`(`LockOn`)와 `:543`(`ChangeLockOn`)의 `Detected->GetController()->IsPlayerController()`는 컨트롤러가 없는 `ACharacter`가 반경 600 안에 있으면 바로 크래시한다. 기존에는 `ECC_GameTraceChannel2`("Player" 채널, `DefaultEngine.ini:138`)로 질의해서 사실상 적만 걸렸지만, `f1051b3`이 `ECC_Pawn`으로 바꾸면 **컨트롤러 없이 배치된 `ACharacter`나 죽어서 컨트롤러가 분리된 캐릭터도 후보**가 된다. 같은 파일 `:165`의 `if (LockOnTarget && !LockOnTarget->GetArrowComponent()) return;`도 조건이 거꾸로라서 `LockOnTarget`이 null이면 바로 아래 `:174`에서 역참조한다(`&&` → `||` 의도로 보임. 9-3의 일반 포인터 문제와 함께 `TWeakObjectPtr<ACharacter>`로 바꾸면 한꺼번에 정리된다).
  ```cpp
  // Warrior.cpp:504, :543
  AController* DetectedController = Detected->GetController();
  if (!DetectedController || DetectedController->IsPlayerController()) continue;
  // Warrior.cpp:165
  if (!LockOnTarget || !LockOnTarget->GetArrowComponent()) return;
  ```
- [ ] **19-3. `EquipmentComponent`가 맵에 없는 키를 `operator[]`로 읽는다. 처음 장착하는 순간 어서션이 걸린다.**
  `EquipmentComponent.cpp:15`의 `EquippedItems[Slot] != InstanceID`와 `EquipmentComponent.h:25`의 `GetEquippedItem`은 `TMap::operator[]`를 쓰는데, 키가 없을 때 `operator[]`는 값을 만들지 않고 `check`에 걸린다. 맵은 비어 있는 채로 시작하므로 첫 `EquipItem`이 그대로 실패한다. `AWarrior::EquipWeapon`(`Warrior.cpp:675`)을 호출하는 곳은 아직 없어서 드러나지 않았다.
  ```cpp
  // Component/EquipmentComponent.cpp:13-19
  void UEquipmentComponent::EquipItem(FGuid InstanceID, EEquipmentSlot Slot)
  {
  	EquippedItems.Add(Slot, InstanceID);   // 있으면 덮어쓴다. 비교는 필요 없다.
  }
  // Component/EquipmentComponent.h:25
  FORCEINLINE FGuid GetEquippedItem(EEquipmentSlot Slot) const
  {
  	const FGuid* Found = EquippedItems.Find(Slot);
  	return Found ? *Found : FGuid();
  }
  ```
- [ ] **19-4. `AWarrior::EquipWeapon`의 null 검사가 같은 변수를 두 번 본다.**
  `Warrior.cpp:677`과 `:683` 둘 다 `if (!InItem)`이다. `:682`에서 `Cast<UWeaponDataAsset>`한 결과(`WeaponData`)를 검사하지 않아서 방어구나 소모품이 들어오면 `:687`의 `WeaponData->WeaponMesh`에서 역참조한다. 또 `:680`의 `Equipment->EquipItem`이 타입 확인보다 먼저 실행되어, 무기가 아닌 아이템도 슬롯에 기록된다.
  ```cpp
  // Warrior.cpp:680-685
  UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(InItem);
  if (!WeaponData)
  	return;

  Equipment->EquipItem(InItem->InstanceID, Slot);
  ```

### 2) 설계와 연동 (중간)

- [ ] **19-5. 소울을 얻는 코드가 없다. 지금은 게임 안에서 레벨업이 불가능하다.** (2026-10-27 부분 해소: `AWarriorPlayerState::AddSouls`(`:111-120`)와 디버그 키 `F1`(`WarriorController.cpp:217`, 함수 `:220`, +5,000)이 추가되어 시험은 가능해짐. 실제 소울 획득 경로(적 처치 보상, 줍기)는 아직 없음)
  `Source/`에서 `Souls`가 늘어나는 곳이 없다(`WarriorPlayerState.cpp:32,88`의 차감과 `StatusWindowWidget.cpp:50`의 표시뿐). 기본값이 0이고 저장도 0이므로 `ApplyLevelUp`을 호출해도 항상 소울 부족이다(17-2의 비용 0 구간 제외). 레벨업 위젯을 완성해도 시험할 방법이 없으니, **적 처치 보상과 디버그 소울 추가(Ahn의 `IMC_Debug`에 키 추가) 중 하나**를 먼저 정할 것. 보상 소울은 `AEnemyBase`에 스탯/보상 데이터가 없어서(9-4, 3-2) 적 설계와 같이 정해야 한다.
- [ ] **19-6. 피격 경로가 생겼지만 방어·무적·사망이 없다. (9-4 일부 해소)**
  `EnemyBase.cpp:141`이 `TargetStatComp->ApplyDamage(50.0f)`를 직접 부른다. 이제 `ApplyDamage` 호출처가 있다는 점에서 9-4와 가이드의 "호출처 없음"(`PROJECT_GUIDE.md:35`)은 바뀌었다. 남은 것은 (1) 50 고정(`StatDefine`의 `Defense`/`AttackPower` 미사용, `Hit.Defense` 계산 없음), (2) 방어 자세(`OverlayState::Block`)와 구르기(`ActionState::Roll`) 중에도 같은 피해, (3) `CurrentHP`가 0이 되어도 아무 일도 없고 조작이 계속되며 화톳불에서 다시 채워진다(17-6의 2번과 합쳐서 "사망 상태" 정의가 필요), (4) 플레이어 → 적 피해 경로가 없어서(적에게 스탯 컴포넌트가 없음, 3-2) 전투가 일방향이다. 데미지 규칙은 `UCharacterStatComponent`에 `TakeDamage(float Raw, ...)` 같은 한 곳을 두고 `Defense`와 상태를 거치게 할 것. Kang의 코드(`@Todo: 데미지 임시 처리`)라 소유자와 함께 정한다.
- [ ] **19-7. UI 체계가 셋이다: `WBP_Menu`(Ahn), `StatusWindow`(`C`), `BonfireMenu`(화톳불). 소유자와 입력 모드 규칙이 없다. (15-1, 15-3과 연결)**
  - `WarriorController.cpp:145-152`가 `WBP_Menu`를 `BeginPlay`에서 항상 `AddToViewport()`한다. 열고 닫는 입력이나 함수가 C++에 없다. `WBP_Menu`에서 `Visibility`/`Hidden`/`Collapsed` 문자열이 한 번씩 보이므로 기본 숨김일 수 있다. **누가 어떻게 여는지 확인이 필요하다.**
  - `WBP_Menu`의 `Stat` 탭 안에는 `WBP_StatusWindow`가 없다(문자열 검색에서 `StatusWindow`가 나타나지 않음). 같은 스탯창을 `C` 키 창, 화톳불 메뉴 안(임베드, 확인됨), `WBP_Menu`의 `Stat` 탭에 쓸 것인지 정해야 한다. 세 곳에서 쓰면 같은 `RefreshStatus()`가 열 때만 불리는 문제(16-11, 18-3)가 세 배가 된다.
  - 입력 모드는 `ToggleStatusWindow`(`:487-517`)와 `OpenBonfireMenu`/`CloseBonfireMenu`(`:519-543`)만 바꾼다. `OnLeaveClicked`가 `CloseBonfireMenu()`를 쓰게 된 것은 15-3의 절반 해소지만, `CloseBonfireMenu`가 스탯창이 열려 있는지 모른 채 `GameOnly`로 되돌리는 15-1은 그대로다. Ahn의 메뉴가 열리는 순간에도 같은 문제가 생기므로 `UpdateUIInputMode()`(15-3의 스케치)를 **세 위젯이 공통으로 쓰게** 만들 것.
  - `Escape`/`Q`가 `CloseBonfireMenu`에 묶였다(`:208-209`). `Escape`는 보통 일시정지/메뉴 닫기 전반에 쓰이고 `Q`는 퀵슬롯(Ahn의 이번 주 목표)과 겹치기 쉽다. 닫기를 "열려 있는 UI 중 가장 위 하나 닫기"로 일반화할 것.
- [x] **19-8. 상호작용 인터페이스가 둘이다. (19-1의 정리 계획)** (2026-10-26: `InteractionInterface.*` 삭제 확인. `Interact()`의 `ActionState`/UI 검사는 15-2로 남음)
  `IInteractableInterface::Interact(AActor*)`(Ahn)와 `IInteractionInterface::Interact(AWarrior*)`(Bae). 팀 표준을 Ahn 쪽으로 정했으니(가이드 13-10) 화톳불을 옮기고 구 인터페이스는 지운다. 옮긴 뒤에는 `Interact`가 `AActor*`를 받아서 `Cast`가 한 줄 늘지만, 대신 화톳불·아이템·NPC가 같은 호출 경로를 쓰고 `Warrior.h`가 구체 상호작용 대상을 알 필요가 없다. 다만 지금 `Interact()`는 `ActionState`(구르기, 공격 중)와 UI가 열려 있는지를 확인하지 않는다(15-2). 이동할 때 같이 넣을 것.
- [ ] **19-9. `InventoryComponent::AddItem`이 공유 데이터 애셋에 인스턴스 ID를 쓴다.**
  `InventoryComponent.cpp:22-23`이 `ItemData->InstanceID = AddedItem.InstanceID;`로 `UItemDataAsset`(애셋, 모든 복사본이 공유)의 필드를 바꾼다. 같은 애셋을 두 번 주우면 두 `FInventoryItem`이 같은 애셋을 가리키고 ID는 마지막 것으로 덮인다. 장착(`EquipWeapon`이 `InItem->InstanceID`를 읽음, `Warrior.cpp:680`)은 그 값을 쓰므로 먼저 주운 것을 장착하려 해도 나중 ID가 기록된다. `FInventoryItem::InstanceID`가 이미 있으니 장착 함수는 `FInventoryItem`(또는 ID)을 받게 하고 애셋의 `InstanceID` 필드는 지울 것. 또 에디터에서 PIE 중에 애셋이 수정된 것으로 표시될 수 있다.
- [ ] **19-10. 충돌 채널 기준이 일관되지 않다.**
  `DefaultEngine.ini:137-138`이 `Enemy`(`GameTraceChannel1`), `Player`(`GameTraceChannel2`) 채널과 프로파일을 정의해 두었다(`Enemy` 프로파일은 `Pawn`을 `Ignore`). 하지만 코드는 락온 `ECC_Pawn`(`f1051b3` 이후), 화톳불의 적 감지 `ECC_Pawn`(`BonFire.cpp:64-71`), 공격 판정 `TraceChannel = ECC_Pawn`(`AttackTraceComponent.h`)을 쓰고 커스텀 채널은 쓰지 않는다. 적 캡슐이 `Pawn` 응답을 가진 프로파일이어야 위 셋이 모두 맞는다. 적 블루프린트가 `Enemy` 프로파일로 바뀌는 순간 락온, 화톳불 적 감지, 공격 판정이 한꺼번에 조용히 안 걸린다. 팀이 "무엇으로 질의하는가"를 한 곳(예: `Config` 주석 또는 `Data/` 헤더의 `constexpr`)에 정할 것. 벽 뒤 판정(Kang의 커밋 메시지)도 같은 채널 규칙 안에서 풀린다.
- [ ] **19-11. 공격 상태 고착 위험이 에셋 의존으로 바뀌었다. (9-1 후속)**
  `PlayAttackMontage()`는 여전히 빈 함수(`WarriorAnimInstance.cpp:230`)지만 공격은 이제 `PlayActionMontage()`의 `Attack` 분기로 간다. 콤보 마지막 타는 `Entries.IsValidIndex(ComboIndex)` 실패 → `false` → `FinishAttack()`(`Warrior.cpp:402-405`, `:449-452`)로 끝난다. 그런데 `Warrior.cpp:289`의 `OnActionMontageEnded`는 `ActionState == Attack`이면 **아무것도 하지 않는다**. 몽타주가 노티파이(`AnimNotify_ComboAttackTransition`) 위치에 도달하기 전에 끝나거나 끊기면(예: 몽타주 블렌드 아웃, `Montage_Stop`) `ActionState`가 `Attack`에 남고 이동·구르기·상호작용이 모두 막힌다(`Move`가 `ActionState != None`이면 반환, `Controller.cpp:228`). 노티파이가 몽타주마다 올바른 위치에 있는지는 에셋 확인이 필요하다. 안전장치로 `OnActionMontageEnded`에서 `bInterrupted`이거나 현재 재생 중인 공격 몽타주가 아니면 `FinishAttack()`을 부를 것. `AnimNotify_ComboCheck`(`WarriorAnimInstance.cpp:134-148`)는 `ResetCombo()`를 부르므로 ABP/몽타주에 아직 이 노티파이가 남아 있으면 콤보 인덱스가 0으로 돌아간다. 에셋에서 쓰는지 확인하고 안 쓰면 지울 것.
- [x] **19-12. `WBP_BonfireMenuWidget`: 임베딩은 확인, 버튼은 확인 안 됨. (18-4 일부)** (2026-10-27: 커밋 `15e944c`의 에셋 문자열에서 `Button_LevelUp`/`Button_Teleport`/`Button_Leave`와 `LevelUpWidget`/`TeleportWidget`이 확인됨. 반대로 `WBP_StatusWindow` 이름은 이제 에셋에 나타나지 않아 스탯창 임베딩은 없어졌다. 섹션 24)
  `b67968c`에서 에셋이 갱신되어(23,480 → 27,441바이트) 문자열 검색에서 `WBP_StatusWindow`가 보인다(임베딩 확인). 반면 `Button_LevelUp`/`Button_Teleport`/`Button_Leave`는 물론 `Button`이 들어간 이름 자체가 하나도 없다(같은 방식으로 `WBP_Menu`에서는 `InventoryButton`, `StatButton`이 보인다). 세 필드가 `BindWidgetOptional`이라 이름이 없으면 조용히 `nullptr`이 된다. 에디터에서 위젯 이름을 확인하고, 없다면 버튼을 만들 때까지 가이드의 "메뉴 버튼 동작" 서술을 보류할 것. `Escape`/`Q`는 컨트롤러에서 바인딩해서 버튼과 관계없이 동작한다. 그리고 `OnLevelUpClicked`는 여전히 별도의 `ToggleStatusWindow()`를 부른다(`BonfireMenuWidget.cpp:28-35`, 16-9).

### 3) 정리와 일관성 (낮음)

- [ ] **19-13. `Bonfire` 파일 이름과 `#include`의 대소문자 (15-5 후속).** Git과 디스크가 둘 다 `BonFire.h/.cpp`로 같아졌다(15-5의 "불일치"는 해소). 그러나 `BonFire.cpp:4`는 `#include "Interactable/Bonfire.h"`, `BonFire.h:8`은 `#include "Bonfire.generated.h"`로 **다른 대소문자**를 쓴다. Windows에서는 되지만 UHT/대소문자 구분 환경에서 오류가 날 수 있다(미확인). 파일 이름 쪽을 바꾸면 Git 이름 변경 문제가 다시 생기므로, 두 `#include`를 실제 이름(`BonFire.h`, `BonFire.generated.h`)에 맞추는 편이 안전하다.
- [ ] **19-14. `IA_Interact`가 입력 매핑에 보이지 않는다. (15-10 후속)** `Controller.cpp:188`이 `IA_Interact`를 `Completed`에 묶었지만 `IMC_Warrior.uasset` 문자열에는 `IA_Interact`가 없다(`IA_Delete`는 있는데 C++에서 쓰는 곳이 없다). 지금은 `:207`의 `BindKey(E)`만 동작한다. 나중에 `IA_Interact`를 `E`에 매핑하면 한 번 누를 때 `Pressed`(BindKey)와 `Completed`(Enhanced Input)가 **두 번** `Interact`를 부른다(화톳불은 점화와 메뉴 열기가 한 번에 일어난다). 한쪽만 남길 것. 가이드 13-5는 "향상된 입력 `IA_Interact` 완료 시"라고 적어 실제와 다르다.
- [ ] **19-15. `ItemListWidget`의 `FClassFinder` 경로에 `_C`가 없다.** (`ItemListWidget.cpp:12`) `"/Game/Blueprints/UI/WBP_ItemButton.WBP_ItemButton"`은 클래스 경로가 아니라서 항상 실패하고 `ItemButtonClass`가 비어 `AddItemButton`이 조용히 반환한다(`:69`). 같은 파일의 `WBP_Menu` 경로(`Controller.cpp:109`)는 `_C`가 있다. `ListMode`/`CurrentFilter`(`ItemListWidget.h:75,78`)는 `UPROPERTY`지만 기본값이 없다. `InitializeItemList`가 비어 있고 `AddItemButton`이 만든 위젯을 컨테이너에 넣지 않는 것은 작업 중으로 본다.
- [ ] **19-16. `AItemObject`가 애셋 목록이 비어 있으면 크래시한다.** (`ItemObject.cpp:48-51`) `ensureAlways(Assets.Num() > 0)`는 경고만 하고 계속 진행해서 `Assets[0]`을 읽는다(`RandRange(0, -1)`). 또 스폰할 때마다 `LoadSynchronous()`(`:59`)를 한다. 임시 코드로 보이지만 다음 작업(무기 스탯 연동) 전에 `if (Assets.IsEmpty()) return;`이 필요하다.
- [ ] **19-17. 작은 것들.**
  - `ObtainItem(UItemDataAsset* detectedItem)`가 인자를 무시하고 `DetectedActors.Last()`에서 다시 찾는다(`Warrior.cpp:698-711`). `ItemObject.cpp:112`가 이미 `Item`을 넘기므로 인자를 쓰면 15-4류의 순서 의존이 사라진다.
  - `Dodge`는 `AnimInstance`를 검사하지 않는다(`Warrior.cpp:474,480`). `Attack`은 검사한다(`:402`).
  - `GetOverlaps()`의 `DrawDebugSphere`(`:594`)는 락온 키를 누를 때마다 빨간 구를 0.2초 그린다. 디버그용이면 `#if !UE_BUILD_SHIPPING`으로 감쌀 것.
  - `WarriorAnimInstance`의 빈 함수 5개(`PlayMontage`, `PlayRollMontage`, `PlayBackStepMontage`, `PlayAttackMontage`, `StopAttackMontage`)가 그대로다. `AnimNotify_ComboCheck`가 `StopAttackMontage()`를 부르지만 아무 일도 안 한다.
  - `TriggerDebugToggle`의 `bIsDebugModeActive = ~bIsDebugModeActive;`(`Controller.cpp:416`)는 1비트 비트필드에 정수 `~`를 대입한다. 값이 우연히 맞게 잘리지만 `= !bIsDebugModeActive;`가 의도를 드러낸다.
  - `Warrior.h:8-9`가 `ItemDataAsset.h`, `ItemObject.h`를 헤더에서 include한다. 전방 선언으로 충분한지 확인할 것(`EEquipmentSlot`은 enum이라 `ItemDataAsset.h`가 필요하다).
- [ ] **19-18. `Bae` 쪽 남은 정리.**
  - `LevelUpWidget.cpp:26-40`: `NativeOnInitialized`는 `Super::`를 부르게 되었지만 **`NativeConstruct`는 아직 `Super::NativeConstruct()`가 없다**(17-5는 절반 해소). 본문이 모두 주석 단계 설명이고 `CanAddPoint`는 `false`를 돌려준다. 주석 `:32`의 "모달 팔업"은 "팝업" 오타.
  - `UIHelpers.h`가 추가되었지만(`UI/UIHelpers.h`) 아무도 include하지 않는다. `StatusWindowWidget.cpp:28-34`는 여전히 지역 람다를 쓴다(16-6, 13-8 그대로). 헤더는 `int32` 오버로드만 있고 `// float...`로 끝난다. 스탯창은 float getter 14개를 `static_cast<int32>`로 바꿔 넘기는데(`StatusWindowWidget.cpp:56-73`), 공용 헬퍼에 `float` 오버로드를 더하면 이 캐스트가 사라진다. 소수점 처리(내림/반올림)는 한 번만 정할 것.
  - `StatusWindowWidget.cpp:51`의 `GetRequiredSouls(SaveData->Level)`는 행이 없으면 `0`을 표시한다(15-9, 17-2 그대로).
- [ ] **19-19. 저장소.** `.git`이 약 1.6GB이고 `.git/objects/pack/tmp_pack_6Zqw9M`(약 107MB)이 "garbage"로 보고된다(`git count-objects -vH`). 중단된 작업의 임시 팩으로 보이며 Git/GitHub Desktop이 실행 중이 아닐 때 지워도 되지만, 리뷰어가 지우지는 않았다. LFS와 에셋 크기(9-17)는 팀 합의 항목 그대로이다.

### 4) 가이드(`PROJECT_GUIDE.md`)와 코드의 새 불일치

- [ ] **19-20.** 아래는 이 병합 이후 가이드가 코드와 달라진 곳이다. 18장과 겹치는 것은 번호만 적었다.
  - `:35` "피격/데미지 경로 ❌ 미구현, `ApplyDamage`를 호출하는 곳이 없음" → `EnemyBase.cpp:141`이 호출한다(19-6). `:1043`의 "`ApplyDamage`/`UseFP` 호출처가 없고"도 `ApplyDamage`는 틀리다(`UseFP`는 여전히 없음).
  - `:38` "장비 및 인벤토리/아이템 ✅ 구현" → `EquipItem`은 첫 호출에서 어서션(19-3), `EquipWeapon`은 호출처 없음, `ItemListWidget`은 본문이 비어 있다. "🟡 구현 중"이 맞다.
  - `:34` "공격 콤보 ✅ 구현", `:1047` "공격 고착 해결" → 코드상 콤보 구조는 있으나 노티파이 배치는 에셋 확인 전이다(19-11). "에디터 확인 필요"를 붙일 것.
  - `:534` "`GetMind()`, `GetIntelligence()`를 호출" → 이런 함수는 없다. 실제는 `GetAttunement()`, `GetVitality()`이다(`CharacterStatComponent.h:117-121`).
  - `:924` "스탯창이 실시간으로 자동 갱신" → 열 때만 갱신(18-3, 16-11). `:864-866` `GetPreviewStats`는 코드에 없음(18-5). `:891`, `:850`의 `CanInteracBonfire` 오타(18-8). `:934-939`의 "Claude Review 16-1…" 번호 인용(18-7). `:40`의 레벨업 "완비"(18-1).
  - `:41` "`WBP_StatusWindow` 임베딩 검증 완료" → 이제 에셋 문자열로 임베딩은 확인된다(18-4의 절반). 버튼 연결은 19-12.
  - `:6` 헤더의 `e97f4ee`는 현재 `Bae`와 일치한다(18-6 해소). 다만 `origin/main`(`f1051b3`)은 한 커밋 앞서 있으므로 커밋 번호를 적는 방식은 곧 다시 낡는다.

### 5) 변동 없음 (코드 확인) — 아직 열려 있는 이전 항목

`CharacterStatComponent.*`와 `WarriorPlayerState.*`는 `153836a` 이후 변경이 없다. 따라서 17-1(음수 포인트), 17-2(11레벨 이상 비용 0), 17-3(`MaxValue`), 17-4(`UpgradeStat` 이중 경로), 17-6(시작/로드 현재 HP, 사망 부활), 17-9(`int32` 함수 안의 `float`), 13-4, 13-5, 13-6, 14-1 ~ 14-9가 그대로다. `GameMode.LoadGame()`도 `PlayerState`만 덮어쓰고 폰의 `UpdateCharacterStat`을 부르지 않는다(`ZTSGameMode.cpp:90`, 7-6). 이동 속도 출처(9-5, 4곳)와 AI 경로 2개(9-7), `EnemyAIData` 기본값(9-6)도 그대로다. `StatGrowth.csv`의 `Strength`/`Dexterity` → `AttackPower`는 0행이고 `LevelRequiredSouls.csv`는 `Level10`까지다.

### 6) 확인된 좋은 점

- 병합 결과가 컴파일 구조에서 깨끗하다. 컨트롤러의 `Interact()`가 한 번만 정의되고 `Warrior.cpp`에 중복 서브오브젝트가 없다. 병합 중에 확인한 위험(조용히 삭제된 `bCanEverTick`, 중복 `CreateDefaultSubobject`)이 남아 있지 않다.
- `AWarrior::DetectedActors`가 `TWeakObjectPtr<AActor>`이고 사용 시 `IsValid`로 확인한다(`Warrior.cpp:607`). 내가 15-4에서 지적한 매달린 포인터 문제를 이 방식이 이미 해결한다. 화톳불이 이 방식으로 옮겨가면 15-4가 같이 사라진다.
- `UAttackTraceComponent`: 평소에는 `Tick`을 꺼 두고(`SetComponentTickEnabled(false)`) 판정 구간에서만 켜며, 이전 프레임 → 현재 프레임 스윕 3지점과 `HitActors` 중복 제거로 한 번의 공격에서 같은 대상이 여러 번 맞지 않는다. 소켓이 없으면 시작하지 않고 `TWeakObjectPtr<UMeshComponent>`로 메쉬 소멸에 안전하다.
- `BTTask_NormalAttack`이 몽타주 종료 델리게이트를 해제하고(`CleanUp`) 중단 시 몽타주를 멈춘다. `bCreateNodeInstance`로 AI마다 독립 인스턴스를 쓴다.
- `UMenuWidget`/`UMenuButtonWidget`가 `AddUniqueDynamic`으로 중복 바인딩을 막는다. 스탯 UI(`PlayerHUDWidget`, `StatusWindowWidget`)는 `ICharacterStatInterface` 한 창구를 쓰는 규칙을 다른 UI들이 따를 수 있는 모양이다.
- `OnLeaveClicked`가 `CloseBonfireMenu()` 한 줄로 바뀌어 입력 모드 전환 코드 복사가 한 곳 줄었다(15-3 절반).

### 7) 이번 점검으로 상태가 바뀐 이전 항목

- 해소: 18-6(가이드 헤더 커밋 번호가 `e97f4ee`로 갱신됨), 15-5의 "Git과 디스크 파일명 불일치"(둘 다 `BonFire.*`. 단 `#include` 대소문자는 19-13), 컨트롤러 `Interact()` 중복 정의 정리(가이드 14장의 기록과 일치, 정의는 `Controller.cpp:378` 한 곳), 18-4의 "임베딩" 부분.
- 일부 해소: 9-4(`ApplyDamage` 호출처 생김, 19-6), 9-1(콤보 시스템 병합, 19-11), 15-3(`OnLeaveClicked`), 17-5(`NativeOnInitialized`의 `Super::`).
- 회귀: 13-11(`E` → 화톳불 호출 경로, 19-1).
- 격상: 9-2(락온 null 역참조, `origin/main` 채널 변경으로 후보가 넓어짐, 19-2).
- 새 항목: 19-1 ~ 19-20.

### 8) 우선순위

1. **19-1** 화톳불을 `IInteractableInterface`로 옮긴다. 이 한 가지가 지금 막힌 유일한 기능이다. 옮기면서 `Interact()`에 `ActionState`/UI 확인(15-2)을 넣고 구 인터페이스를 지운다(19-8). 가이드 13-10의 순서와 같다.
2. `origin/main`(`f1051b3`)을 `Bae`로 가져온다. 그 뒤 **19-2**(락온 null)를 함께 고친다. Ahn의 파일이지만 `ECC_Pawn` 변경으로 확률이 올라간 지점이므로 Ahn에게 알릴 것.
3. **19-3, 19-4**(장착 크래시 2건)는 Ahn에게 전달한다. 이번 주 Ahn의 목표(무기·장비 데이터 + 퀵슬롯)에서 처음 만나는 코드이다.
4. 레벨업 위젯을 이어서 만들기 전에 **17-1, 17-2**(검증)와 **19-5**(소울을 얻는 방법)를 정한다. 소울을 얻을 수 없으면 위젯을 완성해도 시험할 수 없다.
5. **19-7**(UI 체계 소유권, 입력 모드, `WBP_Menu`와 스탯창 연결)을 위젯 작업 전에 Ahn과 정한다. `LevelUpWidget`을 어디에 임베드할지가 여기에 달려 있다.
6. 19-10(채널 규칙)과 19-6(데미지 규칙)은 Kang과 정한다. 19-12는 에디터에서 위젯 이름을 확인한다.
7. 가이드 정정(19-20)은 위 코드 변경이 끝난 뒤에 한꺼번에 한다.

---

## 20. 10-26 작업 리뷰 (2026-10-26, 커밋 `0a9a3ac` `8e0d00b` `bb4211c` + 미커밋 작업 트리)

**대상**: 커밋된 변경 — `Interactable/BonFire.*`, `Interface/InteractionInterface.*`(삭제), `Warrior/Warrior.cpp`(include 한 줄). 미커밋 작업 트리 — `Character/CharacterStatComponent.h/.cpp`(함수 순서 재배치, 주 스탯 `int32` 통일), `UI/LevelUpWidget.h/.cpp`(본문 구현, 모달 제거), `UI/StatusWindowWidget.cpp`(주석), `UI/UIHelpers.h` 사용. 비교용으로 `origin/Ahn`(`233d87d`)과의 시험 병합(`git merge-tree`, 커밋된 상태만)과 `PROJECT_GUIDE.md`(디스크 사본)를 같이 확인했다.
빌드와 실행은 하지 않았다. 코드와 가이드는 수정하지 않았다.

**기준 상태**: 로컬 `Bae` = `origin/Bae` = 로컬 `main` = `origin/main` = `bb4211c`. `origin/Ahn`은 `233d87d`("Item Add completed")로 앞서 있다. `PROJECT_GUIDE.md`와 `Review(Claude).md`는 `bb4211c`에서 Git 추적이 빠져 디스크에만 있다(`git status`에 `??`).

**총평**: 19-1(화톳불이 `E`에 반응하지 않던 문제)이 계획대로 해결되었다. `IInteractableInterface` 상속, 오버랩 등록/해제, `Interact(AActor*)` 시그니처, 구 인터페이스 삭제가 모두 코드에 있다. 레벨업 위젯은 골격에서 **동작하는 본문**이 되었고 계산식은 컴포넌트(`GetRequiredSouls`, `GetTotalRequiredSouls`, `CalculateDerivedStats`)와 PlayerState(`ApplyLevelUp`)를 그대로 쓰는 좋은 구조다. 다만 위젯 본문에 **화면에 잘못 나오는 값 하나(20-1)**와 **17-2/17-3 문제가 UI에서 바로 드러나는 경로(20-2)**가 있고, 이 위젯을 열 수 있는 경로가 아직 없다(20-4).

### 1) 확인된 변경

| 항목 | 결과 | 근거 |
|---|---|---|
| 19-1 화톳불 `E` 상호작용 | **해소** | `BonFire.h:7,27,36`(`IInteractableInterface`, `Interact(AActor*)`), `BonFire.cpp:96,112`(`AddDetectedObject`/`RemoveDetectedObject`), `:122-123`(`Cast<AWarrior>`) |
| 19-8 인터페이스 둘 병존 | 해소 | `InteractionInterface.h/.cpp` 삭제, `Warrior.cpp:26` include 교체 |
| 19-2 ~ 19-4 (락온 null, 장착 크래시) | 그대로 | `Warrior.cpp`/`EquipmentComponent.*`는 이번 변경에 없음. `origin/Ahn`에도 수정 없음 |
| 17-1 ~ 17-4, 17-6 | 그대로 | `WarriorPlayerState.*` 변경 없음 |
| 17-5 `LevelUpWidget::NativeConstruct`의 `Super::` | **해소** | `LevelUpWidget.cpp:46` |
| 16-3, 16-4 (파생값 저장, `FName` 분기) | 그대로 | `VirtualLevel`/`TotalRequiredSouls` 저장, `AddPoint`/`SubPoint`가 `FName` 6갈래 `if-else` |
| 16-6 / 19-18 (헬퍼 중복) | 일부 해소 | `LevelUpWidget.cpp:10`이 `UIHelpers.h`를 사용. 스탯창은 여전히 지역 람다 |
| 13-1 / 17-9 (`int32` 안의 `float`) | 일부 해소 | 주 스탯 멤버·getter가 `int32`(`CharacterStatComponent.h:189-199`, `:114-125`). 함수 안의 `float` 잔재는 그대로(20-8) |

### 2) 동작 오류

- [x] **20-1. (높음) 레벨업 창의 "현재 소울"이 레벨을 보여 준다.** (2026-10-26: `LevelUpWidget.cpp:179`가 `BaseSouls`로 수정된 것을 확인함) (`LevelUpWidget.cpp:182`)
  `SetNumberText(TextBlock_CurrentSouls, BaseLevel);` — `BaseLevel`이 아니라 `BaseSouls`여야 한다. 바로 아래 `TextBlock_RemainingSouls`(`BaseSouls - TotalRequiredSouls`)와 비교하면 값이 서로 맞지 않아 눈에 띄는 오류다.
  ```cpp
  // UI/LevelUpWidget.cpp:182
  SetNumberText(TextBlock_CurrentSouls, BaseSouls);
  ```
- [ ] **20-2. (높음 → 보류: 사용자가 레벨 표 밖 구간을 나중에 다루기로 함, 2026-10-26) 레벨 표에 없는 구간과 `MaxValue`가 위젯에서 막히지 않는다. (17-2, 17-3이 UI에서 드러남)**
  `CanAddPoint`(`LevelUpWidget.cpp:75-88`)는 `GetRequiredSouls(VirtualLevel)`이 행이 없어 `0`을 돌려주면 `(TotalRequiredSouls + 0) <= BaseSouls`로 **항상 참**이다. 레벨 10에서 `+`를 누르면(`Level10` 행 = 1200 필요) 그다음 칸부터는 비용 0이라 같은 소울로 무한히 올라간다. 확정 시 `ApplyLevelUp`도 같은 이유로 통과시킨다(17-2). (2026-10-26 정정: `Button_Confirm`과 `CommitLevelUp`의 `TotalRequiredSouls <= 0`(`:149`, `:223`) 판정은 모든 레벨에 비용이 있다면 "투자한 포인트 없음"과 같은 뜻이라 문제가 없다. 비용 0 구간이 생길 때만 어긋나므로 레벨 표를 채우는 작업(17-2)에 묶어서 다룬다. 사용자 의견에 따라 레벨 표 밖 구간은 나중으로 미룸.)
  위젯 쪽 최소 수정과 PlayerState 쪽 수정을 같이 해야 한다.
  ```cpp
  // SaveData/WarriorSaveData.h  FWarriorAttributeData 안에 추가(17-1과 같은 도우미)
  int32 Total() const { return Vigor + Attunement + Endurance + Vitality + Strength + Dexterity; }
  bool HasNegative() const { return Vigor < 0 || Attunement < 0 || Endurance < 0 || Vitality < 0 || Strength < 0 || Dexterity < 0; }
  ```
  ```cpp
  // UI/LevelUpWidget.cpp:83-87  CanAddPoint — 비용 행이 없으면(0) 올릴 수 없다
  const int32 NextLevelCost = StatComp->GetRequiredSouls(VirtualLevel);
  return NextLevelCost > 0 && (TotalRequiredSouls + NextLevelCost) <= BaseSouls;
  // UI/LevelUpWidget.cpp:149  CommitLevelUp
  if (PendingPoints.Total() <= 0) { return; }
  // UI/LevelUpWidget.cpp:223  RefreshUI
  Button_Confirm->SetIsEnabled(PendingPoints.Total() > 0);
  ```
  `ApplyLevelUp`은 이것과 별개로 `HasNegative()`와 비용 `0`(행 없음) 거부를 넣어야 한다(17-1, 17-2). 위젯만 막으면 블루프린트가 `ApplyLevelUp`을 직접 부르는 경로가 열려 있다. `MaxValue`(99)는 `GetStatInitialValue`처럼 `StatDefine`에서 읽는 짝 함수를 컴포넌트에 두고 `AddPoint`와 `ApplyLevelUp` 양쪽에서 확인한다(17-3).
- [x] **20-3. (낮음) (2026-10-26: `CancelLevelUp`이 `InitializedLevelUp()`만 부르고 `SetVisibility`가 제거된 것을 확인함. 숨기기/보이기는 레벨업 창을 여는 쪽을 만들 때 정함) (하향 기록:  화톳불 메뉴를 닫았다 열면(`RemoveFromParent`/`AddToViewport`) 위젯 객체는 그대로지만 `NativeConstruct`가 다시 실행됨. 메뉴를 연 채 취소 후 재진입할 때만 해당하며 열고 닫는 코드를 만들 때 정하면 됨) `CancelLevelUp`이 위젯을 `Collapsed`로 만든다. 다시 보이게 할 때 초기화되지 않는다.** (`LevelUpWidget.cpp:167-174`)
  `NativeConstruct`는 위젯이 만들어질 때 한 번만 불리므로, `Collapsed`로 숨겼다가 `Visible`로 되돌리면 `InitializedLevelUp()`이 다시 실행되지 않는다. 이전 세션의 `BaseSouls`/`BaseLevel`이 그대로 남아 소울 사용량 변동이 반영되지 않은 화면이 열린다(`ApplyLevelUp` 성공 후에는 `CommitLevelUp`이 초기화하지만, 다른 경로로 소울이 바뀌면 안 된다). 또 위젯이 `WidgetSwitcher`의 자식이면 `Collapsed`로 숨기는 것은 스위처가 관리하는 상태와 어긋난다. "취소 = 장바구니 리셋"만 위젯이 하고, 열고 닫기는 부모(`BonfireMenu`)가 하며 열 때마다 `InitializedLevelUp()`을 부르는 편이 16-7의 결론과 맞다.
  ```cpp
  // UI/LevelUpWidget.cpp:167-174
  void ULevelUpWidget::CancelLevelUp()
  {
  	InitializedLevelUp();   // 리셋만. 닫기는 부모가 한다.
  }
  ```

### 3) 설계와 연동

- [x] **20-4. (중간) 이 위젯을 만들거나 여는 곳이 아직 없다.** (2026-10-27: `WBP_LevelUpWidget.uasset`이 `15e944c`에 추가되고 `WBP_BonfireMenuWidget`에 `LevelUpWidget`으로 들어감. 위젯 이름 18개와 `Button_Confirm`/`Button_Cancel`, `Button_AddPoint`/`Button_SubPoint` 각 6개 확인. +/- 버튼이 `AddPoint`/`SubPoint`에 넘기는 이름 철자는 PIE에서 확인)
  `ULevelUpWidget`을 참조하는 C++가 자기 자신뿐이고(`grep`), `Content/UI/`에는 `WBP_LevelUpWidget`이 없다(`WBP_BonfireMenuWidget`, `WBP_PlayerHUD`, `WBP_StatusWindow`만 있음). `OnLevelUpClicked`는 여전히 `ToggleStatusWindow()`를 부른다(`BonfireMenuWidget.cpp:28-35`, 16-9). 결과적으로 지금 위젯 본문은 빌드는 되지만 **게임에서 한 번도 실행되지 않은 코드**다. `BindWidgetOptional`이므로 에셋을 만들 때 위젯 이름(`TextBlock_*`, `Button_Confirm`, `Button_Cancel`)을 헤더와 정확히 맞춰야 하고, `+`/`-` 버튼은 블루프린트에서 `AddPoint(FName)`/`SubPoint(FName)`을 부르는 구조라 이름 문자열(`"Vigor"` 등)을 블루프린트에 손으로 적게 된다(오타가 조용히 무시됨, 20-6).
- [ ] **20-5. (중간) 확정 결과가 다른 화면에 전달되지 않고, 실패 이유도 보이지 않는다.**
  `CommitLevelUp`(`:146-164`)은 성공하면 자기 `InitializedLevelUp()`만 부른다. 화톳불 메뉴에 임베드된 스탯창과 `HUD`(최대치)는 `HUD`만 `UpdateCharacterStat`의 `BroadcastAllStats()`로 갱신되고, **스탯창은 열 때만 갱신되어(16-11, 18-3) 레벨업 직후 옛 값**이 남는다. `OnLevelUpCommitted` 같은 동적 멀티캐스트를 위젯에 하나 두고 화톳불 메뉴가 받아 임베드된 스탯창의 `RefreshStatus()`를 부르게 하면 해결된다. 실패(`ApplyLevelUp`이 `false`)에는 아무 반응이 없고(17-12), 성공 메시지는 `GEngine->AddOnScreenDebugMessage`(`:158`)로 직접 부른다(`GEngine` 검사 없음, 15-7).
- [ ] **20-6. (낮음) 스탯 이름 `FName` 분기가 두 군데 더 늘었다. (= 16-4, 13-6)**
  `AddPoint`(`:99-105`)와 `SubPoint`(`:123-129`)가 같은 6갈래 `if-else`를 각각 가진다. `UpgradeStat`(`PlayerState.cpp:23-29`)까지 합치면 같은 목록이 세 군데다. 여섯 개를 한 줄씩 늘릴 때마다 세 곳을 고쳐야 한다. 이름으로 필드를 찾는 도우미 하나를 두면 `AddPoint`/`SubPoint` 본문이 각각 세 줄로 줄고, 오타(`nullptr`)는 한 곳에서 로그로 잡을 수 있다.
  ```cpp
  // SaveData/WarriorSaveData.h  (또는 UI/UIHelpers.h)
  inline int32* FindAttribute(FWarriorAttributeData& Data, const FName& Name)
  {
  	if (Name == TEXT("Vigor"))      { return &Data.Vigor; }
  	if (Name == TEXT("Attunement")) { return &Data.Attunement; }
  	if (Name == TEXT("Endurance"))  { return &Data.Endurance; }
  	if (Name == TEXT("Vitality"))   { return &Data.Vitality; }
  	if (Name == TEXT("Strength"))   { return &Data.Strength; }
  	if (Name == TEXT("Dexterity"))  { return &Data.Dexterity; }
  	return nullptr;
  }
  // UI/LevelUpWidget.cpp  AddPoint 98-105를 대체
  int32* Point = FindAttribute(PendingPoints, StatName);
  if (!Point) { return; }
  ++(*Point);
  // SubPoint 122-129를 대체
  int32* Point = FindAttribute(PendingPoints, StatName);
  if (!Point || *Point <= 0) { return; }
  --(*Point);
  ```
  나중에 `EAttributeType` 열거형으로 바꾸면(16-4) 이 도우미의 `FName` 매개변수만 열거형으로 바뀐다.

### 4) 정리와 일관성 (낮음)

- [x] **20-7. 주 스탯 멤버의 기본값이 `float` 리터럴이다.** (2026-10-27: `CharacterStatComponent.h:189-199`가 `= 10;`으로 수정됨) (`CharacterStatComponent.h:189-199`) `int32 Vigor = 10.f;`처럼 6개가 `10.f`로 초기화된다. `= 10;`으로 고칠 것(컴파일러 경고 후보). getter 반환형이 `int32`로 바뀌었으므로 `GetVigor()` 등을 블루프린트 노드에서 쓰는 곳이 있으면 핀 타입이 바뀐다(에셋 확인 필요, `WBP_StatusWindow`는 C++ `RefreshStatus`를 써서 영향이 없을 것으로 보임).
- [ ] **20-8. `UIHelpers`와 `float` 잔재.**
  - `using namespace UIHelpers;`가 `LevelUpWidget.cpp:177`에서 함수 정의 한가운데에 있다. 파일 맨 위 include 아래로 옮기거나 `UIHelpers::SetNumberText(...)`로 쓸 것(선언 위치에 따라 읽는 사람이 어디서부터 유효한지 찾아야 한다).
  - `#include "UIHelpers.h"`(`:10`)는 다른 include가 `"UI/..."` 형식인 것과 다르다. `"UI/UIHelpers.h"`로 맞출 것.
  - `RefreshUI`에 `static_cast<int32>(Preview.* / GetPoise())`가 8개 남았다(`:211-218`). `UIHelpers.h`의 `// float...` 자리에 오버로드를 하나 더하면 사라진다(소수 처리 규칙은 한 번만 정할 것).
  - `StatusWindowWidget.cpp:56-61`의 `static_cast<int32>(StatComp->GetVigor())` 등 6개는 getter가 `int32`로 바뀌어 이제 **불필요한 캐스트**이고, 그 아래 `GetMaxHP()` 등 float 쪽 8개만 필요하다. 스탯창은 지역 람다도 그대로라서 `UIHelpers`로 바꾸면 한 곳으로 모인다.
  - 함수 안의 `float` 잔재(17-9)는 그대로다: `CharacterStatComponent.cpp:98,117,127`의 `return 0.f;`(`int32` 반환), `:107`, `:152`의 `static_cast<float>`.
- [ ] **20-9. 컴포넌트 `.cpp` 함수 순서와 헤더 서식.**
  - `CharacterStatComponent.cpp:13-30`에 `BeginPlay`/`TickComponent`가 **생성자(`:34`)보다 위에** 있다. 헤더의 선언 순서(생성자 → `UpdateCharacterStat` → `BeginPlay` …)와도 다르다. 생성자 → 수명주기(`BeginPlay`, `TickComponent`) → 로딩 → 계산 순으로 맞추면 파일을 읽는 순서가 헤더와 같아진다.
  - `UpdateCharacterStat`이 `CalculateDerivedStats` 뒤(`:216`)로 옮겨진 것은 호출 순서와 맞아 읽기 좋다. 뒤에 빈 줄이 여섯 줄 정도 남아 있다.
  - `LevelUpWidget.h`는 끝이 `	};`(탭 + 닫는 괄호)로 서식이 어긋나 있고(`:118`), 모달을 제거하면서 쓰지 않는 전방 선언 `class UWidget;`(`:13`)가 남았다. 빈 줄 구간(`:45-47`)도 정리할 것.
- [ ] **20-10. 화톳불 마이그레이션의 남은 정리.**
  - `Warrior.h:17`(`class IInteractionInterface;`)과 `:150`(`class IInteractionInterface* CurrentInteractable = nullptr;`)이 삭제된 인터페이스를 가리키는 **죽은 멤버**다. 전방 선언만 있어서 빌드는 되지만 지울 것(15-4 해소).
  - `BonFire.cpp:123`의 `if (!Interactor || !CanInteractBonfire(Interactor))`에서 `!Interactor`는 `CanInteractBonfire`가 이미 검사하므로(`:45`) 한 번만 해도 된다. 캐스트 뒤 널 검사는 남기는 편이 읽기 좋아서 취향의 문제다.
  - `:98`, `:114`의 화면 디버그 메시지(`GEngine` 검사 없음, 15-7), `Interact()`에 `ActionState`/UI 열림 검사가 없는 것(15-2), `#include "Interactable/Bonfire.h"`와 파일 `BonFire.h`의 대소문자 불일치(19-13)가 그대로다.

### 5) 병합 미리보기: `origin/Ahn` (`233d87d`)

`git merge-tree --write-tree HEAD origin/Ahn`은 **텍스트 충돌이 없다**(커밋된 상태만 대상. 미커밋 작업 트리는 포함되지 않는다). 충돌은 없지만 확인할 점이 있다.

- [ ] **20-11. 병합 후 확인 목록.**
  1. **`Escape`가 두 곳에 묶인다.** Ahn이 `IA_Menu`로 메뉴 열기/닫기를 추가했고(`ToggleMenuWindow`, `WarriorController.cpp` Ahn 브랜치 `:553-580`), 헤더 주석이 "화톳불 메뉴에서 ESC를 눌러도 닫고 메뉴 창을 열고 닫을 때도 ESC를 눌러야 하니까 한 번에 묶는 함수를 만들어야 한다"고 적었다. Bae의 `Escape`/`Q` → `CloseBonfireMenu`(`:208-209`)와 합치면 같은 키에서 두 동작이 일어난다. `IMC_Warrior`에서 `IA_Menu`의 키는 에셋이라 확인하지 못했다. 19-7의 "열려 있는 UI 중 가장 위 하나만 닫기"가 이제 실제로 필요하다. `ToggleMenuWindow`와 `ToggleStatusWindow`/`OpenBonfireMenu`가 모두 입력 모드를 직접 `GameAndUI`/`GameOnly`로 바꾸므로(15-1 그대로) 화톳불 메뉴가 열린 상태에서 `IA_Menu`로 인벤토리를 열면 둘이 동시에 떠 있고, 둘 중 하나를 닫으면 나머지가 마우스 없이 남는다.
  2. **`SetInventory`가 `BeginPlay`에서 폰이 없으면 건너뛴다.** Ahn의 `AWarriorController::BeginPlay`가 `Cast<AWarrior>(GetPawn())` 뒤에 `MenuWidget->SetInventory(...)`를 부르는데(Ahn 브랜치 `:153-155`), 첫 플레이어는 월드 `BeginPlay` 전에 폰이 이미 생성되는 경우가 많지만(PIE 기준 추정, 2026-10-26 정정: 앞서 "보통 빙의 전"이라고 단정한 것은 과했다), 리스폰·레벨 이동·나중에 접속하는 경우에는 `BeginPlay`가 빙의보다 먼저일 수 있다. 폰이 `nullptr`이면 인벤토리가 연결되지 않아 메뉴의 아이템 목록이 비어 있게 된다(PIE에서 확인 필요). 이미 있는 `OnPossess`(`WarriorController.cpp:473-485`, 우리 HUD 바인딩이 쓰는 곳)로 옮기는 것이 안전하다. Ahn의 파일이므로 병합 전에 알릴 것.
  3. **Ahn 브랜치는 19-2 ~ 19-4를 아직 고치지 않았다.** `Warrior.cpp`는 include 한 줄, `EquipmentComponent.*`는 변경이 없다. 병합하면 락온 null, 장착 어서션이 그대로 들어온다.
  4. **`FInventoryItem::ItemData` → `FPrimaryAssetId AssetID`로 바뀌었다.** 순환 참조와 직렬화에 더 안전한 방향이다. 다만 `InventoryComponent.cpp:23`의 `ItemData->InstanceID = AddedItem.InstanceID;`(공유 애셋 수정, 19-9)는 그대로다. `GetItemData`가 매 호출마다 `LoadSynchronous`를 부르므로 목록을 그릴 때 호출 빈도를 확인할 것.
  5. `WBP_Menu.uasset`(98,713 → 47,700바이트), `IMC_Warrior.uasset`, `WBP_MenuButton.uasset`는 Ahn만 바꾼 바이너리라 충돌은 없다. 같은 에셋을 둘이 고치지 않는 규칙을 유지할 것.

### 6) 저장소와 문서

- [ ] **20-12. 저장소와 추적 상태.**
  - `6eed7f1`("발표")이 `ZZakTungSoul_Pitch_KusoGames_min7.pptx`(약 60MB), `cardboard_knight.gif`(약 7.6MB), PNG 4개를 추가했고 `bb4211c`가 삭제했다. 이미 `origin/Bae`에 올라갔으므로 **Git 기록에 계속 남는다**(`.git` 약 1.6GB → 1.7GB). 100MB 제한은 넘지 않았다. 다음부터 발표 자료는 저장소 밖에 둘 것(9-17의 LFS/`.gitignore` 논의와 같은 주제).
  - 저장소 루트에 `KusoGames_logo.png`, `cardboard_knight_edited_v2.png`가 추적된 채 남아 있다. 필요하면 `Docs/` 같은 한 폴더로 옮기고 아니면 지울 것.
  - `PROJECT_GUIDE.md`와 `Review(Claude).md`가 Git에서 빠졌다. 의도한 정리(문서는 개인 파일, `main`에 올리지 않음)지만, 이제 **이 두 파일의 기록은 디스크에만 있다.** 가끔 다른 폴더에 복사본을 둘 것.
- [ ] **20-13. 가이드와 코드의 불일치 (10-26 갱신분).**
  - 0장 "주 스탯 → 최종 스탯 계산"의 "불필요한 `static_cast` 완전 제거" → `StatusWindowWidget.cpp:56-61`과 `CharacterStatComponent.cpp:107,152`에 남아 있다(20-8).
  - 0장 "화톳불 레벨업 서브 메뉴 ✅ 구현 (C++ 완료 / 2026-10-26 검증)" → 14장의 2순위("`WBP_LevelUpWidget` 제작/검증")와 모순이고, `WBP_LevelUpWidget`이 없어 실행 검증이 불가능하다. 20-1 ~ 20-3도 남아 있다. "🟡 C++ 본문 구현, 위젯 에셋·검증 필요"가 맞다.
  - 13장 8절(`:950`)의 "[결정] → `Panel_ConfirmModal` 팝업 → [예]" 서술은 모달 제거(`LevelUpWidget.h`) 후 낡았다.
  - 0장 "상호작용 인터페이스 및 화톳불 액터 ✅ (인게임 검증)" → 코드 경로는 리뷰어가 확인했다(19-1 해소). 인게임 동작은 리뷰어가 실행하지 못했으므로 사용자 보고로 취급한다.
  - 아직 그대로: 0장 "피격/데미지 ❌ 미구현"(`EnemyBase.cpp:141`이 `ApplyDamage` 호출, 19-6), "장비 및 인벤토리 ✅"(19-3, 19-4), "레벨업 ✅ 완비"(17-1 ~ 17-3), 12장 `GetMind()`/`GetIntelligence()`(19-20), `GetPreviewStats`(18-5), 헤더 커밋 `6eed7f1`(현재 `bb4211c`).

### 7) 다음 작업(가이드 1순위: 점화 목록 저장) 구현 전 확인

가이드 14장의 1순위는 점화한 화톳불 ID를 PlayerState/SaveGame에 등록하는 것이다. 구현 전에 정할 것이 네 가지 있다.
1. **`BonfireID`가 유일하지 않다.** (`BonFire.h:73-74`) 기본값 `Bonfire_Default`가 모든 배치 인스턴스에 같다. 레벨에 둘 이상 배치하면 한쪽을 밝혀도 둘 다 `Lit` 목록에 같은 이름으로 들어간다. 배치 후 인스턴스마다 다르게 적는 규칙을 정하거나(누락하면 `UE_LOG(Error)`), 레벨에 고정된 액터 이름(`GetFName()`)을 쓸지 정한다.
2. **저장 위치.** `FWarriorSaveData`(`WarriorSaveData.h:32-53`)에 `TArray<FName> LitBonfireIDs`를 추가하면 기존 세이브 파일에는 빈 배열로 읽힌다(호환 문제 없음). 같은 구조체에 이미 쓰이지 않는 `CurrentHP`/`CurrentFP`(17-6)가 있으니 저장 항목을 한 번에 정리할 것.
3. **복원 시점.** `ABonfire::BeginPlay`(`:34-41`)는 `PlayerState`에 세이브 데이터가 로드되기 전에 실행될 수 있다. 화톳불이 `BeginPlay`에서 읽어 `Lit`으로 만들지, 점화 목록이 바뀔 때 `PlayerState`가 방송해 화톳불이 구독하는지를 정한다. 후자가 `LoadGame()`이 폰을 갱신하지 않는 문제(7-6)와도 같은 방식으로 풀린다.
4. **텔레포트의 범위.** 같은 레벨 안 이동인지 레벨 전환인지(16-8). 같은 레벨이면 `BonfireID` → 월드의 화톳불 액터를 찾는 방법이 필요하다(월드의 `TActorIterator<ABonfire>` 또는 레벨 시작 시 등록하는 매니저).

### 8) 확인된 좋은 점

- 화톳불 마이그레이션이 계획대로 최소 변경(`BonFire.h` 3줄, `.cpp` 몇 줄)으로 끝났고 구 인터페이스를 지웠다. 두 인터페이스가 공존하던 기간이 하루도 가지 않았다.
- 레벨업 위젯이 `GetTotalRequiredSouls(BaseLevel, VirtualLevel)`로 누적 비용을 매번 처음부터 계산해서(`:113`, `:137`) 증감을 반복해도 오차가 쌓이지 않는다. 표시용 계산과 확정용 검증이 같은 함수를 쓴다.
- `RefreshUI`가 `CalculateDerivedStats(VirtualAttributes)`를 부르고 이 함수가 `UpdateCharacterStat`과 같은 식이라 "보이는 값 = 확정 후 값"이 구조로 보장된다.
- `ApplyLevelUp` 성공 후 `InitializedLevelUp()`으로 `PlayerState`에서 다시 읽어 와서 위젯의 `Base*`와 실제 값이 어긋나지 않는다.
- 모달 팝업을 걷어내서 위젯 상태가 단순해졌다(`Panel_ConfirmModal`, 예/아니오 버튼 3개와 함수 2개 제거).

### 9) 이번 점검으로 상태가 바뀐 이전 항목

- 해소: 19-1(화톳불 `E`), 19-8(인터페이스 둘 병존), 17-5(`Super::`), 15-4(`CurrentInteractable` 약참조 문제는 죽은 멤버 삭제로 해소됨 — 삭제는 20-10에서 남음), 16-6/19-18의 `UIHelpers` 미사용.
- 일부 해소: 17-9/13-1(`int32` 안의 `float`), 16-6(스탯창 람다는 그대로).
- 그대로: 19-2 ~ 19-7, 19-9 ~ 19-17, 17-1 ~ 17-4, 17-6, 15-1, 15-2, 19-13.
- 새 항목: 20-1 ~ 20-13.

### 10) 우선순위

1. **20-1**(소울 표시)은 한 줄이다. 레벨업 창을 에셋으로 만들기 전에 고친다.
2. **20-2**와 17-1, 17-2, 17-3을 한 묶음으로 처리한다. 위젯(`CanAddPoint`, 확정 조건)과 `ApplyLevelUp`(음수, 비용 0, `MaxValue`)을 같이 막아야 어느 경로로 호출해도 안전하다. `FWarriorAttributeData`에 `Total()`/`HasNegative()` 도우미가 두 쪽에서 모두 쓰인다.
3. **20-3, 20-4, 20-5**: `WBP_LevelUpWidget`을 만들고 `BonfireMenu`에서 열고 닫는 부모를 정한다. 취소/닫기, 열 때 초기화, 확정 후 스탯창 갱신을 한 번에 정할 것.
4. 병합 전에 **20-11**의 `Escape` 중복과 `SetInventory` 위치를 Ahn에게 전달한다. 19-2 ~ 19-4도 같이 전달한다.
5. 점화 목록을 구현하기 전에 **7)**의 네 가지(특히 `BonfireID` 유일성)를 정한다.
6. 20-6 ~ 20-10은 정리 작업으로 묶는다. 20-12의 문서 백업은 지금 해 두는 것이 좋다.

---

## 21. 화톳불 점화 목록 저장과 텔레포트 이동 리뷰 (2026-10-26, 미커밋 작업 트리)

**대상**: `SaveData/WarriorSaveData.h`(`LitBonfireIDs`), `Warrior/WarriorPlayerState.*`(`RegisterLitBonfire`, `IsBonfireLit`, `GetLitBonfireIds`), `Interactable/BonFire.*`(`TeleportPoint`, `GetTeleportTransform`, 최초 점화 시 등록), `Warrior/WarriorController.*`(`TeleportToBonfire`). `LevelUpWidget.*`, `CharacterStatComponent.*`, `StatusWindowWidget.cpp`는 직전 리뷰(섹션 20) 이후 변경이 없다. 기준 커밋은 `bb4211c`이며 `origin/Ahn`과의 관계는 섹션 20 5)와 같다.
빌드와 실행은 하지 않았다. `Content/Blueprints/Interactable/BP_Bonfire.uasset`과 `Content/Maps/TestMap.umap`은 10-23 이후 수정되지 않았다(파일 시간). 코드는 수정하지 않았다.

**총평**: 섹션 20 7)에서 정하자던 네 가지 중 (2)저장 위치와 (4)텔레포트 범위가 구현되었다. 점화 목록을 `FWarriorSaveData`에 두어 기존 세이브 파일은 빈 배열로 읽히고, 등록은 중복을 막고 `None`을 거부하며, 텔레포트는 `ID → 화톳불 액터 → 착지점 → 이동 → 카메라 정렬 → 메뉴 닫기` 순서가 주석 그대로 읽힌다. 남은 문제는 **(1)`BonfireID`가 아직 유일하지 않다는 것과 (3)복원 시점**이다. 이 둘은 지금 코드에서 이미 동작을 틀리게 만든다.

### 1) 동작 오류 (높음)

- [ ] **21-1. (2026-10-26 정정: 코드 결함이 아니라 배치 규칙이다. `BonfireID`가 `EditAnywhere`이므로 레벨에 배치한 인스턴스마다 디테일 패널에서 서로 다른 값을 적으면 해결된다(`BP_Bonfire` 기본값이 아니라 배치된 인스턴스에 적을 것). 아래 `BeginPlay` 로그는 깜빡하는 실수를 바로 잡기 위한 선택적 안전장치) `BonfireID`가 전부 같아서 점화 목록과 텔레포트가 첫 번째 화톳불에만 맞는다. (섹션 20 7)의 (1), 이제 코드가 의존함)**
  `BonFire.h:77`의 기본값이 `Bonfire_Default`이고 `TestMap.umap`에는 `Bonfire_C_1`이 하나 배치되어 있을 뿐 ID를 다르게 지정한 흔적이 없다(문자열 검색). 화톳불을 둘 이상 배치하면
  - 한쪽을 밝히는 순간 `RegisterLitBonfire`(`BonFire.cpp:148`)가 `Bonfire_Default`를 등록하고, 다른 쪽도 이미 목록에 있는 것으로 취급된다(`WarriorPlayerState.cpp:123`의 `Contains`).
  - `TeleportToBonfire`(`WarriorController.cpp:562-569`)는 `TActorIterator`가 처음 만나는 화톳불에서 `break`한다. 어느 쪽을 골라도 같은 화톳불로 이동한다.
  최소한의 방어로 `BeginPlay`에서 기본값/`None`일 때 로그를 남겨 배치할 때 바로 눈에 띄게 한다. 레벨에 배치된 화톳불 중복 검사는 `TActorIterator`로 한 번 돌리면 된다.
  ```cpp
  // Interactable/BonFire.cpp  BeginPlay() 끝(:51-56 부근)
  if (BonfireID.IsNone() || BonfireID == TEXT("Bonfire_Default"))
  {
  	UE_LOG(LogTemp, Error, TEXT("[Bonfire] %s: BonfireID가 기본값입니다. 인스턴스마다 다르게 지정하세요."), *GetName());
  }
  ```
  (`"Bonfire_Default"` 문자열은 `BonFire.h:77`과 같아야 하므로 상수 하나로 묶을 것.)
- [x] **21-2. (2026-10-27: `SyncLitState`가 `BonFire.cpp:113-121`에 추가되어 구체 진입(`:131`)과 `Interact` 시작(`:161`)에서 불림을 확인함. 권장안인 `LoadGame` 순회 대신 처음 제안한 방식이 적용됨) 점화 상태가 복원되지 않는다. 불러오기 후 점화한 화톳불이 `Unlit`으로 보이고 두 번 눌러야 쉴 수 있다. (섹션 20 7)의 (3))**
  `PlayerState`에는 목록이 저장되지만 화톳불 액터의 `BonfireState`(`BonFire.h:88`)는 매번 `Unlit`에서 시작하고, `IsBonfireLit`을 부르는 곳이 없다(`WarriorPlayerState.cpp:106` 정의뿐, `grep`). 그래서 목록에는 있는데 화톳불은 꺼져 있는 상태가 생긴다: 불러오기(`LoadGame`)나 게임을 다시 시작한 뒤 그 화톳불에서 `E`를 누르면 `Unlit` 분기(`BonFire.cpp:141-155`)가 먼저 실행되어 **점화만 하고 반환**(`return`)한다. 휴식과 메뉴는 `E`를 한 번 더 눌러야 열린다. 텔레포트 목록(`GetLitBonfireIds`)에는 이미 올라 있으므로 화면과 목록이 어긋난다.
  `BeginPlay`는 `LoadGame()`(키 입력으로 실행됨)보다 먼저 실행되므로 거기서 읽으면 안 되고, **플레이어가 구체에 들어올 때와 `Interact` 시작 시** `PlayerState`를 보고 맞추는 것이 가장 단순하다.
  ```cpp
  // Interactable/BonFire.h  (protected 구획)
  void SyncLitState(const AWarriorPlayerState* PS);
  // Interactable/BonFire.cpp
  void ABonfire::SyncLitState(const AWarriorPlayerState* PS)
  {
  	if (BonfireState == EBonfireState::Unlit && PS && PS->IsBonfireLit(BonfireID))
  	{
  		BonfireState = EBonfireState::Lit;   // 불꽃 연출(파티클)은 여기서 같이 켠다.
  	}
  }
  // OnInteractionSphereBeginOverlap  Cast 성공 안, AddDetectedObject 다음 줄
  SyncLitState(Warrior->GetPlayerState<AWarriorPlayerState>());
  // Interact  Cast/CanInteractBonfire 통과 직후, "최초 상호작용" 분기 앞(:139 부근)
  SyncLitState(Interactor->GetPlayerState<AWarriorPlayerState>());
  ```
  불러오기가 폰의 스탯을 갱신하지 않는 문제(7-6)와 같은 원인(`LoadGame`이 `PlayerState`만 덮어씀)이므로, `LoadGame` 끝에서 알림을 보내는 방식으로 한꺼번에 해결할 수도 있다. 시점을 정할 때 같이 볼 것.
  **권장안(2026-10-26 논의 결과)**: 위의 구체 진입/`Interact` 호출 대신 `AZTSGameMode::LoadGame()`(`ZTSGameMode.cpp:90`, `LoadSaveDataFrom` 다음)에서 `TActorIterator<ABonfire>`로 레벨의 화톳불을 한 번씩 순회하며 `SyncLitState(PlayerState)`를 부른다(`SyncLitState`는 public, `<EngineUtils.h>`와 `<Interactable/BonFire.h>` include). 불러오는 순간 모든 화톳불이 맞춰져서 접근 전에도 연출이 맞고, 시작 시점(`BeginPlay`)에는 저장 목록이 없으므로 따로 처리할 필요가 없다. 알려야 할 대상이 늘어나면(캐릭터 스탯 7-6, 스탯창 18-3) 그때 `AWarriorPlayerState`에 `OnSaveDataLoaded` 델리게이트를 두고 순회를 방송으로 바꾸면 된다(`SyncLitState`는 그대로 사용).

### 2) 설계와 안전성 (중간)

- [x] **21-3. `TeleportToBonfire`가 점화 여부를 검증하지 않고 이동 실패를 확인하지 않는다.** (2026-10-27: `WarriorController.cpp:558-564` 점화 검사, `:594-597` 캡슐 반높이 보정, `:600-604` `TeleportTo` 결과 확인이 모두 들어감) (`WarriorController.cpp:548-593`)
  1. **점화되지 않은 화톳불로도 이동한다.** 함수가 `BlueprintCallable`이라 블루프린트에서 아무 ID나 넘기면 이동한다. 메뉴가 점화 목록으로만 버튼을 만들더라도, 검증을 호출하는 쪽에만 두면 `ApplyLevelUp`(17-1)과 같은 구멍이 생긴다. 함수 안에서 한 번 더 확인할 것.
  2. **`TeleportTo`의 반환값을 무시한다**(`:582`). 목적지에 다른 물체가 겹치면 이동이 실패하는데 그래도 카메라를 돌리고 메뉴를 닫고 "이동 완료" 메시지를 띄운다.
  3. **착지점 높이.** `TeleportPoint`의 위치는 화톳불 루트 기준 (120, 0, 0)이라 바닥 높이다. 캐릭터의 위치는 캡슐 중심이므로 바닥에 겹쳐서 2번으로 실패하거나 땅에 박힐 수 있다. 캡슐 반높이를 더해 줄 것(PIE에서 확인 필요).
  ```cpp
  // Warrior/WarriorController.cpp  TeleportToBonfire  (:548 시작부)
  AWarriorPlayerState* PS = GetPlayerState<AWarriorPlayerState>();
  if (!PS || !PS->IsBonfireLit(TargetBonfireID)) { return false; }
  // (:578-582)
  FVector TargetLocation = TargetTransform.GetLocation();
  TargetLocation.Z += Warrior->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
  if (!Warrior->TeleportTo(TargetLocation, TargetRotation)) { return false; }
  ```
  `#include <Components/CapsuleComponent.h>`와 `<Warrior/WarriorPlayerState.h>`가 필요하다. 이미 열린 메뉴 중에 이동해도 되는지(적이 가까운 곳으로 이동 등)는 기획 결정이다. 지금은 목적지의 `CanInteractBonfire`와 같은 검사를 하지 않는다.
- [x] **21-4. 서브오브젝트 이름 오타와 컴포넌트 포인터의 `EditAnywhere`.** (2026-10-27: 이름 오타 수정과 `BonFire.h:87`의 `VisibleAnywhere` 변경을 확인함. 쓰지 않는 `GetTeleportPoint()`만 남음) (`BonFire.cpp:34`, `BonFire.h:83-84`)
  - (2026-10-26 수정 확인: `BonFire.cpp:34`가 `TEXT("TeleportPoint")`로 고쳐짐. 아래는 수정 전 기록) `CreateDefaultSubobject<UArrowComponent>(TEXT("TeleportPOint"))` — `POint`의 대소문자 오타다. 이 이름이 `BP_Bonfire`와 레벨의 인스턴스에 직렬화된다. `BP_Bonfire.uasset`과 `TestMap.umap`이 아직 이 컴포넌트를 저장하지 않은 지금(10-23 이후 수정 없음) 고치면 비용이 없고, 저장한 뒤에는 이름을 바꾸면 BP의 위치 조정값이 사라진다. `TEXT("TeleportPoint")`로 맞출 것.
  - `UPROPERTY(EditAnywhere, BlueprintReadOnly) TObjectPtr<UArrowComponent> TeleportPoint;`는 다른 컴포넌트 두 개(`MeshComponent`, `InteractionSphere`)와 달리 `EditAnywhere`다. 컴포넌트 포인터는 `VisibleAnywhere`(+`BlueprintReadOnly`)로 하고, 위치는 컴포넌트의 트랜스폼으로 조정한다. `EditAnywhere`면 디테일 패널에서 포인터 자체를 바꿀 수 있어 `nullptr`이 되기 쉽다(그래서 `GetTeleportTransform()`에 `if (TeleportPoint)` 분기가 필요해진다).
  - `GetTeleportPoint()`(`BonFire.h:45`)와 `GetTeleportTransform()`(`:46`)이 둘 다 열려 있다. 컨트롤러는 후자만 쓴다. 전자는 쓰는 곳이 없어서 14-5의 "쓰지 않는 getter"가 하나 늘었다.

### 3) 정리 (낮음)

- [ ] **21-5. 세부 정리.**
  - `WarriorPlayerState.h:32`의 주석 `// 화톳불 `이 미완성이고 `RegisterLitBonfire(const FName & BonfireID)`의 `&` 앞뒤 공백, `.cpp` 끝의 빈 줄 두 줄(`:129-131`)이 정리되지 않았다. `GetLitBonfireIds`(`Ids`)와 `BonfireID`(`ID`)의 표기가 다르다.
  - `RegisterLitBonfire`의 반환값(새로 등록했는지)을 `Interact`가 쓰지 않는다(`BonFire.cpp:148`). 쓰지 않을 거면 `void`가 읽기 쉽고, "처음 밝힘" 연출에 쓰려면 반환값으로 분기한다.
  - `TeleportToBonfire`가 `Category = "UI|Bonfire"`에 있지만 UI 동작이 아니라 이동이다. `Category = "Bonfire"`가 맞다. `FString::Printf`의 `GetBonfireDisplayName()`은 기본값이 빈 `FText`라서 이름을 지정하지 않으면 " (으)로 이동 완료"만 나온다. 화면 디버그 메시지(`GEngine`, 15-7)는 그대로다.
  - `BonFire.cpp`의 `BeginPlay` 뒤 빈 줄 세 줄, `BonFire.h` 끝의 빈 줄 두 줄.
  - `OnTeleportClicked`(`BonfireMenuWidget.cpp:37`)는 아직 디버그 메시지이고 `UTeleportWidget`도 빈 클래스라서(16-8) 이동 함수를 부르는 쪽이 없다. 목록을 만들 때 `GetLitBonfireIds()`에서 `BonfireID` → 표시 이름을 얻는 방법이 필요하다(화톳불 액터를 `TActorIterator`로 찾아 `GetBonfireDisplayName()`을 읽거나, 표시 이름도 저장). 현재 구조는 ID만 저장하므로 레벨 전환을 하면 목록에 있는 화톳불이 월드에 없을 수 있다.

### 4) 변동 없음 (코드 확인)

`WarriorPlayerState.cpp`의 `ApplyLevelUp`(17-1 음수, 17-2, 17-3)와 `UpgradeStat`은 변경이 없다(이번 diff는 아래에 함수 두 개를 추가한 것뿐). 섹션 20의 20-2(레벨 표 밖 구간, 보류), 20-4 ~ 20-13도 그대로다. 락온 null(19-2), 장착 크래시(19-3, 19-4)는 `Warrior.cpp`/`EquipmentComponent.*` 변경이 없으므로 그대로다.

### 5) 확인된 좋은 점

- 점화 목록이 `FWarriorSaveData`에 들어가서 기존 세이브와 호환되고(`TArray`는 기본값 빈 배열) 별도 저장 코드 없이 기존 `SaveGame`/`LoadGame` 경로에 실린다.
- `RegisterLitBonfire`가 `None`과 중복을 거부한다. 같은 화톳불을 다시 밝혀도 목록이 늘지 않는다.
- 착지점을 `UArrowComponent`로 두어 레벨 디자이너가 에디터에서 위치와 방향을 눈으로 보고 조정할 수 있다. 착지점이 없을 때 화톳불 자신의 트랜스폼으로 되돌아가는 대체 경로도 있다.
- 텔레포트 순서가 이동 → 카메라 → 메뉴 닫기로 정리되어 있어, 이동 후 마우스 커서가 남는 문제(15-1)를 `CloseBonfireMenu`가 처리한다.

### 6) 이번 점검으로 상태가 바뀐 이전 항목

- 일부 해소: 16-8(텔레포트 구현 전 결정): 저장 위치(`LitBonfireIDs`)와 월드에서 화톳불 찾기(`TActorIterator`)는 정해졌고, 레벨 전환 여부와 목록 UI는 남음. 섹션 20 7)의 (2), (4).
- 새 항목: 21-1 ~ 21-5. 섹션 20 7)의 (1)은 21-1, (3)은 21-2로 이어진다.

### 7) 우선순위

1. ~~**21-4의 이름 오타**(`TeleportPOint`)~~ — 2026-10-26 수정 확인(`BonFire.cpp:34`). 남은 21-4는 `TeleportPoint`의 `EditAnywhere` → `VisibleAnywhere`와 쓰지 않는 getter.
2. **21-1**(`BonfireID` 유일성): 화톳불을 두 번째로 배치하기 전에 `BeginPlay` 로그를 넣고 배치된 화톳불에 ID를 적는다.
3. **21-2**(점화 상태 동기화): `SyncLitState`를 구체 진입과 `Interact` 시작에 둔다. 이 코드가 있어야 불러오기 후에도 화톳불이 맞게 동작한다.
4. **21-3**(`TeleportToBonfire` 검증과 `TeleportTo` 결과 확인)는 텔레포트 UI를 연결하기 전에 넣는다. 연결한 뒤에 넣으면 시험 결과를 두 번 확인해야 한다.
5. 21-5와 이전 항목(17-1, 20-2 보류분)은 UI 연결 때 함께 정리한다.

---

## 22. 10-27 작업 리뷰와 프로젝트 전체 재점검 (2026-10-27, `Bae` `b61f76b` + 미커밋 작업 트리)

**대상과 방법**: `Source/` 전체 7,780줄(`.cpp`/`.h` 약 100개)을 다시 읽었다. 오늘 변경은 미커밋 수정 11개(`CharacterStatComponent.h`, `Interactable/BonFire.*`, `UI/BonfireMenuWidget.*`, `UI/LevelUpWidget.*`, `UI/StatusWindowWidget.cpp`, `UI/TeleportWidget.*`, `Warrior/WarriorController.cpp`)와 새 파일 `UI/BonfireMenuButtonWidget.*`이다. 커밋 `b61f76b`(10-26 17:37)는 섹션 20, 21에서 본 내용을 그대로 담고 있다. 에셋(`.uasset`)은 열 수 없어서 파일 목록으로만 확인했다. 빌드와 실행은 하지 않았다. 코드는 수정하지 않았다.

**총평**: 섹션 21이 권고한 세 가지(점화 상태 동기화 21-2, `TeleportToBonfire` 검증 21-3, 서브오브젝트 정리 21-4)가 그대로 코드에 들어갔다. 화톳불 메뉴가 레벨업과 텔레포트를 임베드한 서브메뉴로 열고 닫는 구조도 C++로 완성되었다(16-8, 16-9). 이제 막힌 것은 코드가 아니라 **에셋**이다. `Content/UI/`에는 `WBP_BonfireMenuWidget`, `WBP_PlayerHUD`, `WBP_StatusWindow`뿐이어서 레벨업 창, 텔레포트 창, 텔레포트 버튼 위젯의 블루프린트가 없다(22-1). 그 밖에 오래된 높은 항목(락온 null, 장착 크래시, 레벨업 검증)은 한 줄도 바뀌지 않았다(5절).

### 1) 오늘 변경 확인

| 항목 | 결과 | 근거 |
|---|---|---|
| 21-2 점화 상태 복원 | **해소** | `BonFire.cpp:113-121`의 `SyncLitState`를 구체 진입(`:131`)과 `Interact` 시작(`:161`)에서 호출 |
| 21-3 텔레포트 검증 | **해소** | `WarriorController.cpp:558-564`(점화 확인), `:594-597`(캡슐 반높이), `:600-604`(`TeleportTo` 결과) |
| 21-4 컴포넌트 포인터 | **해소** | `BonFire.h:87`이 `VisibleAnywhere`. 쓰지 않는 `GetTeleportPoint()`(`:46`)만 남음 |
| 21-1 `BonfireID` 기본값 | 안전장치 추가 | `BonFire.cpp:57-61`이 기본값이면 경고 로그. 인스턴스마다 다른 ID를 적는 배치 규칙은 에디터에서 확인 |
| 20-7 주 스탯 `= 10.f` | **해소** | `CharacterStatComponent.h:189-199` |
| 20-8 일부 (스탯창 캐스트) | 일부 해소 | `StatusWindowWidget.cpp:55-60`의 주 스탯 6개 캐스트 제거. 지역 람다(`:28-34`)와 float 캐스트 8개(`:63-72`)는 그대로 |
| 16-8, 16-9 텔레포트, 서브메뉴 | **해소** | `TeleportWidget.cpp:34-119`, `BonfireMenuWidget.cpp:43-83` |
| 20-4 위젯을 여는 곳 | 코드는 해소, 에셋 없음 | `OnLevelUpClicked`(`BonfireMenuWidget.cpp:43-55`)가 임베드한 `LevelUpWidget`을 열도록 바뀜. 에셋은 22-1 |

### 2) 새 항목

- [x] **22-1. (높음, 확인 필요) 서브메뉴 위젯의 블루프린트 에셋이 없어서 버튼이 아무 일도 하지 않는다.** (2026-10-27: 에셋 3개와 텔레포트 항목 위젯이 `15e944c`에 추가됨. 이름은 `WBP_TeleportWidget`에서 `Button_Cancel`만 빠진 것을 빼고 모두 헤더와 일치. 로그 추가는 보류. 섹션 24의 24-1)
  `BonfireMenuWidget`의 `LevelUpWidget`, `TeleportWidget`(`BonfireMenuWidget.h:48-52`)은 `BindWidgetOptional`이라 `WBP_BonfireMenuWidget`에 같은 이름의 자식 위젯이 없으면 조용히 `nullptr`이다. `OnLevelUpClicked`/`OnTeleportClicked`가 `if (LevelUpWidget)`/`if (TeleportWidget)`으로 감싸 있어서 에러 없이 아무 일도 안 한다. `Content/UI/`에는 `WBP_LevelUp...`, `WBP_Teleport...`가 없고(`ls` 확인), 텔레포트 목록의 각 항목은 `TeleportWidget`의 `BonfireButtonClass`(`TeleportWidget.h:61`, `EditDefaultsOnly`)에 블루프린트 클래스를 지정해야 하는데 그 버튼용 위젯 블루프린트도 없다. 지정하지 않으면 `RefreshTeleportList`가 로그 없이 반환한다(`TeleportWidget.cpp:36-39`). 에셋을 만들 때 위젯 이름과 클래스를 헤더와 정확히 맞추고, 아래 두 곳에 로그를 남겨 두면 에디터에서 원인을 바로 찾는다.
  ```cpp
  // TeleportWidget.cpp:36-39
  if (!ScrollBox_BonfireList) { UE_LOG(LogTemp, Warning, TEXT("[Teleport] ScrollBox_BonfireList 위젯 이름이 없습니다.")); return; }
  if (!BonfireButtonClass)    { UE_LOG(LogTemp, Warning, TEXT("[Teleport] BonfireButtonClass가 비었습니다.")); return; }
  ```
  `BonfireMenuWidget.cpp:30-40`의 `NativeOnInitialized`에도 `LevelUpWidget`, `TeleportWidget`이 null이면 경고를 남길 것(15-8과 같은 문제).
- [x] **22-2. (중간) 화톳불 메뉴를 닫았다 다시 열면 마지막에 보던 서브메뉴가 그대로 남는다.** (2026-10-27: `BonfireMenuWidget.cpp:41-48`의 `NativeConstruct`가 `ShowSubMenu(nullptr)`로 초기화함을 확인)
  서브메뉴를 `Collapsed`로 만드는 곳은 `NativeOnInitialized`(`BonfireMenuWidget.cpp:33,38`)와 각 클릭 핸들러뿐이다. `Esc`/`Q`/`떠나기`/텔레포트 후 `CloseBonfireMenu()`는 메뉴를 `RemoveFromParent()`만 하므로(`WarriorController.cpp:541-549`) 레벨업 창을 연 채로 닫으면 다시 열었을 때 레벨업 창이 이미 보이고, `LevelUpWidget`의 `PendingPoints`와 `BaseSouls`도 이전 값 그대로다(`InitializedLevelUp`은 `OnLevelUpClicked`에서만 부름). 열릴 때마다 실행되는 `NativeConstruct`에서 서브메뉴를 모두 숨기는 것이 가장 간단하다(22-3의 `ShowSubMenu(nullptr)`).
- [x] **22-3. (중간) 서브메뉴 보이기/숨기기가 세 함수에 복사되어 있다.** (2026-10-27: `ShowSubMenu`가 추가되고 세 핸들러가 한두 줄로 줄었으며 `OnTeleportClicked`의 디버그 메시지도 지워짐을 확인. `BonfireMenuWidget.cpp:50-85`) (`BonfireMenuWidget.cpp:43-83`)
  `OnLevelUpClicked`, `OnTeleportClicked`, `OnSubMenuClosed`가 모두 "둘 다 `Collapsed`로 한 뒤 하나만 `Visible`" 코드를 각자 가진다. 서브메뉴가 하나 더 늘 때마다 세 함수를 모두 고쳐야 한다. 대상 하나를 받는 함수로 묶으면 각 핸들러가 두 줄이 된다.
  ```cpp
  // BonfireMenuWidget.h (protected)
  void ShowSubMenu(UWidget* Target);   // nullptr이면 모두 숨김
  virtual void NativeConstruct() override;

  // BonfireMenuWidget.cpp
  void UBonfireMenuWidget::ShowSubMenu(UWidget* Target)
  {
  	for (UWidget* SubMenu : { static_cast<UWidget*>(LevelUpWidget), static_cast<UWidget*>(TeleportWidget) })
  	{
  		if (SubMenu)
  		{
  			SubMenu->SetVisibility(SubMenu == Target ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
  		}
  	}
  }
  void UBonfireMenuWidget::NativeConstruct() { Super::NativeConstruct(); ShowSubMenu(nullptr); }   // 22-2
  void UBonfireMenuWidget::OnLevelUpClicked()  { if (LevelUpWidget) { LevelUpWidget->InitializedLevelUp(); } ShowSubMenu(LevelUpWidget); }
  void UBonfireMenuWidget::OnTeleportClicked() { if (TeleportWidget) { TeleportWidget->RefreshTeleportList(); } ShowSubMenu(TeleportWidget); }
  void UBonfireMenuWidget::OnSubMenuClosed()   { ShowSubMenu(nullptr); }
  ```
  `NativeOnInitialized`의 두 `SetVisibility(Collapsed)`도 `ShowSubMenu(nullptr)` 한 줄로 바뀐다. `OnTeleportClicked`의 `GEngine->AddOnScreenDebugMessage`(`:69`)는 15-7이므로 같이 지울 것. 위 코드는 컴파일을 확인하지 않았다.
- [ ] **22-4. (중간) 텔레포트 목록의 내용과 순서가 화면 목적에 맞지 않을 수 있다.** (`TeleportWidget.cpp:64-99`)
  1. 지금 서 있는 화톳불도 목록에 들어가서, 누르면 같은 자리로 `TeleportTo`한다(성공하면 메뉴만 닫힘). 현재 화톳불을 빼거나 비활성으로 표시할 것.
  2. 목록 순서가 점화한 순서가 아니라 `TActorIterator`가 월드에서 만나는 순서이다. 점화 순서를 보여 주려면 `GetLitBonfireIds()`를 바깥 루프로 두고 각 ID에 맞는 액터를 찾는다.
  3. 저장 목록에 있는데 지금 월드에 로드되지 않은 화톳불은 조용히 빠진다. 같은 레벨 하나만 쓰는 지금은 문제없지만 레벨이 늘면 21-5의 마지막 항목(표시 이름도 저장)이 필요하다.
  4. 이동 대상 근처의 적, 구르기·공격 중 이동 허용 여부는 기획 결정이다(21-3). 지금 `TeleportToBonfire`는 `CanInteractBonfire` 같은 검사를 하지 않는다.
- [ ] **22-5. (낮음) `BonfireMenuButtonWidget`의 이름과 위치.**
  이 클래스는 "화톳불 메뉴의 버튼"이 아니라 "텔레포트 목록의 한 줄"이다(`InitializeBonfireButton(ID, 이름)`). 클래스가 하나뿐인 지금은 `UTeleportEntryWidget` 같은 이름이 역할을 말해 준다(블루프린트를 만들기 전이라 이름을 바꾸는 비용이 가장 낮다). `.cpp`의 `#include "BonfireMenuButtonWidget.h"`(`:4`)는 다른 파일과 달리 폴더 없이 쓴다(`"UI/BonfireMenuButtonWidget.h"`). 위젯 객체마다 `OnBonfireButtonClicked`를 `RequestTeleport`에 연결하는 구조(`TeleportWidget.cpp:55`)와 `NativeOnInitialized`의 한 번 바인딩(`BonfireMenuButtonWidget.cpp:11-20`)은 올바르다.
- [x] **22-6. (중간) 커밋 위생.** (2026-10-27: `BonfireMenuButtonWidget.h/.cpp`, `TeleportWidget.*`가 `9c86314`에 함께 커밋됨. 루트의 pdf/pptx 추적 정리는 20-12에서 계속) `UI/BonfireMenuButtonWidget.h/.cpp`가 아직 추적되지 않는다. 이미 수정된 `TeleportWidget.*`가 이 헤더를 include하므로(`TeleportWidget.cpp:5`) 둘을 함께 커밋하지 않으면 다른 사람의 빌드가 깨진다. 저장소 루트의 `4조(...)_기획발표_1026.pdf/.pptx`, `ZZakTungSoul_Pitch_KusoGames_min7.pptx`도 추적되지 않은 큰 파일이다. 20-12와 같은 이유로 저장소 밖에 두거나 `.gitignore`에 올릴 것. `Review(Claude).md`도 여전히 Git 밖이다(20-12).

### 3) 프로젝트 전체를 읽으며 확인한 것 (이전 섹션에 없던 항목)

- [ ] **22-7. (낮음) `UAttackTraceComponent`의 Tick 비활성화가 생성자에서는 효과가 없을 가능성이 있다.** (`AttackTraceComponent.cpp:21`)
  `SetComponentTickEnabled(false)`는 생성자에서 불러도 컴포넌트가 등록될 때 `bStartWithTickEnabled`(기본 `true`)로 다시 켜진다. 처음부터 꺼 두려면 `PrimaryComponentTick.bStartWithTickEnabled = false;`가 필요하다. 지금은 `bIsTracing`이 거짓이면 첫 줄에서 반환해서(`:41-44`) 비용은 작지만, "평소에는 Tick을 끈다"는 섹션 19의 칭찬은 에디터에서 `Show Tick`으로 확인한 뒤에 유지할 것(미확인). `IsTracing()`(`.h:50`)은 `private`이고 쓰는 곳이 없다.
- [ ] **22-8. (낮음) `AEnemyBase`의 상태 이중화와 불필요한 멤버.**
  - `IsStrafeMode()`는 `CurrentState == Strafe`(`EnemyBase.h:50`)이고 `bIsStrafeMode` 플래그(`:103`)는 `SetStrafeMode`(`EnemyBase.cpp:240-260`)가 따로 바꾼다. 같은 "간보기"를 나타내는 진실이 둘이라 `UpdateRotationMode`(상태 기준, `:101-111`)와 `SetStrafeMode`(플래그 기준)가 서로 다른 값을 `bUseControllerRotationYaw`에 쓸 수 있다. 어느 쪽이 정식인지 정하고 하나를 지울 것(K-6의 `bIsCombat`과 같은 종류).
  - `TargetStatComp`(`EnemyBase.h:65-66`)는 `HandleAttackTraceHit` 안에서만 쓰이는 값인데 멤버로 저장한다(`.cpp:134`). 지역 변수로 충분하다. 같은 함수의 `UE_LOG`("Attack Noooooooooo Hit!!!!!", `:127`, `:131`)는 정리할 것. 데미지 `50.0f`(`:141`)는 19-6.
- [ ] **22-9. (낮음) 단순화할 수 있는 곳 (14장의 기준을 프로젝트 전체에 적용).**
  - `AEnemyAIController::UpdateAIFocus`(`EnemyAIController.cpp:62-101`)의 `switch`는 8개 `case` 중 7개가 `ClearFocus`, `Strafe`만 `ToggleFocus()`이고 `PreviousState`는 쓰지 않는다. `if (NewState == EEnemyState::Strafe) { ToggleFocus(); } else { ClearFocus(EAIFocusPriority::Gameplay); }`로 충분하다. `ToggleFocus`는 켜고 끄는 게 아니라 타겟이 있으면 설정, 없으면 해제하므로 `RefreshFocus`가 맞는 이름이다.
  - `UMenuWidget::NativeConstruct`(`MenuWidget.cpp:10-41`)가 11개 버튼에 같은 `AddUniqueDynamic`을 11줄 반복한다. 버튼 포인터를 `TArray<UMenuButtonWidget*>`에 모아 `for`로 묶고 `NativeOnInitialized`로 옮기면 6줄이다(열릴 때마다가 아니라 한 번이면 충분).
  - `WarriorAnimInstance` 생성자(`:41-87`)가 `None`/`OneHand`/`TwoHand` 세 상태의 몽타주 로딩 블록을 3번 복사한다(경로 접두사만 다름). 경로 접두사 표를 `for`로 돌리면 줄어든다. 같은 파일의 빈 함수 5개(`PlayMontage`, `PlayRollMontage`, `PlayBackStepMontage`, `PlayAttackMontage`, `StopAttackMontage`, `:150-153`, `:222-232`, `:239-241`)는 호출처가 없거나 아무 일도 하지 않는다(`AnimNotify_ComboCheck`가 `StopAttackMontage()`를 부름, 19-11, 19-17).
  - `StateEnum.h:43-45`의 `GetOverlayStateTypeHash`는 이름이 `GetTypeHash`가 아니라서 `TMap`이 쓰지 않는 죽은 함수이다. 주석("해시함수 미 작성시 키 값으로 사용 불가")도 UE5의 `enum class` 키에는 맞지 않는다. 지울 것.
  - `AZTSGameMode::LoadGame`(`ZTSGameMode.cpp:72-80`)이 `DoesSaveGameExist`를 두 번 검사한다. 두 번째 `if`를 지우면 중첩이 한 단계 줄어든다. `FWarriorSaveData::CurrentHP/CurrentFP`(`WarriorSaveData.h:48-52`)는 여전히 읽고 쓰는 곳이 없는 죽은 필드이다(17-6).
- [ ] **22-10. (낮음) 작은 안전성.** `BTService_UpdateCombatInfo.cpp:30`의 `OwnerComp.GetAIOwner()->GetPawn()`은 `GetAIOwner()`가 null일 때를 확인하지 않고(다른 BT 노드는 확인함), `AWarrior::Dodge`의 `AnimInstance->PlayActionMontage()`(`Warrior.cpp:474,480`)도 `Attack`과 달리 null 확인이 없다. `ItemObject.h:56`의 `Interact`는 `private` 구획에 있는 `override`라서 인터페이스 포인터로만 호출된다(동작은 문제없으나 의도라면 주석으로).

### 4) 좋은 점

- 섹션 21의 지적 세 가지를 하루 만에 권고한 형태 그대로 반영했고, 안전장치 로그(`BonFire.cpp:57-61`)까지 넣었다.
- `SyncLitState`를 구체 진입과 `Interact` 시작 두 곳에서 부르는 방식이 `LoadGame` 순회 없이 단순하게 불러오기 후 상태를 맞춘다. 접근하기 전 화톳불의 시각 연출만 `TODO`로 남아 있다.
- `TeleportToBonfire`의 순서(검증 → 액터 검색 → 높이 보정 → 이동 결과 확인 → 카메라 → 메뉴 닫기)와 실패 시 로그가 읽는 순서와 같다.
- `TeleportWidget`이 `LitIDs`와 월드의 화톳불을 맞춰서 표시 이름이 비었을 때 ID로 대체한다(`TeleportWidget.cpp:89-93`). 각 버튼은 `NativeOnInitialized`에서 한 번만 바인딩한다.
- `LevelUpWidget`의 `OnLevelUpCanceled` 델리게이트(`:48-49`, `:173`)로 자식이 부모의 구체 클래스를 모르고 닫기를 알린다. `BonfireMenuWidget`, `TeleportWidget`, `LevelUpWidget`이 모두 같은 "자식은 방송, 부모가 보이기/숨기기" 규칙이다.

### 5) 열린 높은 항목의 현재 위치 (이번에 다시 확인, 전부 그대로)

| 항목 | 위치 | 내용 |
|---|---|---|
| 19-2 (= 9-2, 9-3) | `Warrior.cpp:504`, `:543`, `:165`, `Warrior.h:66` | `Detected->GetController()->IsPlayerController()`의 null 역참조, `UpdateLockOnCamera`의 `&&` 조건(`LockOnTarget`이 null이면 `:174`에서 역참조), `LockOnTarget`은 초기화도 없는 일반 포인터 |
| 19-3 | `EquipmentComponent.cpp:15-17`, `.h:25` | `TMap::operator[]`로 없는 키를 읽어 처음 장착할 때 어서션. `EquipWeapon`이 아직 호출처가 없어서 드러나지 않음 |
| 19-4 | `Warrior.cpp:682-684` | `WeaponData`를 만들고 `!InItem`을 다시 검사해서 무기가 아닌 아이템이면 역참조 |
| 19-17 | `Warrior.cpp:698-711`, `ItemObject.cpp:112-118` | `ObtainItem`이 인자를 무시하고 `DetectedActors`의 마지막을 다시 캐스트한다. 마지막 대상이 화톳불이면 인벤토리에 못 넣고도 `AItemObject::Interact`가 `Destroy()`해서 아이템이 사라진다 |
| 19-9 | `InventoryComponent.cpp:22-23` | 공유 데이터 애셋의 `InstanceID`를 덮어씀 |
| 17-1, 17-2, 17-3 | `WarriorPlayerState.cpp:47-103` | `ApplyLevelUp`의 음수 검사, 레벨 표 밖 비용 0(보류), `MaxValue` 상한 |
| 17-4 | `WarriorPlayerState.cpp:14-45` | 호출처 없는 `UpgradeStat`이 비용을 호출자 값으로 믿음. 지울 것 |
| 17-6 | `ZTSGameMode.cpp:90`, `Warrior.cpp:100,324-327` | `LoadGame`이 폰에 알리지 않음. 시작 시 `UpdateCharacterStats`가 `GetSaveData()`로 `PlayerState`를 읽는데, 그 시점에 `nullptr`이어서 조용히 반환하는지 `UE_LOG`로 확인 필요 |
| 19-14 | `WarriorController.cpp:194`, `:213` | `IA_Interact`(`Completed`)와 `BindKey(E)`(`Pressed`)가 둘 다 `Interact`에 묶여 있다. `IA_Interact`가 `E`에 매핑되는 순간 한 번 눌러 두 번 호출 |
| 15-1, 15-2 | `WarriorController.cpp:493-549`, `Warrior.cpp:599-618` | 화톳불 메뉴가 열린 채 `C`로 스탯창을 열었다 닫으면 `GameOnly`로 돌아감, 메뉴 중 이동·공격 입력 유지, `Interact`가 `ActionState`를 확인하지 않음 |
| 19-15, 19-16 | `ItemListWidget.cpp:12`, `ItemObject.cpp:48-54` | `FClassFinder` 경로에 `_C` 없음, 빈 에셋 목록에서 `Assets[0]` 접근 |
| 9-5, 2-4 | `Warrior.cpp:320-322,339,349,465` | 이동 속도 400/600과 구르기 비용 25가 코드에 하드코딩, `GetMaxWalkSpeed()`는 호출처 없음 |
| 9-6, 9-7 | `EnemyAIData.h:38-64`, `WarriorAIController.cpp` | `float` 6개에 기본값 없음, AI 컨트롤러 둘 |

### 6) 이번 점검으로 상태가 바뀐 이전 항목

- 해소: 16-8, 16-9, 20-7, 21-2, 21-3, 21-4. 안전장치 추가: 21-1.
- 일부 해소: 20-8(스탯창 주 스탯 캐스트), 20-4(코드는 해소, 에셋 없음 → 22-1), 20-5(스탯창이 화톳불 메뉴에 임베드되지 않아 갱신 문제는 `C` 키 창으로만 남음).
- 그대로: 섹션 5의 표, 15-3 ~ 15-11 중 `GEngine` 디버그 메시지(`Bonfire.cpp`, `BonfireMenuWidget.cpp:69`, `LevelUpWidget.cpp:158`, `WarriorController.cpp`), 16-3, 16-4, 20-6, 20-9, 20-10(죽은 `CurrentInteractable`, `Warrior.h:17,150`), 14-1 ~ 14-3(인터페이스 → 컴포넌트 조회가 `WarriorPlayerState`, `BonFire`, `PlayerHUDWidget`, `WarriorController`, `LevelUpWidget`로 5곳, `bHasOverlap` 분기, `if (Lit)`).
- 새 항목: 22-1 ~ 22-10.

### 7) 우선순위

1. **22-1**: `WBP_LevelUp...`, `WBP_Teleport...`, 텔레포트 항목용 위젯 블루프린트를 만들고 `WBP_BonfireMenuWidget` 안에 이름을 맞춰 넣는다. 그 전에는 C++를 아무리 고쳐도 PIE에서 확인할 수 없다. 만들기 전에 22-5의 클래스 이름을 정한다.
2. **22-6**: `BonfireMenuButtonWidget.*`와 `TeleportWidget.*`를 같은 커밋으로 올린다.
3. **22-2, 22-3**: 서브메뉴를 `ShowSubMenu` 하나로 묶고 `NativeConstruct`에서 초기화한다. 에셋을 붙이기 전에 하면 한 번에 검증할 수 있다.
4. **19-2 ~ 19-4, 19-17**: Ahn 코드의 충돌/크래시 항목은 아직 한 줄도 바뀌지 않았다. 병합 전에 전달한다.
5. **17-1**(음수 포인트)과 **19-14**(`IA_Interact`와 `BindKey(E)` 중복)는 한 줄 수정이다. 지금 해 둘 것.
6. 22-4, 22-7 ~ 22-10과 14장의 정리 항목은 정리 작업으로 묶는다.

---

## 23. 10-27 오후 블루프린트 작업 전 결정 기록 (2026-10-27, 사용자 결정)

오후에 위젯 블루프린트를 만들 예정이어서, 블루프린트가 만들어진 뒤에 바꾸면 비용이 큰 항목을 골라 사용자와 정했다. 코드는 수정하지 않았다. 이 섹션은 결정만 기록하고, 구현이 끝나면 해당 항목에 `[x]`를 단다(체크는 코드를 확인한 뒤에만 한다).

| 항목 | 결정 | 비고 |
|---|---|---|
| 16-4 / 20-6 (`AddPoint(FName)`/`SubPoint(FName)`을 `EAttributeType` enum으로) | **보류** | 오타만 조심한다. 블루프린트 노드에 쓰는 이름은 `Vigor`, `Attunement`, `Endurance`, `Vitality`, `Strength`, `Dexterity`를 헤더와 같은 철자로 적는다. 오타는 조용히 무시되므로(함수가 그냥 반환) 레벨업이 안 되면 먼저 철자를 본다 |
| 22-5 (`UBonfireMenuButtonWidget` 이름) | **보류, 그대로 사용** | 이름이 역할(텔레포트 목록 한 줄)과 다르지만 동작에는 문제없다. 블루프린트의 부모 클래스를 이 이름으로 지정하게 된다 |
| 15-8 / 22-1 (필수 위젯을 `BindWidget`으로) | **보류** | `BindWidgetOptional`을 유지한다. 위젯 이름이 틀리면 조용히 `nullptr`이므로 PIE에서 버튼이 무반응이면 이름부터 확인한다 |
| 22-2, 22-3 (서브메뉴 초기화, `ShowSubMenu`) | **오늘 작업** | `BonfireMenuWidget`에 `NativeConstruct`와 `ShowSubMenu`를 추가하고 세 핸들러를 줄인다. 에셋을 붙이기 전에 하면 한 번에 검증할 수 있다 |
| 19-5 (소울을 얻을 방법) | **오늘 작업 (디버그용)** | `AWarriorPlayerState::AddSouls(int32)`와 `#if !UE_BUILD_SHIPPING`으로 감싼 디버그 키(`WarriorController`)를 추가한다. 적 처치 보상에도 같은 `AddSouls`를 쓴다 |
| 19-14 (`IA_Interact`와 `BindKey(E)` 중복) | **Ahn 담당** | 오늘 테스트에서 `E` 한 번에 점화와 메뉴가 같이 일어나면 이 문제이다. `IMC_Warrior`는 Ahn도 고치는 바이너리라 같은 날 둘이 고치지 않는다 |
| 22-6 (`BonfireMenuButtonWidget.*` 미추적) | **오늘 작업** | `TeleportWidget.*`와 같은 커밋에 넣는다 |
| 17-1 (`ApplyLevelUp` 음수 포인트), 17-2 (레벨 표 밖 비용 0) | 17-1은 미정, 17-2는 **보류 유지** | 오늘 테스트에서 레벨 10을 넘기지 않는다(`Level1~10` 합계 7,500소울). 디버그 소울 5,000이면 레벨 6 근처까지 올라간다 |
| 19-2 ~ 19-4, 19-17 (Ahn 코드) | 오후 작업과 무관, **병합 전 전달** | 락온 null, 장착 크래시, `ObtainItem` 아이템 소실 |

**오후 작업 순서(확정)**: (1) `ShowSubMenu`/`NativeConstruct`, (2) `AddSouls` + 디버그 키, (3) 빌드와 커밋(22-6 포함), (4) 위젯 블루프린트 제작(`WBP_LevelUp`, `WBP_TeleportEntry`, `WBP_Teleport`, `WBP_BonfireMenuWidget`에 `LevelUpWidget`/`TeleportWidget` 임베드, 화톳불 인스턴스마다 `BonfireID` 다르게), (5) PIE 확인 순서는 아래.

**PIE 확인 순서**
1. Output Log에서 `BonfireID` 기본값 경고가 나오지 않는지.
2. 디버그 키로 소울을 넣고 → 화톳불 점화 → 다시 `E`로 메뉴 열기.
3. 레벨업 창에서 +/- 숫자가 바뀌는지, 확정 후 소울이 줄고 `C` 스탯창 값이 반영되는지(스탯창은 열 때 갱신되므로 닫았다 다시 연다).
4. 서브메뉴를 연 채로 `Esc`로 닫았다 다시 열면 서브메뉴가 모두 숨겨져 있는지(22-2).
5. 두 번째 화톳불을 점화 → 텔레포트 목록에 나타나는지 → 이동 후 메뉴가 닫히는지.
6. 메뉴가 열린 상태에서 `C`로 스탯창을 열었다 닫을 때 마우스 커서 상태(15-1, 알려진 문제)를 기록해 둔다.

---

## 24. 10-27 오후 작업 리뷰 (2026-10-27, 커밋 `9c86314` `15e944c` `422c8dc`)

**대상**: `BonfireMenuWidget.*`(`ShowSubMenu`, `NativeConstruct`), `WarriorPlayerState.*`(`AddSouls`, 음수 검사), `WarriorController.*`(`DebugAddSouls`, 텔레포트 검증), `BonFire.*`(`SyncLitState`), `TeleportWidget.*`, `BonfireMenuButtonWidget.*`, `LevelUpWidget.*`, 에셋 `WBP_LevelUpWidget`, `WBP_TeleportWidget`, `WBP_BonfireMenuButton`, `WBP_BonfireMenuWidget`, `BaeTestMap.umap`. 에셋은 열 수 없어서 **문자열 검색**으로 위젯 이름과 클래스 참조만 확인했다(결과가 "없음"이어도 에디터 확인이 필요하다). 빌드와 실행은 하지 않았다. 코드는 수정하지 않았다.
**기준 상태**: `Bae` = `422c8dc`. `422c8dc`는 `PROJECT_GUIDE.md`를 Git에서 뺐다(디스크에는 있고 `git status`에 `??`). `Review(Claude).md`도 같다(20-12).

**총평**: 섹션 23에서 정한 오늘 작업이 모두 반영되었다. 계획에 없던 17-1(음수 포인트 검사)까지 들어갔다. 에셋 이름도 대부분 헤더와 정확히 일치한다. 남은 것은 **위젯 하나(텔레포트 창의 뒤로가기 버튼)**와 이번 에셋 작업으로 **낡게 된 가이드 서술**이다.

### 1) 확인된 변경

| 항목 | 결과 | 근거 |
|---|---|---|
| 22-2, 22-3 서브메뉴 초기화·통합 | **해소** | `BonfireMenuWidget.cpp:41-48`(`NativeConstruct` → `ShowSubMenu(nullptr)`), `:50-60`(`ShowSubMenu`), 핸들러가 한두 줄(`:62-85`). `NativeOnInitialized`에서 `Collapsed` 두 줄 제거 |
| 19-5 소울 획득 | 부분 해소 | `AddSouls`(`WarriorPlayerState.cpp:111-120`), 디버그 키 `F1`(`WarriorController.cpp:217`, `DebugAddSouls` `:220`). 실제 획득 경로는 아직 없음 |
| 17-1 음수 포인트 | **해소** | `WarriorPlayerState.cpp:49-53` |
| 22-6 커밋 위생 | **해소** | `BonfireMenuButtonWidget.*`와 `TeleportWidget.*`가 함께 커밋됨 |
| 21-1 `BonfireID` 유일성 | **해소(배치 확인됨)** | `BaeTestMap.umap` 문자열에 `Bonfire_One`, `Bonfire_Two`, 인스턴스 `BP_Bonfire_C_1`, `_3` |
| 22-1, 20-4, 19-12 에셋 | **해소** (24-1만 남음) | 아래 2절 |
| 17-2, 17-3 (레벨 표 밖 비용 0, `MaxValue`) | 보류 유지 | `ApplyLevelUp`에 변경 없음. 디버그 소울 5,000 → 레벨 6 근처까지만 올릴 것 |

### 2) 에셋 문자열 검색 결과

| 에셋 | 헤더가 요구하는 이름 | 결과 |
|---|---|---|
| `WBP_BonfireMenuWidget` | `Button_LevelUp`, `Button_Teleport`, `Button_Leave`, `LevelUpWidget`, `TeleportWidget` | 모두 있음. `WBP_StatusWindow`는 **없음** (10-22 이전 에셋에는 있었다) |
| `WBP_LevelUpWidget` | `TextBlock_` 18개(현재 레벨/소울/총 비용/남은 소울, 주 스탯 6, 풀 3, 전투 5), `Button_Confirm`, `Button_Cancel` | 모두 있음. `Button_AddPoint`/`Button_SubPoint` 각 6개, `AddPoint`/`SubPoint` 호출 확인 |
| `WBP_TeleportWidget` | `ScrollBox_BonfireList`, `Button_Cancel`, `BonfireButtonClass` | `ScrollBox_BonfireList`와 `BonfireButtonClass`(= `WBP_BonfireMenuButton`)는 있음. **`Button_`으로 시작하는 이름이 하나도 없음** |
| `WBP_BonfireMenuButton` | `Button_Bonfire`, `TextBlock_BonfireName` | 모두 있음 |

### 3) 새 항목

- [ ] **24-1. (중간, 확인 필요) 텔레포트 창에 뒤로가기(`Button_Cancel`)가 없는 것으로 보인다.** (`WBP_TeleportWidget.uasset`, `TeleportWidget.h:67-69`)
  `Button_Cancel`은 `BindWidgetOptional`이라 없으면 `nullptr`이고 `OnCancelClicked`/`OnTeleportCanceled`가 실행되지 않는다. 텔레포트 창을 열면 `레벨업` 버튼으로 바꾸거나 `Esc`/`Q`로 메뉴 전체를 닫는 방법밖에 없다. 의도라면 괜찮지만, 레벨업 창은 취소 버튼이 있어서 두 서브메뉴의 조작이 다르다. 다른 이름으로 만들었다면 이름을 `Button_Cancel`로 맞출 것. 15-8(필수 위젯 `BindWidget`)을 보류한 것의 결과이기도 하다.
- [ ] **24-2. (중간) 화톳불 메뉴의 스탯창 임베딩이 없어졌는데 가이드는 여전히 있다고 쓴다.**
  `WBP_BonfireMenuWidget`에 `WBP_StatusWindow` 이름이 나타나지 않는다(코드의 `BonfireMenuWidget.h`에도 스탯창 멤버가 없다). 가이드(디스크 사본)에는 아직 다음이 남아 있다: `:45` "`WBP_StatusWindow` 임베딩 검증 완료", `:108`/`:120` "C키/화톳불 임베딩" 다이어그램과 `BonfireUI -->|스탯창 임베딩| StatusUI`, `:242`, `:929-930` "임베딩하여 ... 실시간으로 자동 갱신". 지금은 화톳불 메뉴 → 서브메뉴(`LevelUpWidget`, `TeleportWidget`) 구조이고, 스탯창은 `C` 키 창뿐이다. 임베딩과 "실시간 자동 갱신" 서술(16-11, 18-3)을 한꺼번에 지울 수 있다.
- [ ] **24-3. (낮음) `DebugAddSouls`가 `BlueprintCallable` `AddSouls`와 함께 출시 빌드에 남는다.**
  `WarriorController.cpp:217`의 `BindKey(F1)`은 `#if !UE_BUILD_SHIPPING` 밖에 있고(함수 본문만 가드), `AddSouls`는 `UFUNCTION(BlueprintCallable)`이라 출시 빌드에서도 소울을 올릴 수 있다. 적 처치 보상에 같은 함수를 쓸 계획이므로 `AddSouls`는 그대로 두되 `BindKey`와 `DebugAddSouls` 선언까지 `#if !UE_BUILD_SHIPPING`으로 묶는 편이 일관된다. `F1`은 에디터 뷰포트에서 조명 모드 단축키와 겹칠 수 있어 PIE에서 안 먹으면 다른 키로 바꿀 것. `GEngine` 검사 없는 화면 메시지는 15-7.
- [ ] **24-4. (낮음) `ApplyLevelUp`의 음수 검사가 여섯 필드를 한 줄에 나열한다.** (`WarriorPlayerState.cpp:49-53`) `FWarriorAttributeData`에 `HasNegative()`를 두면 같은 검사를 위젯의 `CanAddPoint`와 확정 조건이 공유한다(20-2). 실패 이유를 구분하지 못하는 `false`(17-12)는 그대로다.
- [ ] **24-5. (낮음) 텔레포트 목록은 아직 22-4 그대로다.** 현재 서 있는 화톳불도 목록에 들어가고, 순서는 월드의 액터 순서이며, 저장 목록에 있는데 월드에 없는 화톳불은 조용히 빠진다(`TeleportWidget.cpp:64-99`). `BaeTestMap`에 화톳불이 둘이므로 PIE에서 "지금 있는 화톳불로 이동"을 눌러 보고 결과를 기록해 둘 것.

### 4) PIE 확인 기록 (사용자가 확인한 것으로 가정되지 않은 항목)

커밋 메시지는 "인게임 연동 검증 완료"이다. 리뷰어는 실행하지 못했으므로 사용자 보고로 취급한다. 아래는 에셋 문자열만으로는 확인할 수 없어서 검증 기록에 남겨 두면 좋은 항목이다.
1. 여섯 +/- 버튼이 넘기는 이름의 철자(`Vigor`, `Attunement`, `Endurance`, `Vitality`, `Strength`, `Dexterity`). 철자가 틀리면 그 스탯만 조용히 안 올라간다(16-4 보류의 위험).
2. 첫 `E`(점화)에서 메뉴가 안 열리고 두 번째 `E`에서 열리는지(`E` 이중 바인딩 19-14는 Ahn 담당, 한 번에 둘 다 일어나면 이 문제).
3. 서브메뉴를 연 채 `Esc`로 닫았다 다시 열면 메인 메뉴만 보이는지(22-2 수정의 결과).
4. 화톳불 둘을 점화한 뒤 목록에 둘 다 나타나는지, 저장/불러오기 후에도 `Lit`인지(`SyncLitState`).
5. 메뉴가 열린 채 `C`로 스탯창을 열었다 닫았을 때 마우스 커서(15-1).

### 5) 가이드(디스크 사본)에서 낡은 곳 (24-2 외)

`:6` 헤더 커밋 `6eed7f1`(현재 `422c8dc`), `:540`의 `GetMind()`/`GetIntelligence()`(없는 함수, 실제는 `GetAttunement()`/`GetVitality()`), `:856`/`:897`의 `CanInteracBonfire` 오타가 그대로다(19-20). 가이드가 Git에서 빠져서 팀이 읽지 않게 되었다면 우선순위는 낮지만, 읽는 사람(AI 포함)이 있다면 위 항목들을 한 번에 정정할 것.

### 6) 확인된 좋은 점

- `ShowSubMenu`가 `Target` 하나를 받는 구조라 서브메뉴가 하나 늘어도 핸들러를 고치지 않는다. 부모는 `NativeConstruct`에서 항상 초기 상태로 돌아오고, 서브메뉴 쪽에서 취소를 방송(`OnLevelUpCanceled`, `OnTeleportCanceled`)하는 규칙이 두 위젯에서 같다.
- 디버그 소울이 `PlayerState::AddSouls`라는 한 곳을 지나서 나중에 적 보상이 같은 경로를 쓴다. 소울 증가 로그(`[PlayerState] 소울 획득`)가 남는다.
- 에셋 이름을 헤더와 글자 하나까지 맞췄다. `WBP_LevelUpWidget`은 TextBlock 18개가 모두 일치한다.
- 화톳불 둘의 `BonfireID`를 서로 다르게(`Bonfire_One`, `Bonfire_Two`) 지정했다.

### 7) 이번 점검으로 상태가 바뀐 이전 항목

- 해소: 22-1, 22-2, 22-3, 22-6, 17-1, 20-4, 19-12, 21-1(배치 확인). 부분 해소: 19-5.
- 보류 유지: 16-4, 22-5, 15-8, 17-2, 17-3. Ahn 담당: 19-14. 병합 전 전달: 19-2 ~ 19-4, 19-17.
- 새 항목: 24-1 ~ 24-5.

### 8) 우선순위

1. **24-1**: `WBP_TeleportWidget`에 `Button_Cancel`을 만들거나(이름 정확히), 의도적으로 없앴다면 `Esc`로 닫는다는 점을 기록한다.
2. **24-2**: 가이드의 스탯창 임베딩 서술을 지운다(`:45`, `:108`, `:120`, `:242`, `:929-930`).
3. **24-3**: `BindKey(F1)`과 선언을 `#if !UE_BUILD_SHIPPING`으로 묶는다. 한 줄씩이다.
4. PIE 확인(4절)을 한 번 돌려 보고 철자 문제(1번)와 `E` 이중 호출(2번)을 기록한다.
5. 병합 전에 Ahn에게 19-2 ~ 19-4, 19-17, 19-14를 전달한다.

---

## 25. 자동 저장, 메인 메뉴, 병합 전 점검 (2026-10-28, 미커밋 작업 트리 + `origin/*` 시험 병합)

**대상**: 섹션 24 이후 새로 생긴 변경만 본다. 미커밋 — `Interactable/BonFire.cpp`(자동 저장 두 곳), `UI/LevelUpWidget.cpp`(자동 저장, `CommitLevelUp` 정리), 새 파일 `UI/MainMenuWidget.h/.cpp`. 병합 전 점검 — `git merge-tree`로 `HEAD`와 `origin/Ahn`(`681830f`), `origin/Kang`(`75d7530`), `origin/Lee`, `origin/main`을 시험 병합했다(커밋된 상태만, 미커밋 작업 트리는 제외). `origin/main`은 `Bae`에 PR #7을 병합한 커밋 하나만 앞서 있고 파일 내용은 같다.
섹션 22, 24에서 이미 다룬 항목(서브메뉴 구조, 텔레포트 검증, 에셋 이름)은 반복하지 않는다. 빌드와 실행은 하지 않았다. 코드는 수정하지 않았다.

**총평**: 섹션 24의 지적은 대부분 해소된 상태에서 다음 단계로 넘어갔다. 새 코드의 방향(화톳불·레벨업 때 자동 저장, 메인 메뉴에서 새 게임/이어하기)은 소울라이크의 흐름과 맞다. 다만 **저장은 생겼는데 불러오기가 아직 한 곳(`IA_Load` 키)뿐**이어서, 메인 메뉴의 "이어하기"를 누르고 화톳불을 쓰면 기존 세이브가 기본 상태로 덮인다(25-1). 병합 쪽에서는 `origin/Ahn`이 장착 UI를 연결해서 19-3, 19-4가 이제 실제로 닿는다(25-3).

### 1) 새 항목

- [x] **25-1. (높음) (2026-10-29 해소: 맵 옵션 `LoadGame=1` + `GameMode::BeginPlay`의 `LoadGame`으로 구현됨, 잔여는 26-2) 메인 메뉴의 "이어하기"가 세이브를 적용하지 않고, 자동 저장이 기존 세이브를 덮어쓴다.**
  1. `MainMenuWidget.cpp:54-65` `OnLoadGameClicked`는 세이브 파일이 있는지만 확인하고 `OpenLevel(InGameMapName)`만 부른다.
  2. 세이브를 `PlayerState`에 적용하는 `AZTSGameMode::LoadGame()`의 호출처는 `WarriorController.cpp:421`(`IA_Load` 키) **하나뿐**이다. 레벨을 새로 열면 `PlayerState`는 기본값(레벨 1, 소울 0, 점화 목록 없음)으로 새로 만들어진다.
  3. 플레이어가 화톳불에서 `E`를 누르면 새로 생긴 자동 저장이 실행된다(`BonFire.cpp:184-187`, `:208-211`). `SaveGame()`은 슬롯의 기존 세이브 객체를 읽어 와서 `SetSaveData(PlayerState->GetSaveData())`로 **통째로 덮어쓴다**(`ZTSGameMode.cpp:44-67`).
  결과: 이어하기를 눌렀는데 기본 상태로 시작하고, 첫 화톳불 상호작용에서 이전 진행(레벨, 소울, 점화 목록)이 기본 상태로 저장되어 사라진다. 사용자가 시작 직후 `IA_Load`를 누르면 복원되지만, 누르기 전에 화톳불을 쓰면 늦다.
  `새 게임`이 슬롯을 지우므로(`MainMenuWidget.cpp:45-48`) "슬롯이 있으면 이어하기"라는 규칙을 쓸 수 있다. 그렇다면 인게임 맵이 시작될 때 폰이 만들어지기 **전에** 자동으로 불러오게 하는 것이 가장 작다. 폰이 만들어지기 전에 불러오면 `Warrior::BeginPlay`의 `UpdateCharacterStats()`가 불러온 값으로 스탯을 계산해서, 불러온 뒤 스탯이 갱신되지 않는 문제(7-6)도 같이 풀린다.
  ```cpp
  // GamePlay/ZTSGameMode.h  (public)
  virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
  // GamePlay/ZTSGameMode.cpp
  void AZTSGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
  {
  	// 폰이 생성되기 전에 세이브를 PlayerState에 적용한다. 슬롯이 없으면 LoadGame이 그대로 반환한다.
  	LoadGame();
  	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
  }
  ```
  주의: 에디터에서 `BaeTestMap`을 직접 열어도 슬롯이 있으면 불러오므로, 처음부터 시험하려면 슬롯을 지우거나 메인 메뉴의 새 게임을 써야 한다. `LoadGame`은 `GetFirstPlayerController()`를 쓰므로 단일 플레이어를 전제한다(컴파일 확인 전의 제안이다). 이 방식을 쓰지 않을 때는 최소한 자동 저장을 "불러온 적이 있거나 새 게임을 시작했을 때"만 하도록 막을 것.
- [ ] **25-2. (중간) 자동 저장 호출이 세 곳에 복사되어 있다.**
  `BonFire.cpp:184-187`(점화), `:208-211`(휴식), `LevelUpWidget.cpp:166-169`(레벨업 확정) 모두 `Cast<AZTSGameMode>(GetWorld()->GetAuthGameMode())` → `SaveGame()` 네 줄이다. 저장 방식(슬롯 분리, 저장 중 표시)이 바뀌면 세 곳을 고쳐야 하고, `GetAuthGameMode()`는 서버에서만 유효해서 위젯에서 부르는 쪽은 멀티플레이를 전제하지 않는다. 컨트롤러에 `AutoSave()`를 하나 두고 화톳불은 `Interactor->GetController<AWarriorController>()`, 위젯은 `GetOwningPlayer()`로 같은 함수를 부르면 된다.
  ```cpp
  // Warrior/WarriorController.h (public)
  UFUNCTION(BlueprintCallable, Category = "Save")
  void AutoSave();
  // Warrior/WarriorController.cpp
  void AWarriorController::AutoSave()
  {
  	if (AZTSGameMode* GM = Cast<AZTSGameMode>(GetWorld()->GetAuthGameMode()))
  	{
  		GM->SaveGame();
  	}
  }
  ```
  메시지 문구도 정리할 것: `LevelUpWidget.cpp`의 "레벨업 완 + 자동저장."(글자 누락)과 `BonFire.cpp`의 "현재 활성화 상태 화톳불 + 자동저장."(휴식을 알리는 문구가 아님). 휴식(`Lit` 분기)은 변경이 없어도 매번 저장하므로, 점화와 레벨업 때만 저장해도 된다. `CommitLevelUp`의 `if (PS && ...)`(`LevelUpWidget.cpp:160`)는 바로 위에서 `!PS`를 이미 거르므로 `PS &&`가 중복이다.
- [ ] **25-3. (높음, 병합 전) `origin/Ahn` 병합: 텍스트 충돌 1건, 그리고 장착 크래시가 실제로 닿는다.**
  1. **`WarriorController.cpp` 충돌.** 양쪽이 파일 맨 끝에 함수를 추가했다(Bae의 `TeleportToBonfire` vs Ahn의 `EquipItem`, `ToggleMenuWindow`). 충돌 표시(`<<<<<<<`, `=======`, `>>>>>>>`) 사이의 두 덩어리를 그대로 이어 붙이면 된다. 같은 파일의 `.h`는 자동으로 합쳐진다.
  2. **19-3, 19-4가 이제 실제로 호출된다.** Ahn이 `AWarriorController::EquipItem` → `Warrior->EquipWeapon(InItemData, InSlot)`을 연결했다. 그런데 `origin/Ahn`의 `EquipmentComponent.cpp:15`(`EquippedItems[Slot]`)와 `Warrior.cpp:683`(`if (!InItem)`)은 지적한 채 그대로다. 장착 UI에서 아이템을 누르는 순간 `check` 어서션이 걸린다. 병합 전에 Ahn에게 전달할 것(수정 코드는 19-3, 19-4).
  3. 같은 브랜치에서 그대로인 것: `Warrior.cpp:504`, `:543`(19-2 락온 null), 컨트롤러 `BeginPlay`의 `SetInventory`(20-11의 2), `Escape`와 `IA_Menu`의 키 겹침 가능성(20-11의 1).
  4. **`origin/Lee`: `Content/Maps/TestMap.umap` 바이너리 충돌.** `.umap`은 내용을 합칠 수 없다. 어느 쪽 변경을 살릴지 정하고 나머지 변경은 다시 배치해야 한다. 앞으로는 맵 편집 담당을 정하거나 각자 시험 맵을 쓸 것(`BaeTestMap`, `TestKangMap`처럼).
  5. **`origin/Kang`: 충돌 없음.** `EnemyStatComponent`, `DamageInfo.h`, `EnemyStat.csv`가 새로 들어와 적의 체력과 데미지가 별도 컴포넌트로 가는 구조다(3-2, 4-5, 9-4, 19-6과 관련). 이 코드는 아직 리뷰하지 않았다.
- [ ] **25-4. (낮음) `MainMenuWidget`.** (미커밋 새 파일)
  - 세이브 슬롯 이름 `"SaveSlot"`이 `ZTSGameMode.h:39`와 `MainMenuWidget.h:48`에 각각 있다. 한쪽이 바뀌면 "세이브 있음" 판정과 실제 저장 위치가 어긋난다. 상수 하나를 `SaveData/` 쪽에 두고 같이 쓸 것.
  - `InGameMapName` 기본값이 개인 시험 맵 `BaeTestMap`이다. 실제 시작 맵이 정해지면 바꿀 것. 이름이 틀리면 `OpenLevel`이 로그만 남기고 아무 일도 하지 않는다.
  - 메뉴 화면의 입력 모드(마우스 커서, UI 전용)를 설정하는 코드가 C++에 없다. 레벨 블루프린트나 별도 컨트롤러에서 처리하는지 확인할 것.
  - `OnLoadGameClicked`의 `GEngine->AddOnScreenDebugMessage`는 `GEngine` 검사가 없다(15-7).
  - 새 게임은 슬롯만 지운다. 같은 세션에서 다시 `OpenLevel`하면 `PlayerState`가 새로 만들어지므로 문제없지만, 25-1을 `HandleStartingNewPlayer`로 풀면 "슬롯 없음 = 새 게임" 규칙이 이 지우기에 의존한다는 점을 주석으로 남길 것.

### 2) 이전 항목 상태

- 변동 없음(코드 확인): 17-2, 17-3, 20-2(레벨 표 밖 구간, 보류), 24-1 ~ 24-5, 22-4, 19-2, 19-6, 19-7, 19-10, 15-1, 15-2.
- `CharacterStatComponent.cpp`의 함수 순서(20-9), `LevelUpWidget.cpp`의 `using namespace UIHelpers;` 위치(20-8)와 `float` 캐스트 8개도 그대로다.

### 3) 우선순위

1. **25-1**: 이어하기와 자동 저장의 순서를 정한다. 메인 메뉴를 연결하기 전에 `HandleStartingNewPlayer`에서 불러오는 방식(또는 그에 준하는 방식)을 넣는다. 지금 상태로 메인 메뉴를 쓰면 시험 중에 세이브가 지워진다.
2. **25-3**: Ahn과 병합하기 전에 19-3, 19-4를 전달한다. `WarriorController.cpp` 충돌은 두 덩어리를 이어 붙이면 된다. `TestMap.umap`은 병합 방법을 정한다.
3. 25-2(`AutoSave()` 한 개), 25-4(슬롯 이름 상수)는 정리 작업으로 묶는다.

---

## 26. 메인 메뉴 이어하기·로딩 화면·마지막 화톳불 이동 검수 (2026-10-29, `Bae` `8855464` + 미커밋 작업 트리)

**대상**: 커밋되지 않은 변경 — `GamePlay/ZTSGameMode.*`(`InitGame` 옵션, `BeginPlay` 불러오기, 마지막 화톳불로 이동), `UI/MainMenuWidget.*`(로딩 화면 경유), `UI/LoadingScreenWidget.*`(폴더 스캔, 레벨 전환), `UI/BonfireMenuWidget.*`(`CloseCurrentSubMenu`), `Warrior/WarriorController.cpp`(`BeginPlay` 입력 모드, 메뉴 열 때 이동 매핑 해제), `Warrior/WarriorPlayerState.h`(`LastBonfireID`), `SaveData/WarriorSaveData.h`, `Interactable/BonFire.cpp`(마지막 화톳불 기록), `ZZakTungSoul.Build.cs`(`AssetRegistry`), `Content/Data/StatDefine.csv`(+`DT_StatDefine`의 `Stance` 행), UMG 에셋. 병합으로 들어온 팀원 코드 중 사용자 파일을 건드린 부분(`CharacterStatComponent`의 `Stance`)은 같이 확인했다. 팀원 코드(`EnemyStatComponent`, 아이템 UI 등)는 이번에 리뷰하지 않았다.
에셋(`.uasset`)은 열 수 없어서 파일 이름과 문자열로만 확인했다. 빌드와 실행은 하지 않았다. 코드는 수정하지 않았다.

**기준 상태**: 로컬 `Bae` = `origin/Bae` = `8855464`("Merge branch 'main' into Bae"). `Review(Claude).md`와 `PROJECT_GUIDE.md`는 저장소 밖 `Desktop\깃허브사생호라\`로 옮겨져 있다(20-12의 백업 권고가 반영됨. 이 경로를 기준으로 갱신한다).

**총평**: 25-1(이어하기가 세이브를 적용하지 않고 자동 저장이 덮어씀)이 **맵 옵션 방식으로 해소**되었다. 메인 메뉴가 `LoadGame=1`을 붙여 레벨을 열고 `GameMode`가 `InitGame`에서 옵션을 읽어 `BeginPlay`에서 불러오며, 새 게임은 슬롯을 지우고 빈 옵션으로 연다. 불러오기 직후 `LoadSaveDataFrom`이 폰의 스탯도 갱신하고(7-6 해소), 마지막으로 쓴 화톳불로 이동까지 해서 "이어하기"의 모양이 갖춰졌다. 이번에 걸리는 것은 **텔레포트 후 메뉴가 닫히지 않는 회귀(26-1)**, **자동 저장 가드가 없어 에디터에서 맵을 직접 열면 세이브가 덮이는 경로가 남은 것(26-2)**, **로딩 화면의 선행 로딩이 레벨 전환 때 풀릴 가능성(26-3)**, 그리고 **`Stance` 코드의 복사 오류(26-4)**다.

### 1) 이전 항목 해소 확인

| 항목 | 결과 | 근거 |
|---|---|---|
| 25-1 이어하기 + 자동 저장 덮어쓰기 | **해소(방식 변경)** | `MainMenuWidget.cpp:53,66`(옵션), `ZTSGameMode.cpp:26-47`(`InitGame` → `BeginPlay`에서 `LoadGame`) |
| 7-6 불러온 뒤 스탯 미갱신 | **해소** | `LoadSaveDataFrom`(`WarriorPlayerState.cpp:13-26`)이 폰의 `UpdateCharacterStat`을 부름 |
| 15-2 메뉴 중 이동·공격 입력 | **해소** | `OpenBonfireMenu`가 `IMC_Warrior`를 제거(`WarriorController.cpp:687`), `CloseBonfireMenu`가 복구(`:715`). `Esc`/`Q`/`E`는 `BindKey`라 매핑 해제와 무관 |
| 15-1 서브메뉴 `Esc` | 일부 해소 | `CloseBonfireMenu`가 서브메뉴를 먼저 닫음(`:703-706`). 단 26-1 |
| 25-2 `AutoSave()` 통합, 25-4 슬롯 이름 상수 | 그대로 | `BonFire.cpp`, `LevelUpWidget.cpp`의 `SaveGame` 세 곳 복사, `"SaveSlot"` 문자열 `ZTSGameMode.h`/`MainMenuWidget.h` 두 곳 |
| 25-3 `origin/Ahn` 병합 | 해소 | `main`에 병합됨(`Bae`가 `main`을 병합). 19-3, 19-4는 별도 확인 필요(5절) |

### 2) 동작 오류 (높음 ~ 중간)

- [ ] **26-1. (높음) 텔레포트 후 화톳불 메인 메뉴가 닫히지 않고 입력이 UI 모드로 남는다. (`CloseBonfireMenu`의 새 서브메뉴 우선 규칙 때문)**
  `CloseBonfireMenu`(`WarriorController.cpp:698`)가 `Esc`/`Q`용으로 "서브메뉴가 열려 있으면 그것만 닫고 반환"하도록 바뀌었다(`:703-706`). 그런데 `TeleportToBonfire`는 이동 성공 후 마지막에 같은 함수를 부른다(`:825`, 주석은 "열려 있던 화톳불 메뉴 닫기 및 게임 입력 복구"). 텔레포트를 누르는 시점에는 **텔레포트 서브메뉴가 열려 있으므로**(`TeleportWidget`이 `Visible`) 이 호출은 서브메뉴만 숨기고 반환한다. 결과: 캐릭터는 새 화톳불로 이동했는데 화톳불 메인 메뉴가 화면에 남고, `IMC_Warrior`는 제거된 채(`:687`) `GameAndUI` 모드로 남아 이동·공격이 안 된다. `Esc`를 한 번 더 눌러야 풀린다. 레벨업 서브메뉴에서 `떠나기`를 누르는 경우도 같다(`OnLeaveClicked` → `CloseBonfireMenu`).
  "서브메뉴 먼저"는 `Esc`/`Q` 키에만 맞는 규칙이므로 함수를 둘로 나누는 것이 가장 단순하다.
  ```cpp
  // Warrior/WarriorController.h (public)
  void CloseBonfireMenu();        // Esc/Q: 서브메뉴가 있으면 그것만, 없으면 전체 닫기
  void ForceCloseBonfireMenu();   // 텔레포트·떠나기: 서브메뉴 상태와 상관없이 전체 닫기
  // Warrior/WarriorController.cpp
  void AWarriorController::CloseBonfireMenu()
  {
  	if (BonfireMenuWidget && BonfireMenuWidget->IsInViewport() && BonfireMenuWidget->CloseCurrentSubMenu())
  	{
  		return;
  	}
  	ForceCloseBonfireMenu();
  }
  void AWarriorController::ForceCloseBonfireMenu()
  {
  	if (BonfireMenuWidget && BonfireMenuWidget->IsInViewport())
  	{
  		BonfireMenuWidget->RemoveFromParent();
  		bShowMouseCursor = false;
  		if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
  		{
  			Subsystem->AddMappingContext(IMC_Warrior, 0);
  		}
  		SetInputMode(FInputModeGameOnly());
  	}
  }
  // :825  TeleportToBonfire 끝          ForceCloseBonfireMenu();
  // BonfireMenuWidget.cpp  OnLeaveClicked   PC->ForceCloseBonfireMenu();
  ```
  (컴파일 확인 전의 제안이다.) 나머지 두 키 바인딩(`:253-254`)은 `CloseBonfireMenu`를 그대로 쓴다.
  덧붙여 `BonfireMenuWidget::CloseCurrentSubMenu`(`BonfireMenuWidget.cpp:96-113`)는 두 분기가 똑같이 `ShowSubMenu(nullptr); return true;`이므로 `const bool bOpen = (레벨업 열림) || (텔레포트 열림); if (bOpen) { ShowSubMenu(nullptr); } return bOpen;`으로 줄일 수 있다.
- [ ] **26-2. (중간) 에디터에서 인게임 맵을 직접 열면 자동 저장이 기존 세이브를 덮는 경로가 남아 있다. (25-1의 잔여)**
  메인 메뉴를 거치면 불러오거나(`LoadGame=1`) 슬롯을 지우므로(새 게임) 안전하다. 그러나 `BaeTestMap`을 PIE로 직접 열면 옵션이 없어 `PlayerState`가 기본값인데, 슬롯은 남아 있다. 이때 화톳불에서 `E`를 누르면 자동 저장(`BonFire.cpp`, `LevelUpWidget.cpp`)이 기본 상태로 슬롯을 덮는다. 개발 중에 가장 자주 하는 동작이라 이어하기 시험용 세이브가 사라질 수 있다. `SaveGame()` 안에서 "이번 세션에서 불러왔거나 슬롯이 없을 때만 저장"하는 가드 하나면 막힌다.
  ```cpp
  // GamePlay/ZTSGameMode.h (private)
  bool bLoadedThisSession = false;
  // GamePlay/ZTSGameMode.cpp  LoadGame()에서 LoadSaveDataFrom 다음 줄에 bLoadedThisSession = true;
  // SaveGame() 시작부(:67)
  if (!bLoadedThisSession && UGameplayStatics::DoesSaveGameExist(SlotName, 0))
  {
  	UE_LOG(LogTemp, Warning, TEXT("[GameMode] 불러오지 않은 세션이라 기존 세이브를 덮어쓰지 않습니다."));
  	return;
  }
  ```
  새 게임은 슬롯을 지운 뒤 시작하므로 첫 저장이 허용된다. 이렇게 하면 에디터에서 맵을 직접 열어 시험하다가 이어서 저장되지 않는다는 점은 알아 둘 것(시험용 저장을 원하면 `IA_Load`로 먼저 불러오거나 슬롯을 지운다).
- [ ] **26-3. (중간) 로딩 화면의 선행 로딩이 레벨 전환 때 풀릴 가능성이 높다. 효과 확인 필요.**
  `ULoadingScreenWidget`이 에셋을 자기 멤버 `FStreamableManager`(`LoadingScreenWidget.h:66`)와 핸들(`LoadingHandle`)로 불러 둔다. 100%가 되면 `OpenLevel`을 부르고(`UpdateLoadingProgress`의 끝), 위젯은 맵 전환과 함께 파괴되며 `NativeDestruct`가 `LoadingHandle->CancelHandle()`을 부른다(`:44-51`). 핸들이 사라지면 불러 둔 에셋을 붙잡는 강한 참조도 같이 사라져서, 맵 전환 중 가비지 컬렉션 때 풀릴 수 있다. 결과적으로 로딩 화면이 "불러 두는 척"만 하고 인게임 맵이 다시 불러올 수 있다. 확인하려면 로딩 후 인게임 진입 시간을 이 폴더 목록을 비웠을 때와 비교해 본다.
  유지하려면 (1) 전역 스트리밍 관리자(`UAssetManager::GetStreamableManager()`)를 쓰고, (2) 핸들을 위젯이 아니라 `GameInstance`(또는 `UGameInstanceSubsystem`)가 들고 있어야 한다. 그리고 `TargetFolders`로 한 폴더 전체를 긁으면 큰 에셋 팩(`Frank_Slash_Pack` 약 957MB, 9-17) 폴더를 실수로 넣는 순간 메모리와 시간이 크게 늘어난다. 폴더는 인게임 맵이 참조하는 필수 에셋 위주로 좁힐 것.
- [ ] **26-4. (중간) `TakeStanceDamage`에 복사 오류가 있다. (`CharacterStatComponent.cpp`, 팀원 병합분이지만 이 파일의 `Stance`)**
  1. `:355` `SetCurrentStance(CurrentStamina - Amount);` — `CurrentStance`여야 한다. 지금은 스태미나 값으로 그로기 수치를 덮는다(예: 스태미나 60, 피해 15면 그로기가 45로 바뀜).
  2. `:358-359` 그로기 피해를 받을 때 `LastStaminaUseTime`을 갱신해서 **스태미나 회복이 지연**된다. 스태미나 함수에서 복사한 줄로 보인다(`LastStanceUseTime`이 선언만 되어 있음, `.h:191`).
  3. `RegenerateStance`는 선언만 있고 정의도 호출도 없어서(`.h:192`) 그로기가 회복되지 않는다. 지금은 쓰지 않는 선언이라 링크 오류는 없다.
  4. `LoadCharacterStat`(`:78`)은 `MaxStance`를 `StatDefine`에서 읽지만 `CurrentStance`는 읽지 않아, 헤더 기본값 100이 `MaxStance` 10보다 큰 채로 시작한다. `CurrentStance = MaxStance;`를 같이 둘 것(`CurrentHP`와 같은 방식).
  5. 의미가 어긋난다: `StatDefine.csv`의 설명은 "이 수치가 **다 차면** 자세가 무너짐"인데 코드는 `Max`에서 줄어들다 **0이 되면** 무너지는 방식이다. 어느 쪽이 정식인지 정하고 CSV 설명이나 함수 중 하나를 맞출 것.
  ```cpp
  // Character/CharacterStatComponent.cpp:355-360
  SetCurrentStance(CurrentStance - Amount);
  if (UWorld* World = GetWorld())
  {
  	LastStanceUseTime = World->GetTimeSeconds();
  }
  ```
  `Warrior.cpp:742`, `:867`이 이 함수의 반환값으로 그로기 상태를 판단하므로 위 1번은 이미 전투에 영향을 준다.

### 3) 설계와 안전성 (낮음 ~ 중간)

- [ ] **26-5. (낮음) 마지막 화톳불 기록과 이동.**
  - `ZTSGameMode.cpp:122`는 `TeleportToBonfire`의 반환값(`bool`)을 무시하고 바로 "이동 완료" 로그를 남긴다. 이동이 실패해도(예: `LastBonfireID`에 해당하는 화톳불이 이 맵에 없음) 성공으로 기록된다. `if (PC->TeleportToBonfire(...))` 안에서만 로그를 남길 것.
  - 이 코드는 `IA_Load` 키로 불러올 때도 실행되어, 키를 누르면 마지막 화톳불로 이동한다. 의도라면 괜찮지만 "저장 위치로 이동"이라는 사실을 `LoadGame` 주석에 남길 것.
  - `WarriorPlayerState.h:45-46`의 `GetLastBonfireID()`는 `const`가 아니고, `SetLastBonfireID(const FName& IsBonfireID)`의 매개변수 이름이 `IsBonfireID`(불리언처럼 읽힘)이다. `BonfireID`로 고칠 것.
  - 화톳불은 점화(`BonFire.cpp:181`)와 휴식(`:211`)에서 마지막 화톳불을 갱신한다. 텔레포트로 도착한 화톳불은 갱신되지 않는다(도착 후 `E`를 눌러 쉬어야 기록됨). 의도라면 괜찮다.
- [ ] **26-6. (낮음) `MainMenuWidget`, 옵션 문자열.**
  - `"LoadGame=1"`(`MainMenuWidget.cpp:66`)과 `HasOption(Options, TEXT("LoadGame"))`(`ZTSGameMode.cpp:30`)이 같은 문자열을 두 곳에서 따로 쓴다. 상수 하나로 묶고, 받는 쪽은 키 존재 여부(`HasOption`)가 아니라 값(`GetIntOption(Options, TEXT("LoadGame"), 0) != 0`)을 보는 편이 `LoadGame=0`을 다르게 처리한다.
  - `StartGameWithLoadingScreen`이 `SetShowMouseCursor(false)` + `FInputModeUIOnly`로 바꾼 뒤 로딩 위젯을 만든다. 로딩 위젯이 만들어지지 않으면 `RemoveFromParent()` 후 즉시 `OpenLevel`하는 대체 경로(`:96-98` 부근)가 있어 좋다. 인게임 쪽은 `AWarriorController::BeginPlay`(`:151-154`)가 `GameOnly`와 커서 숨김으로 되돌리므로 짝이 맞는다. 다만 이 `BeginPlay` 변경은 **`AWarriorController`를 쓰는 모든 레벨**을 강제로 게임 입력으로 시작하게 한다. 타이틀 맵은 `BP_TitleGameMode`(미추적)가 다른 컨트롤러를 쓴다고 가정한 것이므로, 타이틀에서 `AWarriorController`가 지정되어 있지 않은지 확인할 것.
  - 25-4의 슬롯 이름 중복(`"SaveSlot"`), `InGameMapName` 기본값(`BaeTestMap`), `GEngine` 검사 없는 메시지는 그대로다.
- [ ] **26-7. (낮음) `LoadingScreenWidget`의 작은 문제.**
  - `LoadingScreenWidget.cpp:126`의 `static float LastLogTime`은 함수 안 정적 변수라 위젯 인스턴스가 바뀌어도 값이 남는다. 두 번째 로딩부터는 `ElapsedTime - LastLogTime`이 음수여서 로그가 뜨지 않는다. 멤버로 옮길 것. 같은 줄의 형식 문자열 `(에셋: %.9f%%`는 괄호가 닫히지 않고 소수 자리가 9개다.
  - `ElapsedTime += 0.05f`(`:90`)는 타이머 주기를 가정한 값이라 프레임이 끊기면 최소 표시 시간(1.5초)이 실제보다 길어진다. `GetWorld()->GetDeltaSeconds()`나 `FApp::GetCurrentTime()` 차이를 쓸 것.
  - 에셋 로딩이 끝나지 않으면(`bIsAssetLoadFinished`가 영원히 거짓) 진행률이 90%에서 멈추고 레벨 전환이 일어나지 않는다. 최대 대기 시간(타임아웃)이 있으면 안전하다.
  - `ConverToPackagePath`는 `Convert`의 오타(헤더 선언까지). `NativeDestruct`의 `Super::NativeDestruct()`가 `if` 밖으로 나온 것은 올바른 수정이다(이전에는 핸들이 있을 때만 `Super`가 불렸다).
  - `OutAssets.AddUnique(SoftPath)`(`:189`)는 에셋이 수천 개일 때 O(n²)다. `TSet`으로 모은 뒤 배열로 바꾸면 된다(폴더를 좁히면 문제없음).
- [ ] **26-8. (낮음) 자잘한 것.**
  - `BonfireMenuWidget.cpp` 첫 줄에 BOM(`U+FEFF`)이 추가되었다. 프로젝트의 다른 파일도 BOM이 섞여 있어 크게 문제는 아니나 에디터 설정을 통일할 것.
  - `StatDefine.csv`의 `Stance` 행은 CSV와 `DT_StatDefine.uasset`이 함께 수정되어 동기화되었다(9-15 규칙을 지킴).
  - 25-2(`AutoSave()` 통합)는 이번에도 그대로여서 `BonFire.cpp`에는 `SaveGame` 호출이 두 곳 + `SetLastBonfireID`가 각각 붙은 형태가 되었다. `AutoSave()` 안에 "마지막 화톳불 기록 + 저장"까지 묶으면 점화/휴식 두 분기가 짧아진다.

### 4) 확인된 좋은 점

- 이어하기를 `GameInstance` 전역 변수가 아니라 **맵 옵션**으로 넘겼다. 레벨 전환 뒤에 새로 만들어지는 `GameMode`가 자기 일을 스스로 알게 되고, 에디터에서 `?LoadGame=1`을 붙여 같은 경로를 시험할 수도 있다.
- `LoadSaveDataFrom`이 데이터 교체와 폰 스탯 갱신을 한 함수로 묶어서, 어디서 불러오든(`IA_Load`, 이어하기) 스탯이 같은 방식으로 맞춰진다.
- 서브메뉴를 `Esc`로 단계적으로 닫는 규칙과 화톳불 메뉴가 열려 있는 동안 이동 매핑을 빼는 처리가 소울라이크의 메뉴 조작과 맞는다. 26-1만 분리하면 두 규칙이 모두 살아난다.
- `ULoadingScreenWidget::NativeDestruct`의 `Super::` 위치 수정, `Build.cs`의 `AssetRegistry` 추가가 정확하다.
- 새 게임이 기존 세이브를 지운 뒤 시작하고, 이어하기 버튼은 세이브가 없으면 비활성화된다(`MainMenuWidget.cpp:35-38`).

### 5) 병합으로 들어온 팀원 코드에서 이 문서에 이어 확인할 것

- 19-3, 19-4(Ahn의 장착 어서션/null)가 수정되었는지는 이번에 코드를 다시 읽지 않았다. 이전 점검 때 장착 UI가 연결되어 닿는 상태였다(25-3). 장착 버튼을 눌러 보기 전에 `EquipmentComponent.cpp`와 `Warrior.cpp`의 `EquipWeapon`을 먼저 확인할 것.
- `Warrior::ReceiveDamage`(`Warrior.cpp:711~`)가 `bIsInvincibility`/`bIsGuard`를 보고 `CharacterStatComponent`로 이어진다. 이 쪽은 19-6의 "방어·무적·사망 없음"이 일부 해소된 형태다. 사망 처리와 `CurrentHP <= 0`에서의 레벨업 부활(17-6)은 여전히 열려 있다.
- `EnemyStatComponent`/`CombatDamageInterface`는 적 쪽이 별도 컴포넌트로 가는 구조다. 이 문서의 3-2, 4-5 항목은 "별도 컴포넌트로 결정됨"으로 정리할 수 있다.

### 6) 우선순위

1. **26-1**: `CloseBonfireMenu`를 `Esc`/`Q`용과 전체 닫기용으로 나눈다. 텔레포트 후 메뉴가 남는 문제가 바로 보이는 증상이다. `떠나기`도 같이 고친다.
2. **26-4**: `TakeStanceDamage`의 `CurrentStamina` → `CurrentStance`, `LastStaminaUseTime` 갱신 제거. 한 곳이고 이미 전투에 영향을 준다.
3. **26-2**: `SaveGame`에 `bLoadedThisSession` 가드. 에디터 시험 중 세이브가 덮이기 전에 넣는다.
4. **26-3**: 로딩 화면의 선행 로딩이 실제로 효과가 있는지 시간을 재 본다. 없다면 단순하게 `OpenLevel`만 쓰거나, 핸들을 `GameInstance`로 옮긴다.
5. 26-5 ~ 26-8과 25-2, 25-4는 정리 작업으로 묶는다.
