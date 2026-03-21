// © 2026 RadZib. All rights reserved.

#include "FrameworkBase/EotRGameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "FrameworkBase/EotRLogChannels.h"

namespace EotRGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Heal, "SetByCaller.Heal", "SetByCaller tag used by healing gameplay effects.");

	// Abilities
	UE_DEFINE_GAMEPLAY_TAG(Ability_Camera_Look, "Ability.Camera.Look");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Move, "Ability.Movement.Move");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Traversal, "Ability.Movement.Traversal");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Run, "Ability.Movement.Run");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Sprint, "Ability.Movement.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Crouch, "Ability.Movement.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interaction_Interact, "Ability.Interaction.Interact");

	// States
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Traversing, "State.Movement.Traversing");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Running, "State.Movement.Running");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Run_Disabled, "State.Movement.Run.Disabled");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Run_Enabled, "State.Movement.Run.Enabled");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Sprinting, "State.Movement.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Crouching, "State.Movement.Crouching");

	// Sounds
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Jump, "Foley.Event.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Land, "Foley.Event.Land");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Handplant, "Foley.Event.Handplant");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Run, "Foley.Event.Run");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_RunBackwds, "Foley.Event.RunBackwds");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_RunStrafe, "Foley.Event.RunStrafe");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Scuff, "Foley.Event.Scuff");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_ScuffPivot, "Foley.Event.ScuffPivot");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_ScuffWall, "Foley.Event.ScuffWall");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Tumble, "Foley.Event.Tumble");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_Walk, "Foley.Event.Walk");
	UE_DEFINE_GAMEPLAY_TAG(Foley_Event_WalkBackwds, "Foley.Event.WalkBackwds");

	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogEotR, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}