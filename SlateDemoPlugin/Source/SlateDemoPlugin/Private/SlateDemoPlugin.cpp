// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlateDemoPlugin.h"
#include "SlateDemoPluginStyle.h"
#include "SlateDemoPluginCommands.h"
#include "ToolMenus.h"
#include "SSlateDemoPanel.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"


static const FName SlateDemoPluginTabName("SlateDemoPlugin");

#define LOCTEXT_NAMESPACE "FSlateDemoPluginModule"

void FSlateDemoPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSlateDemoPluginStyle::Initialize();
	FSlateDemoPluginStyle::ReloadTextures();

	FSlateDemoPluginCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSlateDemoPluginCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSlateDemoPluginModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSlateDemoPluginModule::RegisterMenus));
}

void FSlateDemoPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSlateDemoPluginStyle::Shutdown();

	FSlateDemoPluginCommands::Unregister();
}

void FSlateDemoPluginModule::PluginButtonClicked()
{
	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(FText::FromString(TEXT("Slate Demo Panel")))
		.ClientSize(FVector2D(400, 300))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SSlateDemoPanel)
		];

	FSlateApplication::Get().AddWindow(Window);
}

void FSlateDemoPluginModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSlateDemoPluginCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSlateDemoPluginCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSlateDemoPluginModule, SlateDemoPlugin)