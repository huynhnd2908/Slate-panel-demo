#include "SlateDemoLogic.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void FSlateDemoLogic::BuildDefaultTypes(TArray<TSharedPtr<FString>>& OutTypes)
{
	OutTypes.Reset();
	OutTypes.Add(MakeShared<FString>(TEXT("Sword")));
	OutTypes.Add(MakeShared<FString>(TEXT("Bow")));
	OutTypes.Add(MakeShared<FString>(TEXT("Magic")));
	OutTypes.Add(MakeShared<FString>(TEXT("Shield")));
}

void FSlateDemoLogic::BuildDefaultPresets(TArray<TSharedPtr<FSlateDemoPreset>>& OutPresets)
{
	OutPresets.Reset();
	OutPresets.Add(MakeShared<FSlateDemoPreset>(FSlateDemoPreset{ TEXT("Custom"), FSlateDemoData(), TEXT("Manual values"), false }));
	OutPresets.Add(MakeShared<FSlateDemoPreset>(FSlateDemoPreset{ TEXT("Warrior Burst"), { 82.0f, 6, TEXT("Sword"), true }, TEXT("Balanced melee burst"), false }));
	OutPresets.Add(MakeShared<FSlateDemoPreset>(FSlateDemoPreset{ TEXT("Archer Sustain"), { 64.0f, 5, TEXT("Bow"), true }, TEXT("Lower cost ranged profile"), false }));
	OutPresets.Add(MakeShared<FSlateDemoPreset>(FSlateDemoPreset{ TEXT("Mage Nuke"), { 96.0f, 8, TEXT("Magic"), true }, TEXT("High score, high resource cost"), false }));
	OutPresets.Add(MakeShared<FSlateDemoPreset>(FSlateDemoPreset{ TEXT("Guardian Control"), { 48.0f, 10, TEXT("Shield"), true }, TEXT("Defensive high-level profile"), false }));
}

bool FSlateDemoLogic::AreEqual(const FSlateDemoData& A, const FSlateDemoData& B)
{
	return FMath::IsNearlyEqual(A.Power, B.Power)
		&& A.Level == B.Level
		&& A.Type == B.Type
		&& A.bEnabled == B.bEnabled;
}

FSlateDemoDerivedStats FSlateDemoLogic::CalculateStats(const FSlateDemoData& Data)
{
	float TypeMultiplier = 1.0f;
	float CostMultiplier = 1.0f;

	if (Data.Type == TEXT("Sword"))
	{
		TypeMultiplier = 1.12f;
		CostMultiplier = 0.95f;
	}
	else if (Data.Type == TEXT("Bow"))
	{
		TypeMultiplier = 1.04f;
		CostMultiplier = 0.82f;
	}
	else if (Data.Type == TEXT("Magic"))
	{
		TypeMultiplier = 1.28f;
		CostMultiplier = 1.35f;
	}
	else if (Data.Type == TEXT("Shield"))
	{
		TypeMultiplier = 0.9f;
		CostMultiplier = 0.72f;
	}

	const float EnabledMultiplier = Data.bEnabled ? 1.0f : 0.0f;

	FSlateDemoDerivedStats Stats;
	Stats.Damage = Data.Power * (0.7f + Data.Level * 0.22f) * TypeMultiplier * EnabledMultiplier;
	Stats.Cooldown = FMath::Clamp(3.2f - Data.Level * 0.13f + Data.Power * 0.006f, 0.8f, 4.8f);
	Stats.ResourceCost = FMath::Max(1.0f, (Data.Power * 0.36f + Data.Level * 4.0f) * CostMultiplier);
	Stats.Efficiency = Stats.ResourceCost > 0.0f ? Stats.Damage / Stats.ResourceCost : 0.0f;
	Stats.Score = Stats.Damage * Stats.Efficiency / FMath::Max(Stats.Cooldown, 0.1f);
	return Stats;
}

void FSlateDemoLogic::BuildValidationIssues(
	const FSlateDemoData& Data,
	const FSlateDemoDerivedStats& Stats,
	TArray<TSharedPtr<FSlateDemoValidationIssue>>& OutIssues)
{
	OutIssues.Reset();

	if (!Data.bEnabled)
	{
		OutIssues.Add(MakeShared<FSlateDemoValidationIssue>(FSlateDemoValidationIssue{
			ESlateDemoIssueSeverity::Info,
			TEXT("Disabled build"),
			TEXT("Apply is allowed, but score and derived damage are intentionally zero.")
		}));
		return;
	}

	if (Data.Power < 5.0f)
	{
		OutIssues.Add(MakeShared<FSlateDemoValidationIssue>(FSlateDemoValidationIssue{
			ESlateDemoIssueSeverity::Error,
			TEXT("Power too low"),
			TEXT("Power must be at least 5 to produce a meaningful snapshot.")
		}));
	}

	if (Data.Power >= 90.0f && Data.Level <= 2)
	{
		OutIssues.Add(MakeShared<FSlateDemoValidationIssue>(FSlateDemoValidationIssue{
			ESlateDemoIssueSeverity::Warning,
			TEXT("Unstable burst"),
			TEXT("High Power on Level 1-2 can produce extreme scaling.")
		}));
	}

	if (Data.Type == TEXT("Magic") && Data.Power < 40.0f)
	{
		OutIssues.Add(MakeShared<FSlateDemoValidationIssue>(FSlateDemoValidationIssue{
			ESlateDemoIssueSeverity::Warning,
			TEXT("Underpowered Magic"),
			TEXT("Magic presets are tuned around Power 40 or higher.")
		}));
	}

	if (Data.Type == TEXT("Shield") && Data.Level < 3)
	{
		OutIssues.Add(MakeShared<FSlateDemoValidationIssue>(FSlateDemoValidationIssue{
			ESlateDemoIssueSeverity::Warning,
			TEXT("Low defensive level"),
			TEXT("Shield builds usually expect Level 3+.")
		}));
	}

	if (Stats.Efficiency < 1.5f && Data.Power > 20.0f)
	{
		OutIssues.Add(MakeShared<FSlateDemoValidationIssue>(FSlateDemoValidationIssue{
			ESlateDemoIssueSeverity::Info,
			TEXT("Low efficiency"),
			TEXT("The build is valid, but cost is high for the generated damage.")
		}));
	}

	if (OutIssues.Num() == 0)
	{
		OutIssues.Add(MakeShared<FSlateDemoValidationIssue>(FSlateDemoValidationIssue{
			ESlateDemoIssueSeverity::Info,
			TEXT("Ready"),
			TEXT("No blocking validation issues.")
		}));
	}
}

