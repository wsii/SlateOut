// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlateOut.h"
#include "SlateOutStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "ToolMenus.h"
#include "ClassBrowser/ClassBrowserTab.h"
#include "IconBrowser/IconBrowserTab.h"
#include "IconBrowser/SlateIconBrowser.h"
#include "Widgets/Testing/SPerfSuite.h"
#include "Widgets/Testing/SStarshipSuite.h"
#include "Widgets/Testing/STestSuite.h"

struct FToolMenuSection;
class UToolMenu;
static const FName SlateOutTabName("SlateOut");
static const FName SlateIconBrowserTTabName("SlateIconBrowserTab");
static const FName IconBrowserTabName("IconBrowserTab");
static const FName ClassBrowserTabName("ClassBrowser");




#define LOCTEXT_NAMESPACE "FSlateOutModule"


void FSlateOutModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FSlateOutStyle::Initialize();
	FSlateOutStyle::ReloadTextures();
	
	if (!IsRunningGame())
	{
		if (FSlateApplication::IsInitialized())
		{

			ToolMenusHandle = UToolMenus::RegisterStartupCallback(
				//RegisterGameEditorMenus
				FSimpleMulticastDelegate::FDelegate::CreateRaw(this
					,&FSlateOutModule::RegisterGameEditorMenus));
		}
	}

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SlateIconBrowserTTabName, FOnSpawnTab::CreateRaw(this,&FSlateOutModule::OpenSlateIconBrowser))
	.SetDisplayName(FText::FromName(TEXT("Slate图标")))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(IconBrowserTabName, FOnSpawnTab::CreateRaw(this,&FSlateOutModule::OpenIconBrowser))
	.SetDisplayName(FText::FromName(TEXT("Slate图标全")))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ClassBrowserTabName, FOnSpawnTab::CreateRaw(this,&FSlateOutModule::OpenClassBrowser))
	.SetDisplayName(FText::FromName(TEXT("ClassBrowser")))
	.SetMenuType(ETabSpawnerMenuType::Hidden);
	
}

void FSlateOutModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
		

	FSlateOutStyle::Shutdown();
}


static bool HasPlayWorld()
{
	return GEditor->PlayWorld != nullptr;
}

static bool HasNoPlayWorld()
{
	return !HasPlayWorld();
}

static void ShowStarshipSuite()
{
	RestoreSlateTestSuite();
}

static void ShowPrefSuite()
{
	RestoreStarshipSuite();
	
}

static void ShowTestSuite()
{
	SummonPerfTestSuite();
	
}


TSharedRef<SDockTab> FSlateOutModule::OpenSlateIconBrowser(const FSpawnTabArgs& Args) const
{
	// UE_LOG(LogTemp,Log,TEXT("Test"));

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SSlateIconBrowser)
			
		];
}

TSharedRef<SDockTab> FSlateOutModule::OpenIconBrowser(const FSpawnTabArgs& Args) const
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SIconBrowserTab)
			
		];
}

TSharedRef<SDockTab> FSlateOutModule::OpenClassBrowser(const FSpawnTabArgs& Args) const
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SClassBrowserTab)
		];
}

void FSlateOutModule::OnSlateIconBrowserClicked() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(SlateIconBrowserTTabName);
}

void FSlateOutModule::OnIconBrowserClicked() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(IconBrowserTabName);
}

void FSlateOutModule::OnClassBrowser() const
{
	FGlobalTabmanager::Get()->TryInvokeTab(ClassBrowserTabName);
}


