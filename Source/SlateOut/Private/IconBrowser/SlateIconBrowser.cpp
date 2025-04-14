// Copyright 2022 sirjofri. Licensed under MIT license. See License.txt for full license text.

#include "IconBrowser/SlateIconBrowser.h"

#include "ClassBrowser/ClassItem.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Layout/WidgetPath.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/Notifications/NotificationManager.h"
#include "IconBrowser/IconInfo.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/Text/SRichTextBlock.h"

#define LOCTEXT_NAMESPACE "SSlateIconBrowser"

const static FString DefaultIconDetail("Click Icon to Select One");
const static FText DefaultIconName = FText::FromString("No Icon is Selected");
const static FSlateIcon DefaultIcon(FAppStyle::GetAppStyleSetName(), "Default");

void SSlateIconBrowser::Construct(const FArguments& InArgs)
{
	CacheAllStyleNames();
	ChildSlot
	[
		// Put your tab content here!
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				MakeMainMenu()
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
				.Orientation(EOrientation::Orient_Horizontal)
			]
			+SVerticalBox::Slot()
			.FillHeight(1.0)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(0.3f)
				.MaxWidth(500)
				[
					SAssignNew(StylePanel, SBorder)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SAssignNew(IconPanel, SBorder)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SAssignNew(IconInfoPanel, SBorder)
				]
			]
	];

	StylePanel->Construct(
		SBorder::FArguments()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(0.1f)
			.MaxHeight(40)
			[
				SNew(SBorder)
				[
					SAssignNew(StyleSearchBox, SSearchBox)
					.OnTextChanged(SharedThis(this), &SSlateIconBrowser::OnStyleNameFilterChanged)
				]
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SBorder)
				[
					SAssignNew(StyleListView, SListView<TSharedPtr<FName>>)
					.OnGenerateRow(this, &SSlateIconBrowser::GenerateStyleRow)
					.ListItemsSource(&AllStyles)
					.SelectionMode(ESelectionMode::Single)
				]
			]
			// +SVerticalBox::Slot()
			// .FillHeight(1.0f)
			// .VAlign(EVerticalAlignment::VAlign_Center)
			// [
			// 	SNew(SBox)
			// 	.HAlign(EHorizontalAlignment::HAlign_Center)
			// 	.Visibility_Lambda([this]
			// 	{
			// 		return AllStyles.IsEmpty() ? EVisibility::Visible : EVisibility::Collapsed;
			// 	})
			// 	[
			// 		SNew(STextBlock)
			// 		.Text(LOCTEXT("NoResults", "No results found."))
			// 	]
			// ]	
		]
	);

	IconPanel->Construct(
		SBorder::FArguments()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				// .MaxWidth(400)
				.FillWidth(0.3)
				[
					SNew(SBorder)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.FillHeight(0.1f)
						.MaxHeight(40)
						[
							SNew(SBorder)
							[
								SAssignNew(IconSearchBox, SSearchBox)
								.OnTextChanged(SharedThis(this), &SSlateIconBrowser::OnIconInfoFilterChanged)
							]
						]
						+ SVerticalBox::Slot()
						.FillHeight(1.0)
						[

							SAssignNew(IconListView, SListView<TSharedPtr<FName>>)
							.ListItemsSource(&AllIconLines)
							.OnGenerateRow_Raw(this, &SSlateIconBrowser::GenerateIconRow)
							.SelectionMode(ESelectionMode::Single)
							.Visibility_Lambda([this]
							{
								return LinesCache.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
							})
						]
					]
				]
			]
			+SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.Padding(FMargin(10, 5))
				[
					SAssignNew(CopyNoteTextBlock, STextBlock)
					.Text_Lambda([&]
					{
						return FText::Format(LOCTEXT("CopyNote", "Double click a line to copy the {0} C++ code."),
							GetCodeStyleText(CopyCodeStyle));
					})
				]
			]
		]
	);
	IconInfoPanel->Construct(
		SBorder::FArguments()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				
				SNew(SBorder)
				[

					
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.HeightOverride(72)
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						.HeightOverride(72)
						[
							SNew(SBorder)
							[
								SAssignNew(DetailIcon, SImage)
								.DesiredSizeOverride(FVector2d(64, 64))
								.Image(DefaultIcon.GetIcon())
							]
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(32)
						[
							SAssignNew(SelectedIconName, SMultiLineEditableText)
							.AutoWrapText(true)
							.AllowMultiLine(true)
							.IsReadOnly(true)
							.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
							.Text(DefaultIconName)
						]

					]
					// + SVerticalBox::Slot()
					// .AutoHeight()
					// [
					// 	SNew(SBox)
					// 	[
					// 		SNew(SButton)
					// 		.HAlign(HAlign_Center)
					// 		.VAlign(VAlign_Center)
					// 		.Text(FText::FromString("Copy Code"))
					// 		.OnClicked(this, &SSlateIconBrowser::OnResetIcon)
					// 		
					// 	]
					// ]
					// detail text
					+ SVerticalBox::Slot()
					[
						SNew(SBox)
						.Padding(2.0f, 8.0f)
						[
							SAssignNew(DetailText, SMultiLineEditableText)
							.AutoWrapText(true)
							.AllowMultiLine(true)
							.IsReadOnly(true)
							.AllowContextMenu(true)
							.Text(FText::FromString(DefaultIconDetail))
						]
					]
				]
			]
		]
	);
	FillDefaultStyleSetCodes();
	// CacheAllLines();
}


