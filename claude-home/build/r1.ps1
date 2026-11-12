# ================= 1 COVER =================
$s = BlankSlide $KRAFT
$bg = CorrBand $s 0 0 960 540; $bg.ZOrder(1)
$null = Card $s 56 70 580 372 $PAPER 6 14
$null = Tape $s 36 58 76 22 -32
$null = Tape $s 586 58 76 22 32
$null = Tape $s 36 426 76 22 32
$null = Chip $s ('원티드 포텐업  |  베타 발표') 80 94 260 32 $ORG $INK 14
$null = AddT $s '2026.11.13' 350 96 270 28 14 $false $MUTE 3 3
$null = AddT $s 'ZZakTungSoul' 74 140 550 78 56 $true $INK 1 3
$null = AddT $s '소울도 유쾌할 수 있다' 76 222 540 44 30 $true $RED 1 3
$dl = $s.Shapes.AddLine(80,284,616,284); $dl.Line.ForeColor.RGB = $DKRAFT; $dl.Line.Weight = 1.5; $dl.Line.DashStyle = 4
$null = AddT $s 'KusoGames  (쿠소게임즈)' 76 294 540 32 22 $true $INK 1 3
$lg = AddPic $s 'logo.png' 80 340 64 64
$null = AddT $s 'Ahn  ·  Bae  ·  Kang  ·  Lee' 156 344 470 30 20 $true $INK 1 3
$null = AddT $s '2026.10.26 ~ 11.13   |   Unreal Engine 5.8 · C++' 156 376 470 24 13 $false $MUTE 1 3
$null = FramePic $s 'knight.png' 666 68 256 293
$null = Tape $s 760 56 70 20 -4
$null = Chip $s '골판지 기사  ·  KusoGames 마스코트' 666 380 256 36 $ORG $INK 14
$pill = Card $s 820 506 112 28 $PAPER 0 8
$s.Shapes('Group 4').ZOrder(0)
SetNotes $s @(
 '[발표 멘트]',
 '안녕하세요, 쿠소게임즈입니다. 저희가 만든 ZZakTungSoul을 소개합니다. "소울도 유쾌할 수 있다"가 이 게임의 한 줄 요약입니다.',
 '팀은 Ahn, Bae, Kang, Lee 4명이고, 10월 26일부터 11월 13일까지 Unreal Engine 5.8과 C++로 만들었습니다.',
 '',
 '[상세 자료]',
 '- 행사: 원티드 포텐업 베타 발표 (2026.11.13)',
 '- 팀명: KusoGames (쿠소게임즈) / 프로젝트명: ZZakTungSoul (짝퉁 소울)',
 '- 표지의 기사는 인게임 골판지 갑옷 캐릭터(팀 마스코트)'
)

# ================= 2 TOC =================
$s = Base $CREAM
$null = AddT $s '목차' 56 90 330 96 66 $true $INK 1 3
$null = Thick $s 62 196 190 196 $ORG 8
$null = AddT $s 'ZZakTungSoul  베타 발표' 56 214 330 28 16 $false $MUTE 1 3
$toc = @('프로젝트 팀 구성 및 역할','프로젝트 개요','협업을 위한 노력 / 문서화','프로젝트 수행 경과 및 방법','성능 최적화 및 개선 사항','향후 확장 가능성','시연 (베타 영상)','마무리')
for($i=0;$i -lt 8;$i++){
  $y = 60 + $i*54
  $null = AddT $s ('{0:00}' -f ($i+1)) 420 $y 60 46 26 $true (Rgb 214 120 0) 1 3
  $null = AddT $s $toc[$i] 484 $y 440 46 24 $true $INK 1 3
  if ($i -lt 7) { $null = Thick $s 424 ($y+50) 920 ($y+50) $TAPE 1.25 4 }
}
SetNotes $s @(
 '[발표 멘트]',
 '발표는 평가안의 8개 항목 순서대로 진행합니다. 가장 비중이 큰 곳은 04번 수행 경과이고, 마지막에 5분 시연 영상을 보여 드립니다.',
 '',
 '[시간 배분 제안 - 발표 시간에 맞춰 조정]',
 '- 01~03: 약 3분 / 04: 약 7분 / 05: 약 3분 / 06: 1분 / 07 시연 영상: 5분 / 08: 1분'
)

# ================= 3 TEAM =================
$s = Base $CREAM
$null = Head $s '01' '프로젝트 팀 구성 및 역할' ('4명이 영역별 "주인"이 되어' + "`r" + '하나의 게임을 만들었다') 36 600
$m = @( @('Ahn','통합 · 아이템',$PUR), @('Bae','화톳불 · UI · 패키징',$BLU), @('Kang','적 AI · 스포너',$GRN), @('Lee','전투 · 보스',$RED) )
for($i=0;$i -lt 4;$i++){
  $y = 218 + $i*68
  $null = Dot $s 62 ($y+27) 26 $m[$i][2] 2.5
  $null = AddT $s $m[$i][0] 90 $y 130 54 32 $true $INK 1 3
  $null = AddT $s $m[$i][1] 226 $y 380 54 26 $true $INK 1 3
  if ($i -lt 3) { $null = Thick $s 48 ($y+60) 600 ($y+60) $TAPE 1.25 4 }
}
$null = FramePic $s 'knight.png' 668 130 240 275
SetNotes $s @(
 '[발표 멘트]',
 '4명이 영역별로 주인을 맡았습니다. Ahn은 아이템·통합, Bae는 화톳불·UI·패키징, Kang은 적 AI와 스포너, Lee는 전투와 보스를 맡았습니다. "사람 냄새 나는 게임을 만들자"가 팀 슬로건입니다.',
 '',
 '[상세 자료 - 담당 업무]',
 '- Ahn (ahnw00) 통합 · 아이템: main 병합 담당 · 인벤토리·장비·아이템 UI · 아이템 세이브/로드 · 패링 · 처형(앞잡·뒤잡) · 물약·랜턴 · 플레이어 애니메이션',
 '- Bae (dino3208) 화톳불 · UI · 패키징: 스탯 컴포넌트·HUD · 화톳불(레벨업·텔레포트·프롭 복원·불 이펙트) · 메인화면·로딩·세이브/로드 파이프라인 · 처형 카메라 · UI 이미지 · 패키징',
 '- Kang (koo1805) 적 AI · 스포너: 적 베이스·Behavior Tree(순찰·탐색·추격·공격) · 공격 판정 Trace 컴포넌트·데미지 시스템 · 몹 스포너 · 매복 적 · 아이템·소울 드랍 · 사운드',
 '- Lee (goodleeches) 전투 · 보스: 콤보 공격 · 피격·그로기(Poise/Stance) · 삐에로 보스·2페이즈 · 보스방(안개문·컷신) · 알림 UI · 보스 HUD · 피격·패링 이펙트',
 '',
 '[기능별 역할 분담표 - 커밋·개발일지 기준 초안, 팀원 확인 필요] (●주 담당 ○지원)',
 '- 전투·판정: Ahn ●, Kang ●, Lee ●  / AI·스포너: Kang ●',
 '- 보스·보스방: Lee ●, Bae ○  / 아이템·인벤토리: Ahn ●',
 '- 화톳불·성장: Bae ●  / UI·HUD·알림: Bae ●, Lee ●, Ahn ○',
 '- 세이브/로드: Ahn ●, Bae ●, Lee ○  / 연출·사운드·이펙트: 전원 ●',
 '- 통합·패키징: Ahn ●, Bae ●'
)

