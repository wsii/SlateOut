// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlateOutCommands.h"

#define LOCTEXT_NAMESPACE "FSlateOutModule"

void FSlateOutCommands::RegisterCommands()
{
	UI_COMMAND(SlateOut, "SlateOut", "Bring up SlateOut window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
