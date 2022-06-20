#include "pch.h"
#include "formatting.h"

std::string Formatting::GetUPropertySpecifiers(EPropertyFlags Flags) {
	std::string result = "UPROPERTY(";

	// result += (Flags & EPropertyFlags::CPF_Edit) ? "EditAnywhere, " : ""; // TODO: Format like this
	if (Flags & EPropertyFlags::CPF_Edit) {
		result += "EditAnywhere, ";
	}

	if (Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_DisableEditOnTemplate) {
		result += "EditInstanceOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_DisableEditOnInstance) != 0)
	{
		result += "EditDefaultsOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_EditConst) != 0)
	{
		result += "VisibleAnywhere, ";
	}

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_EditConst & EPropertyFlags::CPF_DisableEditOnTemplate) != 0)
	{
		result += "VisibleInstanceOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_EditConst & EPropertyFlags::CPF_DisableEditOnInstance) != 0)
	{
		result += "VisibleDefaultsOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_BlueprintVisible) != 0)
	{
		if ((Flags & EPropertyFlags::CPF_BlueprintReadOnly) != 0)
		{
			result += "BlueprintReadOnly, ";
		}
		else {
			result += "BlueprintReadWrite, ";
		}
	}

	if ((Flags & EPropertyFlags::CPF_Config) != 0)
	{
		if ((Flags & EPropertyFlags::CPF_GlobalConfig) != 0)
		{
			result += "GlobalConfig, ";
		}
		else
		{
			result += "Config, ";
		}
	}

	if ((Flags & EPropertyFlags::CPF_Transient) != 0)
	{
		result += "Transient, ";
	}

	if ((Flags & EPropertyFlags::CPF_DuplicateTransient) != 0)
	{
		result += "DuplicateTransient, ";
	}

	if ((Flags & EPropertyFlags::CPF_TextExportTransient) != 0)
	{
		result += "TextExportTransient, ";
	}

	if ((Flags & EPropertyFlags::CPF_NonPIEDuplicateTransient) != 0)
	{
		result += "NonPIETransient, ";
	}

	if ((Flags & EPropertyFlags::CPF_NonPIEDuplicateTransient) != 0)
	{
		result += "NonPIEDuplicateTransient, ";
	}

	if ((Flags & EPropertyFlags::CPF_ExportObject) != 0)
	{
		result += "Export, ";
	}

	//  EditInline -> deprecated

	if ((Flags & EPropertyFlags::CPF_NoClear) != 0)
	{
		result += "NoClear, ";
	}

	if ((Flags & EPropertyFlags::CPF_EditFixedSize) != 0)
	{
		result += "EditFixedSize, ";
	}

	if ((Flags & EPropertyFlags::CPF_Net) != 0)
	{
		result += "Replicated, ";

		if ((Flags & EPropertyFlags::CPF_RepNotify) != 0)
		{
			result += "ReplicatedUsing, ";
		}
	}

	// RepRetry -> deprecated

	if ((Flags & EPropertyFlags::CPF_Edit & EPropertyFlags::CPF_BlueprintVisible & EPropertyFlags::CPF_Interp) != 0)
	{
		result += "Interp, ";
	}

	if ((Flags & EPropertyFlags::CPF_NonTransactional) != 0)
	{
		result += "NonTransactional, ";
	}

	if ((Flags & EPropertyFlags::CPF_PersistentInstance & EPropertyFlags::CPF_ExportObject & EPropertyFlags::CPF_InstancedReference) != 0)
	{
		result += "Instanced, ";
	}

	if ((Flags & EPropertyFlags::CPF_BlueprintAssignable) != 0)
	{
		result += "BlueprintAssignable, ";
	}

	if ((Flags & EPropertyFlags::CPF_BlueprintCallable) != 0)
	{
		result += "BlueprintCallable, ";
	}

	if ((Flags & EPropertyFlags::CPF_BlueprintAuthorityOnly) != 0)
	{
		result += "BlueprintAuthorityOnly, ";
	}

	if ((Flags & EPropertyFlags::CPF_AssetRegistrySearchable) != 0)
	{
		result += "AssetRegistrySearchable, ";
	}

	if ((Flags & EPropertyFlags::CPF_SimpleDisplay) != 0)
	{
		result += "SimpleDisplay, ";
	}

	if ((Flags & EPropertyFlags::CPF_AdvancedDisplay) != 0)
	{
		result += "AdvancedDisplay, ";
	}

	if ((Flags & EPropertyFlags::CPF_SaveGame) != 0)
	{
		result += "SaveGame, ";
	}

	if ((Flags & EPropertyFlags::CPF_SkipSerialization) != 0)
	{
		result += "SkipSerialization, ";
	}

	// if (VariableCategory != EVariableCategory::Member)

	if ((Flags & EPropertyFlags::CPF_ConstParm) != 0)
	{
		result += "Const, ";
	}

	if ((Flags & EPropertyFlags::CPF_OutParm & EPropertyFlags::CPF_ReferenceParm) != 0)
	{
		result += "Ref, ";
	}

	if ((Flags & EPropertyFlags::CPF_RepSkip) != 0)
	{
		result += "NotReplicated, ";
		result += "ReplicatedParameter, ";
	}

	auto idx = result.find_last_of(", ");

	if (idx != std::string::npos)
		result = result.erase(idx - 1, idx);

	return result + ");";
}