# ================= 3-1 AHN WORK (screenshots) =================
$s = Base $CREAM
$null = Head $s '01' '프로젝트 팀 구성 및 역할 · Ahn 담당 작업' ('플레이어 애니메이션과' + "`r" + '인벤토리 UI를 만들었다') 34 880
$null = FramePic $s 'ahn_anim.png' 40 244 440 214
$null = FramePic $s 'ahn_ui.png' 520 244 387 214
$null = AddT $s '플레이어 애니메이션  ·  ABP_Warrior' 40 470 440 26 15 $true $INK 1 3
$null = AddT $s '인벤토리 UI  ·  WBP_ItemListWidget' 520 470 387 26 15 $true $INK 1 3
SetNotes $s @(
 '[발표 멘트]',
 'Ahn이 담당한 작업의 화면입니다. 왼쪽은 플레이어 애니메이션 블루프린트(ABP_Warrior)로, 이동과 방어(Block) 상태를 무기 종류에 따라 블렌드합니다. 오른쪽은 인벤토리 UI(WBP_ItemListWidget)로, 골판지 스타일의 아이템 목록과 아이템 정보 창입니다.',
 '',
 '[상세 자료 - 스크린샷에서 확인되는 내용]',
 '- ABP_Warrior(부모 클래스: Warrior Anim Instance): AnimGraph의 Locomotion 상태 머신에 Move / Block 상태와 Move to Block, Block to Move 전환 규칙이 있다.',
 '- Block 상태: BS_Block_SwordAndShield, BS_Block_TwoHanded 블렌드 스페이스(Direction · Speed 입력)를 Weapon State(None / Two Hand / One Hand With Shield / Katana)로 Blend Poses by Enum하여 최종 포즈로 출력한다. EventGraph는 BlueprintUpdateAnimation에서 Pawn Speed · Direction · Move State · Action State · Weapon State 값을 갱신한다.',
 '- WBP_ItemListWidget(부모 클래스: Item List Widget): 필터 버튼 영역(FilterContainer), 아이템 목록(ScrollBox · ItemContainer), 아이템 정보 창(InfoWindow)으로 구성. 정보 창에는 아이콘, 아이템명, 설명, 공격력, 데미지 경감률, 무게, 방어력, 저항, 강인도 텍스트가 있고 HandWriting 폰트와 외곽선으로 골판지 스타일을 맞췄다.',
 '- 담당 범위(팀 구성 슬라이드와 연결): 플레이어 애니메이션, 인벤토리·장비·아이템 UI.'
)
# ================= 3-2 LEE WORK (screenshots) =================
$s = Base $CREAM
$null = Head $s '01' '프로젝트 팀 구성 및 역할 · Lee 담당 작업' ('삐에로 보스와' + "`r" + '보스방(안개문 · 트리거)을 만들었다') 34 880
$null = FramePic $s 'lee_boss.png' 80 222 297 250
$null = FramePic $s 'lee_room.png' 401 222 478 250
$null = AddT $s '삐에로 보스 캐릭터' 80 482 297 22 14 $true $INK 1 3
$null = AddT $s '보스방  ·  BP_BossRoom' 401 482 478 22 14 $true $INK 1 3
SetNotes $s @(
 '[발표 멘트]',
 'Lee가 담당한 작업의 화면입니다. 왼쪽은 보스로 쓴 삐에로(광대) 캐릭터입니다. 오른쪽은 보스방 블루프린트(BP_BossRoom)로, 안개문 역할을 하는 게이트 블로커와 진입 트리거, 보스 접근 음악 등을 컴포넌트로 구성했습니다.',
 '',
 '[상세 자료 - 스크린샷에서 확인되는 내용]',
 '- 삐에로 보스 메시: 스켈레탈 메시 에디터에서 본 보스 캐릭터. 양손 도끼를 든 광대 디자인이고, 스켈레톤에 hand_r_Socket, foot_l_Socket, foot_r_Socket이 있다(삼각형 약 76,000개, 정점 약 45,000개).',
 '- BP_BossRoom(부모 클래스: Boss Room): 컴포넌트로 Gate Blocker, Inside Point, Clown Approach Music, Interaction Trigger, Other Side Gate Blocker(BoxComponent), Other Side Fog Mesh 등이 있고, EventGraph에는 BeginPlay, ActorBeginOverlap, Tick 이벤트가 있다. 대부분의 컴포넌트가 C++(Boss Room 클래스)에서 정의되어 블루프린트에서는 "Edit in C++"로 표시된다.',
 '- 같이 열려 있는 탭 WBP_DeathNotification, WBP_BossClearNotification은 사망 · 보스 클리어 알림 UI 작업이다.',
 '- 담당 범위(팀 구성 슬라이드와 연결): 콤보 공격 · 피격(Poise/Stance) · 삐에로 보스 · 보스방(안개문 · 컷신) · 알림 UI · 보스 HUD.'
)
# ================= 3-3 KANG WORK (screenshots) =================
$s = Base $CREAM
$null = Head $s '01' '프로젝트 팀 구성 및 역할 · Kang 담당 작업' ('몹 스포너를 데이터로 설정하고,' + "`r" + '내비 메시 위에서 AI를 테스트했다') 34 880
$null = FramePic $s 'kang_spawner.png' 38 226 468 224
$null = FramePic $s 'kang_map.png' 526 226 395 224
$null = AddT $s 'BP_EnemySpawner  ·  Spawn Data List' 38 462 468 24 14 $true $INK 1 3
$null = AddT $s 'AI 테스트 맵  ·  NavMesh' 526 462 395 24 14 $true $INK 1 3
SetNotes $s @(
 '[발표 멘트]',
 'Kang이 담당한 작업의 화면입니다. 왼쪽은 몹 스포너 블루프린트(BP_EnemySpawner)로, 에디터에서 Spawn Data List에 적 클래스, 스폰 지점, 순찰 경로를 넣어 적을 배치합니다. 오른쪽은 AI를 테스트한 맵으로, 내비 메시 위에서 적이 순찰하고 플레이어를 추적하는지 확인했습니다.',
 '',
 '[상세 자료 - 스크린샷에서 확인되는 내용]',
 '- BP_EnemySpawner의 Spawn Data List는 요소 2개이고, 각 요소는 3개 멤버(Enemy Class, Spawn Point, Patrol Path)이다. Index 0: BP_MeleeEnemy02 / TargetPoint / 순찰 경로 없음(None). Index 1: BP_MeleeEnemy02 / TargetPoint4 / BP_PatrolPath.',
 '- 코드 없이 에디터에서 적 종류 · 위치 · 순찰 경로를 바꿀 수 있는 데이터 기반 구조이다(필수 요건 1.4와 연결).',
 '- 테스트 맵: 흰 벽 구조물과 상자 · 통 소품이 있는 임시 맵에서 RecastNavMesh(초록색 영역, 디버그 정보 표시) 위에 적 캐릭터를 배치해 순찰 · 추적을 확인한다. 화면에는 HUD(Souls, Q · E 벨트 슬롯)도 함께 보인다.',
 '- 담당 범위(팀 구성 슬라이드와 연결): 적 베이스 · Behavior Tree(순찰 · 탐색 · 추격 · 공격) · 공격 판정 Trace · 몹 스포너 · 매복 적 · 드랍.'
)
# ================= 3-4 BAE WORK (GIF placeholder - to be filled at home) =================
$s = Base $CREAM
$null = Head $s '01' '프로젝트 팀 구성 및 역할 · Bae 담당 작업' ('마커리스 모션 캡처로' + "`r" + '직접 연기한 영상') 34 880
$bx = Card $s 60 206 520 292 $LGRAY 5 10
$bx.Name = 'GIF_PLACEHOLDER'
$bx.Line.DashStyle = 4; $bx.Line.ForeColor.RGB = $MUTE; $bx.Line.Weight = 1.5
$lb = AddT $s ('[ 모션 캡처 연기 GIF 삽입 ]' + "`r" + '(집에서 추가 예정)') 60 316 520 70 20 $false $MUTE 2 3
$lb.Name = 'GIF_PLACEHOLDER_LABEL'
$tx = AddT $s ('(작성 예정)' + "`r" + '사용한 도구 · 연기한 동작' + "`r" + '게임에 적용한 위치') 620 236 300 120 18 $false $MUTE 1 1
$tx.Name = 'TEXT_PLACEHOLDER'
SetNotes $s @(
 '[발표 멘트 - 작성 예정]',
 'Bae가 마커리스 모션 캡처로 직접 연기한 영상입니다. (사용한 도구, 어떤 동작을 연기했는지, 게임의 어느 애니메이션에 적용했는지를 여기에 작성)',
 '',
 '[TODO - 집에서 채울 것]',
 '- 왼쪽 점선 박스(GIF_PLACEHOLDER)에 모션 캡처 연기 GIF를 넣는다. claude-home 폴더의 insert_gif.ps1 사용.',
 '- 오른쪽 "(작성 예정)" 텍스트(TEXT_PLACEHOLDER)에 사용한 도구 · 연기한 동작 · 적용 위치를 쓴다.',
 '- 제목 "마커리스 모션 캡처로 직접 연기한 영상"은 임시 문구이므로 실제 내용에 맞게 바꾼다.',
 '- 담당 범위(팀 구성 슬라이드와 연결): 스탯 컴포넌트 · HUD · 화톳불 · 메인화면 · 로딩 · 세이브/로드 · 처형 카메라 · UI 이미지 · 패키징.'
)
# ================= 4 INTENT =================
$s = Base $CREAM
$null = Head $s '02' '프로젝트 개요 · 기획 의도' ('왜 유쾌한 소울은' + "`r" + '없을까?') 54 600
$null = Thick $s 46 262 186 262 $ORG 9
$null = AddT $s ('긴장감은 지키되,' + "`r" + '죽는 과정 자체가 웃음이 되는 게임') 40 286 580 90 24 $true $INK 1 1
$null = FramePic $s 'knight.png' 650 120 258 296
$null = AddT $s '골판지 기사 — KusoGames 마스코트' 640 428 280 22 12 $false $MUTE 2 3
SetNotes $s @(
 '[발표 멘트]',
 '기획의 출발점은 "왜 유쾌한 소울은 없을까?"라는 질문이었습니다. 소울류의 긴장감은 그대로 두고, 죽고 도전하는 과정 자체가 웃음이 되도록 만들었습니다.',
 '축은 세 가지입니다. 소울류의 기본기, 유쾌한 분위기, 수제 연출입니다.',
 '',
 '[상세 자료]',
 '- 기획 의도: 매너리즘에 빠진 소울 장르에 경각심을 던지고, 소울류 긴장감은 유지하되 도전하고 죽는 과정 자체가 웃음이 되는 게임을 만든다.',
 '- 참고한 유쾌한 소울라이크: Unleash The Avatar (AeosGames), Another Crab''s Treasure (Aggro Crab)',
 '- 01 소울류의 기본기: 락온 · 구르기 · 스태미나 / 몹·보스 패턴 / 섬세한 히트박스',
 '- 02 유쾌한 분위기: 어이없는 처형·피격 모션 / 모든 아이템이 무기(에스트병도 들 수 있다)',
 '- 03 수제 연출: 처형(앞잡·뒤잡) 카메라 / 보스 인트로 레벨 시퀀스 / 골판지 갑옷 세계관',
 '- 타깃: 소울류를 알지만 진지함이 부담스러운 유저, 어려워서 망설였던 입문 유저'
)

# ================= 5 STRUCTURE =================
$s = Base $CREAM
$null = Head $s '02' '프로젝트 개요 · 프로젝트 구조' ('목적: 끝까지 플레이 가능한' + "`r" + '소울라이크를 팀으로 완성한다') 36 880
$null = Thick $s 50 350 910 350 $INK 7
$ph = @( @('10.01 ~ 10.25','기반 구축'), @('10.26 ~ 10.30','코어 루프'), @('11.02 ~ 11.06','콘텐츠 연결'), @('11.07 ~ 11.13','완성도 · 패키징') )
for($i=0;$i -lt 4;$i++){
  $cx = 140 + $i*226
  $null = Dot $s $cx 350 40 $(if($i -eq 3){$INK}else{$ORG}) 3
  $null = AddT $s $ph[$i][0] ($cx-100) 290 200 28 16 $true $DKRAFT 2 3
  $null = AddT $s $ph[$i][1] ($cx-105) 386 210 40 24 $true $INK 2 3
}
SetNotes $s @(
 '[발표 멘트]',
 '목적은 기획부터 개발, 협업, 테스트까지 팀으로 경험하며 끝까지 플레이 가능한 소울라이크를 완성하는 것입니다. 일정은 네 단계로, 기반 구축 → 코어 루프 → 콘텐츠 연결 → 완성도·패키징 순서로 진행했고 11월 11일에 패키징을 마쳤습니다.',
 '수치로는 main 기준 커밋 335개, C++ 파일 238개입니다.',
 '',
 '[상세 자료 - 단계별 구현 내역]',
 '- 10.01~10.25 기반 구축: 캐릭터·스탯·HUD·세이브 / 적 AI 골격(Perception·BT) / 화톳불 액터·공격 판정 Trace',
 '- 10.26~10.30 코어 루프: 화톳불 레벨업·텔레포트 / 메인화면·비동기 로딩 / 데미지 시스템·보스 클래스',
 '- 11.02~11.06 콘텐츠 연결: 보스방·삐에로 보스 / 패링·처형·물약·랜턴 / 몹 스포너 1차·아이템 저장',
 '- 11.07~11.13 완성도·패키징: 프롭 복원·불 이펙트·사운드 / 드랍·매복 적·보스 HUD / 패키징(11.11)·맵 배치',
 '- 통계: 커밋 335(main 기준), C++ 파일 238개(약 2.9만 줄), main 병합 38회, 팀원 4명·개인 브랜치 4개',
 '',
 '[수행 절차 - 계획(기획 발표) 대비 실제와 대응]',
 '- 1주차(10.26~10.30) 계획: 코어 루프 - 전투·피격·사망, 일반 몹 사이클, 화톳불·레벨업, 맵 구성 / 실제: 화톳불 레벨업·텔레포트, 메인화면·비동기 로딩, 데미지 시스템, 벨트 UI / 대응: 메인화면·텔레포트를 앞당겨 사용자 흐름을 먼저 확보',
 '- 2주차(11.02~11.06) 계획: 인벤토리·아이템, 보스 1, 세이브 연동, 수제 애니메이션 / 실제: 삐에로 보스·보스방, 패링·처형, 물약·랜턴, 순찰·탐색, 몹 스포너 1차, 아이템 저장 / 대응: 보스 2종 → 삐에로 1종(2페이즈)에 집중, 패링·처형 추가',
 '- 3주차(11.09~11.13) 계획: 보스 2·텔레포트, 메인화면·로딩·크레딧, 카메라 연출, 통합 테스트 / 실제: 프롭 복원·불 이펙트, 사운드, 드랍·매복 적, 몹 스포너 완료, 패키징(11.11), 맵 배치 / 대응: 크레딧 등 보류 항목을 후순위로 두고 패키징 먼저 검증'
)

