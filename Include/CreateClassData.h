
#ifndef _CREATECLASSDATA_H_
#define _CREATECLASSDATA_H_

#include "GENERAL.H"
#include "PICS.H"
#include "EQUIP.H"
#include "SPELTYPE.H"

#define MAXCLASSTITLELENGTH 64
#define MAXCLASSDESCRLENGTH 384
#define MAXLEVELTITLELENGTH 128
#define MAXITEMNAMELENGTH	64

#define NUM_CLASSES 15
#define NUM_ATTRIBUTES 6 //Str, Spd, Mgc, Acc, Stl, Con
#define NUM_TITLES_PER_CLASS 20
#define NUM_ARMOR_TYPES 3

typedef enum
{
    CLASS_CITIZEN,
    CLASS_KNIGHT,
    CLASS_MAGE,
    CLASS_WARLOCK,
    CLASS_PRIEST,
    CLASS_ROGUE,
    CLASS_ARCHER,
    CLASS_SAILOR,
    CLASS_PALADIN,
    CLASS_MERCENARY,
    CLASS_MAGICIAN,
	CLASS_BARBARIAN,
	CLASS_MONK,
	CLASS_NINJA,
	CLASS_NECRO,
    CLASS_UNKNOWN
} E_statsClassType;

typedef enum
{
	ARMOR_TYPE_LEATHER,
	ARMOR_TYPE_CHAIN,
	ARMOR_TYPE_PLATE
} E_ArmorTypes;

typedef struct {
	char name[MAXITEMNAMELENGTH];
	T_word32 num;
}ItemListEntry;
ItemListEntry ItemList[128];
int ItemListCount;

typedef struct {
	T_word32 num;
	T_byte8 count;
} StartingItemData;

typedef struct {
	T_byte8 Title[MAXCLASSTITLELENGTH];
	T_byte8 Descr[MAXCLASSDESCRLENGTH];
	T_byte8 ClassNum;
	T_word16 CanUseFlag;
	float RegenHealthModifier;
	float RegenManaModifier;
	float JumpModifier;
	float DamageModifier;
	float MoveModifier;
	float ThiefModifier;
	T_byte8 BasePunchDamage;
	T_byte8 SpellSystem;
	T_byte8 Attributes[NUM_ATTRIBUTES];
	T_byte8 Advancement[NUM_ATTRIBUTES];
	T_byte8 LevelTitles[NUM_TITLES_PER_CLASS][MAXLEVELTITLELENGTH];
	T_bitmap *Picture;
	StartingItemData *StartingItems;
	int	StartingItemsCount;
	E_Boolean CanUseWeapon[EQUIP_WEAPON_TYPE_UNKNOWN];
	E_Boolean CanUseArmor[NUM_ARMOR_TYPES];
}CreateClassData;

CreateClassData CreateClassDatas[NUM_CLASSES];


T_void InitCreateClassData (T_byte8 classnum);
T_void LoadCreateClassDatas(T_void);
T_void DestroyCreateClassDatas(T_void);


#endif
