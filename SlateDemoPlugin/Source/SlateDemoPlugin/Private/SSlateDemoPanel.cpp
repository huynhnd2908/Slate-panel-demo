#include "SSlateDemoPanel.h"

#include "SlateDemoLogic.h"

#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SHeaderRow.h"
#include "Widgets/Views/STableRow.h"

namespace SlateDemoPanel
{
	constexpr float LabelWidth = 98.0f;
	const FLinearColor CardColor(0.055f, 0.055f, 0.055f, 0.94f);
	const FLinearColor FooterColor(0.045f, 0.045f, 0.045f, 0.95f);
	const FLinearColor MutedTextColor(0.62f, 0.62f, 0.62f, 1.0f);
	const FLinearColor GoodColor(0.35f, 0.9f, 0.55f, 1.0f);
	const FLinearColor WarningColor(1.0f, 0.72f, 0.2f, 1.0f);
	const FLinearColor ErrorColor(1.0f, 0.32f, 0.28f, 1.0f);
}

void SSlateDemoPanel::Construct(const FArguments& InArgs)
{
	InitializeOptions();
	RefreshComputedState();

	auto MakeControlRow = [](const TCHAR* Label, const TSharedRef<SWidget>& ControlWidget)
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 14.0f, 0.0f)
			[
				SNew(SBox)
					.WidthOverride(SlateDemoPanel::LabelWidth)
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

	auto MakeCard = [](const TSharedRef<SWidget>& Content)
	{
		return SNew(SBorder)
			.Padding(12.0f)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.BorderBackgroundColor(FSlateColor(SlateDemoPanel::CardColor))
			[
				Content
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
							.Text(FText::FromString(TEXT("Slate C++ Ability Tuning Tool")))
							.Font(FCoreStyle::GetDefaultFontStyle(FName(TEXT("Bold")), 17))
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 14.0f)
					[
						SNew(STextBlock)
							.Text(FText::FromString(TEXT("Runtime presets, undo/redo, validation, JSON export, immutable snapshots, and restoreable history.")))
							.Font(FCoreStyle::GetDefaultFontStyle(FName(TEXT("Regular")), 9))
							.ColorAndOpacity(FSlateColor(SlateDemoPanel::MutedTextColor))
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
						SNew(SHorizontalBox)

							+ SHorizontalBox::Slot()
							.FillWidth(0.58f)
							.Padding(0.0f, 0.0f, 12.0f, 0.0f)
							[
								MakeCard(
									SNew(SVerticalBox)

									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0.0f, 0.0f, 0.0f, 11.0f)
									[
										SNew(SHorizontalBox)
											+ SHorizontalBox::Slot()
											.FillWidth(1.0f)
											.VAlign(VAlign_Center)
											[
												SNew(STextBlock)
													.Text(FText::FromString(TEXT("Configuration")))
													.Font(FCoreStyle::GetDefaultFontStyle(FName(TEXT("Bold")), 10))
											]
											+ SHorizontalBox::Slot()
											.AutoWidth()
											.VAlign(VAlign_Center)
											[
												SNew(STextBlock)
													.Text(this, &SSlateDemoPanel::GetDirtyStateText)
													.ColorAndOpacity(this, &SSlateDemoPanel::GetDirtyStateColor)
											]
									]

									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0.0f, 4.0f)
									[
										MakeControlRow(TEXT("Preset"),
											SNew(SComboBox<TSharedPtr<FSlateDemoPreset>>)
												.OptionsSource(&PresetItems)
												.InitiallySelectedItem(SelectedPresetItem)
												.OnSelectionChanged(this, &SSlateDemoPanel::OnPresetChanged)
												.OnGenerateWidget_Lambda([](TSharedPtr<FSlateDemoPreset> Item)
													{
														return SNew(STextBlock)
															.Text(FText::FromString(Item.IsValid() ? Item->Name : TEXT("None")));
													})
												[
													SNew(STextBlock)
														.Text(this, &SSlateDemoPanel::GetSelectedPresetText)
												])
									]

									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0.0f, 8.0f, 0.0f, 4.0f)
									[
										MakeControlRow(TEXT("Preset Name"),
											SNew(SEditableTextBox)
												.Text(this, &SSlateDemoPanel::GetNewPresetNameText)
												.HintText(FText::FromString(TEXT("Runtime preset name")))
												.OnTextChanged(this, &SSlateDemoPanel::OnNewPresetNameChanged))
									]

									+ SVerticalBox::Slot()
									.AutoHeight()
									.HAlign(HAlign_Right)
									.Padding(0.0f, 2.0f, 0.0f, 8.0f)
									[
										SNew(SHorizontalBox)
											+ SHorizontalBox::Slot()
											.AutoWidth()
											.Padding(0.0f, 0.0f, 6.0f, 0.0f)
											[
												SNew(SButton)
													.IsEnabled(this, &SSlateDemoPanel::CanSavePreset)
													.Text(FText::FromString(TEXT("Save Preset")))
													.OnClicked(this, &SSlateDemoPanel::OnSavePresetClicked)
											]
											+ SHorizontalBox::Slot()
											.AutoWidth()
											[
												SNew(SButton)
													.IsEnabled(this, &SSlateDemoPanel::CanDeletePreset)
													.Text(FText::FromString(TEXT("Delete Preset")))
													.OnClicked(this, &SSlateDemoPanel::OnDeletePresetClicked)
											]
									]

									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0.0f, 8.0f, 0.0f, 4.0f)
									[
										MakeControlRow(TEXT("Power"),
											SNew(SSpinBox<float>)
												.MinValue(0.0f)
												.MaxValue(100.0f)
												.MinSliderValue(0.0f)
												.MaxSliderValue(100.0f)
												.Value(this, &SSlateDemoPanel::GetPowerValue)
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
												.Value(this, &SSlateDemoPanel::GetLevelValue)
												.OnValueChanged(this, &SSlateDemoPanel::OnLevelChanged))
									]

									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0.0f, 8.0f, 0.0f, 4.0f)
									[
										MakeControlRow(TEXT("Type"),
											SNew(SComboBox<TSharedPtr<FString>>)
												.OptionsSource(&TypeItems)
												.InitiallySelectedItem(SelectedTypeItem)
												.OnSelectionChanged(this, &SSlateDemoPanel::OnTypeChanged)
												.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
													{
														return SNew(STextBlock)
															.Text(FText::FromString(Item.IsValid() ? *Item : TEXT("None")));
													})
												[
													SNew(STextBlock)
														.Text(this, &SSlateDemoPanel::GetSelectedTypeText)
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
													.Text(FText::FromString(TEXT("Participates in score calculation")))
											])
									]

									+ SVerticalBox::Slot()
									.AutoHeight()
									.HAlign(HAlign_Right)
									.Padding(0.0f, 12.0f, 0.0f, 0.0f)
									[
										SNew(SHorizontalBox)
											+ SHorizontalBox::Slot()
											.AutoWidth()
											.Padding(0.0f, 0.0f, 6.0f, 0.0f)
											[
												SNew(SButton)
													.IsEnabled(this, &SSlateDemoPanel::CanUndo)
													.Text(FText::FromString(TEXT("Undo")))
													.OnClicked(this, &SSlateDemoPanel::OnUndoClicked)
											]
											+ SHorizontalBox::Slot()
											.AutoWidth()
											.Padding(0.0f, 0.0f, 6.0f, 0.0f)
											[
												SNew(SButton)
													.IsEnabled(this, &SSlateDemoPanel::CanRedo)
													.Text(FText::FromString(TEXT("Redo")))
													.OnClicked(this, &SSlateDemoPanel::OnRedoClicked)
											]
											+ SHorizontalBox::Slot()
											.AutoWidth()
											[
												SNew(SButton)
													.Text(FText::FromString(TEXT("Reset")))
													.OnClicked(this, &SSlateDemoPanel::OnResetClicked)
											]
									])
							]

							+ SHorizontalBox::Slot()
							.FillWidth(0.42f)
							[
								MakeCard(
									SNew(SVerticalBox)

									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0.0f, 0.0f, 0.0f, 8.0f)
									[
										SNew(STextBlock)
											.Text(FText::FromString(TEXT("Diagnostics")))
											.Font(FCoreStyle::GetDefaultFontStyle(FName(TEXT("Bold")), 10))
									]

									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0.0f, 0.0f, 0.0f, 8.0f)
									[
										SNew(SHorizontalBox)
											+ SHorizontalBox::Slot()
											.FillWidth(0.38f)
											.Padding(0.0f, 0.0f, 12.0f, 0.0f)
											[
												SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.AutoHeight()
													[
														SNew(STextBlock)
															.Text(FText::FromString(TEXT("Score")))
															.Font(FCoreStyle::GetDefaultFontStyle(FName(TEXT("Regular")), 8))
															.ColorAndOpacity(FSlateColor(SlateDemoPanel::MutedTextColor))
													]
													+ SVerticalBox::Slot()
													.AutoHeight()
													[
														SNew(STextBlock)
															.Text(this, &SSlateDemoPanel::GetScoreText)
															.Font(FCoreStyle::GetDefaultFontStyle(FName(TEXT("Bold")), 22))
															.ColorAndOpacity(this, &SSlateDemoPanel::GetStatusColor)
													]
											]
											+ SHorizontalBox::Slot()
											.FillWidth(0.62f)
											[
												SNew(STextBlock)
													.Text(this, &SSlateDemoPanel::GetDerivedStatsText)
											]
									]

									+ SVerticalBox::Slot()
									.AutoHeight()
									.Padding(0.0f, 4.0f, 0.0f, 6.0f)
									[
										SNew(SSeparator)
									]

									+ SVerticalBox::Slot()
									.FillHeight(1.0f)
									[
										SAssignNew(ValidationListView, SListView<TSharedPtr<FSlateDemoValidationIssue>>)
											.ListItemsSource(&ValidationIssues)
											.SelectionMode(ESelectionMode::None)
											.OnGenerateRow(this, &SSlateDemoPanel::OnGenerateValidationRow)
									])
							]
					]

					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					.Padding(0.0f, 0.0f, 0.0f, 12.0f)
					[
						SNew(SBox)
							.MinDesiredHeight(150.0f)
						[
							MakeCard(
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								.Padding(0.0f, 0.0f, 0.0f, 8.0f)
								[
									SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.FillWidth(1.0f)
										.VAlign(VAlign_Center)
										[
											SNew(STextBlock)
												.Text(FText::FromString(TEXT("Apply History")))
												.Font(FCoreStyle::GetDefaultFontStyle(FName(TEXT("Bold")), 10))
										]
										+ SHorizontalBox::Slot()
										.AutoWidth()
										[
											SNew(SButton)
												.Text(FText::FromString(TEXT("Clear History")))
												.OnClicked(this, &SSlateDemoPanel::OnClearHistoryClicked)
										]
								]
								+ SVerticalBox::Slot()
								.FillHeight(1.0f)
								[
									SAssignNew(HistoryListView, SListView<TSharedPtr<FSlateDemoHistoryEntry>>)
										.ListItemsSource(&HistoryItems)
										.SelectionMode(ESelectionMode::None)
										.OnGenerateRow(this, &SSlateDemoPanel::OnGenerateHistoryRow)
										.HeaderRow(
											SNew(SHeaderRow)
											+ SHeaderRow::Column(TEXT("Id"))
											.DefaultLabel(FText::FromString(TEXT("Id")))
											.FixedWidth(54.0f)
											+ SHeaderRow::Column(TEXT("Time"))
											.DefaultLabel(FText::FromString(TEXT("Time")))
											.FixedWidth(82.0f)
											+ SHeaderRow::Column(TEXT("Build"))
											.DefaultLabel(FText::FromString(TEXT("Build")))
											.FillWidth(0.38f)
											+ SHeaderRow::Column(TEXT("Score"))
											.DefaultLabel(FText::FromString(TEXT("Score")))
											.FixedWidth(74.0f)
											+ SHeaderRow::Column(TEXT("Status"))
											.DefaultLabel(FText::FromString(TEXT("Status")))
											.FillWidth(0.42f)
											+ SHeaderRow::Column(TEXT("Action"))
											.DefaultLabel(FText::FromString(TEXT("")))
											.FixedWidth(76.0f))
								])
						]
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBorder)
							.Padding(12.0f)
							.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
							.BorderBackgroundColor(FSlateColor(SlateDemoPanel::FooterColor))
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
												.Font(FCoreStyle::GetDefaultFontStyle(FName(TEXT("Regular")), 8))
												.ColorAndOpacity(FSlateColor(SlateDemoPanel::MutedTextColor))
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
								.Padding(0.0f, 0.0f, 8.0f, 0.0f)
								[
									SNew(SButton)
										.Text(FText::FromString(TEXT("Export JSON")))
										.OnClicked(this, &SSlateDemoPanel::OnExportJsonClicked)
								]

								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								[
									SNew(SBox)
										.WidthOverride(136.0f)
										.HeightOverride(38.0f)
									[
										SNew(SButton)
											.HAlign(HAlign_Center)
											.VAlign(VAlign_Center)
											.IsEnabled(this, &SSlateDemoPanel::IsApplyEnabled)
											.Text(FText::FromString(TEXT("Apply Snapshot")))
											.OnClicked(this, &SSlateDemoPanel::OnApplyClicked)
									]
								]
						]
					]
			]
		];
}