# ================= 6 TECH =================
$s = Base $CREAM
$null = Head $s '02' '프로젝트 개요 · 활용 기술' ('Unreal Engine 5.8 + C++' + "`r" + '으로 만든 소울라이크') 46 880
$null = FramePic $s 'hud.png' 48 250 480 175
$null = AddT $s '인게임 HUD (골판지 크레용 스타일)' 48 436 480 22 12 $false $MUTE 1 3
$null = AddT $s ('Blueprint · Enhanced Input' + "`r" + 'Behavior Tree · Niagara' + "`r" + 'GitHub · Notion · Drive') 580 262 340 120 20 $false $MUTE 1 1
SetNotes $s @(
 '[발표 멘트]',
 '엔진은 Unreal Engine 5.8이고 C++과 블루프린트를 함께 썼습니다. 협업은 GitHub 개인 브랜치, Slack 주간 미팅, Notion 개발일지, git에 못 올리는 에셋은 Google Drive로 공유했습니다.',
 '',
 '[상세 자료]',
 '- 개발 환경: Unreal Engine 5.8 / C++(Visual Studio) + Blueprint / Windows · PIE 테스트 · 패키징 / 에디터 MCP + Claude Code(AI 보조 개발) / Level Sequencer · Niagara · Chaos 에디터',
 '- 기술 스택: 게임플레이 프레임워크(GameMode·Controller·PlayerState·Character) / Enhanced Input / AI: Perception + Behavior Tree·Blackboard / UMG UI·Level Sequence 컷신 / Niagara 이펙트·Chaos Geometry Collection / DataTable·DataAsset·SaveGame / Subsystem·인터페이스·델리게이트',
 '- 협업 도구: GitHub(main + 개인 브랜치) / Slack: 주간 미팅 후 정리 / Notion: 팀 개발일지·회의록 / Google Drive: git 제외 에셋(Content/Assets) 공유'
)

# ================= 7 COLLAB =================
$s = Base $CREAM
$null = Head $s '03' '협업을 위한 노력 / 문서화' ('개인 브랜치에서 작업하고,' + "`r" + '한 명이 main에 병합한다') 36 880
$null = AddT $s 'main' 40 236 100 36 22 $true $INK 1 3
$null = Thick $s 130 254 900 254 $INK 8
$names = @('Ahn','Bae','Kang','Lee'); $bc = @($PUR,$BLU,$GRN,$RED)
for($i=0;$i -lt 4;$i++){
  $y = 314 + $i*46
  $null = Chip $s $names[$i] 40 ($y-15) 70 30 $bc[$i] $WHITE 14
  $null = Thick $s 120 $y 810 $y $bc[$i] 4
  foreach($dx in 220,340,460,580,700){ $null = Dot $s $dx $y 14 $bc[$i] 1.5 }
}
$null = Thick $s 850 478 850 262 $MUTE 3 4
$null = AddArrow $s 850 300 850 262 $MUTE 3
$null = AddT $s '병합 (Ahn)' 800 484 100 22 13 $true $MUTE 2 3
SetNotes $s @(
 '[발표 멘트]',
 '각자 개인 브랜치에서 작업하고, 병합 전에 커밋한 뒤 병합 담당 한 명이 main에 합치는 방식으로 충돌을 줄였습니다. 병합 후에는 모두가 pull을 받아 PIE로 확인했습니다. 소통은 매주 미팅 후 Slack에 결정 사항을 정리했고, Notion에 팀 개발일지와 회의 결과를 남겼습니다.',
 '',
 '[상세 자료]',
 '- 흐름: 개인 브랜치 커밋 → 머지 전 커밋 → main 병합(Ahn) → 각자 pull + PIE 확인',
 '- 커밋: 335개(main 기준). 개인별 커밋(전 브랜치, 병합 제외): Ahn 77 · Lee 68 · Bae 55 · Kang 53 / main 병합 38회',
 '- Git 규칙: PR #1~#16 이후에는 Merge 커밋으로 병합(병합 담당 1인). git 제외 에셋(Content/Assets)은 Drive로 공유하고 경로를 유지.',
 '- 코드 스타일·리뷰: UE 코딩 표준(A/U/F/E 접두사), UPROPERTY·UFUNCTION 노출. 병합 전 점검 + 리뷰 문서 작성(AI 보조). ※ 스타일 가이드를 별도 문서로 정리했는지는 확인 필요',
 '- 소통·문서화: Slack 주간 미팅(10.23 · 10.30 · 11.06) 후 결정 사항 정리 / Notion 팀 개발일지(브랜치별 기록·구현 현황 표) + 회의 결과 페이지'
)

# ================= 7-1 GITHUB RETROSPECT (Ahn) =================
$s = Base $CREAM
$null = Head $s '03' '협업을 위한 노력 / 문서화 · GitHub 사용 후기' ('서버 관리형 협업 툴 없이 GitHub로,' + "`r" + '머지는 오래 걸렸지만 배운 게 많았다') 34 880
$null = Chip $s '아쉬운 점' 60 232 170 52 $RED $WHITE 20
$null = AddT $s '머지 처리에 시간이 오래 걸렸다' 256 226 660 64 28 $true $INK 1 3
$null = Thick $s 60 318 900 318 $TAPE 1.5 4
$null = Chip $s '좋은 점' 60 346 170 52 $GRN $WHITE 20
$null = AddT $s ('충돌을 최소화하는 머지 방법을 터득했다' + "`r" + '브랜치 · 커밋 관리로 단위별 작업과 버전 관리를 배웠다') 256 332 670 96 24 $true $INK 1 3
SetNotes $s @(
 '[발표 멘트]',
 '서버 관리형 협업 툴 없이 GitHub만 쓰면서 아쉬웠던 점은 머지를 처리하는 데 시간이 오래 걸렸다는 것입니다. 하지만 좋은 점도 있었습니다. 충돌을 최소화하면서 효율적으로 머지하는 방법을 터득했고, 브랜치 관리와 커밋 관리를 통해 단위별로 작업하고 버전을 관리하는 방법을 배웠습니다.',
 '',
 '[상세 자료 - 팀 의견]',
 '- 아쉬운 점: 서버 관리형 협업 툴을 사용하지 않고 GitHub를 사용하면서, 머지를 처리하는 과정에서 시간이 오래 걸렸다.',
 '- 좋은 점: 머지를 효율적으로 충돌을 최소화하여 하는 방법을 터득할 수 있었고, 브랜치 관리와 커밋 관리를 통해 단위별 작업 및 버전 관리를 하는 방법을 배울 수 있었다.',
 '- 앞 슬라이드(협업 · 문서화)의 병합 담당 방식과 연결해서 설명하면 좋습니다.'
)

# ================= 8 LOOP =================
$s = Base $CREAM
$null = Head $s '04' '수행 경과 · 서비스 흐름도' ('죽고, 쉬고, 강해져서' + "`r" + '다시 도전한다') 36 420
$null = Node2 $s '탐험 · 전투' '전투 · 카메라 · AI' 450 196 220 62 $ORG $INK 20 13
$null = Node2 $s '소울 · 아이템' '아이템 · 인벤토리' 730 296 190 62 $LORG $INK 20 13
$null = Node2 $s '화톳불 휴식' '레벨업 · 저장 · 복원' 450 410 220 62 $ORG $INK 20 13
$null = Node2 $s '보스 도전' '안개문 · 보스 연출' 190 296 190 62 $LORG $INK 20 13
$null = AddArrow $s 672 232 770 292 $INK 4
$null = AddArrow $s 820 360 672 436 $INK 4
$null = AddArrow $s 448 436 300 362 $INK 4
$null = AddArrow $s 280 294 448 232 $INK 4
$null = AddT $s '핵심 루프' 460 304 200 40 26 $true $MUTE 2 3
$null = AddT $s '사망하면 3초 뒤 마지막 화톳불에서' 420 342 280 24 13 $true $RED 2 3
SetNotes $s @(
 '[발표 멘트]',
 '게임의 흐름입니다. 탐험하며 몹을 잡으면 소울과 아이템이 나오고, 화톳불에서 쉬면 회복과 저장이 되고 프롭과 몹이 되살아납니다. 소울로 레벨업해서 보스방에 도전하고, 죽으면 3초 뒤 마지막 화톳불에서 다시 시작합니다. 이 루프를 만드는 세 시스템(전투·카메라, AI·스테이지, 아이템·성장)을 다음 슬라이드에서 설명드립니다.',
 '이 흐름이 필수 요건 1.1(시작 - 플레이 - 성공/실패 - 재시작)에 해당합니다.',
 '',
 '[상세 자료 - 서비스 흐름도]',
 '- 타이틀 메인메뉴(뉴게임·로드·종료) → 비동기 로딩(이어하기 지원) → 탐험·일반 몹 전투(락온·구르기·패링·처형) → 소울·아이템 드랍(몹 사망 시)',
 '- 화톳불 휴식: 회복·자동저장·프롭/몹 복원 → 레벨업·텔레포트(소울로 스탯 강화) → 보스방 입장(안개문·인트로 컷신) → 보스전(삐에로 1·2페이즈)',
 '- 성공: 보스 클리어(클리어 알림·새 화톳불·저장) → 다음 구역 진행(엔딩·크레딧은 보류)',
 '- 실패: 사망 → 3초 뒤 마지막 화톳불에서 리스폰'
)

