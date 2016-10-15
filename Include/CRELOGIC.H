/****************************************************************************/
/*    FILE:  CRELOGIC.H                                                      */
/****************************************************************************/
#ifndef _CRELOGIC_H_
#define _CRELOGIC_H_

#include "3D_VIEW.H"
#include "GENERAL.H"
#include "OBJGEN.H"
#include "3D_COLLI.H"
#include "AREASND.H"
#include "EFFECT.H"
#include "EQUIP.H"

typedef T_void *T_creaturesHandle ;
#define CREATURES_HANDLE_BAD NULL
#define NUM_JUNK_ITEMS 40

#define HEALTH_DECAY_RATE_TICKS	360

T_void CreaturesInitialize(T_void) ;

T_void CreaturesFinish(T_void) ;

T_void CreatureTakeDamage(
           T_3dObject *p_obj,
           T_word32 damage,
           T_word16 type,
           T_word16 ownerID,
		   T_sword16 damageObjectType,
		   T_byte8 weaponDamage);

#ifdef SERVER_ONLY
T_3dObject *CreatureLookForPlayer(T_3dObject *p_obj) ;
T_void CreaturePlayerGone(T_player player) ;
#else
T_void CreaturePlayerGone(T_word16 playerId) ;
#endif

T_void CreaturesUpdate(T_void) ;

T_void CreatureAttachToObject(T_3dObject *p_obj) ;

T_void CreatureDetachFromObject(T_3dObject *p_obj) ;

E_Boolean CreatureIsMissile(T_3dObject *p_obj) ;

T_void CreatureSetTarget(T_3dObject *p_obj, T_word16 targetID) ;

T_void CreaturesHearSoundOfPlayer(T_3dObject *p_player, T_word16 distance) ;

T_word32 CreaturesKillAll(T_void) ;

T_word16 CreatureStolenFrom(T_3dObject *p_obj) ;

T_void CreatureGoSplat(
           T_3dObject *p_obj,
           T_word16 amount,
           T_word16 damageType) ;

T_void CreaturesAllOnOneTarget(T_word16 targetId);

T_doubleLinkListElement ICreatureFindViaObjectPtr(T_3dObject *p_obj) ;
typedef enum {
    NAV_LOGIC_PKG_NONE,
    NAV_LOGIC_PKG_BERSERKER_A,
    NAV_LOGIC_PKG_BERSERKER_B,
    NAV_LOGIC_PKG_BARBARIAN_GUARD,
    NAV_LOGIC_PKG_BARBARIAN_ARCHER,
    NAV_LOGIC_PKG_NEUTRAL_WANDERER,
    NAV_LOGIC_PKG_SCAREDY_CAT,
    NAV_LOGIC_PKG_ROAMING_GUARD,
    NAV_LOGIC_PKG_COMBO_FIGHTER_ARCHER,
    NAV_LOGIC_PKG_BARBARIAN_ARCHER_B,
    NAV_LOGIC_PKG_DIPPER,
    NAV_LOGIC_PKG_TELEPORTER,
    NAV_LOGIC_PKG_HOMING,
    NAV_LOGIC_PKG_STRAIGHT_LINE,
    NAV_LOGIC_PKG_CLOUD,
    NAV_LOGIC_PKG_APPROACH_AND_SHOOT,
    NAV_LOGIC_PKG_UNKNOWN
} E_navigateLogicPackage ;

typedef enum {
    TARGET_LOGIC_PACKAGE_NONE,
    TARGET_LOGIC_PACKAGE_STANDARD_HIT_OR_SHOOT,
    TARGET_LOGIC_PACKAGE_SUICIDE_EXPLOSION,
    TARGET_LOGIC_PACKAGE_EXPLODE_ON_COLLISION,
    TARGET_LOGIC_PACKAGE_SCREAM,
    TARGET_LOGIC_PACKAGE_SUMMON_CREATURE,
    TARGET_LOGIC_PACKAGE_CONSTANT_DAMAGE,
    TARGET_LOGIC_PACKAGE_UNKNOWN
} E_targetLogicPackage ;

typedef enum {
	CREATURE_FACTION_NONE,
	CREATURE_FACTION_DRUID,
	CREATURE_FACTION_KNIGHTS_OF_ANDREW,
	CREATURE_FACTION_BARBARIAN,
	CREATURE_FACTION_UNDEAD,
	CREATURE_FACTION_EVIL_KNIGHTS,
	CREATURE_FACTION_DARK_KNIGHTS,
	CREATURE_FACTION_DRAGON,
	CREATURE_FACTION_UNKNOWN
} E_creatureFaction;