void SSlateIconBrowser::SelectCodeStyle(ECopyCodeStyle CopyStyle)
{
	CopyCodeStyle = CopyStyle;
	CopyNoteTextBlock->Invalidate(EInvalidateWidgetReason::Paint);
}


FText SSlateIconBrowser::GetCodeStyleText(ECopyCodeStyle CopyStyle)
{
	switch (CopyStyle) {
	case CS_FSlateIcon:
		return FText::FromString(TEXT("FSlateIcon(...)"));
		break;
	case CS_FSlateIconFinderFindIcon:
		return FText::FromString(TEXT("FSlateIconFinder::FindIcon(...)"));
		break;
	}
	return FText::GetEmpty();
}

FText SSlateIconBrowser::GetCodeStyleTooltip(ECopyCodeStyle CopyStyle)
{
	return FText::Format(LOCTEXT("CopyStyleTooltip", "Copy code in {0} style"), GetCodeStyleText(CopyStyle));
}

void SSlateIconBrowser::FillSettingsMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(TEXT("CopySettings"), LOCTEXT("CopySettings", "Code to copy"));
	{
		MenuBuilder.AddMenuEntry(
			GetCodeStyleText(CS_FSlateIcon),
			GetCodeStyleTooltip(CS_FSlateIcon),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateRaw(this, &SSlateIconBrowser::SelectCodeStyle, CS_FSlateIcon),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([&]
				{
					return CopyCodeStyle == CS_FSlateIcon;
				})
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton);
		MenuBuilder.AddMenuEntry(
			GetCodeStyleText(CS_FSlateIconFinderFindIcon),
			GetCodeStyleTooltip(CS_FSlateIconFinderFindIcon),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateRaw(this, &SSlateIconBrowser::SelectCodeStyle, CS_FSlateIconFinderFindIcon),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([&]
				{
					return CopyCodeStyle == CS_FSlateIconFinderFindIcon;
				})
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton);
	}
	MenuBuilder.EndSection();
}

void SSlateIconBrowser::FillHelpMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(TEXT("HelpMenu"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("HelpDocumentation", "Documentation"),
			LOCTEXT("HelpDocumentationTooltip", "Opens the documentation"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Documentation"),
			FUIAction(
				FExecuteAction::CreateLambda([=]
				{
					FPlatformProcess::LaunchURL(TEXT("https://github.com/sirjofri/SlateIconBrowser"), nullptr, nullptr);
				})
			));
	}
	MenuBuilder.EndSection();
}