# ================= 9 ARCHITECTURE =================
$s = Base $CREAM
$null = Head $s '04' '수행 경과 · 아키텍처 · C++ / Blueprint · 데이터' ('핵심은 C++,' + "`r" + '값과 콘텐츠는 Blueprint와 데이터') 36 880
$null = Chip $s 'Blueprint   UI · 애님 · BT · 연출' 150 216 660 74 $LORG $INK 24 5 3
$null = Chip $s 'C++   게임 규칙 · 전투 · AI · 저장' 150 306 660 74 $ORG $INK 24 5 3
$null = Chip $s 'Data   DataTable · DataAsset · SaveGame' 150 396 660 74 $PAPER $INK 24 5 3
SetNotes $s @(
 '[발표 멘트]',
 'C++은 스탯·전투·세이브/로드, AI, 서브시스템 같은 핵심 로직을 맡고, 블루프린트는 값과 에셋을 지정하고 UI·애님·BT·연출을 설정하는 데 썼습니다. 콘텐츠는 DataTable과 DataAsset으로 코드 밖에서 관리해서 능력치·아이템·적을 코드 수정 없이 조절할 수 있습니다.',
 '구조는 GameMode(규칙) · Controller(입력·UI) · Character(행동) · PlayerState(진행)로 나누었고, 객체끼리는 인터페이스 9종, 델리게이트 24종, 컴포넌트, 상속, 서브시스템으로 통신합니다.',
 '',
 '[상세 자료 - 아키텍처]',
 '- 엔진·기술: Unreal Engine 5.8 / Enhanced Input / AI(Perception·BT) / UMG / Niagara / Chaos GC / Level Sequence',
 '- 프레임워크: ZTSGameMode(규칙·로드/레벨 이동) / WarriorController(입력·UI 생성) / Warrior(CharacterBase, 캐릭터 행동) / WarriorPlayerState(진행·저장 데이터)',
 '- 컴포넌트: CharacterStat · CharacterState · Inventory · Equipment · AttackTrace · LockOn',
 '- 월드·콘텐츠: EnemyBase → Normal · Ambush · Boss + EnemyAIController(BT) / Bonfire(IInteractable) / BossRoom(상태·안개문) / Subsystem 5종(RestoreProp · EnemySpawn · Music · Notification, 보스방 포함)',
 '',
 '[상세 자료 - 주요 클래스의 책임]',
 '- AZTSGameMode: 게임 규칙, 새 게임/로드, 레벨 전환 / SaveGame · 로딩 위젯 호출',
 '- AWarriorController: Enhanced Input 바인딩, HUD·메뉴 생성 / UMG 위젯 · Character에 입력 전달',
 '- AWarrior : ACharacterBase: 이동·구르기·공격·패링·처형·사망/리스폰 / 인터페이스 · 델리게이트',
 '- AWarriorPlayerState: 레벨·소울·능력치·화톳불 진행 보관 / FWarriorSaveData로 저장',
 '- UCharacterStatComponent: HP/FP/스태미나, 성장·Poise/Stance 계산 / OnHPZero · 스탯 변경 델리게이트 → HUD',
 '- UInventoryComponent · UEquipmentComponent: 아이템 보관, 장착/해제 / OnEquipItem · OnUnequipItem',
 '- UAttackTraceComponent: 소켓 Trace 판정, 중복 방지 / ICombatDamageInterface',
 '- AEnemyBase → Normal · Ambush · Boss: 적 상태(enum), 스탯, AI 데이터 / EnemyAIController + BT/Blackboard',
 '- ABonfire · ABossRoom: 휴식·점화·메뉴 / 보스방 상태·안개문 / IInteractableInterface · OnBonfireRest',
 '- Subsystem 5종: 프롭 복원 · 스폰 · 보스방 · 음악 · 알림 / 화톳불·보스방에서 호출',
 '',
 '[상세 자료 - C++ / Blueprint / 데이터]',
 '- C++: 스탯·성장 계산, 전투 판정, 세이브/로드 / AI Task·Service·Decorator / 인터페이스·델리게이트·컴포넌트 / 서브시스템. C++ 238 파일 · UPROPERTY 738 · UFUNCTION 131',
 '- Blueprint: BP_Warrior · BP_Bonfire · 프롭 BP 등 값·에셋 지정 / UMG 위젯(WBP) · 애님 BP · 몽타주 / Behavior Tree · Blackboard · 카메라/연출 BP. BP 25 · WBP 23 · ABP 5 · 몽타주 33 · BT 10 · BB 5',
 '- DataTable(4): StatDefine · StatGrowth · LevelRequiredSouls · EnemyStat (CSV 재임포트) / DataAsset(19): 아이템 · EnemyAIData · EnemyAttackData · BossAIData',
 '- Struct 설정: FEnemySpawnData(적 클래스·위치·순찰 경로) · FDamageInfo / SaveGame: FWarriorSaveData(레벨·소울·능력치·아이템·장착·물약 횟수·화톳불·보스방 진행)',
 '- UPROPERTY(EditAnywhere / EditDefaultsOnly)로 노출해 BP에서 값을 바꾼다'
)

# ================= 10 REQUIRED =================
$s = Base $CREAM
$null = Head $s '04' '수행 경과 · 공통 필수 요건' ('공통 필수 요건 8개 중' + "`r" + '6개 완료, 2개는 일부 구현') 36 880
for($i=0;$i -lt 8;$i++){
  $cx = 112 + $i*104
  $part = ($i -eq 0 -or $i -eq 6)
  $lab = '1.' + ($i+1)
  $null = Dot $s $cx 310 70 $(if($part){$ORG}else{$GRN}) 3
  $null = AddT $s $lab ($cx-35) 288 70 44 20 $true $(if($part){$INK}else{$WHITE}) 2 3
}
$null = AddT $s ('엔딩 · 크레딧' + "`r" + '보류') 52 358 120 48 15 $true $MUTE 2 1
$null = AddT $s ('키 안내 UI' + "`r" + '확인 필요') 632 358 120 48 15 $true $MUTE 2 1
SetNotes $s @(
 '[발표 멘트]',
 '평가안의 공통 필수 요건 8개를 모두 적용했습니다. 1.1과 1.7은 일부 구현으로 표시했는데, 엔딩·크레딧이 보류 상태이고 키 안내 UI는 확인이 필요하기 때문입니다.',
 '',
 '[상세 자료 - 요건별 적용 위치]',
 '- 1.1 완결된 게임 흐름 (△): 타이틀(뉴게임·로드·종료) → 비동기 로딩 → 플레이 → 보스 클리어 / 사망(리스폰). 엔딩·크레딧은 미구현',
 '- 1.2 게임플레이 프레임워크 (✓): ZTSGameMode(규칙) · WarriorController(입력·UI) · Warrior(행동) · WarriorPlayerState(진행)',
 '- 1.3 C++ / Blueprint 분리 (✓): 핵심 로직은 C++ 238파일, 에셋·UI·애님·BT는 BP/WBP/ABP. UPROPERTY·UFUNCTION으로 노출',
 '- 1.4 데이터 기반 콘텐츠 (✓): DataTable 4 · DataAsset 19 · Struct 설정으로 능력치·아이템·AI·스폰을 코드 밖에서 관리',
 '- 1.5 객체 간 통신 (✓): 인터페이스 9 · 델리게이트 24 · 컴포넌트 · 상속 · 직접 참조 (5가지 모두 사용, 다음 슬라이드에서 이유 설명)',
 '- 1.6 입력 시스템 (✓): Enhanced Input - IMC_Warrior + IA_* 를 WarriorController에서 바인딩',
 '- 1.7 UI · 피드백 (△): HUD(HP·FP·스태미나·벨트) · 보스 HUD · 알림(사망·보스 클리어) · 이펙트 · 사운드. 키 안내 UI는 확인 필요',
 '- 1.8 저장 (✓): SaveGame - 화톳불 자동저장 · 로드게임 · 아이템 · 장착 · 보스방 진행'
)

# ================= 10-1 REQUIREMENT 1.5 COMMUNICATION =================
$s = Base $CREAM
$null = Head $s '04' '수행 경과 · 필수 요건 1.5  객체 간 통신' ('객체 간 통신 5가지를 모두 쓰고,' + "`r" + '의존성을 낮추는 쪽을 택했다') 34 880
$null = AddT $s '강한 결합' 30 178 100 20 13 $true $MUTE 1 3
$null = Thick $s 62 204 62 448 $INK 4
$null = AddArrow $s 62 400 62 450 $INK 4
$null = AddT $s '약한 결합' 30 454 100 20 13 $true $MUTE 1 3
$cm = @(
  @('직접 참조','1:1로 단순할 때 — 화톳불 → 플레이어 상태',$ORG),
  @('상속 · 가상 함수','공통은 부모, 차이는 override — Character → Warrior · Enemy',$ORG),
  @('Actor Component','스탯 · 인벤토리 · 판정을 떼어 재사용',$LORG),
  @('인터페이스','타입이 달라도 같은 요청 — 상호작용 · 데미지',$LORG),
  @('델리게이트','HUD가 HP · 소울 변화를 구독 — 보내는 쪽은 모른다',$PAPER)
)
for($i=0;$i -lt 5;$i++){
  $y = 200 + $i*56
  $null = Chip $s $cm[$i][0] 100 $y 220 46 $cm[$i][2] $INK 18
  $null = AddT $s $cm[$i][1] 336 ($y-2) 590 50 18 $true $INK 1 3
}
SetNotes $s @(
 '[발표 멘트]',
 '필수 요건 1.5는 객체 간 통신 방법을 두 가지 이상 쓰고 그 이유를 설명하는 것입니다. 저희는 다섯 가지를 모두 썼고, 목표는 객체 간 의존성을 낮추는 것이었습니다. 위쪽일수록 결합이 강하고 아래쪽일수록 약합니다.',
 '1대1로 단순한 관계는 직접 참조, 공통 기능은 부모 클래스, 재사용할 기능은 컴포넌트, 타입이 달라도 같은 요청을 보내야 하면 인터페이스, 보내는 쪽이 받는 쪽을 몰라도 되는 알림은 델리게이트를 썼습니다.',
 '',
 '[상세 자료 - 방법별 사용 이유와 코드 예시]',
 '- 언리얼 인터페이스(9종): 상호작용 대상을 IInteractableInterface로 처리(AItemObject · ABossRoom · 화톳불), 데미지는 ICombatDamageInterface(ACharacterBase)로 플레이어·적·보스를 같은 함수로 처리, IFatalAttackableInterface(처형 가능 대상), ISoulStateInterface(WarriorPlayerState), ISaveLoadInterface, IItemInterface, IItemFunctionInterface, ICharacterStatInterface, IAIDataInterface. 이유: 호출하는 쪽이 상대의 구체 클래스를 몰라도 되게 하려고.',
 '- 델리게이트(24종 선언, 코드 대조 완료): 적 사망 OnEnemyDied(EnemySpawner가 구독), HP 변화 OnHPChanged(PlayerHUDWidget::UpdateHP가 구독), HP 0 OnHPZero(Warrior::SetDead가 구독), 소울 변화 OnSoulChanged(PlayerHUDWidget::UpdateSouls가 구독), 장착 OnEquipItem · OnUnequipItem(Warrior가 구독), 장착 이미지 변경 OnEquipItemImageChanged(컨트롤러가 벨트·메뉴 위젯에 바인딩), 아이템 획득 OnItemObtained(Warrior가 구독해 저장 데이터 갱신), 화톳불 휴식 OnBonfireRest(발행), 판정 OnAttackTraceHit · OnGetParried. 이유: HUD·UI가 구독만 하면 되고, 보내는 쪽은 누가 듣는지 몰라도 되므로.',
 '- Actor Component: CharacterStatComponent · CharacterStateComponent · EnemyStatComponent · InventoryComponent · EquipmentComponent · AttackTraceComponent · LockOnComponent. 이유: 체력·능력치, 인벤토리, 공격 판정을 캐릭터 클래스에서 분리해 플레이어·적·보스가 함께 재사용하려고.',
 '- 상속과 가상 함수 재정의: ACharacterBase → AWarrior, AEnemyBase → NormalEnemy · AmbushEnemy · Boss (virtual/override 사용). 이유: 이동·피격·사망 같은 공통 기능은 부모에 한 번만 구현하고 차이만 자식에서 재정의하려고.',
 '- 객체 참조를 이용한 직접 통신(코드 대조 완료): 게임모드(ZTSGameMode)가 로드 후 모든 화톳불에 플레이어 상태를 넘겨 SyncLitState 호출, 화톳불(BonFire)이 월드 서브시스템에서 URestorePropSubsystem을 가져와 RestoreProps 호출, 컨트롤러가 위젯을 생성·보관하며 SetInventory · SetEquipment로 참조를 전달. 이유: 대상이 하나로 정해진 단순한 1:1 관계에는 델리게이트가 오히려 과하기 때문.',
 '- 그 밖에 서브시스템(프롭 복원 · 몹 스폰 · 보스방 · 음악 · 알림)으로 전역 기능에 접근.',
 '- 정리: 의존성을 낮추는 쪽(인터페이스·델리게이트·컴포넌트)을 기본으로 하고, 단순한 곳에만 직접 참조를 사용.'
)

