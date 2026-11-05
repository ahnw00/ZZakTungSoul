#include <Enemy/Boss/Encounter/BossRoomSubsystem.h>

bool UBossRoomSubsystem::HasEntered(FName RoomID) const
{
	const FBossRoomProgress* Progress = RoomProgress.Find(RoomID);

	return Progress ? Progress->bEntered : false;
}

bool UBossRoomSubsystem::IsCleared(FName RoomID) const
{
	const FBossRoomProgress* Progress = RoomProgress.Find(RoomID);

	return Progress ? Progress->bCleared : false;
}

void UBossRoomSubsystem::MarkEntered(FName RoomID)
{
	if (RoomID.IsNone())
	{
		return;
	}

	RoomProgress.FindOrAdd(RoomID).bEntered = true;
}

void UBossRoomSubsystem::MarkCleared(FName RoomID)
{
	if (RoomID.IsNone())
	{
		return;
	}

	FBossRoomProgress& Progress = RoomProgress.FindOrAdd(RoomID);
	Progress.bEntered = true;
	Progress.bCleared = true;
}

void UBossRoomSubsystem::ResetRoom(FName RoomID)
{
	RoomProgress.Remove(RoomID);
}