TSharedRef<SWidget> SSlateIconBrowser::MakeMainMenu()
{
	FMenuBarBuilder MenuBuilder(nullptr);
	{
		MenuBuilder.AddPullDownMenu(
			LOCTEXT("SettingsMenu", "Settings"),
			LOCTEXT("SettingsMenuTooltip", "Opens the settings menu."),
			FNewMenuDelegate::CreateRaw(this, &SSlateIconBrowser::FillSettingsMenu));
		MenuBuilder.AddPullDownMenu(
			LOCTEXT("HelpMenu", "Help"),
			LOCTEXT("HelpMenuTooltip", "Opens the help menu."),
			FNewMenuDelegate::CreateRaw(this, &SSlateIconBrowser::FillHelpMenu));
	}
	TSharedRef<SWidget> Widget = MenuBuilder.MakeWidget();
	Widget->SetVisibility(EVisibility::Visible);
	return Widget;
}

FString SSlateIconBrowser::TranslateDefaultStyleSets(FName StyleSet)
{
	FString* StrPtr = DefaultStyleSetCode.Find(StyleSet);
	if (StrPtr)
		return *StrPtr;
	return FString::Printf(TEXT("FName(\"%s\")"), *StyleSet.ToString());
}

void SSlateIconBrowser::FillDefaultStyleSetCodes()
{
#define STYLECODE(A, B) DefaultStyleSetCode.Add(FName(A), TEXT(B));
	
	STYLECODE("EditorStyle", "FAppStyle::GetAppStyleSetName()");
	STYLECODE("CoreStyle", "FAppStyle::GetAppStyleSetName()");
	STYLECODE("UMGCoreStyle", "FUMGCoreStyle::Get().GetStyleSetName()");

	;
#undef STYLECODE
}

void SSlateIconBrowser::CacheAllStyleNames()
{
	AllStyles.Empty(AllStyles.Num());
	
	FSlateStyleRegistry::IterateAllStyles(
		[&](const ISlateStyle& Style)
		{
			StyleListCache.Add(Style.GetStyleSetName().ToString());
			
			return true;
		}
	);
	if (StyleListCache.Num()>0)
	{
		StyleListCache.Sort();
		for (auto StyleName :StyleListCache)
		{
			AllStyles.Add(MakeShareable(new FName(StyleName)));
		}
	}
	
	// AllStyles.Sort();
}

void SSlateIconBrowser::UpdateIconListItems()
{
	//FindSlateStyle
	const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle(SelectedStyle);
	check(Style);
	TSet<FName> keys = Style->GetStyleKeys();

	LinesCache.Empty(keys.Num());
	LinesCache.Reserve(keys.Num());

	for (FName key : keys) {
		const FSlateBrush* brush = Style->GetOptionalBrush(key);
		if (!brush || brush == FStyleDefaults::GetNoBrush())
			continue;
		LinesCache.Add(key.ToString());
	}

	LinesCache.Sort();
	AllIconLines.Empty(LinesCache.Num());
	
	for (FString s : LinesCache)
		AllIconLines.Add(MakeShareable(new FName(s)));
	
	if (IconListView.IsValid())
		IconListView->RequestListRefresh();
		IconListView->ScrollToTop();
}


FReply SSlateIconBrowser::OnClickIcon(const FGeometry&, const FPointerEvent&,TSharedPtr<FName> Name)
{

	auto Brush = FSlateStyleRegistry::FindSlateStyle(SelectedStyle)->GetOptionalBrush(*Name);
	FVector2D DesiredIconSize = Brush->GetImageSize();
	if (Brush->GetImageType() == ESlateBrushImageType::NoImage)
		DesiredIconSize = FVector2D(20);
	SelectedIconName->SetText(FText::FromName(*Name));
	DetailIcon->SetImage(Brush);
	DetailText->SetText(FText::Format(
		FText::FromString(TEXT(
			"[Resource]: {1}\n"
			"[Size]: {2} x {3}\n")),
		{
			FText::FromName(*Name),
			FText::FromString(FPaths::ConvertRelativePathToFull(Brush->GetResourceName().ToString())),
			FText::AsNumber(static_cast<int>(DesiredIconSize.X)),
			FText::AsNumber(static_cast<int>(DesiredIconSize.Y))
		}
	));

	return FReply::Handled();
}

