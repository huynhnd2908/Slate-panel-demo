// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "SlateDemoPluginStyle.h"

class FSlateDemoPluginCommands : public TCommands<FSlateDemoPluginCommands>
{
public:

	FSlateDemoPluginCommands()
		: TCommands<FSlateDemoPluginCommands>(TEXT("SlateDemoPlugin"), NSLOCTEXT("Contexts", "SlateDemoPlugin", "SlateDemoPlugin Plugin"), NAME_None, FSlateDemoPluginStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
