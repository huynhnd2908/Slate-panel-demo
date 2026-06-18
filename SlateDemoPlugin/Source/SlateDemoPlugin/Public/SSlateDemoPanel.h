#pragma once

#include "CoreMinimal.h"
#include "Types/SlateEnums.h"
#include "Widgets/SCompoundWidget.h"

struct FSlateDemoData
{
	float Power = 50.0f;
	int32 Level = 1;
	FString Type = TEXT("Sword");
	bool bEnabled = true;
};

class SSlateDemoPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSlateDemoPanel) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FSlateDemoData Data;

	TArray<TSharedPtr<FString>> ComboItems;
	TSharedPtr<FString> SelectedItem;

	void OnPowerChanged(float NewValue);
	void OnLevelChanged(int32 NewValue);
	void OnComboChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	void OnEnabledChanged(ECheckBoxState NewState);
	FText GetSelectedComboText() const;
	ECheckBoxState GetEnabledCheckState() const;
	FText GetSummaryText() const;
	FReply OnApplyClicked();
};
