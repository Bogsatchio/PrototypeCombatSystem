#pragma once


// Makro that makes an enum UnrealEngine Specific and editable
UENUM(BlueprintType)
// In Unreal Engine we use scoped enums (adding "class" keyword) It means that when we use enum constant we have to fully qualify it with type name
enum class ECharacterState : uint8		//specyfying type for ennumerations for enum constants
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"), // Makro that allows to change name of the enum constants for Blueprints
	ECS_NoShield UMETA(DisplayName = "No Shield"),
	ECS_EquippedShield UMETA(DisplayName = "Equipped Shield"),
	//ESC_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	//ESC_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};


UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Shooting UMETA(DisplayName = "Shooting"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Dead UMETA(DisplayName = "Dead"),
	EAS_Blocking UMETA(DisplayName = "Block"),
	EAS_Parrying UMETA(DisplayName = "Parry"),
	EAS_UsingConsumable UMETA(DisplayName = "Using Consumable"),
	EAS_Staggered UMETA(DisplayName = "Staggered")
};


UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),

	EDP_MAX UMETA(DisplayName = "DefaultMAX")
	
};


UENUM(BlueprintType)
enum class EEnemyState : uint8		//specyfying type for ennumerations for enum constants
{
	EES_NoState UMETA(DisplayName = "NoState"),

	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"), 
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),
	EES_Dodge UMETA(DisplayName = "Dodge"),
	EES_Block UMETA(DisplayName = "Block"),
	EES_HitReaction UMETA(DisplayName = "HitReaction")
};


UENUM(BlueprintType)
enum class EAttackType : uint8
{
	EAT_None UMETA(DisplayName = "None"),
	EAT_NormalAttack UMETA(DisplayName = "Normal Attack"),
	EAT_HeavyAttack UMETA(DisplayName = "Heavy Attack")
};