bool FSlateDemoLogic::HasBlockingErrors(const TArray<TSharedPtr<FSlateDemoValidationIssue>>& Issues)
{
	for (const TSharedPtr<FSlateDemoValidationIssue>& Issue : Issues)
	{
		if (Issue.IsValid() && Issue->Severity == ESlateDemoIssueSeverity::Error)
		{
			return true;
		}
	}

	return false;
}

FString FSlateDemoLogic::GetWorstStatus(const TArray<TSharedPtr<FSlateDemoValidationIssue>>& Issues)
{
	if (HasBlockingErrors(Issues))
	{
		return TEXT("Error");
	}

	for (const TSharedPtr<FSlateDemoValidationIssue>& Issue : Issues)
	{
		if (Issue.IsValid() && Issue->Severity == ESlateDemoIssueSeverity::Warning)
		{
			return TEXT("Warning");
		}
	}

	return TEXT("Ready");
}

FString FSlateDemoLogic::GetSeverityText(ESlateDemoIssueSeverity Severity)
{
	switch (Severity)
	{
	case ESlateDemoIssueSeverity::Error:
		return TEXT("ERROR");
	case ESlateDemoIssueSeverity::Warning:
		return TEXT("WARN");
	default:
		return TEXT("INFO");
	}
}

FSlateColor FSlateDemoLogic::GetSeverityColor(ESlateDemoIssueSeverity Severity)
{
	switch (Severity)
	{
	case ESlateDemoIssueSeverity::Error:
		return FSlateColor(FLinearColor(1.0f, 0.32f, 0.28f, 1.0f));
	case ESlateDemoIssueSeverity::Warning:
		return FSlateColor(FLinearColor(1.0f, 0.72f, 0.2f, 1.0f));
	default:
		return FSlateColor(FLinearColor(0.62f, 0.62f, 0.62f, 1.0f));
	}
}

FString FSlateDemoLogic::ExportDataToJson(
	const FSlateDemoData& Data,
	const FSlateDemoDerivedStats& Stats,
	const TArray<TSharedPtr<FSlateDemoValidationIssue>>& Issues,
	int32 Revision)
{
	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetNumberField(TEXT("revision"), Revision);

	TSharedPtr<FJsonObject> DataObject = MakeShared<FJsonObject>();
	DataObject->SetNumberField(TEXT("power"), Data.Power);
	DataObject->SetNumberField(TEXT("level"), Data.Level);
	DataObject->SetStringField(TEXT("type"), Data.Type);
	DataObject->SetBoolField(TEXT("enabled"), Data.bEnabled);
	Root->SetObjectField(TEXT("data"), DataObject);

	TSharedPtr<FJsonObject> StatsObject = MakeShared<FJsonObject>();
	StatsObject->SetNumberField(TEXT("score"), Stats.Score);
	StatsObject->SetNumberField(TEXT("damage"), Stats.Damage);
	StatsObject->SetNumberField(TEXT("cooldown"), Stats.Cooldown);
	StatsObject->SetNumberField(TEXT("resourceCost"), Stats.ResourceCost);
	StatsObject->SetNumberField(TEXT("efficiency"), Stats.Efficiency);
	Root->SetObjectField(TEXT("stats"), StatsObject);

	TArray<TSharedPtr<FJsonValue>> IssueValues;
	for (const TSharedPtr<FSlateDemoValidationIssue>& Issue : Issues)
	{
		if (!Issue.IsValid())
		{
			continue;
		}

		TSharedPtr<FJsonObject> IssueObject = MakeShared<FJsonObject>();
		IssueObject->SetStringField(TEXT("severity"), GetSeverityText(Issue->Severity));
		IssueObject->SetStringField(TEXT("title"), Issue->Title);
		IssueObject->SetStringField(TEXT("details"), Issue->Details);
		IssueValues.Add(MakeShared<FJsonValueObject>(IssueObject));
	}
	Root->SetArrayField(TEXT("validation"), IssueValues);

	FString Json;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);
	return Json;
}
