// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SlateOutStyle.h"

class FSlateOutCommands : public TCommands<FSlateOutCommands>
{
public:

	FSlateOutCommands()
		: TCommands<FSlateOutCommands>(TEXT("SlateOut"), NSLOCTEXT("Contexts", "SlateOut", "SlateOut Plugin"), NAME_None, FSlateOutStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > SlateOut;
};