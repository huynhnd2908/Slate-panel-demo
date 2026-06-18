#include "SSlateDemoPanel.h"

#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Text/STextBlock.h"

void SSlateDemoPanel::Construct(const FArguments& InArgs)
{
	ComboItems.Add(MakeShared<FString>(TEXT("Sword")));
	ComboItems.Add(MakeShared<FString>(TEXT("Bow")));
	ComboItems.Add(MakeShared<FString>(TEXT("Magic")));
	ComboItems.Add(MakeShared<FString>(TEXT("Shield")));

	SelectedItem = ComboItems[0];
	Data.Type = *SelectedItem;

	constexpr float LabelWidth = 86.0f;
	const FLinearColor MutedTextColor(0.62f, 0.62f, 0.62f, 1.0f);
	const FLinearColor CardColor(0.055f, 0.055f, 0.055f, 0.92f);
	const FLinearColor FooterColor(0.045f, 0.045f, 0.045f, 0.92f);

	auto MakeControlRow = [LabelWidth](const TCHAR* Label, const TSharedRef<SWidget>& ControlWidget)
	{
		return SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 14.0f, 0.0f)
			[
				SNew(SBox)
					.WidthOverride(LabelWidth)
				[
					SNew(STextBlock)
						.Text(FText::FromString(Label))
						.ColorAndOpacity(FSlateColor(FLinearColor(0.78f, 0.78f, 0.78f, 1.0f)))
				]
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			[
				ControlWidget
			];
	};

	ChildSlot
		[
			SNew(SBorder)
				.Padding(16.0f)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 5.0f)
					[
						SNew(STextBlock)
							.Text(FText::FromString(TEXT("Slate C++ Tool Demo")))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 15))
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 14.0f)
					[
						SNew(STextBlock)
							.Text(FText::FromString(TEXT("Edit the runtime data and apply it to the Output Log.")))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
							.ColorAndOpacity(FSlateColor(MutedTextColor))
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 14.0f)
					[
						SNew(SSeparator)
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 12.0f)
					[
						SNew(SBorder)
							.Padding(12.0f)
							.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
							.BorderBackgroundColor(FSlateColor(CardColor))
						[
							SNew(SVerticalBox)

							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 0.0f, 0.0f, 11.0f)
							[
								SNew(STextBlock)
									.Text(FText::FromString(TEXT("Demo Data")))
									.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
							]

							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 4.0f)
							[
								MakeControlRow(TEXT("Power"),
									SNew(SSpinBox<float>)
										.MinValue(0.0f)
										.MaxValue(100.0f)
										.MinSliderValue(0.0f)
										.MaxSliderValue(100.0f)
										.Value(Data.Power)
										.OnValueChanged(this, &SSlateDemoPanel::OnPowerChanged))
							]

							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 8.0f, 0.0f, 4.0f)
							[
								MakeControlRow(TEXT("Level"),
									SNew(SSpinBox<int32>)
										.MinValue(1)
										.MaxValue(10)
										.MinSliderValue(1)
										.MaxSliderValue(10)
										.Value(Data.Level)
										.OnValueChanged(this, &SSlateDemoPanel::OnLevelChanged))
							]

							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 8.0f, 0.0f, 4.0f)
							[
								MakeControlRow(TEXT("Type"),
									SNew(SComboBox<TSharedPtr<FString>>)
										.OptionsSource(&ComboItems)
										.InitiallySelectedItem(SelectedItem)
										.OnSelectionChanged(this, &SSlateDemoPanel::OnComboChanged)
										.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
											{
												return SNew(STextBlock)
													.Text(FText::FromString(Item.IsValid() ? *Item : TEXT("None")));
											})
										[
											SNew(STextBlock)
												.Text(this, &SSlateDemoPanel::GetSelectedComboText)
										])
							]

							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 8.0f, 0.0f, 2.0f)
							[
								MakeControlRow(TEXT("Enabled"),
									SNew(SCheckBox)
										.IsChecked(this, &SSlateDemoPanel::GetEnabledCheckState)
										.OnCheckStateChanged(this, &SSlateDemoPanel::OnEnabledChanged)
									[
										SNew(STextBlock)
											.Text(FText::FromString(TEXT("Use current values")))
									])
							]
						]
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBorder)
							.Padding(12.0f)
							.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
							.BorderBackgroundColor(FSlateColor(FooterColor))
						[
							SNew(SHorizontalBox)

								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								.VAlign(VAlign_Center)
								.Padding(0.0f, 0.0f, 18.0f, 0.0f)
								[
									SNew(SVerticalBox)

										+ SVerticalBox::Slot()
										.AutoHeight()
										.Padding(0.0f, 0.0f, 0.0f, 4.0f)
										[
											SNew(STextBlock)
												.Text(FText::FromString(TEXT("Live Summary")))
												.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
												.ColorAndOpacity(FSlateColor(MutedTextColor))
										]

										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											SNew(STextBlock)
												.Text(this, &SSlateDemoPanel::GetSummaryText)
										]
								]

								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								[
									SNew(SBox)
										.WidthOverride(112.0f)
										.HeightOverride(34.0f)
									[
										SNew(SButton)
											.HAlign(HAlign_Center)
											.VAlign(VAlign_Center)
											.Text(FText::FromString(TEXT("Apply")))
											.OnClicked(this, &SSlateDemoPanel::OnApplyClicked)
									]
								]
						]
					]
			]
		];
}

void SSlateDemoPanel::OnPowerChanged(float NewValue)
{
	// UI callbacks write directly into this simple C++ struct; bound text reads it back live.
	Data.Power = NewValue;
}

void SSlateDemoPanel::OnLevelChanged(int32 NewValue)
{
	Data.Level = NewValue;
}

void SSlateDemoPanel::OnComboChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	SelectedItem = NewSelection;

	if (SelectedItem.IsValid())
	{
		Data.Type = *SelectedItem;
	}
}

void SSlateDemoPanel::OnEnabledChanged(ECheckBoxState NewState)
{
	Data.bEnabled = NewState == ECheckBoxState::Checked;
}

FText SSlateDemoPanel::GetSelectedComboText() const
{
	return FText::FromString(SelectedItem.IsValid() ? *SelectedItem : TEXT("None"));
}

ECheckBoxState SSlateDemoPanel::GetEnabledCheckState() const
{
	return Data.bEnabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FText SSlateDemoPanel::GetSummaryText() const
{
	return FText::FromString(FString::Printf(
		TEXT("Power: %.0f | Level: %d | Type: %s | Enabled: %s"),
		Data.Power,
		Data.Level,
		*Data.Type,
		Data.bEnabled ? TEXT("true") : TEXT("false")));
}

FReply SSlateDemoPanel::OnApplyClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("SlateDemoPlugin Apply - Power: %.2f, Level: %d, Type: %s, Enabled: %s"),
		Data.Power,
		Data.Level,
		*Data.Type,
		Data.bEnabled ? TEXT("true") : TEXT("false"));
	return FReply::Handled();
}