typedef enum {
	MONSTER_TYPE_NONE,
	MONSTER_TYPE_LOW_ARMOR,
	MONSTER_TYPE_PLATE_ARMOR,
	MONSTER_TYPE_UNDEAD,
	MONSTER_TYPE_LARGE,
	MONSTER_TYPE_UNKNOWN
} E_monsterType;

typedef T_void (*T_monsterUpdateCallback)(T_void *p_data) ;


typedef enum {
    TREASURE_TYPE_NONE,
    TREASURE_TYPE_1_PLAT,
    TREASURE_TYPE_1_GOLD,
    TREASURE_TYPE_1_SILVER,
    TREASURE_TYPE_5_COPPER,
    TREASURE_TYPE_SHADOW,
    TREASURE_TYPE_LICH,
    TREASURE_TYPE_EXIGUUS, /* Cemetary wizard */
    TREASURE_TYPE_BAD_KNIGHT,
    TREASURE_TYPE_MITHRIL_BAD_KNIGHT,
    TREASURE_TYPE_EVIL_ORANGE_WIZARD,
    TREASURE_TYPE_BLUE_WIZARD,
    TREASURE_TYPE_DRUID,
    TREASURE_TYPE_POISON_DRUID,
    TREASURE_TYPE_EVIL_KNIGHT,
    TREASURE_TYPE_SILVER_EVIL_KNIGHT,
    TREASURE_TYPE_ELK,
    TREASURE_TYPE_ELF_ARCHER,
    TREASURE_TYPE_VITORIX,
    TREASURE_TYPE_JUGURTHA,
    TREASURE_TYPE_EVIL_ARCHER,
    TREASURE_TYPE_ELYMAS,
    TREASURE_TYPE_MIGHTY_HUNTER,
    TREASURE_TYPE_MATTAN,
    TREASURE_TYPE_MAMMA_DRAGON,
    TREASURE_TYPE_KOA_FOOTMAN,
    TREASURE_TYPE_KOA_HORSEMAN,
    TREASURE_TYPE_BRONZE_EVIL_KNIGHT,
    TREASURE_TYPE_TROJAN,
    TREASURE_TYPE_UNKNOWN
} E_treasureType ;

typedef enum {
    DEATH_LOGIC_NONE,
    DEATH_LOGIC_SINK,
    DEATH_LOGIC_FAST_DISAPPEAR,
    DEATH_LOGIC_NORMAL_DECAY,
    DEATH_LOGIC_UNKNOWN
} E_deathLogic ;

typedef enum {
    SCANB_FACING_LEFT_90,
    SCANB_FACING_LEFT_45,
    SCANB_FACING_MIDDLE,
    SCANB_FACING_RIGHT_45,
    SCANB_FACING_RIGHT_90,
    SCANB_FACING_UNKNOWN
} E_scanBFacing ;

typedef enum {
    SCANB_DIR_TURN_LEFT,
    SCANB_DIR_TURN_RIGHT
} E_scanBDir ;

typedef enum {
    BLOCK_TURN_DIR_NONE,
    BLOCK_TURN_DIR_LEFT,
    BLOCK_TURN_DIR_RIGHT,
    BLOCK_TURN_DIR_UNKNOWN
} E_blockTurnDir ;

typedef struct {
    T_word32 objectType ;
    E_navigateLogicPackage navigateLogic ;
    E_targetLogicPackage targetLogic ;
    E_Boolean canFly ;
    E_Boolean ignoreWalls ;
    T_word32 acceleration ;
    T_word32 maxVelocity ;
    T_word32 updateTime ;
    T_word32 stepSize ;
    T_word32 maxMeleeRange ;
    T_word32 minMissileRange ;
    T_word32 maxMissileRange ;
    T_word32 meleeAttackDelay ;
    T_word32 missileAttackDelay ;
    T_word16 hitPoints ;
    T_word16 regenRate ;
    T_word16 missileType ;
    E_effectDamageType damageType ;
    T_word16 meleeDamage;
    T_monsterUpdateCallback callback ;
    E_effectDamageType damageResist ;
    T_word32 hurtHealth ;
    E_equipArmorTypes armorType ;
    E_treasureType treasureToDrop ;
    T_word32 visionField ;
    T_word32 wanderSound;
    T_word32 attackSound;
    T_word32 attackSound2;
    T_word32 hurtSound;
    T_word32 hurtSound2;
    T_word32 targetSound;
    T_word32 dieSound;
    T_word16 faceDelay ;
    E_Boolean explodeOnCollision ;
    E_Boolean canHurtSelf ;
    E_deathLogic deathLogic ;
    T_word16 decayObject ;
    T_word16 stayOnSectorType ;
    T_byte8 chanceAttackCreature ;
    T_byte8 chanceContinueAttack ;
    T_word16 efxAtDeath ;
    T_byte8 bloodEffect ;
    E_Boolean canOpenDoors ;
	//pwmod creature values
	T_byte8 FactionType;
	T_byte8 MonsterType;
} T_creatureLogic ;