void SSlateDemoPanel::InitializeOptions()
{
	FSlateDemoLogic::BuildDefaultTypes(TypeItems);
	FSlateDemoLogic::BuildDefaultPresets(PresetItems);

	SelectedTypeItem = TypeItems.Num() > 0 ? TypeItems[0] : nullptr;
	SelectedPresetItem = PresetItems.Num() > 0 ? PresetItems[0] : nullptr;

	if (SelectedTypeItem.IsValid())
	{
		Data.Type = *SelectedTypeItem;
	}
}

void SSlateDemoPanel::RefreshComputedState()
{
	CachedStats = FSlateDemoLogic::CalculateStats(Data);
	FSlateDemoLogic::BuildValidationIssues(Data, CachedStats, ValidationIssues);

	if (ValidationListView.IsValid())
	{
		ValidationListView->RequestListRefresh();
	}
}

void SSlateDemoPanel::ApplyData(const FSlateDemoData& NewData, bool bRecordUndo, bool bSelectCustomPreset)
{
	if (FSlateDemoLogic::AreEqual(Data, NewData))
	{
		return;
	}

	if (bRecordUndo)
	{
		PushUndoSnapshot();
		RedoStack.Reset();
	}

	Data = NewData;
	++Revision;

	SelectType(Data.Type);
	if (bSelectCustomPreset)
	{
		SelectPresetByName(TEXT("Custom"));
	}

	RefreshComputedState();
}

