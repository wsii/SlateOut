// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IconBrowser/SlateIconBrowser.h"
#include "Modules/ModuleManager.h"

class SSlateIconBrowser;

UENUM()
enum class EGameMapToSet : uint8
{
	EditorStartupMap,
	GameDefaultMap,
	TransitionMap
};

class FSlateOutModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/** This function will be bound to Command (by default it will bring up plugin window) */
	
	//Slate Wideget
	// TSharedPtr<SSlateIconBrowser> SlateIconBrowser;
	
private:
	
	FDelegateHandle ToolMenusHandle;

	TSharedPtr<FSlateIconBrowser> IconBrowser;
	
	/**
	 * @brief 创建下拉插件菜单内容
	 * @return 
	 */
	TSharedRef<SWidget> GetSlateOutsDropdown() const;
	void RegisterGameEditorMenus();
	
	void ShowSlateIconBrowser() const;
	TSharedRef<SDockTab> CreateSlateIconBrowser(const FSpawnTabArgs& Args) const;


};

