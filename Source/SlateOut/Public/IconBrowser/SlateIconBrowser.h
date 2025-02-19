// Copyright 2022 sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
class FIconInfo;
class SMultiLineEditableText;
class FToolBarBuilder;
class FMenuBuilder;
class ITableRow;
class STableViewBase;
class STextBlock;
template<typename T> class SListView;


enum ECopyCodeStyle
{
	CS_FSlateIcon,
	CS_FSlateIconFinderFindIcon,
};

class SSlateIconBrowser final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSlateIconBrowser)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	static void OnClose() {};
	void FillSettingsMenu(FMenuBuilder& MenuBuilder);
	void FillHelpMenu(FMenuBuilder& MenuBuilder);
	void SelectCodeStyle(ECopyCodeStyle CopyStyle);
	void CopyIconCodeToClipboard(FName Name, ECopyCodeStyle CodeStyle);
	FString GenerateCopyCode(FName Name, ECopyCodeStyle CodeStyle);

	FString TranslateDefaultStyleSets(FName StyleSet);
	void FillDefaultStyleSetCodes();

	void CacheAllStyleNames();

	void OnClickStyleItem(const TSharedRef<FName>& ItemRef);
	
	FText GetCodeStyleText(ECopyCodeStyle CopyStyle);
	FText GetCodeStyleTooltip(ECopyCodeStyle CopyStyle);

	TSharedRef<SWidget> MakeMainMenu();
	FReply EntryContextMenu(const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName Name);

	//View
	void UpdateStyleList();
	//Data
	void UpdateStyleListItems();

	//View
	void UpdateIconList();
	//Data
	void UpdateIconListItems();
	
	void UpdateIconInfoListItems();
	
	
	void OnStyleNameFilterChanged(const FText& InFilterText);
	TSharedPtr<FActiveTimerHandle> StyleNameSearchTimer;
	void OnIconInfoFilterChanged(const FText& InFilterText);
	TSharedPtr<FActiveTimerHandle> IconSearchTimer;
	
// private:
	FString FilterString;
	
	FName SelectedStyle;
	
	TEnumAsByte<ECopyCodeStyle> CopyCodeStyle;
	
	//StyleList
	TSharedPtr<SSearchBox>	StyleSearchBox;
	TSharedPtr<SBorder>		StylePanel;
	TSharedPtr<SListView<TSharedPtr<FName>>> StyleListView;
	//StyleRow
	TSharedRef<ITableRow> GenerateStyleRow(TSharedPtr<FName> Name, const TSharedRef<STableViewBase>& TableViewBase);
	//AllStyles data
	TArray<TSharedPtr<FName>> AllStyles;
	TArray<FString> StyleListCache;

	
	//IconList
	TSharedPtr<SSearchBox>	IconSearchBox;
	TSharedPtr<SBorder>		IconPanel;
	const float						IconSearchDelay = 0.1f;
	TSharedPtr<SListView<TSharedPtr<FName>>> IconListView;
	//IconRow
	TSharedRef<ITableRow> GenerateIconRow(TSharedPtr<FName> Name, const TSharedRef<STableViewBase>& TableViewBase);
	//icon list data
	TArray<TSharedPtr<FName>> AllIconLines;
	TSharedPtr<FName> IconItem;
	//icon list cache
	TArray<FString> Lines;
	// TArray<TSharedPtr<FString>> Lines;
	
	//Icon info Panel
	TSharedPtr<FIconInfo>			SelectedIcon;
	TSharedPtr<SMultiLineEditableText>		SelectedIconName;
	TSharedPtr<SMultiLineEditableText>		DetailText;
	TSharedPtr<SImage>				DetailIcon;
	
	TSharedPtr<STextBlock> CopyNoteTextBlock;

	TMap<FName,FString> DefaultStyleSetCode;

	const float SearchDelay = 0.1f;
	FText CustomStyleTooltipText = NSLOCTEXT("FSlateIconBrowserModule", "CustomStyleTooltipText", "Enter custom style. $1 will be replaced by the icon name.");
};