void SSlateDemoPanel::PushUndoSnapshot()
{
	UndoStack.Add(Data);

	constexpr int32 MaxUndoItems = 30;
	if (UndoStack.Num() > MaxUndoItems)
	{
		UndoStack.RemoveAt(0);
	}
}

void SSlateDemoPanel::SelectType(const FString& TypeName)
{
	for (const TSharedPtr<FString>& Item : TypeItems)
	{
		if (Item.IsValid() && *Item == TypeName)
		{
			SelectedTypeItem = Item;
			return;
		}
	}
}

void SSlateDemoPanel::SelectPresetByName(const FString& PresetName)
{
	SelectedPresetItem = FindPresetByName(PresetName);
}

TSharedPtr<FSlateDemoPreset> SSlateDemoPanel::FindPresetByName(const FString& PresetName) const
{
	for (const TSharedPtr<FSlateDemoPreset>& Preset : PresetItems)
	{
		if (Preset.IsValid() && Preset->Name.Equals(PresetName, ESearchCase::IgnoreCase))
		{
			return Preset;
		}
	}

	return nullptr;
}

float SSlateDemoPanel::GetPowerValue() const
{
	return Data.Power;
}

int32 SSlateDemoPanel::GetLevelValue() const
{
	return Data.Level;
}

void SSlateDemoPanel::OnPowerChanged(float NewValue)
{
	FSlateDemoData NewData = Data;
	NewData.Power = NewValue;
	ApplyData(NewData, true, true);
}

