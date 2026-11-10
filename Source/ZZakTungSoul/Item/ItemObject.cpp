// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemObject.h"
#include <Components/BoxComponent.h>
#include <ItemData/ItemDataAsset.h>
#include <Warrior/Warrior.h>

#include <Engine/AssetManager.h>


// Sets default values
AItemObject::AItemObject()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ItemMeshRef(
		TEXT("/Game/Assets/Planet4/planet4.planet4")
	);
	if (ItemMeshRef.Succeeded())
	{
		Mesh->SetSkeletalMesh(ItemMeshRef.Object);
	}

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
}

// Called when the game starts or when spawned
void AItemObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemObject::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(
		this, &AItemObject::OnBeginOverlap
	);

	Trigger->OnComponentEndOverlap.AddDynamic(
		this, &AItemObject::OnEndOverlap
	);

	// 애셋 매니저를 통해서 사용할 애셋 목록 가져오기
	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> Assets;
	Manager.GetPrimaryAssetIdList(TEXT("ItemData"), Assets);

	// 확인
	ensureAlways(Assets.Num() > 0);

	// 랜덤으로 선택
	int32 RandomIndex = FMath::RandRange(0, Assets.Num() - 1);

	// 선택된 인덱스로 접근해서 애셋 참조
	FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(Assets[RandomIndex]));

	// 애셋 로드 - 로드되지 않은 애셋 로드
	if (AssetPtr.IsPending())
	{
		AssetPtr.LoadSynchronous();
	}

	// 로드한 애셋을 기반으로 아이템 데이터 설정
	InstanceID = FGuid::NewGuid();
	Item = Cast<UItemDataAsset>(AssetPtr.Get());
	//Item->InstanceID = InstanceID;

	// 확인 - 아이템 값이 제대로 설정됐는지
	ensureAlways(Item);
}

void AItemObject::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	AWarrior* Warrior = Cast<AWarrior>(OtherActor);
	if (!Warrior)
		return;

	Warrior->AddDetectedObject(this);

	// Todo: UI 띄워주기
}

void AItemObject::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	if (!OtherActor)
		return;

	AWarrior* Warrior = Cast<AWarrior>(OtherActor);
	if (!Warrior)
		return;

	Warrior->RemoveDetectedObject(this);
}

void AItemObject::Interact(AActor* detectedActor)
{
	AWarrior* Warrior = Cast<AWarrior>(detectedActor);
	if (!Warrior)
		return;

	// 인벤에 아이템 추가해주기
	Warrior->ObtainItem(Item);

	// DetectedObjects에서 제거해주기
	Warrior->RemoveDetectedObject(this);

	// 맵에서 Destroy
	this->Destroy();
}