# ================= 11 SELECTED 1 =================
$s = Base $CREAM
$null = Head $s '04' '수행 경과 · 선택 필수 ①  전투 · 카메라' ('타격 하나가 피격 · 그로기,' + "`r" + '처형 연출까지 이어진다') 36 880
$null = Chain3 $s @( @('소켓 Trace 판정','몽타주 구간'), @('데미지 · Poise','HP · Stance 계산'), @('Hit · Stun · 처형','앞잡/뒤잡 + 카메라') )
$null = Bottom $s '전투가 소울과 아이템을 얻는 출발점이다' 440
SetNotes $s @(
 '[발표 멘트]',
 '전투는 몽타주의 판정 구간에서 소켓 Trace로 맞았는지 확인하고, 데미지를 인터페이스로 전달해 Poise와 HP를 계산한 뒤 Hit·Stun·Dead 상태와 HUD·이펙트로 이어집니다. 처형은 앞잡과 뒤잡 두 가지이고 방향별 몽타주와 전용 카메라를 만들었습니다. 이 전투가 소울과 아이템을 얻는 출발점입니다.',
 '',
 '[상세 자료]',
 '- 흐름: 몽타주 판정 구간(AnimNotifyState) → 소켓 Trace 판정(AttackTraceComponent) → 데미지 전달(ICombatDamageInterface) → Poise·Stance·HP(CharacterStatComponent) → Hit·Stun·Dead 상태 + HUD·이펙트',
 '- 공격·방어: 콤보 공격(AnimNotifyState_ComboAttack) / 패링: 방패를 든 때만 판정(AnimNotifyState_Parry) / 피격·그로기: Poise/Stance 분기 → Hit / Stun / 락온(LockOnComponent)',
 '- 처형(앞잡·뒤잡): FatalAttackableInterface로 처형 가능 대상 판정 / 앞·뒤 방향별 몽타주 + 카메라 / 처형 카메라 BP(Fatal_Camera) + 카메라 셰이크 / 연출 후 카메라 복귀 블렌드',
 '- 카메라·연출: 락온 시점·처형 카메라(앞/뒤 시퀀스) / 보스 인트로 Level Sequence(BossRoom) / 안개문 진입 연출 / 피격·패링 이펙트(Niagara)',
 '- 게임플레이 연계: 전투가 소울·아이템 획득의 시작점이고, 처형·패링·락온이 "소울류의 기본기"를 만든다'
)

# ================= 12 SELECTED 2 =================
$s = Base $CREAM
$null = Head $s '04' '수행 경과 · 선택 필수 ②  AI · 스테이지' ('몹은 보고, 판단하고,' + "`r" + '죽으면 소울을 남긴다') 36 880
$null = Chain3 $s @( @('감지','AI Perception'), @('판단','Behavior Tree'), @('행동 · 드랍','추격 · 공격 · 소울') )
$null = Bottom $s '스포너가 배치하고, 화톳불에서 쉬면 다시 스폰한다' 440
SetNotes $s @(
 '[발표 멘트]',
 '적 AI는 Perception으로 감지하고 Blackboard 값을 보고 Behavior Tree가 순찰, 추격, 간보기, 공격, 탐색 상태를 오갑니다. 일반 몹은 근접형과 매복형이 있고 죽으면 소울과 아이템을 드랍합니다. 스포너가 적을 배치하고 사망을 기록해서 화톳불에서 쉬면 다시 스폰합니다. 보스방은 입장-인트로-전투-클리어 상태로 관리합니다.',
 '',
 '[상세 자료]',
 '- 상태: Idle · Patrol(순찰) · Chase(추격) · Strafe(간보기) · Attack(공격) · Search(탐색)',
 '- EnemyAIController: AI Perception → Blackboard → Behavior Tree (Task: FindPatrol·FindSearch·FindStrafe·SelectCombatAction·NormalAttack / Service: UpdateCombatInfo)',
 '- 일반 몹: NormalEnemyBase(근접) · AmbushEnemy(매복 트리거) / EnemyAIData·EnemyAttackData로 수치 조절 / 순찰은 PatrolPath 웨이포인트 / 사망 시 아이템·소울 드랍',
 '- 스테이지·스포너: EnemySpawner - SpawnDataList(적 클래스·위치·순찰 경로) / 사망 상태 기록, 휴식 시 재스폰(EnemySpawnSubsystem) / 프롭은 RestorePropSubsystem으로 복원',
 '- 보스방·삐에로 보스: BossRoom 상태 Ready → Intro → Fighting → Cleared / 안개문·인트로 시퀀스·클리어 이력 저장 / Phase1·Phase2 BT 전환, 야바위·투사체 기믹 / 보스 HUD',
 '- 게임플레이 연계: 적이 소울·아이템을 주고, 화톳불에서 쉬면 맵이 초기화되어 "죽고 도전하는 반복"이 성립한다'
)

# ================= 13 SELECTED 3 =================
$s = Base $CREAM
$null = Head $s '04' '수행 경과 · 선택 필수 ③  아이템 · 인벤토리 · 성장' ('드랍으로 얻고,' + "`r" + '소울로 강해진다') 36 880
$null = Chain3 $s @( @('드랍','아이템 · 소울'), @('장착','무기 · 방어구 · 벨트'), @('레벨업','화톳불 · DataTable') )
$null = Bottom $s '모든 아이템이 무기다 — 유쾌한 소울의 규칙' 440
SetNotes $s @(
 '[발표 멘트]',
 '아이템은 몹이 드랍하면 인벤토리에 보관하고, 무기와 방어구로 장착하고, 물약과 랜턴은 벨트 퀵슬롯에 넣어 씁니다. 성장은 몹에게서 얻은 소울로 화톳불에서 레벨업하는 구조입니다. "모든 아이템이 무기"라는 유쾌한 규칙 아래 장착 슬롯 기반으로 처리했고, 방어구는 골판지 박스에 스티커를 붙인 세계관을 에셋으로 만들었습니다.',
 '',
 '[상세 자료]',
 '- 아이템 흐름: 획득/드랍(ItemObject) → 인벤토리(보관·리스트·필터) → 장착(무기 공격력·방어구) → 벨트 퀵슬롯(물약·랜턴)',
 '- 성장 흐름: 소울 드랍(몹 처치) → 화톳불 레벨업(가상 수치 확인 후 적용) → 성장 계산(StatGrowth DataTable) → HUD 반영·자동저장',
 '- 아이템 데이터: ItemDataAsset 계열 13종(무기·방패·물약·랜턴·방어구 8) / "모든 아이템이 무기" 규칙: 장착 슬롯 기반 처리 / 방어구는 골판지 박스 + 스티커 머티리얼(에셋). ※ 착용 이벤트와 머티리얼 파라미터 연결 코드는 아직 확인 필요',
 '- 성장·저장: 레벨업 일괄 승인 파이프라인(소울 → 능력치) / LevelRequiredSouls·StatGrowth를 DataTable로 관리 / 인벤토리·장착·물약 횟수를 SaveGame에 저장',
 '- 게임플레이 연계: 아이템은 전투력과 생존을, 소울은 성장을 만들어 "강해져서 재도전"하는 핵심 루프를 완성'
)

