#include "DAValidator.h"

#include "GameplayTagContainer.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "UObject/SoftObjectPtr.h"
#include "Misc/StringBuilder.h"

namespace
{
	struct FPathSegment
	{
		enum class EKind : uint8 { Name, ArrayIndex } Kind = EKind::Name;

		FName Name = NAME_None;
		int32 Index = INDEX_NONE;
	};

	class FPathStack
	{
	public:
		void PushName(FName InName)
		{
			FPathSegment& S = Segments.AddDefaulted_GetRef();
			S.Kind = FPathSegment::EKind::Name;
			S.Name = InName;
		}

		void PushIndex(int32 InIndex)
		{
			FPathSegment& S = Segments.AddDefaulted_GetRef();
			S.Kind = FPathSegment::EKind::ArrayIndex;
			S.Index = InIndex;
		}

		void Pop()
		{
			Segments.Pop(EAllowShrinking::No);
		}

		FString ToString() const
		{
			TStringBuilder<256> B;
			bool bNeedDot = false;

			for (const FPathSegment& S : Segments)
			{
				if (S.Kind == FPathSegment::EKind::Name)
				{
					if (bNeedDot) { B.AppendChar(TEXT('.')); }
					B.Append(S.Name.ToString());
					bNeedDot = true;
				}
				else
				{
					B.AppendChar(TEXT('['));
					B.Appendf(TEXT("%d"), S.Index);
					B.AppendChar(TEXT(']'));
				}
			}

			return FString(B);
		}

	private:
		TArray<FPathSegment, TInlineAllocator<32>> Segments;
	};
}

namespace DAValidator::Private
{
	static bool ShouldTraverseProperty(const FProperty* P)
	{
		if (!P)
		{
			return false;
		}

		const EPropertyFlags Flags = P->GetPropertyFlags();

		if (EnumHasAnyFlags(Flags, CPF_Transient | CPF_DuplicateTransient | CPF_NonPIEDuplicateTransient | CPF_SkipSerialization))
		{
			return false;
		}

		if (P->HasMetaData(TEXT("NoDAValidation")))
		{
			return false;
		}

		if (!EnumHasAnyFlags(Flags, CPF_Edit)) 
		{ 
			return false; 
		}

		return true;
	}

	static DAValidator::EIssueSeverity GetSeverity(const FProperty* P, bool bHardFailure)
	{
		if (P && P->HasMetaData(TEXT("DARequired")))
		{
			return DAValidator::EIssueSeverity::Error;
		}

		if (bHardFailure)
		{
			return DAValidator::EIssueSeverity::Error;
		}

		return DAValidator::EIssueSeverity::Warning;
	}

	static void AddIssue(
		const FProperty* P,
		const FPathStack& Path,
		const TCHAR* Msg,
		TArray<DAValidator::FIssue>& Out,
		bool bHardFailure = false)
	{
		DAValidator::FIssue Issue;
		Issue.Severity = GetSeverity(P, bHardFailure);
		Issue.Path = Path.ToString();
		Issue.Message = Msg;
		Out.Add(MoveTemp(Issue));
	}

	static void ValidateValueRecursive(
		const FProperty* Property,
		const void* ValuePtr,
		FPathStack& Path,
		TArray<DAValidator::FIssue>& OutIssues,
		int32 Depth);

	static void ValidateStruct(
		const FStructProperty* StructProp,
		const void* StructValuePtr,
		FPathStack& Path,
		TArray<DAValidator::FIssue>& OutIssues,
		int32 Depth)
	{
		if (!StructProp || !StructValuePtr)
		{
			return;
		}

		const UStruct* Struct = StructProp->Struct;
		if (!Struct)
		{
			return;
		}

		if (Struct == FGameplayTagContainer::StaticStruct())
		{
			const FGameplayTagContainer* Tags = static_cast<const FGameplayTagContainer*>(StructValuePtr);
			if (!Tags || Tags->IsEmpty())
			{
				AddIssue(StructProp, Path, TEXT("GameplayTagContainer is empty (None)."), OutIssues);
			}
			return;
		}

		if (Struct == FGameplayTag::StaticStruct())
		{
			const FGameplayTag* Tag = static_cast<const FGameplayTag*>(StructValuePtr);
			if (!Tag || !Tag->IsValid())
			{
				AddIssue(StructProp, Path, TEXT("GameplayTag is None/invalid."), OutIssues);
			}
			return;
		}

		for (TFieldIterator<FProperty> It(Struct); It; ++It)
		{
			const FProperty* Child = *It;
			if (!ShouldTraverseProperty(Child))
			{
				continue;
			}

			const void* ChildPtr = Child->ContainerPtrToValuePtr<void>(StructValuePtr);
			Path.PushName(Child->GetFName());
			ValidateValueRecursive(Child, ChildPtr, Path, OutIssues, Depth + 1);
			Path.Pop();
		}
	}

	static void ValidateArray(
		const FArrayProperty* ArrayProp,
		const void* ValuePtr,
		FPathStack& Path,
		TArray<DAValidator::FIssue>& OutIssues,
		int32 Depth)
	{
		if (!ArrayProp || !ValuePtr || !ArrayProp->Inner)
		{
			return;
		}

		FScriptArrayHelper Helper(ArrayProp, ValuePtr);
		const int32 Num = Helper.Num();

		for (int32 i = 0; i < Num; ++i)
		{
			const void* ElemPtr = Helper.GetRawPtr(i);
			Path.PushIndex(i);
			ValidateValueRecursive(ArrayProp->Inner, ElemPtr, Path, OutIssues, Depth + 1);
			Path.Pop();
		}
	}