FReply SSlateIconBrowser::OnResetIcon()
{
	SelectedIconName->SetText(DefaultIconName);
	DetailIcon->SetImage(DefaultIcon.GetIcon());
	DetailText->SetText(FText::FromString(DefaultIconDetail));

	return FReply::Handled();
}


void SSlateIconBrowser::OnStyleNameFilterChanged(const FText& InFilterText)
{
	if (StyleNameSearchTimer.IsValid())
	{
		UnRegisterActiveTimer(StyleNameSearchTimer.ToSharedRef());
	}
	
	StyleNameSearchTimer = RegisterActiveTimer(SearchDelay, FWidgetActiveTimerDelegate::CreateLambda(
	[self = SharedThis(this)](double, double)
	{
		self->StyleNameSearchTimer.Reset();
		return EActiveTimerReturnType::Stop;
	}));

	
	StyleFilterString = InFilterText.ToString();

	AllStyles.Empty(StyleListCache.Num());
	
	if (StyleFilterString.IsEmpty()) {
		for (FString s : StyleListCache)
			AllStyles.Add(MakeShareable(new FName(s)));

		if (StyleListView.IsValid())
			StyleListView.Get()->RequestListRefresh();
		return;
	}
	else
	{
		for (FString s : StyleListCache) {
			if (s.Contains(StyleFilterString))
				AllStyles.Add(MakeShareable(new FName(s)));

			if (StyleListView.IsValid())
				StyleListView.Get()->RequestListRefresh();
		}
	}
	
}

void SSlateIconBrowser::OnIconInfoFilterChanged(const FText& InFilterText)
{
	if (IconSearchTimer.IsValid())
	{
		UnRegisterActiveTimer(IconSearchTimer.ToSharedRef());
	}
	
	IconSearchTimer = RegisterActiveTimer(SearchDelay, FWidgetActiveTimerDelegate::CreateLambda(
	[self = SharedThis(this)](double, double)
	{
		self->IconSearchTimer.Reset();
		return EActiveTimerReturnType::Stop;
	}));

	IconFilterString = InFilterText.ToString();

	AllIconLines.Empty(LinesCache.Num());
	
	if (IconFilterString.IsEmpty()) {
		for (FString s : LinesCache)
			AllIconLines.Add(MakeShareable(new FName(s)));

		if (IconListView.IsValid())
			IconListView.Get()->RequestListRefresh();
		return;
	}
	else
	{
		for (FString s : LinesCache) {
			if (s.Contains(IconFilterString))
				AllIconLines.Add(MakeShareable(new FName(s)));

			if (IconListView.IsValid())
				IconListView.Get()->RequestListRefresh();
		}
	}

}

void SSlateIconBrowser::CopyIconCodeToClipboard(FName Name, ECopyCodeStyle CodeStyle)
{
	FString CopyText = GenerateCopyCode(Name, CodeStyle);
	FPlatformApplicationMisc::ClipboardCopy(*CopyText);
	UE_LOG(LogTemp, Warning, TEXT("Copy code to clipboard: %s"), *CopyText);

	FNotificationInfo Info(LOCTEXT("CopiedNotification", "C++ code copied to clipboard"));
	Info.ExpireDuration = 3.f;
	Info.SubText = FText::FromString(CopyText);
	FSlateNotificationManager::Get().AddNotification(Info);
}

FString SSlateIconBrowser::GenerateCopyCode(FName Name, ECopyCodeStyle CodeStyle)
{
	FString CopyText(TEXT(""));
	switch (CodeStyle) {
	case CS_FSlateIcon:
		CopyText = FString::Printf(TEXT("FSlateIcon(%s, \"%s\")"), *TranslateDefaultStyleSets(SelectedStyle), *Name.ToString());
		break;
	case CS_FSlateIconFinderFindIcon:
		CopyText = FString::Printf(TEXT("FSlateIconFinder::FindIcon(\"%s\")"), *Name.ToString());
		break;

	}
	return CopyText;
}

