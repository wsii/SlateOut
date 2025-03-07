// Copyright Epic Games, Inc. All Rights Reserved.

#include "SlateOutStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FSlateOutStyle::StyleInstance = nullptr;

void FSlateOutStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSlateOutStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FSlateOutStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SlateOutStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FSlateOutStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("SlateOutStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SlateOut")->GetBaseDir() / TEXT("Resources"));

	Style->Set("SlateOut.SlateOut", new IMAGE_BRUSH_SVG(TEXT("Icon"), Icon20x20));

	return Style;
}

void FSlateOutStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FSlateOutStyle::Get()
{
	return *StyleInstance;
}