# ================= 13-1 ITEM DESIGN (Ahn) =================
$s = Base $CREAM
$null = Head $s '04' '수행 경과 · 선택 필수 ③  아이템 · 인벤토리 설계' ('아이템은 InventoryComponent 한 곳이 책임지고,' + "`r" + '데이터는 DataAsset에서 가져온다') 32 880
$null = Card $s 40 224 400 224 $PAPER 4 12
$null = Chip $s 'InventoryComponent' 56 208 250 36 $ORG $INK 16
$null = Chip $s '카타나  InstanceID 1' 64 262 352 56 $LORG $INK 18 5 2
$null = Chip $s '카타나  InstanceID 2' 64 332 352 56 $LORG $INK 18 5 2
$null = AddT $s 'InstanceID · AssetID · Quantity · UpgradeLevel' 56 396 376 40 13 $false $MUTE 1 3
$null = AddArrow $s 420 290 636 342 $INK 4
$null = AddArrow $s 420 360 636 358 $INK 4
$null = AddT $s ('AssetID로' + "`r" + '같은 에셋 참조') 450 262 160 44 14 $true $MUTE 2 3
$null = Node2 $s '카타나 DataAsset' '메쉬 · 아이콘 · 장착 슬롯' 640 306 280 96 $ORG $INK 20 14
$null = AddT $s 'Asset Manager가 로드' 640 410 280 24 14 $true $MUTE 2 3
$null = Bottom $s 'InstanceID가 달라서, 같은 카타나도 서로 다른 아이템이다' 466
SetNotes $s @(
 '[발표 멘트]',
 '아이템을 불러오는 것은 무조건 InventoryComponent를 통하도록 설계해서, 아이템 관리의 책임을 InventoryComponent 한 곳에 분명하게 두었습니다. 인벤토리에는 아이템 구조체 배열이 있고, 구조체에는 저장과 로드에 꼭 필요한 값(InstanceID, AssetID, Quantity, UpgradeLevel)만 담았습니다. 메쉬, 아이콘, 장착 위치 같은 것은 데이터 에셋으로 관리하고 Asset Manager로 불러옵니다.',
 '예를 들어 카타나가 인벤토리에 두 개 있으면, AssetID가 같아서 같은 데이터 에셋을 참조하지만 InstanceID가 달라서 서로 다른 아이템으로 인식됩니다.',
 '',
 '[상세 자료 - 설계 내용]',
 '- 아이템을 불러오는 것은 무조건 InventoryComponent를 통해서 하도록 설계. 아이템 관리를 InventoryComponent에게 확실하게 책임이 있도록 함.',
 '- InventoryComponent에 아이템 구조체를 담고 있는 배열이 있다. 구조체 내용: InstanceID, AssetID, Quantity, UpgradeLevel. 이 배열을 저장하고 로드하기 때문에 필수적인 수치만 구조체에 담았다.',
 '- 메쉬, 아이콘, 장착 가능 슬롯 등은 데이터 에셋(UItemDataAsset)으로 관리한다. (코드 대조: SkeletalMesh · Icon · MaxStack · AllowedSlots)',
 '- 코드 대조: FInventoryItem 구조체는 InstanceID(FGuid) · AssetID(FPrimaryAssetId) · Quantity · UpgradeLevel이고, GetItemData가 UAssetManager로 AssetID의 에셋 경로를 찾아 로드한다. 같은 AssetID 아이템은 MaxStack까지는 Quantity를 늘리고, 넘으면(또는 MaxStack 1이면) 새 InstanceID로 따로 추가한다.',
 '- Asset Manager를 통해 데이터 에셋을 불러오고, 인벤토리 아이템의 AssetID로 로드된 에셋을 사용한다. (카타나 두 개: AssetID가 같아 같은 데이터 에셋을 참조하지만 InstanceID가 달라 다른 아이템으로 인식)',
 '- 연결: 트러블슈팅에서 다룬 "인벤토리 UI 겹침"은 InstanceID 중복이 원인이었고, 이 구조에서는 InstanceID가 아이템을 구분하는 핵심이라 그 수정이 중요했다.'
)

# ================= 13-2 ITEM DELEGATE (Ahn) =================
$s = Base $CREAM
$null = Head $s '04' '수행 경과 · 선택 필수 ③  아이템 · 인벤토리 설계' ('델리게이트로 알려서,' + "`r" + '인벤토리가 저장 · UI를 직접 알지 못하게 했다') 34 880
$nd = @( @('InventoryComponent','획득 시 델리게이트 발행',$ORG), @('Warrior','구독 → 저장 데이터 갱신',$LORG), @('WarriorController','위젯 생성 · 참조 전달',$LORG), @('Widget','장착 변경 시 UI 갱신',$ORG) )
for($i=0;$i -lt 4;$i++){
  $x = 40 + $i*232
  $null = Node2 $s $nd[$i][0] $nd[$i][1] $x 262 196 84 $nd[$i][2] $INK 17 13
}
$null = AddArrow $s 270 304 238 304 $INK 4
$null = AddArrow $s 470 304 502 304 $INK 4
$null = AddArrow $s 702 304 734 304 $INK 4
$null = AddT $s '델리게이트 구독(Bind)' 180 354 150 24 13 $true $MUTE 2 3
$null = Bottom $s '아이템 획득 → 델리게이트 → 자동 저장   |   장착 변경 → 델리게이트 → UI 갱신' 430
SetNotes $s @(
 '[발표 멘트]',
 '인벤토리 컴포넌트는 아이템을 획득하면 델리게이트를 발행합니다. 인벤토리를 가진 Warrior가 그 델리게이트를 구독해서 저장 데이터를 갱신하기 때문에, 인벤토리는 저장 로직을 전혀 모릅니다. UI도 같은 방식입니다. 위젯은 WarriorController가 생성하면서 인벤토리·장비 참조를 넘겨 주고, 장착이 바뀌면 장비 컴포넌트의 델리게이트로 위젯이 갱신됩니다. 그래서 실제 연결 관계는 InventoryComponent ← Warrior → WarriorController → Widget 순서입니다.',
 '이 방식으로 인벤토리와 저장·UI 사이의 의존성을 낮출 수 있었습니다. 앞에서 설명한 객체 간 통신 방법 중 "델리게이트"의 구체적인 사례입니다.',
 '',
 '[상세 자료 - 설명]',
 '- InventoryComponent가 아이템 관리를 하고, 아이템 획득 시 호출할 델리게이트(OnItemObtained)를 발행한다.',
 '- InventoryComponent를 가지고 있는 Warrior 클래스에서 그 델리게이트에 함수를 바인드해서 의존성을 낮출 수 있었다.',
 '- WarriorController에서 위젯들을 생성하기 때문에 실질적인 연관 관계는 InventoryComponent ← Warrior → WarriorController → Widget.',
 '',
 '[코드 대조 결과 - 처음 설명과 달라서 슬라이드를 코드에 맞춰 수정함]',
 '- InventoryComponent의 델리게이트는 OnItemObtained(획득·수량 증가 시) 하나뿐이고, 아이템 삭제용 델리게이트·삭제 함수는 현재 코드에 없다. (그래서 슬라이드에서 "추가 · 삭제" 대신 "획득"으로 표기)',
 '- Warrior::PostInitializeComponents에서 Inventory->OnItemObtained를 SaveWarriorInventoryData에 바인드한다. 이 함수는 PlayerState의 저장 데이터(Items)를 갱신한다. 즉 Warrior가 바인드하는 것은 UI 갱신 함수가 아니라 저장 갱신 함수이다.',
 '- UI 갱신: WarriorController가 PlayerHUD·메뉴 위젯을 생성하며 SetInventory · SetEquipment로 참조를 전달하고, 장비 컴포넌트의 OnEquipItemImageChanged를 BeltWidget::OnEquipmentChanged와 MenuWidget::ChangeButtonsImage에 바인드한다. 인벤토리 델리게이트에 UI를 직접 바인드하는 코드는 없다.',
 '- 만약 "아이템 추가·삭제 시 UI가 갱신된다"를 말하고 싶다면, 인벤토리 변경 델리게이트에 UI 갱신 함수를 연결하는 코드를 추가한 뒤 슬라이드를 원래 설명으로 되돌리면 된다.'
)

# ================= 14 PROP RESTORE =================
$s = Base $CREAM
$null = Head $s '05' '성능 최적화 및 개선 사항 · 트러블슈팅' ('맵을 다시 불러오지 않고' + "`r" + '부서진 프롭을 되살렸다') 36 880
$fl = @( @('기록','월드 시작 때'), @('삭제','쉴 때 전부'), @('재스폰','원래 자리에') )
for($i=0;$i -lt 3;$i++){
  $x = 50 + $i*310
  $null = Node2 $s $fl[$i][0] $fl[$i][1] $x 262 260 124 $(if($i -eq 1){$LORG}else{$ORG}) $INK 38 16
  if ($i -lt 2) { $null = AddArrow $s ($x+264) 324 ($x+306) 324 $INK 4 }
}
$null = Bottom $s '로딩 없이 즉시 · 반복해도 중복 없음' 430
SetNotes $s @(
 '[발표 멘트]',
 '화톳불에서 쉬면 부서진 프롭이 원래대로 돌아와야 했는데, 프롭 블루프린트가 깨진 뒤 스스로 사라지는 구조라서 복원할 대상을 알 수 없었습니다. 그래서 월드가 시작될 때 프롭의 클래스와 위치를 전부 기록해 두고, 쉴 때 전부 지운 뒤 전부 다시 스폰하는 방식을 택했습니다. 로딩 없이 즉시 복원되고 반복해도 중복이 생기지 않습니다.',
 '',
 '[상세 자료]',
 '- 문제: 화톳불에서 쉬면 맵의 부서진 프롭이 원래대로 돌아와야 한다. 그런데 프롭은 깨지면 스스로 사라진다.',
 '- 원인: 프롭 BP가 깨진 뒤 Delay → DestroyActor로 소멸해, 사라진 뒤에는 복원 대상을 알 수 없다. "사라진 것만 다시 스폰"하면 Delay 중인 프롭이 남는다. 맵 재로드는 로딩과 진행 상태 저장이 필요하다.',
 '- 해결: 부모 C++ 클래스 ARestorableProp + URestorePropSubsystem(UWorldSubsystem). 월드 시작 때 클래스와 위치를 전부 기록하고, 쉴 때 전부 Destroy한 뒤 전부 다시 스폰(AlwaysSpawn)한다. BonFire 휴식 분기에서 호출.',
 '- 결과: 부수고 쉬면 복원되고, 반복해도·안 부수고 쉬어도 중복이 없다. 로딩 없이 즉시 복원. 같은 부모 클래스에서 파괴음도 일괄 처리.',
 '',
 '[그 밖의 트러블슈팅 - 원인·해결·결과]',
 '- 처형 카메라 복귀 시 화면이 튄다 (Bae): 연출이 끝난 프레임에서 카메라 액터를 바로 Destroy해 블렌드가 튄 자세에서 출발 → 연출 중 카메라 위치·회전을 저장, 끝난 첫 프레임에 복귀 후 ChangeViewTarget(0.6초 블렌드) → SetLifeSpan / 결과: 앞잡·뒤잡 카메라 완료(작은 튐 일부 남음)',
 '- 공격 판정이 벽 뒤에서도 나고 채널 설정이 없다 (Kang): Trace 충돌 채널 미설정으로 Pawn에만 반응 → TraceChannel을 컴포넌트 변수(기본 Pawn)로 노출, 소켓 기반 컴포넌트로 공용화 / 결과: 플레이어·적·보스가 같은 판정 컴포넌트 사용',
 '- 피격 처리 분기가 동작하지 않는다 (Lee): ReceiveDamage의 null 검사 조건이 중첩·반전되어 Poise/Stance 분기가 실행되지 않음 → 조건을 early return으로 분리하고 Hit / Stun 분기를 정리 / 결과: 피격·그로기 상태 전환 동작',
 '- 인벤토리 UI가 겹치고 이미지가 해제되지 않는다 (Ahn): 아이템 InstanceID가 중복되어 UI가 서로 덮어씀 → InstanceID 생성·조회 수정(InventoryComponent · ItemObject · ItemButtonWidget) / 결과: 아이템 목록 UI 정상 동작',
 '- ※ 커밋 메시지·소스 diff 기준으로 정리한 초안입니다. 담당자가 원인·결과를 직접 보완하세요.',
 '',
 '[개선 내역]',
 '- 비동기 로딩: FStreamableManager + 폴더 단위 에셋 수집으로 로딩 위젯 구현 / 효과: 로딩 중 UI 유지(수치 측정 필요)',
 '- 저장소 관리: 대용량 에셋(.zip · FBX) 추적 제거, git 제외 + Drive 공유 / 효과: 저장소 경량화',
 '- 사운드 동시 재생: 프롭 파괴음에 Sound Concurrency 적용(최대 16, 오래된 소리 정지) / 효과: 소리 폭주 방지',
 '- 로딩 시간·프레임 같은 개선 전·후 수치는 아직 측정하지 못했습니다.'
)