void SSlateDemoPanel::OnLevelChanged(int32 NewValue)
{
	FSlateDemoData NewData = Data;
	NewData.Level = NewValue;
	ApplyData(NewData, true, true);
}

void SSlateDemoPanel::OnTypeChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (!NewSelection.IsValid())
	{
		return;
	}

	FSlateDemoData NewData = Data;
	NewData.Type = *NewSelection;
	ApplyData(NewData, true, true);
}

void SSlateDemoPanel::OnEnabledChanged(ECheckBoxState NewState)
{
	FSlateDemoData NewData = Data;
	NewData.bEnabled = NewState == ECheckBoxState::Checked;
	ApplyData(NewData, true, true);
}

void SSlateDemoPanel::OnPresetChanged(TSharedPtr<FSlateDemoPreset> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (!NewSelection.IsValid())
	{
		return;
	}

	SelectedPresetItem = NewSelection;

	if (SelectedPresetItem->Name == TEXT("Custom"))
	{
		return;
	}

	ApplyData(SelectedPresetItem->Data, true, false);
}

void SSlateDemoPanel::OnNewPresetNameChanged(const FText& NewText)
{
	NewPresetName = NewText.ToString();
}

FReply SSlateDemoPanel::OnSavePresetClicked()
{
	const FString Name = NewPresetName.TrimStartAndEnd();
	if (Name.IsEmpty())
	{
		return FReply::Handled();
	}

	TSharedPtr<FSlateDemoPreset> ExistingPreset = FindPresetByName(Name);
	if (ExistingPreset.IsValid())
	{
		if (!ExistingPreset->bRuntimePreset)
		{
			UE_LOG(LogTemp, Warning, TEXT("SlateDemoPlugin - Cannot overwrite built-in preset: %s"), *Name);
			return FReply::Handled();
		}

		ExistingPreset->Data = Data;
		ExistingPreset->Description = TEXT("Updated runtime preset");
		SelectedPresetItem = ExistingPreset;
	}
	else
	{
		SelectedPresetItem = MakeShared<FSlateDemoPreset>(FSlateDemoPreset{
			Name,
			Data,
			TEXT("Runtime preset saved from current data"),
			true
		});
		PresetItems.Add(SelectedPresetItem);
	}

	NewPresetName.Reset();
	return FReply::Handled();
}

