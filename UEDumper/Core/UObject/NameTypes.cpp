#include "pch.h"
#include "NameTypes.h"
#include "../../utils.h"

FString FName::ToString() const {
	return Utils::UKismetStringLibrary::Conv_NameToString((FName*)this);
}