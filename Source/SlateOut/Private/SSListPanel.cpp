// Fill out your copyright notice in the Description page of Project Settings.


#include "SSListPanel.h"
#include "Widgets/Testing/SPerfSuite.h"
#include "Widgets/Testing/SStarshipSuite.h"
#include "Widgets/Testing/STestSuite.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Styling/CoreStyle.h"
#include "Modules/ModuleManager.h"
#include "Framework/Application/SlateApplication.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SSListPanel::Construct(const FArguments& InArgs)
{

	ChildSlot
	[
		// Put your tab content here!
        SNew(SBox)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4.0f )
        .HAlign(HAlign_Left)
        [
	        SNew( SButton )
			.Text( NSLOCTEXT("DeveloperToolbox", "ShowStarshipSuite", "组件") )
			.OnClicked( this, &SSListPanel::ShowStarshipSuite) 

        ]
        
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4.0f )
        .HAlign(HAlign_Left)
        [
	        SNew( SButton )
			.Text( NSLOCTEXT("DeveloperToolbox", "ShowPrefSuite", "列表") )
			.OnClicked( this, &SSListPanel::ShowPrefSuite) 
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 4.0f )
        .HAlign(HAlign_Left)
        [
	        SNew( SButton )
			.Text( NSLOCTEXT("DeveloperToolbox", "ShowTestSuite", "测试集合") )
			.OnClicked( this, &SSListPanel::ShowTestSuite) 
        ]
        ]
	];

}

FReply SSListPanel::ShowTestSuite()
{
    RestoreSlateTestSuite();
	return FReply::Handled();
}

FReply SSListPanel::ShowStarshipSuite()
{
	RestoreStarshipSuite();
	return FReply::Handled();
}

FReply SSListPanel::ShowPrefSuite()
{
	SummonPerfTestSuite();
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