typedef struct {
    E_Boolean targetAcquired ;
    T_word32 targetID ;
    T_sword16 targetX, targetY, targetZ ;
    T_word32 lastTargetID ;
    T_word32 targetDistance ;
    T_word16 targetAngle ; /* Angle is based on map axis, */
                           /* not creature facing */
    E_Boolean canSeeTarget ;
    T_word16 health ;
    T_creatureLogic *p_logic ;
    T_word16 objectID ;
    T_3dObject *p_obj ;
    T_word32 lastUpdateTime ;
    E_scanBFacing scanBFacing ;
    E_scanBDir scanBDir ;
    T_byte8 scanBCount ;
    E_Boolean moveBlocked ;
    E_blockTurnDir blockTurnDir ;
    T_byte8 blockCount ;
    T_byte8 faceDelayCount ;
    E_Boolean targettingActive ;
    T_word16 meleeDelayCount ;
    T_word16 missileDelayCount ;
    T_word16 poisonLevel ;
    E_Boolean markedForDestroy ;

    /* The following flag is set by IUpdateTarget when the target */
    /* for the creature is at a position in front of the creature. */
    E_Boolean canMeleeHitTarget ;

    /* Flag that tells if the creature is running away. */
    E_Boolean isFleeing ;

    /* Flag telling if the creature is fully dipped into the */
    /* ground / water. */
    E_Boolean isDipped ;

    /* Flag telling if the creature is fully out of the water / ground. */
    E_Boolean isUndipped ;

    /* Note when we died for decaying objects. */
    T_word32 timeOfDeath ;

    /* Flag to say this creature is immune to all damage. */
    E_Boolean immuneToDamage ;

    /* Teleport information (if a teleporter) */
    T_objectGeneratorPosition *p_teleportList ;
    T_word16 lengthTeleportList ;
    T_word16 teleportPosition ;
    T_word32 lastTeleportTime ;

    /* Creature summoning info. */
    T_word32 timeToSummon ;

    /* Creature is slowed? and how long. */
    E_Boolean isSlowed ;
    T_word32 timeSlowEnds ;

    /* Creature is earthbound? and how long. */
    E_Boolean isEarthbound ;
    T_word32 timeEarthboundEnds ;

    /* Last time homing worked. */
    T_word32 timeLastHoming ;

    /* Last time checked for walls. */
    T_word32 lastWallCheck ;

    /* Area sound attached to object. */
    T_areaSound areaSound ;

    /* Time of last wandering sound. */
    T_word32 lastWanderSound ;

    /* Z location of where to hit enemy. */
    T_sword16 targetAttackZ ;

    /* Delayed attack time (in absolute time) */
    T_word32 delayedAttackTime ;

    /* Keep track of the last target's view info. */
    T_lineOfSightLast sight ;

    /* Last location. */
    T_sword16 lastX, lastY ;

    /* Flag stolen items from this guy. */
    E_Boolean wasStolenFrom ;

    /* Flag to say if gravity will make this creature fall. */
    E_Boolean allowFall ;

    /* Time to update berserk state */
    T_word32 timeCheckBerserk ;

	/*Time before forgetting about hidden enemies*/
	E_Boolean losingInterest;
	T_word32 timeTargetInterest;

	/* Health Decay Rate */
	T_word16 healthDecayRate;
	T_word16 healthDecayRateLast;

	float missileDelayMod;
	T_word16 meleeDamage;
	E_effectDamageType damageResist;

	//Charmed - Special damage types currently active
	T_word16 CharmValue;

	T_word32 LastTouchedID; //Variable for tracking last person to push or pull this monster so that fall damage XP can be awarded
	T_word32 TimeLastTouched; //Timeout before falldamage XP no longer counts. (We have a small time to push them off a cliff, if they accidentally wander off 20 seconds later it wasn't our fault)
} T_creatureState ;

#ifdef SERVER_ONLY
T_3dObject *CreatureLookForPlayer(T_3dObject *p_obj);
#else
T_word16 CreatureLookForPlayer(T_3dObject *p_obj);
#endif

#ifndef NDEBUG
T_void CreaturesCheck(T_void) ;
#else
#define CreaturesCheck()
#endif

#endif

/****************************************************************************/
/*    END OF FILE:  CRELOGIC.H                                               */
/****************************************************************************/
