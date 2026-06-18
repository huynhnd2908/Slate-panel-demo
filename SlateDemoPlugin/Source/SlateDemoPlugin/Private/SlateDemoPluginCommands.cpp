// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlateDemoPluginCommands.h"

#define LOCTEXT_NAMESPACE "FSlateDemoPluginModule"

void FSlateDemoPluginCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "SlateDemoPlugin", "Execute SlateDemoPlugin action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
