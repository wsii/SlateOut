// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlateOut.h"
#include "SlateOutStyle.h"
#include "SlateOutCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"

#include "SSListPanel.h"
#include "ToolMenus.h"

static const FName SlateOutTabName("SlateOut");

#define LOCTEXT_NAMESPACE "FSlateOutModule"

void FSlateOutModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSlateOutStyle::Initialize();
	FSlateOutStyle::ReloadTextures();

	FSlateOutCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSlateOutCommands::Get().SlateOut,
		FExecuteAction::CreateRaw(this, &FSlateOutModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSlateOutModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SlateOutTabName, FOnSpawnTab::CreateRaw(this, &FSlateOutModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FSlateOutTabTitle", "SlateOut"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FSlateOutModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSlateOutStyle::Shutdown();

	FSlateOutCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SlateOutTabName);
}

TSharedRef<SDockTab> FSlateOutModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
            	SNew(SSListPanel)
            ]
		];
}

void FSlateOutModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(SlateOutTabName);
}

void FSlateOutModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSlateOutCommands::Get().SlateOut, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Game");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSlateOutCommands::Get().SlateOut));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSlateOutModule, SlateOut)


