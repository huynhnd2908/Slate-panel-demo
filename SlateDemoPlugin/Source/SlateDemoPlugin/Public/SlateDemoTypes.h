#pragma once

#include "CoreMinimal.h"

enum class ESlateDemoIssueSeverity : uint8
{
	Info,
	Warning,
	Error
};

struct FSlateDemoData
{
	float Power = 50.0f;
	int32 Level = 1;
	FString Type = TEXT("Sword");
	bool bEnabled = true;
};

struct FSlateDemoPreset
{
	FString Name;
	FSlateDemoData Data;
	FString Description;
	bool bRuntimePreset = false;
};

struct FSlateDemoDerivedStats
{
	float Score = 0.0f;
	float Damage = 0.0f;
	float Cooldown = 0.0f;
	float ResourceCost = 0.0f;
	float Efficiency = 0.0f;
};

struct FSlateDemoValidationIssue
{
	ESlateDemoIssueSeverity Severity = ESlateDemoIssueSeverity::Info;
	FString Title;
	FString Details;
};

struct FSlateDemoHistoryEntry
{
	int32 Id = 0;
	int32 Revision = 0;
	FDateTime AppliedAt;
	FSlateDemoData Snapshot;
	FSlateDemoDerivedStats Stats;
	FString Status;
};