FReply SSlateIconBrowser::EntryContextMenu(const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName Name)
{
	if (PointerEvent.GetEffectingButton() != EKeys::RightMouseButton)
		return FReply::Unhandled();

	if (PointerEvent.GetEventPath() == nullptr)
		return FReply::Unhandled();

	FString CopyCode;
	auto Clipboard = [&](FName N, ECopyCodeStyle Style)
	{
		CopyIconCodeToClipboard(N, Style);
	};

	FMenuBuilder MenuBuilder(true, nullptr);
	MenuBuilder.BeginSection("CopyOptions", LOCTEXT("CopyCodeOptions", "Copy Code"));
	{
		CopyCode = GenerateCopyCode(Name, CS_FSlateIcon);
		MenuBuilder.AddMenuEntry(
			FText::FromString(CopyCode),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda(Clipboard, Name, CS_FSlateIcon)));
		CopyCode = GenerateCopyCode(Name, CS_FSlateIconFinderFindIcon);
		MenuBuilder.AddMenuEntry(
			FText::FromString(CopyCode),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda(Clipboard, Name, CS_FSlateIconFinderFindIcon)));
		
	}
	MenuBuilder.EndSection();

	TSharedPtr<SWidget> MenuWidget = MenuBuilder.MakeWidget();
	FWidgetPath WidgetPath = *PointerEvent.GetEventPath();
	const FVector2D& Location = PointerEvent.GetScreenSpacePosition();
	FSlateApplication::Get().PushMenu(WidgetPath.Widgets.Last().Widget, WidgetPath, MenuWidget.ToSharedRef(), Location, FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
			
	return FReply::Handled();
}


TSharedRef<ITableRow> SSlateIconBrowser::GenerateStyleRow(TSharedPtr<FName> Item,
	const TSharedRef<STableViewBase>& TableViewBase)
{
	
	return SNew(SComboRow<TSharedRef<FName>>, TableViewBase)
		.Style(ClassBrowser_Detail::FClassStyle::Get()->TableRowStyle.Get())
	[
			SNew(SBox)
			.HeightOverride(32)
			.Padding(FMargin(1, 1))
			[
				SNew(SButton)
				.VAlign(VAlign_Center)
				[
					SNew(SRichTextBlock)
					.DecoratorStyleSet(&FAppStyle::Get())
					.Text(FText::FromString(Item->ToString() ))
				]
				.OnClicked(FOnClicked::CreateLambda([this,ItemRef=Item.ToSharedRef()]
				{
					if (StyleListView.IsValid())
					{
						StyleListView->SetSelection(ItemRef);
					}
					
					OnClickStyleItem(ItemRef);
					return FReply::Handled();
				}))
			]
	];
}

void SSlateIconBrowser::OnClickStyleItem(const TSharedRef<FName>& ItemRef)
{
	SelectedStyle = ItemRef.Get();
	UpdateIconListItems();
}


TSharedRef<ITableRow> SSlateIconBrowser::GenerateIconRow(TSharedPtr<FName> Name,
                                                            const TSharedRef<STableViewBase>& TableViewBase)
{
	auto Brush = FSlateStyleRegistry::FindSlateStyle(SelectedStyle)->GetOptionalBrush(*Name.Get());
	FVector2D DesiredIconSize = Brush->GetImageSize();
	if (Brush->GetImageType() == ESlateBrushImageType::NoImage)
		DesiredIconSize = FVector2D(20);
	
	return SNew(SComboRow<TSharedRef<FName>>, TableViewBase)
	[
		SNew(SBorder)
		.OnMouseDoubleClick_Lambda([&](const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName N)
		{
			CopyIconCodeToClipboard(N, CopyCodeStyle);
			return FReply::Handled();
		}, *Name.Get())
		.OnMouseButtonDown_Raw(this, &SSlateIconBrowser::OnClickIcon, Name)
		.OnMouseButtonUp_Raw(this, &SSlateIconBrowser::EntryContextMenu, *Name.Get())
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(10, 5))
			[
				SNew(STextBlock)
				.Text(FText::FromName(*Name.Get()))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(10, 5))
			[
				SNew(SImage)
				.DesiredSizeOverride(DesiredIconSize)
				.Image(Brush)
			]
		]
		

		

	];
}




#undef LOCTEXT_NAMESPACE