FReply SSlateDemoPanel::OnDeletePresetClicked()
{
	if (!SelectedPresetItem.IsValid() || !SelectedPresetItem->bRuntimePreset)
	{
		return FReply::Handled();
	}

	PresetItems.Remove(SelectedPresetItem);
	SelectPresetByName(TEXT("Custom"));
	return FReply::Handled();
}

FReply SSlateDemoPanel::OnUndoClicked()
{
	if (UndoStack.Num() == 0)
	{
		return FReply::Handled();
	}

	RedoStack.Add(Data);
	Data = UndoStack.Pop(EAllowShrinking::No);
	++Revision;
	SelectType(Data.Type);
	SelectPresetByName(TEXT("Custom"));
	RefreshComputedState();
	return FReply::Handled();
}

FReply SSlateDemoPanel::OnRedoClicked()
{
	if (RedoStack.Num() == 0)
	{
		return FReply::Handled();
	}

	UndoStack.Add(Data);
	Data = RedoStack.Pop(EAllowShrinking::No);
	++Revision;
	SelectType(Data.Type);
	SelectPresetByName(TEXT("Custom"));
	RefreshComputedState();
	return FReply::Handled();
}

FReply SSlateDemoPanel::OnResetClicked()
{
	ApplyData(FSlateDemoData(), true, true);
	return FReply::Handled();
}

FReply SSlateDemoPanel::OnClearHistoryClicked()
{
	HistoryItems.Reset();

	if (HistoryListView.IsValid())
	{
		HistoryListView->RequestListRefresh();
	}

	return FReply::Handled();
}

FReply SSlateDemoPanel::OnExportJsonClicked()
{
	RefreshComputedState();
	const FString Json = FSlateDemoLogic::ExportDataToJson(Data, CachedStats, ValidationIssues, Revision);
	UE_LOG(LogTemp, Warning, TEXT("SlateDemoPlugin Export JSON:\n%s"), *Json);
	return FReply::Handled();
}