TSharedRef<SWidget> FSlateOutModule::GetSlateOutsDropdown() const
{
	FMenuBuilder MenuBuilder(true, nullptr);

	// Add a button to add the current level to the bookmarks list
	MenuBuilder.AddMenuEntry(
		LOCTEXT("ShowStarshipSuite", "组件"),
		LOCTEXT("ShowStarshipSuiteTooltip", "打开组件面板"),
		FSlateIcon(FSlateOutStyle::GetStyleSetName(), "SlateOut.SlateOut"),
		FUIAction(
			FExecuteAction::CreateStatic(&ShowStarshipSuite),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);
	
	// Add a line separator
	MenuBuilder.AddMenuSeparator();

	// Add a button to clear the level bookmarks list
	MenuBuilder.AddMenuEntry(
		LOCTEXT("ShowPrefSuite", "列表"),
		LOCTEXT("ShowPrefSuiteTooltip", "打开列表面板"),
		FSlateIcon(FSlateOutStyle::GetStyleSetName(), "SlateOut.SlateOut"),
		FUIAction(
			FExecuteAction::CreateStatic(&ShowPrefSuite),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);
	
	// Add a line separator
	MenuBuilder.AddMenuSeparator();

	// Add a button to add the current level to the cook maps list
	MenuBuilder.AddMenuEntry(
		LOCTEXT("ShowTestSuite", "测试集合"),
		LOCTEXT("ShowTestSuiteTooltip", "打开测试集合面板"),
		FSlateIcon(FSlateOutStyle::GetStyleSetName(), "SlateOut.SlateOut"),
		FUIAction(
			FExecuteAction::CreateStatic(&ShowTestSuite),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);

	MenuBuilder.AddMenuSeparator();
	// Add a button to set the current level as the game default map
	MenuBuilder.AddMenuEntry(
		LOCTEXT("IconBrowserTab", "图标面板"),
		LOCTEXT("IconBrowserTabTooltip", "打开图标面板"),
		FSlateIcon(FSlateOutStyle::Get().GetStyleSetName(), TEXT("SlateOut.SlateOut")),
		FUIAction(
			FExecuteAction::CreateRaw(this,&FSlateOutModule::OnSlateIconBrowserClicked),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);
	MenuBuilder.AddMenuSeparator();
	// // Add a button to set the current level as the project's default map
	MenuBuilder.AddMenuEntry(
		LOCTEXT("SlateIconBrowserTab", "所有图标面板"),
		LOCTEXT("SlateIconBrowserTabtip", "打开图标面板"),
		FSlateIcon(FSlateOutStyle::Get().GetStyleSetName(), TEXT("SlateOut.SlateOut")),
		FUIAction(
			FExecuteAction::CreateRaw(this,&FSlateOutModule::OnIconBrowserClicked),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);
	MenuBuilder.AddMenuSeparator();
	// Add a button to set the current level as the transition map
	MenuBuilder.AddMenuEntry(
		LOCTEXT("ClassBrowser", "Class类合集面板"),
		LOCTEXT("ClassBrowsertip", "打开Class类合集面板"),
		FSlateIcon(FSlateOutStyle::Get().GetStyleSetName(), TEXT("SlateOut.SlateOut")),
		FUIAction(
			FExecuteAction::CreateRaw(this,&FSlateOutModule::OnClassBrowser),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateStatic(&HasNoPlayWorld)
		)
	);
	
	return MenuBuilder.MakeWidget();
}

void FSlateOutModule::RegisterGameEditorMenus()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuSection& Section = Menu->AddSection("PluginTools", TAttribute<FText>(), FToolMenuInsert("SlateOutSection", EToolMenuInsertType::Default));

	FToolMenuEntry SlateOutEntry = FToolMenuEntry::InitComboButton(
		"SlateOut",
		FUIAction(
			FExecuteAction(),
			FCanExecuteAction::CreateStatic(&HasNoPlayWorld),
			FIsActionChecked()
			),
		FOnGetContent::CreateRaw(this,&FSlateOutModule::GetSlateOutsDropdown),
		LOCTEXT("SlateOut_Label", ""),
		LOCTEXT("SlateOut_ToolTip", "Open SlateOut"),
		FSlateIcon(FSlateOutStyle::GetStyleSetName(), "SlateOut.SlateOut")
	);
	Section.AddEntry(SlateOutEntry);

	UE_LOG(LogTemp,Log,TEXT("插件菜单注册完成。"));
}

#undef LOCTEXT_NAMESPACE


	
IMPLEMENT_MODULE(FSlateOutModule, SlateOut)


