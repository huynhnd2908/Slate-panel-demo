#pragma once

#include "CoreMinimal.h"
#include "SlateDemoTypes.h"
#include "Types/SlateEnums.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

class SEditableTextBox;

class SSlateDemoPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSlateDemoPanel) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FSlateDemoData Data;
	FSlateDemoDerivedStats CachedStats;

	TArray<TSharedPtr<FString>> TypeItems;
	TArray<TSharedPtr<FSlateDemoPreset>> PresetItems;
	TArray<TSharedPtr<FSlateDemoValidationIssue>> ValidationIssues;
	TArray<TSharedPtr<FSlateDemoHistoryEntry>> HistoryItems;
	TArray<FSlateDemoData> UndoStack;
	TArray<FSlateDemoData> RedoStack;

	TSharedPtr<FString> SelectedTypeItem;
	TSharedPtr<FSlateDemoPreset> SelectedPresetItem;
	TSharedPtr<SListView<TSharedPtr<FSlateDemoValidationIssue>>> ValidationListView;
	TSharedPtr<SListView<TSharedPtr<FSlateDemoHistoryEntry>>> HistoryListView;

	FString NewPresetName;
	int32 NextHistoryId = 1;
	int32 Revision = 1;
	int32 LastAppliedRevision = 0;

	void InitializeOptions();
	void RefreshComputedState();
	void ApplyData(const FSlateDemoData& NewData, bool bRecordUndo, bool bSelectCustomPreset);
	void PushUndoSnapshot();
	void SelectType(const FString& TypeName);
	void SelectPresetByName(const FString& PresetName);
	TSharedPtr<FSlateDemoPreset> FindPresetByName(const FString& PresetName) const;

	float GetPowerValue() const;
	int32 GetLevelValue() const;
	void OnPowerChanged(float NewValue);
	void OnLevelChanged(int32 NewValue);
	void OnTypeChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	void OnEnabledChanged(ECheckBoxState NewState);
	void OnPresetChanged(TSharedPtr<FSlateDemoPreset> NewSelection, ESelectInfo::Type SelectInfo);
	void OnNewPresetNameChanged(const FText& NewText);

	FReply OnSavePresetClicked();
	FReply OnDeletePresetClicked();
	FReply OnUndoClicked();
	FReply OnRedoClicked();
	FReply OnResetClicked();
	FReply OnClearHistoryClicked();
	FReply OnExportJsonClicked();
	FReply OnApplyClicked();
	FReply OnRestoreHistoryClicked(TSharedPtr<FSlateDemoHistoryEntry> Entry);

	FText GetSelectedTypeText() const;
	FText GetSelectedPresetText() const;
	FText GetNewPresetNameText() const;
	ECheckBoxState GetEnabledCheckState() const;
	FText GetSummaryText() const;
	FText GetScoreText() const;
	FText GetDerivedStatsText() const;
	FText GetDirtyStateText() const;
	FSlateColor GetDirtyStateColor() const;
	FSlateColor GetStatusColor() const;

	bool CanSavePreset() const;
	bool CanDeletePreset() const;
	bool CanUndo() const;
	bool CanRedo() const;
	bool IsApplyEnabled() const;

	TSharedRef<ITableRow> OnGenerateValidationRow(TSharedPtr<FSlateDemoValidationIssue> Issue, const TSharedRef<STableViewBase>& OwnerTable) const;
	TSharedRef<ITableRow> OnGenerateHistoryRow(TSharedPtr<FSlateDemoHistoryEntry> Entry, const TSharedRef<STableViewBase>& OwnerTable);
};