FReply SSlateDemoPanel::OnApplyClicked()
{
	RefreshComputedState();

	const FString Status = FSlateDemoLogic::GetWorstStatus(ValidationIssues);
	HistoryItems.Insert(MakeShared<FSlateDemoHistoryEntry>(FSlateDemoHistoryEntry{
		NextHistoryId++,
		Revision,
		FDateTime::Now(),
		Data,
		CachedStats,
		Status
	}), 0);

	constexpr int32 MaxHistoryItems = 10;
	if (HistoryItems.Num() > MaxHistoryItems)
	{
		HistoryItems.SetNum(MaxHistoryItems);
	}

	LastAppliedRevision = Revision;

	if (HistoryListView.IsValid())
	{
		HistoryListView->RequestListRefresh();
	}

	UE_LOG(LogTemp, Warning, TEXT("SlateDemoPlugin Apply - Revision: %d, Power: %.2f, Level: %d, Type: %s, Enabled: %s, Score: %.2f, Damage: %.2f, Cooldown: %.2f, Cost: %.2f, Efficiency: %.2f, Status: %s, HistoryCount: %d"),
		Revision,
		Data.Power,
		Data.Level,
		*Data.Type,
		Data.bEnabled ? TEXT("true") : TEXT("false"),
		CachedStats.Score,
		CachedStats.Damage,
		CachedStats.Cooldown,
		CachedStats.ResourceCost,
		CachedStats.Efficiency,
		*Status,
		HistoryItems.Num());

	return FReply::Handled();
}

FReply SSlateDemoPanel::OnRestoreHistoryClicked(TSharedPtr<FSlateDemoHistoryEntry> Entry)
{
	if (Entry.IsValid())
	{
		ApplyData(Entry->Snapshot, true, true);
	}

	return FReply::Handled();
}

FText SSlateDemoPanel::GetSelectedTypeText() const
{
	return FText::FromString(SelectedTypeItem.IsValid() ? *SelectedTypeItem : TEXT("None"));
}

FText SSlateDemoPanel::GetSelectedPresetText() const
{
	return FText::FromString(SelectedPresetItem.IsValid() ? SelectedPresetItem->Name : TEXT("Custom"));
}

FText SSlateDemoPanel::GetNewPresetNameText() const
{
	return FText::FromString(NewPresetName);
}

ECheckBoxState SSlateDemoPanel::GetEnabledCheckState() const
{
	return Data.bEnabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FText SSlateDemoPanel::GetSummaryText() const
{
	return FText::FromString(FString::Printf(
		TEXT("Rev %d | %s | Power %.0f | Level %d | Enabled %s | Score %.1f | %s"),
		Revision,
		*Data.Type,
		Data.Power,
		Data.Level,
		Data.bEnabled ? TEXT("true") : TEXT("false"),
		CachedStats.Score,
		*FSlateDemoLogic::GetWorstStatus(ValidationIssues)));
}

FText SSlateDemoPanel::GetScoreText() const
{
	return FText::FromString(FString::Printf(TEXT("%.1f"), CachedStats.Score));
}

FText SSlateDemoPanel::GetDerivedStatsText() const
{
	return FText::FromString(FString::Printf(
		TEXT("Damage: %.1f\nCooldown: %.2fs\nCost: %.1f\nEfficiency: %.2f"),
		CachedStats.Damage,
		CachedStats.Cooldown,
		CachedStats.ResourceCost,
		CachedStats.Efficiency));
}

FText SSlateDemoPanel::GetDirtyStateText() const
{
	return FText::FromString(Revision == LastAppliedRevision ? TEXT("Applied") : TEXT("Unsaved"));
}

FSlateColor SSlateDemoPanel::GetDirtyStateColor() const
{
	return Revision == LastAppliedRevision
		? FSlateColor(SlateDemoPanel::GoodColor)
		: FSlateColor(SlateDemoPanel::WarningColor);
}

FSlateColor SSlateDemoPanel::GetStatusColor() const
{
	if (FSlateDemoLogic::HasBlockingErrors(ValidationIssues))
	{
		return FSlateColor(SlateDemoPanel::ErrorColor);
	}

	for (const TSharedPtr<FSlateDemoValidationIssue>& Issue : ValidationIssues)
	{
		if (Issue.IsValid() && Issue->Severity == ESlateDemoIssueSeverity::Warning)
		{
			return FSlateColor(SlateDemoPanel::WarningColor);
		}
	}

	return FSlateColor(SlateDemoPanel::GoodColor);
}

bool SSlateDemoPanel::CanSavePreset() const
{
	const FString Name = NewPresetName.TrimStartAndEnd();
	if (Name.IsEmpty())
	{
		return false;
	}

	const TSharedPtr<FSlateDemoPreset> ExistingPreset = FindPresetByName(Name);
	return !ExistingPreset.IsValid() || ExistingPreset->bRuntimePreset;
}

bool SSlateDemoPanel::CanDeletePreset() const
{
	return SelectedPresetItem.IsValid() && SelectedPresetItem->bRuntimePreset;
}

bool SSlateDemoPanel::CanUndo() const
{
	return UndoStack.Num() > 0;
}

bool SSlateDemoPanel::CanRedo() const
{
	return RedoStack.Num() > 0;
}

bool SSlateDemoPanel::IsApplyEnabled() const
{
	return !FSlateDemoLogic::HasBlockingErrors(ValidationIssues) && Revision != LastAppliedRevision;
}

TSharedRef<ITableRow> SSlateDemoPanel::OnGenerateValidationRow(TSharedPtr<FSlateDemoValidationIssue> Issue, const TSharedRef<STableViewBase>& OwnerTable) const
{
	return SNew(STableRow<TSharedPtr<FSlateDemoValidationIssue>>, OwnerTable)
		.Padding(4.0f)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(0.0f, 0.0f, 8.0f, 0.0f)
						[
							SNew(STextBlock)
								.Text(FText::FromString(Issue.IsValid() ? FSlateDemoLogic::GetSeverityText(Issue->Severity) : TEXT("INFO")))
								.ColorAndOpacity(Issue.IsValid() ? FSlateDemoLogic::GetSeverityColor(Issue->Severity) : FSlateColor(SlateDemoPanel::MutedTextColor))
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(STextBlock)
								.Text(FText::FromString(Issue.IsValid() ? Issue->Title : TEXT("Unknown")))
						]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 2.0f, 0.0f, 0.0f)
				[
					SNew(STextBlock)
						.Text(FText::FromString(Issue.IsValid() ? Issue->Details : TEXT("")))
						.ColorAndOpacity(FSlateColor(SlateDemoPanel::MutedTextColor))
				]
		];
}