	static void ValidateSet(
		const FSetProperty* SetProp,
		const void* ValuePtr,
		FPathStack& Path,
		TArray<DAValidator::FIssue>& OutIssues,
		int32 Depth)
	{
		if (!SetProp || !ValuePtr || !SetProp->ElementProp)
		{
			return;
		}

		FScriptSetHelper Helper(SetProp, ValuePtr);

		int32 LogicalIndex = 0;
		for (int32 i = 0; i < Helper.GetMaxIndex(); ++i)
		{
			if (!Helper.IsValidIndex(i))
			{
				continue;
			}

			const void* ElemPtr = Helper.GetElementPtr(i);
			Path.PushIndex(LogicalIndex);
			ValidateValueRecursive(SetProp->ElementProp, ElemPtr, Path, OutIssues, Depth + 1);
			Path.Pop();

			++LogicalIndex;
		}
	}

	static void ValidateMap(
		const FMapProperty* MapProp,
		const void* ValuePtr,
		FPathStack& Path,
		TArray<DAValidator::FIssue>& OutIssues,
		int32 Depth)
	{
		if (!MapProp || !ValuePtr || !MapProp->KeyProp || !MapProp->ValueProp)
		{
			return;
		}

		FScriptMapHelper Helper(MapProp, ValuePtr);

		int32 LogicalIndex = 0;
		for (int32 i = 0; i < Helper.GetMaxIndex(); ++i)
		{
			if (!Helper.IsValidIndex(i))
			{
				continue;
			}

			const void* KeyPtr = Helper.GetKeyPtr(i);
			const void* ValPtr = Helper.GetValuePtr(i);

			Path.PushIndex(LogicalIndex);

			Path.PushName(TEXT("Key"));
			ValidateValueRecursive(MapProp->KeyProp, KeyPtr, Path, OutIssues, Depth + 1);
			Path.Pop();

			Path.PushName(TEXT("Value"));
			ValidateValueRecursive(MapProp->ValueProp, ValPtr, Path, OutIssues, Depth + 1);
			Path.Pop();

			Path.Pop();
			++LogicalIndex;
		}
	}

	static void ValidateValueRecursive(
		const FProperty* Property,
		const void* ValuePtr,
		FPathStack& Path,
		TArray<DAValidator::FIssue>& OutIssues,
		int32 Depth)
	{
		if (!Property || !ValuePtr || Depth > 64)
		{
			return;
		}

		if (const FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(Property))
		{
			UObject* Obj = ObjProp->GetObjectPropertyValue(ValuePtr);
			if (Obj == nullptr)
			{
				AddIssue(Property, Path, TEXT("Object reference is null."), OutIssues, true);
			}
			return;
		}

		if (const FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Property))
		{
			const FSoftObjectPtr* SoftPtr = SoftObjProp->GetPropertyValuePtr(ValuePtr);
			if (!SoftPtr || SoftPtr->IsNull())
			{
				AddIssue(Property, Path, TEXT("Soft object reference is empty."), OutIssues);
			}
			return;
		}

		if (const FClassProperty* ClassProp = CastField<FClassProperty>(Property))
		{
			UObject* AsObj = ClassProp->GetObjectPropertyValue(ValuePtr);
			UClass* Cls = Cast<UClass>(AsObj);
			if (Cls == nullptr)
			{
				AddIssue(Property, Path, TEXT("Class reference is null."), OutIssues);
			}
			return;
		}

		if (const FSoftClassProperty* SoftClassProp = CastField<FSoftClassProperty>(Property))
		{
			const FSoftObjectPtr* SoftPtr = SoftClassProp->GetPropertyValuePtr(ValuePtr);
			if (!SoftPtr || SoftPtr->IsNull())
			{
				AddIssue(Property, Path, TEXT("Soft class reference is empty."), OutIssues);
			}
			return;
		}

		if (const FStructProperty* StructProp = CastField<FStructProperty>(Property))
		{
			ValidateStruct(StructProp, ValuePtr, Path, OutIssues, Depth);
			return;
		}

		if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property))
		{
			ValidateArray(ArrayProp, ValuePtr, Path, OutIssues, Depth);
			return;
		}

		if (const FSetProperty* SetProp = CastField<FSetProperty>(Property))
		{
			ValidateSet(SetProp, ValuePtr, Path, OutIssues, Depth);
			return;
		}

		if (const FMapProperty* MapProp = CastField<FMapProperty>(Property))
		{
			ValidateMap(MapProp, ValuePtr, Path, OutIssues, Depth);
			return;
		}
	}

	static void ValidateObjectInternal(UObject* Object, TArray<DAValidator::FIssue>& OutIssues)
	{
		if (!Object)
		{
			return;
		}

		UClass* Class = Object->GetClass();
		if (!Class)
		{
			return;
		}

		FPathStack Path;

		for (TFieldIterator<FProperty> It(Class); It; ++It)
		{
			const FProperty* P = *It;
			if (!ShouldTraverseProperty(P))
			{
				continue;
			}

			const void* ValuePtr = P->ContainerPtrToValuePtr<void>(Object);
			Path.PushName(P->GetFName());
			ValidateValueRecursive(P, ValuePtr, Path, OutIssues, 0);
			Path.Pop();
		}
	}
}

void DAValidator::ValidateObject(UObject* Object, TArray<FIssue>& OutIssues)
{
	OutIssues.Reset();
	DAValidator::Private::ValidateObjectInternal(Object, OutIssues);
}