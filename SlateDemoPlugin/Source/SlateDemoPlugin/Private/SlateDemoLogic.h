#pragma once

#include "SlateDemoTypes.h"
#include "Styling/SlateColor.h"

class FSlateDemoLogic
{
public:
	static void BuildDefaultTypes(TArray<TSharedPtr<FString>>& OutTypes);
	static void BuildDefaultPresets(TArray<TSharedPtr<FSlateDemoPreset>>& OutPresets);

	static bool AreEqual(const FSlateDemoData& A, const FSlateDemoData& B);
	static FSlateDemoDerivedStats CalculateStats(const FSlateDemoData& Data);
	static void BuildValidationIssues(
		const FSlateDemoData& Data,
		const FSlateDemoDerivedStats& Stats,
		TArray<TSharedPtr<FSlateDemoValidationIssue>>& OutIssues);

	static bool HasBlockingErrors(const TArray<TSharedPtr<FSlateDemoValidationIssue>>& Issues);
	static FString GetWorstStatus(const TArray<TSharedPtr<FSlateDemoValidationIssue>>& Issues);
	static FString GetSeverityText(ESlateDemoIssueSeverity Severity);
	static FSlateColor GetSeverityColor(ESlateDemoIssueSeverity Severity);

	static FString ExportDataToJson(
		const FSlateDemoData& Data,
		const FSlateDemoDerivedStats& Stats,
		const TArray<TSharedPtr<FSlateDemoValidationIssue>>& Issues,
		int32 Revision);
};