TSharedRef<ITableRow> SSlateDemoPanel::OnGenerateHistoryRow(TSharedPtr<FSlateDemoHistoryEntry> Entry, const TSharedRef<STableViewBase>& OwnerTable)
{
	const FSlateDemoData Snapshot = Entry.IsValid() ? Entry->Snapshot : FSlateDemoData();

	return SNew(STableRow<TSharedPtr<FSlateDemoHistoryEntry>>, OwnerTable)
		.Padding(3.0f)
		[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 10.0f, 0.0f)
				[
					SNew(SBox)
						.WidthOverride(44.0f)
					[
						SNew(STextBlock)
							.Text(FText::FromString(Entry.IsValid() ? FString::Printf(TEXT("#%02d"), Entry->Id) : TEXT("-")))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 12.0f, 0.0f)
				[
					SNew(SBox)
						.WidthOverride(70.0f)
					[
						SNew(STextBlock)
							.Text(FText::FromString(Entry.IsValid() ? Entry->AppliedAt.ToString(TEXT("%H:%M:%S")) : TEXT("-")))
					]
				]
				+ SHorizontalBox::Slot()
				.FillWidth(0.38f)
				.Padding(0.0f, 0.0f, 12.0f, 0.0f)
				[
					SNew(STextBlock)
						.Text(FText::FromString(FString::Printf(TEXT("%s L%d P%.0f"), *Snapshot.Type, Snapshot.Level, Snapshot.Power)))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.0f, 0.0f, 14.0f, 0.0f)
				[
					SNew(SBox)
						.WidthOverride(60.0f)
					[
						SNew(STextBlock)
							.Text(FText::FromString(Entry.IsValid() ? FString::Printf(TEXT("%.1f"), Entry->Stats.Score) : TEXT("-")))
					]
				]
				+ SHorizontalBox::Slot()
				.FillWidth(0.42f)
				[
					SNew(STextBlock)
						.Text(FText::FromString(Entry.IsValid() ? Entry->Status : TEXT("-")))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
						.Text(FText::FromString(TEXT("Restore")))
						.OnClicked(this, &SSlateDemoPanel::OnRestoreHistoryClicked, Entry)
				]
		];
}
