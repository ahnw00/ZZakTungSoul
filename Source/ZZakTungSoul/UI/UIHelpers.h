#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"

namespace UIHelpers
{
	inline void SetNumberText(UTextBlock* TextBlock, int32 Value)
	{
		if (TextBlock)
		{
			TextBlock->SetText(FText::AsNumber(Value));
		}
	}

	// float...
}