# ================= 15 FEEDBACK =================
$s = Base $CREAM
$null = Head $s '05' '성능 최적화 및 개선 사항 · 피드백 반영' ('"골판지 느낌이 부족하다"' + "`r" + '→ 타이틀 이미지를 다시 만들었다') 34 880
$null = FramePic $s 'title_dark.png' 130 226 700 210
$null = AddT $s '포장테이프 · 골판지 단면 · 찢긴 겉지를 추가' 130 450 700 26 16 $false $MUTE 2 3
SetNotes $s @(
 '[발표 멘트]',
 '피드백으로는 타이틀 이미지가 골판지 느낌이 부족하다는 의견을 받았습니다. 그래서 포장테이프, 골판지 단면, 찢긴 겉지를 추가해 다시 만들었습니다.',
 '',
 '[작성 필요]',
 '- 기획 발표 피드백 → 반영 내용: (작성)',
 '- 강사·멘토 피드백 → 반영 내용: (작성)',
 '- 여기에 이전 타이틀 이미지를 나란히 놓으면 "전 → 후" 비교가 더 잘 보입니다.'
)

# ================= 16 FUTURE =================
$s = Base $CREAM
$null = Head $s '06' '향후 확장 가능성' ('다음은 두 번째 보스와' + "`r" + '엔딩 · 크레딧') 36 880
$null = Thick $s 60 350 900 350 $INK 7
$fu = @( @('단기','튜토리얼 · 설정 · 키 안내'), @('중기','두 번째 보스 · 엔딩 · 크레딧'), @('장기','스킬 · 모캡 애니 · 스토어 출시') )
for($i=0;$i -lt 3;$i++){
  $cx = 170 + $i*310
  $null = Dot $s $cx 350 40 $(if($i -eq 1){$INK}else{$ORG}) 3
  $null = AddT $s $fu[$i][0] ($cx-100) 292 200 30 20 $true $DKRAFT 2 3
  $null = AddT $s $fu[$i][1] ($cx-140) 386 280 64 20 $true $INK 2 1
}
SetNotes $s @(
 '[발표 멘트]',
 '업데이트는 단기에 튜토리얼·설정 메뉴·키 안내를, 중기에 두 번째 보스와 엔딩·크레딧을, 장기에 스킬과 모션 캡처 애니메이션 확대, 스토어 출시 검토를 계획합니다.',
 '기대 효과는 세 가지입니다. 소울류의 긴장감은 두고 웃음으로 입문 장벽을 낮추고, 데이터 기반 구조라 콘텐츠를 늘리기 쉽고, 이번 협업 방식을 다음 프로젝트에도 재사용할 수 있습니다.',
 '',
 '[상세 자료]',
 '- 기대 효과: 소울류 입문 장벽 낮추기(긴장감은 유지하면서 웃음으로 도전 부담을 덜어 준다) / 데이터 기반 확장(적·아이템·스폰을 데이터로 추가) / 협업·UE C++ 경험(개인 브랜치 + 개발일지 방식 재사용)',
 '- 단기·보완: 패키징 안정화·밸런스 조정 / 튜토리얼·설정 메뉴·키 안내',
 '- 중기·콘텐츠: 두 번째 보스(덩치 큰 보스) / 원거리·엘리트 몹, 비밀방·보물 상자·미믹 / 엔딩·크레딧, 지역 3곳 확장',
 '- 장기·시스템: FP·마나·스킬, 모션 캡처 애니메이션 확대 / 출시 가능성 검토: 인디 게임 스토어(Steam 등)'
)

# ================= 17 DEMO =================
$s = Base $CREAM
$null = Head $s '07' '시연 (베타 영상)' '5분으로 보는 ZZakTungSoul' 40 880
$bx = Card $s 200 150 560 316 $LGRAY 5 10
$bx.Line.DashStyle = 4
$null = AddT $s '[ 시연 영상 · 스크린샷 ]' 200 380 560 40 20 $false $MUTE 2 3
$tri = $s.Shapes.AddShape(7,430,238,100,110); $tri.Rotation = 90
$tri.Fill.Solid(); $tri.Fill.ForeColor.RGB = $ORG; $tri.Line.ForeColor.RGB = $INK; $tri.Line.Weight = 4; $tri.Shadow.Visible = 0
SetNotes $s @(
 '[발표 멘트]',
 '지금부터 5분 영상으로 핵심 흐름을 보여 드리겠습니다. 타이틀에서 시작해 전투, 일반 몹 AI, 아이템, 화톳불, 보스전 순서입니다.',
 '',
 '[영상 구성 - 확인되는 요건]',
 '- 0:00 타이틀 → 로딩 → 뉴게임/로드: 게임 흐름 · 저장',
 '- 0:40 이동 · 락온 · 구르기 · 콤보 · 패링 · 처형: 전투 · 카메라 · 입력',
 '- 1:40 일반 몹 AI(순찰·탐색·추격) · 매복 · 몹 스포너 · 드랍: AI · 스테이지',
 '- 2:30 아이템 획득 · 장착 · 물약 · 랜턴 · 벨트: 아이템 · 인벤토리',
 '- 3:10 화톳불: 휴식 · 프롭 복원 · 레벨업 · 텔레포트 · 저장: 성장 · 저장 · 데이터',
 '- 3:50 보스방 입장 → 삐에로 1·2페이즈 → 클리어 / 사망 리스폰: 보스 · 연출 · UI',
 '',
 '[준비물 / 제출 체크리스트]',
 '- 위 박스는 영상 재생 화면 자리입니다. 스크린샷을 넣거나 영상으로 바꾸세요(제출용 스크린샷도 3장 이상 필요).',
 '- 제출: 패키징 exe zip · 발표자료 + 영상 · 스크린샷 3장 이상 · GitHub 링크 · README · 팀 개발 로그 링크 (마감 11/17)',
 '- 영상은 5분 이내, 기능 이름 자막 권장'
)

# ================= 18 COMPLETION =================
$s = Base $CREAM
$null = Head $s '08' '마무리 · 기획 대비 완성도' ('핵심 루프는 완성했고,' + "`r" + '보스 1종 · 엔딩은 남았다') 36 880
$null = AddT $s '__ / 10점' 140 240 680 150 100 $true $ORG 2 3
$null = AddT $s '팀 자체 평가 (회의 후 기입)' 140 396 680 30 18 $false $MUTE 2 3
SetNotes $s @(
 '[발표 멘트]',
 '기획 대비 완성도입니다. 메인화면·로딩·저장, 전투, 일반 몹 AI, 아이템, 화톳불은 완료했고, 보스는 2종 중 삐에로 1종만 구현했습니다. 크레딧·엔딩과 튜토리얼·설정은 후순위로 보류했습니다. 팀 자체 평가는 ___점입니다. (회의 후 점수를 정해서 말씀하세요)',
 '',
 '[상세 자료 - 기획 대비 결과]',
 '- 메인화면·로딩·세이브 ✓: 타이틀 · 비동기 로딩 · 이어하기',
 '- 플레이어 조작·전투 ✓: 계획에 없던 패링·처형 추가',
 '- 일반 몹 AI·몹 스포너 ✓: 매복 적 추가, 스포너 11.08 완료',
 '- 아이템·인벤토리·장비 ✓: 방어구 스티커 에셋 완료 (착용 연결은 확인 필요)',
 '- 화톳불(휴식·레벨업·텔레포트) ✓: 프롭 복원 · 불 이펙트 추가',
 '- 보스 2종 △: 삐에로 1종(2페이즈) 구현, 덩치 큰 보스는 미구현',
 '- 컷신·연출 △: 보스 인트로 시퀀스 · 처형 카메라만 구현',
 '- 크레딧·엔딩 ✗: 보류(후순위) / 튜토리얼·설정·비밀방 ✗: 보류 (확인 필요)'
)

# ================= 19 REVIEW =================
$s = Base $CREAM
$null = Head $s '08' '마무리 · 회고' '완성하며 남은 것들' 44 880
$rv = @( @('잘한 점',$GRN,$WHITE,'병렬 개발 · 패키징 · 충돌 줄이는 머지 방법'), @('아쉬운 점',$RED,$WHITE,'마일스톤 · 공통 그림 · 프로토타입 · 머지 시간'), @('배운 점',$ORG,$INK,'기획 초반 작업이 개발 전체를 좌우한다') )
for($i=0;$i -lt 3;$i++){
  $y = 210 + $i*88
  $null = Chip $s $rv[$i][0] 50 ($y+4) 170 52 $rv[$i][1] $rv[$i][2] 20
  $null = AddT $s $rv[$i][3] 244 $y 680 60 24 $true $INK 1 3
}
SetNotes $s @(
 '[발표 멘트]',
 '잘한 점은 개인 브랜치와 병합 담당, 개발일지로 4명이 병렬로 개발을 유지한 것, 그리고 패키징까지 끝낸 것입니다.',
 '아쉬운 점은 크게 세 가지입니다. 마일스톤이 없었고, 완성된 게임의 그림을 모두가 공유하지 못했고, 초반에 프로토타입을 만들지 못했습니다. 이 셋이 결국 한 가지를 가르쳐 줬는데, 컨셉아트나 마일스톤 같은 기획 초반 작업이 정말 중요하다는 것입니다. 다음 세 장에서 하나씩 말씀드리겠습니다.',
 '',
 '[상세 자료 - 초안, 팀 회의에서 실제 회고로 수정]',
 '- 그 밖의 아쉬운 점(이전 초안): 보스 1종·엔딩 미구현 / 병합 과정의 복원·재작업 커밋 / 성능 최적화 수치를 남기지 못함',
 '- 개선·보완할 점: 보스 2종째·엔딩·크레딧 / 튜토리얼·설정 메뉴·키 안내 UI / 로딩·프레임 등 성능 수치 측정 / 코드 스타일 가이드 문서화',
 '- 잘한 점: 개인 브랜치 + 병합 담당 + 개발일지로 4인 병렬 개발 유지 / DataTable·DataAsset 기반으로 콘텐츠 확장이 쉬운 구조 / 인터페이스·델리게이트·서브시스템으로 의존성 분리 / 패키징까지 완료',
 '- 아쉬운 점: 보스 1종·엔딩 미구현 / 병합 과정의 복원·재작업 커밋 / 성능 최적화 수치를 남기지 못함'
)

# ================= 19-1 REGRET 1: NO MILESTONE =================
$s = Base $CREAM
$null = Head $s '08' '마무리 · 아쉬운 점 ①  마일스톤의 부재' ('전체 설계 없이 시작해,' + "`r" + '회의 때마다 기능이 늘어났다') 36 880
$bl = @( @('초기 설계',150,440,360,$ORG), @('+ 10.23 회의',168,382,320,$LORG), @('+ 10.30 회의',146,324,320,$LORG), @('+ 11.06 회의',172,266,300,$LORG) )
foreach($b in $bl){ $null = Chip $s $b[0] $b[1] $b[2] $b[3] 54 $b[4] $INK 20 5 3 }
$null = Thick $s 600 470 600 262 $RED 5
$null = AddArrow $s 600 300 600 258 $RED 5
$null = AddT $s ('계속 늘어남' + "`r" + '(기준선이 없었다)') 628 330 280 70 24 $true $RED 1 3
SetNotes $s @(
 '[발표 멘트]',
 '첫 번째 아쉬운 점은 마일스톤이 없었다는 것입니다. 전체적인 설계를 확실하게 만들어 놓지 못한 채 시작해서, 회의 때마다 계속 기능이 추가되었습니다. 기준선이 없으니 어디까지가 이번 목표인지 정하기 어려웠습니다.',
 '',
 '[상세 자료]',
 '- 문제: 전체적인 설계를 확실하게 만들어 놓지 못하고, 회의 때마다 계속 추가되었다.',
 '- 슬라이드의 블록은 주간 미팅(10.23 · 10.30 · 11.06)마다 설계가 쌓인 모습을 표현한 것입니다.',
 '- 참고: 기획 발표(10.26)에서 마일스톤 M1~M3는 있었지만, 그 이후 추가된 기능은 마일스톤에 반영하지 못했습니다(개발일지 기준 - 사실과 다르면 수정).'
)

# ================= 19-2 REGRET 2: NO SHARED PICTURE =================
$s = Base $CREAM
$null = Head $s '08' '마무리 · 아쉬운 점 ②  공유되지 않은 완성본' ('각자 머릿속의 게임이 달랐고,' + "`r" + '작업마다 확인이 필요했다') 36 880
$pos = @( @('Ahn',230,310,$PUR), @('Bae',730,310,$BLU), @('Kang',230,430,$GRN), @('Lee',730,430,$RED) )
foreach($p in $pos){ $null = Thick $s $p[1] $p[2] 480 370 $MUTE 3 4 }
$null = Dot $s 480 370 120 $LORG 4
$null = AddT $s '?' 420 330 120 80 56 $true $INK 2 3
foreach($p in $pos){ $null = Chip $s $p[0] ($p[1]-60) ($p[2]-22) 120 44 $p[3] $WHITE 18 }
SetNotes $s @(
 '[발표 멘트]',
 '두 번째는 완성된 게임의 정확한 그림을 모두가 공유하지 못했다는 점입니다. 각자 생각하는 게임이 조금씩 달랐고, 그 생각을 완벽하게 공유할 수는 없으니 관련된 항목을 작업할 때마다 관련자에게 확인을 받아야 했습니다. 그 확인 과정이 시간을 많이 썼습니다.',
 '',
 '[상세 자료]',
 '- 문제: 완성된 게임의 정확한 그림을 모두가 공유하지 못하였음. 각자의 생각을 완벽하게 공유할 수 없어서, 관련 항목을 작업할 때마다 관련자에게 확인을 받아야 하였음.',
 '- 결과: 확인 대기와 재작업이 생겼고, 작업 속도가 늦어짐(구체적인 사례를 한두 개 말하면 설득력이 커집니다: 예) 처형 카메라, 보스 연출 등).',
 '- 이어지는 교훈: 컨셉아트처럼 완성된 모습을 눈으로 보여 주는 자료가 있었다면 확인 횟수가 줄었을 것입니다.'
)

# ================= 19-3 REGRET 3: NO EARLY PROTOTYPE =================
$s = Base $CREAM
$null = Head $s '08' '마무리 · 아쉬운 점 ③  초반 프로토타입의 부재' ('파트별로 완성해 마지막에 조립하는 방식은' + "`r" + '생각보다 훨씬 어려웠다') 34 880
$null = Chip $s '우리' 40 262 90 44 $RED $WHITE 18
$rw = @('전투','AI','UI · 아이템','보스')
for($i=0;$i -lt 4;$i++){ $null = Chip $s $rw[$i] (150+$i*135) 262 120 44 $PAPER $INK 16 5 2 }
$null = AddArrow $s 692 284 736 284 $INK 4
$null = Chip $s '마지막 조립' 742 262 170 44 $RED $WHITE 17
$null = Chip $s '목표' 40 372 90 44 $GRN $WHITE 18
$null = Chip $s '짧은 프로토타입' 150 372 200 44 $ORG $INK 16
$l = $s.Shapes.AddLine(354,394,390,394); $l.Line.ForeColor.RGB = $INK; $l.Line.Weight = 4; $l.Line.BeginArrowheadStyle = 2; $l.Line.EndArrowheadStyle = 2
$null = Chip $s '수정 · 보완 반복' 395 372 200 44 $LORG $INK 16
$null = AddArrow $s 600 394 640 394 $INK 4
$null = Chip $s '완성' 646 372 140 44 $GRN $WHITE 17
SetNotes $s @(
 '[발표 멘트]',
 '세 번째는 초반에 프로토타입을 만들지 못한 것입니다. 아주 짧게라도 게임 루프를 경험할 수 있는 초반 프로토타입을 만들고 계속 수정·보완해 나가야 했는데, 저희는 각자 파트를 완성시킨 다음 마지막에 최종 게임으로 조립하는 계획을 세웠습니다. 이 방식은 생각보다 어려움이 많았습니다.',
 '',
 '[상세 자료]',
 '- 한 일: 각자 파트(전투·AI·UI·아이템·보스 등)를 완성 → 마지막에 조립하며 최종 게임 완성',
 '- 해야 했던 일: 아주 짧게 게임 루프를 경험할 수 있는 초반 프로토타입을 만들고, 그것을 수정·보완하며 확장',
 '- 조립 단계에서 겪은 어려움: 병합 과정의 복원·재작업 커밋, 파트 간 연결 문제(개발일지·커밋 기준 - 구체적인 사례를 팀원과 확인해서 추가)'
)

# ================= 19-4 LESSON =================
$s = BlankSlide $KRAFT
$bg = CorrBand $s 0 0 960 540; $bg.ZOrder(1)
$null = Card $s 110 110 740 320 $PAPER 6 14
$null = Tape $s 90 98 76 22 -32
$null = Tape $s 794 98 76 22 32
$null = Chip $s '08  마무리 · 회고' 140 138 200 32 $ORG $INK 14
$null = AddT $s ('컨셉아트와 마일스톤,' + "`r" + '초반 프로토타입이' + "`r" + '개발 전체를 좌우한다') 140 186 680 150 38 $true $INK 1 1
$null = Thick $s 146 352 300 352 $ORG 8
$null = AddT $s '다음 프로젝트는 이것부터 시작한다' 140 366 680 36 20 $true $DKRAFT 1 3
$pill = Card $s 820 506 112 28 $PAPER 0 8
$s.Shapes('Group 4').ZOrder(0)
SetNotes $s @(
 '[발표 멘트]',
 '세 가지 아쉬움은 결국 한 가지로 이어집니다. 컨셉아트나 마일스톤 수립 같은 게임 기획 초반 작업이 얼마나 중요한지 느꼈다는 것입니다. 완성된 모습을 그림으로 먼저 보여 주고(컨셉아트), 기준선을 세우고(마일스톤), 짧은 루프를 먼저 만들어 보면(프로토타입) 설계가 흔들리지 않고, 서로 확인하는 시간이 줄고, 조립도 쉬워집니다. 다음 프로젝트에서는 이것부터 하겠습니다.',
 '',
 '[연결]',
 '- 마일스톤의 부재 → 마일스톤 수립',
 '- 완성본 그림을 공유하지 못함 → 컨셉아트',
 '- 초반 프로토타입의 부재 → 짧은 게임 루프 프로토타입을 먼저 만들고 수정·보완',
 '',
 '[다음 프로젝트에서 해 볼 것 - 제안, 팀 회의에서 수정]',
 '- 첫 주에 컨셉아트 + 마일스톤(주 단위)을 확정하고 모두가 같은 문서를 본다.',
 '- 1주차 안에 "시작 → 전투 → 보스 → 죽음/클리어"의 짧은 루프를 연결한 프로토타입을 만든다.',
 '- 그 뒤 파트별 작업은 프로토타입 위에 얹으며 매주 통합한다.'
)

# ================= 20 THANKS =================
$s = BlankSlide $KRAFT
$bg = CorrBand $s 0 0 960 540; $bg.ZOrder(1)
$null = Card $s 160 110 640 310 $PAPER 6 14
$null = Tape $s 140 98 76 22 -32
$null = Tape $s 744 98 76 22 32
$null = AddPic $s 'logo.png' 436 134 88 88
$null = AddT $s '감사합니다' 160 232 640 96 66 $true $INK 2 3
$null = AddT $s 'KusoGames  ·  Ahn · Bae · Kang · Lee' 160 340 640 32 20 $true $INK 2 3
$null = AddT $s 'ZZakTungSoul — 소울도 유쾌할 수 있다' 160 376 640 26 14 $false $MUTE 2 3
$pill = Card $s 820 506 112 28 $PAPER 0 8
$s.Shapes('Group 4').ZOrder(0)
SetNotes $s @(
 '[발표 멘트]',
 '지금까지 쿠소게임즈의 ZZakTungSoul을 소개했습니다. 팀원들의 한마디로 마무리하겠습니다. 감사합니다.',
 '',
 '[팀원 한마디 - 기획 발표(10.26)의 다짐 문구, 최종 한마디로 교체 가능]',
 '- Ahn: 나도 1인분을 하고 말테다.',
 '- Bae: 진짜 프로젝트 열심히 깎아보겠습니다! 저희 쿠소게임즈의 이름에 맞게 향기나는 게임 만들테니 기대해주세요!',
 '- Kang: 선생님, 이건 버그가 아니라 게임의 세계관입니다. 팀 이름부터 망했지만, 프로젝트까지 망하란 법은 없다! 쿠소게임즈 화이팅!',
 '- Lee: 쿠소게임이라고 무시하지 마세요! 재미있는 게임 만들기 위해 최선을 다하겠습니다!',
 '',
 '[질의응답 대비]',
 '- 필수 요건 1.1(엔딩·크레딧 미구현)과 1.7(키 안내 UI)은 일부 구현입니다. 이유와 보완 계획을 준비하세요.',
 '- 성능 수치(로딩 시간·프레임)는 아직 측정하지 않았습니다.'
)
