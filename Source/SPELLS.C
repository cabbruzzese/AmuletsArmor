/*-------------------------------------------------------------------------*
 * File:  SPELLS.C
 *-------------------------------------------------------------------------*/
/**
 * All spell casting goes through here.
 *
 * @addtogroup SPELLS
 * @brief Spells System
 * @see http://www.amuletsandarmor.com/AALicense.txt
 * @{
 *
 *<!-----------------------------------------------------------------------*/
#include "BANNER.H"
#include "CLIENT.H"
#include "COLOR.H"
#include "DBLLINK.H"
#include "EFFECT.H"
#include "KEYSCAN.H"
#include "MESSAGE.H"
#include "PICS.H"
#include "SOUND.H"
#include "SOUNDS.H"
#include "SPELLS.H"
#include "STATS.H"
#include "SYNCTIME.H"

/* define if all spells available for any class */
#define MANA_BACKCOLOR 225

static T_byte8 G_curspell[4]={0,0,0,0};
static T_byte8 G_curSound=0;
static T_doubleLinkList G_spellsList;
static T_sword16 G_beaconX=0;
static T_sword16 G_beaconY=0;
static T_word16  G_facing=0;
static E_Boolean G_clearSpells=FALSE;
static E_Boolean G_isInit = FALSE ;

/*-------------------------------------------------------------------------*
 * Routine:  SpellsInitSpells
 *-------------------------------------------------------------------------*/
/**
 *  Initializes variables associated with spell casting.
 *  Must be called prior to client login.
 *
 *<!-----------------------------------------------------------------------*/
T_void SpellsInitSpells (T_void)
{
	T_word16 count=0;
    T_byte8 stmp[32];
    T_word16 i;
    T_resource res;
    T_spellStruct *p_spell;
    E_spellsSpellSystemType system;
    E_Boolean unload=FALSE;

	DebugRoutine ("SpellsInitSpells");
    DebugCheck(G_isInit == FALSE) ;

    G_isInit = TRUE ;

    /* now, create new linked list for storage of spells */
    G_spellsList=DoubleLinkListCreate();
    DebugCheck (G_spellsList!=DOUBLE_LINK_LIST_BAD);

    /* get current character spell system */
    system = StatsGetPlayerSkillSystem();

    /* scan through resource files and lock in all available spells */
    /* for current class type */
    sprintf (stmp,"SPELDESC/SPL%05d",count++);
    while (PictureExist(stmp))
    {
        /* lock in a new spell structure */
        p_spell = (T_spellStruct *)PictureLockData (stmp,&res);

        DebugCheck (p_spell != NULL);

        /* remap casting code to keyboard */
        for (i=0;i<4;i++)
        {
            if (p_spell->code[i]==1) p_spell->code[i]=KEY_SCAN_CODE_KEYPAD_1;
            else if (p_spell->code[i]==2) p_spell->code[i]=KEY_SCAN_CODE_KEYPAD_2;
            else if (p_spell->code[i]==3) p_spell->code[i]=KEY_SCAN_CODE_KEYPAD_3;
            else if (p_spell->code[i]==4) p_spell->code[i]=KEY_SCAN_CODE_KEYPAD_4;
            else if (p_spell->code[i]==5) p_spell->code[i]=KEY_SCAN_CODE_KEYPAD_5;
            else if (p_spell->code[i]==6) p_spell->code[i]=KEY_SCAN_CODE_KEYPAD_6;
            else if (p_spell->code[i]==7) p_spell->code[i]=KEY_SCAN_CODE_KEYPAD_7;
            else if (p_spell->code[i]==8) p_spell->code[i]=KEY_SCAN_CODE_KEYPAD_8;
            else if (p_spell->code[i]==9) p_spell->code[i]=KEY_SCAN_CODE_KEYPAD_9;
        }
/*
        printf ("\n\nloading spell:\n");
        printf ("code = %d,%d,%d,%d\n",p_spell->code[0],
                                       p_spell->code[1],
                                       p_spell->code[2],
                                       p_spell->code[3]);
        printf ("effect type =%d\n",p_spell->type);
        printf ("effect sub  =%d\n",p_spell->subtype);
        printf ("duration    =%d\n",p_spell->duration);
        printf ("duration mod=%d\n",p_spell->durationmod);
        printf ("power       =%d\n",p_spell->power);
        printf ("power mod   =%d\n",p_spell->powermod);
        printf ("cost        =%d\n",p_spell->cost);
        printf ("cost mod    =%d\n",p_spell->costmod);
        printf ("hardness    =%d\n",p_spell->hardness);
        printf ("hardness mod=%d\n",p_spell->hardnessmod);
        printf ("minimum lev =%d\n",p_spell->minlevel);
        printf ("system      =%d\n",p_spell->system);
        printf ("splash red  =%d\n",p_spell->filtr);
        printf ("splash grn  =%d\n",p_spell->filtg);
        printf ("splash blu  =%d\n",p_spell->filtb);
        printf ("\n\n\n");
        fflush (stdout);
*/
        /* determine if the character can cast this spell */
#ifndef COMPILE_OPTION_LOAD_ALL_SPELLS
        unload = TRUE;
        switch (system)
        {
            case SPELL_SYSTEM_MAGE:
            if (p_spell->system == SPELL_SYSTEM_MAGE ||
                p_spell->system == SPELL_SYSTEM_MAGE_AND_CLERIC ||
                p_spell->system == SPELL_SYSTEM_MAGE_AND_ARCANE ||
                p_spell->system == SPELL_SYSTEM_ANY) unload=FALSE;
            break;

            case SPELL_SYSTEM_CLERIC:
            if (p_spell->system == SPELL_SYSTEM_CLERIC ||
                p_spell->system == SPELL_SYSTEM_MAGE_AND_CLERIC ||
                p_spell->system == SPELL_SYSTEM_CLERIC_AND_ARCANE ||
                p_spell->system == SPELL_SYSTEM_ANY) unload=FALSE;
            break;

            case SPELL_SYSTEM_ARCANE:
            if (p_spell->system == SPELL_SYSTEM_ARCANE ||
                p_spell->system == SPELL_SYSTEM_MAGE_AND_ARCANE ||
                p_spell->system == SPELL_SYSTEM_CLERIC_AND_ARCANE ||
                p_spell->system == SPELL_SYSTEM_ANY) unload=FALSE;
            break;

            default:
            /* fail! improper spell system returned */
            DebugCheck (-1);
        }
#endif

        if (unload == TRUE)
        {
            /* nevermind, we don't want this spell anyway. */
//            printf ("unloading this spell, improper casting class\n");
//            fflush (stdout);
            PictureUnlockAndUnfind (res);
        }
        else
        {
//            printf ("saving this spell in list\n");
//            fflush (stdout);

            /* add spell pointer to linked list */
            DoubleLinkListAddElementAtEnd (G_spellsList, res);
        }
        sprintf (stmp,"SPELDESC/SPL%05d",count++);
    }
	DebugEnd();
}

T_void SkillLogicsInitialize(T_void)
{
	DebugRoutine("InitSkills");
	//setup skill Logics
	//none
	G_SkillLogics[SKILL_SYSTEM_NONE].UsesRunes = FALSE;
	G_SkillLogics[SKILL_SYSTEM_NONE].IsMultiRune = FALSE;
	G_SkillLogics[SKILL_SYSTEM_NONE].UsesSpells = FALSE;
	G_SkillLogics[SKILL_SYSTEM_NONE].CanPickupRunes = FALSE;
	G_SkillLogics[SKILL_SYSTEM_NONE].RuneType = SPELL_SYSTEM_NONE;
	//mage
	G_SkillLogics[SKILL_SYSTEM_MAGIC_MAGE].UsesRunes = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_MAGE].IsMultiRune = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_MAGE].UsesSpells = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_MAGE].CanPickupRunes = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_MAGE].RuneType = SPELL_SYSTEM_MAGE;
	//Cleric
	G_SkillLogics[SKILL_SYSTEM_MAGIC_CLERIC].UsesRunes = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_CLERIC].IsMultiRune = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_CLERIC].UsesSpells = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_CLERIC].CanPickupRunes = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_CLERIC].RuneType = SPELL_SYSTEM_CLERIC;
	//Arcane
	G_SkillLogics[SKILL_SYSTEM_MAGIC_ARCANE].UsesRunes = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_ARCANE].IsMultiRune = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_ARCANE].UsesSpells = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_ARCANE].CanPickupRunes = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MAGIC_ARCANE].RuneType = SPELL_SYSTEM_ARCANE;
	//Barbarian
	G_SkillLogics[SKILL_SYSTEM_BARBARIAN].UsesRunes = TRUE;
	G_SkillLogics[SKILL_SYSTEM_BARBARIAN].IsMultiRune = FALSE;
	G_SkillLogics[SKILL_SYSTEM_BARBARIAN].UsesSpells = FALSE;
	G_SkillLogics[SKILL_SYSTEM_BARBARIAN].CanPickupRunes = FALSE;
	G_SkillLogics[SKILL_SYSTEM_BARBARIAN].RuneType = SPELL_SYSTEM_ARCANE;
	//Monk
	G_SkillLogics[SKILL_SYSTEM_MONK].UsesRunes = TRUE;
	G_SkillLogics[SKILL_SYSTEM_MONK].IsMultiRune = FALSE;
	G_SkillLogics[SKILL_SYSTEM_MONK].UsesSpells = FALSE;
	G_SkillLogics[SKILL_SYSTEM_MONK].CanPickupRunes = FALSE;
	G_SkillLogics[SKILL_SYSTEM_MONK].RuneType = SPELL_SYSTEM_CLERIC;
	//Ninja
	G_SkillLogics[SKILL_SYSTEM_NINJA].UsesRunes = TRUE;
	G_SkillLogics[SKILL_SYSTEM_NINJA].IsMultiRune = FALSE;
	G_SkillLogics[SKILL_SYSTEM_NINJA].UsesSpells = FALSE;
	G_SkillLogics[SKILL_SYSTEM_NINJA].CanPickupRunes = FALSE;
	G_SkillLogics[SKILL_SYSTEM_NINJA].RuneType = SPELL_SYSTEM_MAGE;

	DebugEnd();
}



/*-------------------------------------------------------------------------*
 * Routine:  SpellsFinish
 *-------------------------------------------------------------------------*/
/**
 *  SpellsFinish gets rid of any memory or resources associated with
 *  all the spells, either in effect, or able to be used.
 *
 *<!-----------------------------------------------------------------------*/
T_void SpellsFinish(T_void)
{
	T_word16 i ;
    T_doubleLinkListElement element, nextElement ;
    T_resource res ;

	DebugRoutine ("SpellsFinish");
    DebugCheck(G_isInit == TRUE) ;

    G_isInit = FALSE ;

//    printf ("spells finish called\n");
//    fflush (stdout);

    /* unlock and unfind all spell structure resources */

    element = DoubleLinkListGetFirst(G_spellsList) ;
    while (element != DOUBLE_LINK_LIST_ELEMENT_BAD)
    {
        nextElement = DoubleLinkListElementGetNext(element) ;

        res = DoubleLinkListElementGetData(element) ;

        PictureUnlockAndUnfind(res) ;
        DoubleLinkListRemoveElement(element) ;

        element = nextElement ;
    }

    /* clear globals */
    for (i=0;i<4;i++) G_curspell[i]=0;
    G_clearSpells=FALSE;

	DebugEnd();

}

/*-------------------------------------------------------------------------*
 * Routine:  SpellsAddRune
 *-------------------------------------------------------------------------*/
/**
 *  Callback routine assigned to a rune button, adds rune to spell box
 *
 *<!-----------------------------------------------------------------------*/
T_void SpellsAddRune (T_buttonID buttonID)
{
	T_buttonStruct *p_button;
	T_word16 i;
	T_word16 manasuck;

	DebugRoutine ("SpellsAddRune");
	DebugCheck (buttonID != NULL);

	//barbarians can't set runes
	if (StatsGetPlayerSkillLogic()->UsesRunes == FALSE)
	{
		DebugEnd();
		return;
	}
	
	//skill systems can only have a single rune active
	if (StatsGetPlayerSkillLogic()->IsMultiRune == FALSE)
	{
		SpellsClearRunes (buttonID);

		p_button=(T_buttonStruct *)buttonID;

		ColorSetColor (MANA_BACKCOLOR,30,0,30);
		G_curspell[0]=(T_byte8)p_button->scancode;   //add scancode to spell key
		SpellsDrawRuneBox();
		
		DebugEnd();
		return;
	}

    if (G_clearSpells==TRUE)
    {
        G_clearSpells=FALSE;
    	SpellsClearRunes (buttonID);
    }

	if (G_curspell[3]!=0)
	{
		ColorSetColor (MANA_BACKCOLOR,55,0,0);
	}
	else if (buttonID != NULL)
	{
		p_button=(T_buttonStruct *)buttonID;
		for (i=0;i<4;i++)
		{
			if (G_curspell[i]==0) //if slot is empty
			{
				manasuck=25;
				StatsChangePlayerMana(-manasuck);
				if (StatsGetPlayerMana()>0)
				{
				  ColorSetColor (MANA_BACKCOLOR,30,0,30);
				  G_curspell[i]=(T_byte8)p_button->scancode;   //add scancode to spell key
				  SpellsDrawRuneBox();
				} else ColorSetColor (MANA_BACKCOLOR,55,0,0);
				break;
			}
		}
	}
	DebugEnd();
}


/*-------------------------------------------------------------------------*
 * Routine:  SpellsClearRunes
 *-------------------------------------------------------------------------*/
/**
 *  Callback routine assigned to the rune clearbox button, clears runes
 *  in spell box
 *
 *<!-----------------------------------------------------------------------*/
T_void SpellsClearRunes (T_buttonID buttonID)
{
	T_word16 i;
	T_bitmap *pic ;
	T_resource res ;
	DebugRoutine ("SpellsClearRunes");

	for (i=0;i<4;i++) G_curspell[i]=0;

	pic = (T_bitmap *)PictureLockData("UI/3DUI/SPSTRIP", &res) ;
	DebugCheck(pic != NULL) ;

	if (pic != NULL)
	{
		GrScreenSet(GRAPHICS_ACTUAL_SCREEN) ;
        MouseHide();
		GrDrawBitmap(pic, 176, 181) ;
        MouseShow();
		PictureUnlock(res) ;
	PictureUnfind(res) ;
	}

	DebugEnd();
}



/*-------------------------------------------------------------------------*
 * Routine:  SpellsBackSpace
 *-------------------------------------------------------------------------*/
/**
 *  Callback routine assigned to the rune backspace button, removes last
 *  tune entered.
 *
 *  NOTE: 
 *  Currently same as SpellsClear
 *
 *<!-----------------------------------------------------------------------*/
T_void SpellsBackspace (T_buttonID buttonID)
{
	T_word16 i;
	T_bitmap *pic ;
	T_resource res ;
	DebugRoutine ("SpellsBackspace");

	for (i=0;i<4;i++) G_curspell[i]=0;

	pic = (T_bitmap *)PictureLockData("UI/3DUI/SPSTRIP", &res) ;
	DebugCheck(pic != NULL) ;

	if (pic != NULL)
	{
		GrScreenSet(GRAPHICS_ACTUAL_SCREEN) ;
		GrDrawBitmap(pic, 176, 181) ;
		PictureUnlock(res) ;
		PictureUnfind(res) ;
	}

	DebugEnd();
}



/*-------------------------------------------------------------------------*
 * Routine:  SpellsDrawRuneBox
 *-------------------------------------------------------------------------*/
/**
 *  This routine draws the selected icons for the current spell in the
 *  spell box
 *
 *<!-----------------------------------------------------------------------*/
T_void SpellsDrawRuneBox (T_void)
{
	T_word16 i;
	T_buttonID runebutton;
    E_Boolean pushed=FALSE;
	T_graphicID runepic;

	DebugRoutine ("SpellsDrawRuneBox");

    if (BannerButtonsOk())
    {
	    for (i=0;i<4;i++)
	    {
		    if (G_curspell[i]!=0)
		    {
		         runebutton=ButtonGetByKey (G_curspell[i]);
                 pushed=ButtonIsPushed(runebutton);
                 if (pushed==FALSE)
                 {
                    ButtonDownNoAction (runebutton);
                 }
		         runepic=ButtonGetGraphic(runebutton);
                 GraphicSetShadow (runepic,255);
                 MouseHide();
		         GraphicDrawAt (runepic,177+(i*15),182);
                 MouseShow();
                 if (pushed==FALSE)
                 {
                    ButtonUpNoAction (runebutton);
                 }
		    }
	    }
    }
	DebugEnd();
}


/*-------------------------------------------------------------------------*
 * Routine:  SpellsCastSpell
 *-------------------------------------------------------------------------*/
/**
 *  This routine attempts to cast the currently selected spell
 *
 *<!-----------------------------------------------------------------------*/
#if 0
T_void SpellsCastSpell (T_buttonID buttonID)
{
    DebugRoutine ("SpellsCastSpell");

    /* start sound sequence */
    G_curSound=0;

    MessagePrintf ("code=%d %d %d %d",G_curspell[0],G_curspell[1],G_curspell[2],G_curspell[3]);

    SpellsMakeNextSound (NULL);

    DebugEnd();
}

T_void SpellsMakeNextSound (T_void *p_data)
{
    T_word16 soundNum;

    DebugRoutine ("SpellsMakeNextSound");


    if (StatsGetPlayerSpellSystem()==SPELL_SYSTEM_CLERIC )
    {
        if (G_curSound==3)
        {
            soundNum=G_curspell[G_curSound]+SOUND_SPELL_CLERIC_B_SET - 70;
        }
        else if (G_curspell[G_curSound+1]==0)
        {
            soundNum=G_curspell[G_curSound]+SOUND_SPELL_CLERIC_B_SET - 70;
        }
        else
        {
            soundNum=G_curspell[G_curSound]+SOUND_SPELL_CLERIC_A_SET - 70;
        }
    }
    else
    {
        if (G_curSound==3)
        {
            soundNum=G_curspell[G_curSound]+SOUND_SPELL_MAGE_B_SET - 70;
        }
        else if (G_curspell[G_curSound+1]==0)
        {
            soundNum=G_curspell[G_curSound]+SOUND_SPELL_MAGE_B_SET - 70;
        }
        else
        {
            soundNum=G_curspell[G_curSound]+SOUND_SPELL_MAGE_A_SET - 70;
        }
    }

    if (G_curSound == 3)
    {
        SoundPlayByNumber(soundNum,255);
        SpellsFinishSpell();
    }
    else if (G_curspell[G_curSound+1]==0)
    {
        SoundPlayByNumber(soundNum,255);
        SpellsFinishSpell();
    }
    else
    {
        MessagePrintf ("cs=%d G_cursp=%d",G_curSound,G_curspell[G_curSound]);
        MessagePrintf ("playing sound %d",soundNum);
        SoundPlayByNumberWithCallback(
            soundNum,
            255,
            SpellsMakeNextSound,
            NULL) ;
    }

    G_curSound++;

    DebugEnd();
}

#endif

//Must have unique spell entries to track adding/removing buffs
// Every buff is indexed by the spell pointer.
T_spellStruct BerserkerSkillSpells[7] =
{
	{//battle rage stat 1
		0,0,0,0,0,0,0,0,
		0,0,1000
	},
	{//battle rage stat 2
		0,0,0,0,0,0,0,0,
		0,0,1000
	},
	{//piercing damage
		0,0,0,0,0,0,0,0,
		0,0,1000
	},
	{//fury stat 1
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{//fury stat 2
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{//fury stat 3
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{//shield
		0,0,0,0,0,0,0,0,
		0,0,1000
	}
};

T_spellStruct NinjaSkillSpells[9] =
{
	{ // water walking
		0,0,0,0,0,0,0,0,
		0,0,500
	},
	{ // Night vision
		0,0,0,0,0,0,0,0,
		0,0,750
	},
	{ // Jump stat 1
		0,0,0,0,0,0,0,0,
		0,0,1000
	},
	{ // Jump stat 2
		0,0,0,0,0,0,0,0,
		0,0,1000
	},
	{ // poison hands
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{ // lava walk
		0,0,0,0,0,0,0,0,
		0,0,1000
	},
	{ // speed
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{ // spirit warrior stat 1
		0,0,0,0,0,0,0,0,
		0,0,4000
	},
	{ // spirit warrior stat 2
		0,0,0,0,0,0,0,0,
		0,0,4000
	}
};

T_spellStruct MonkSkillSpells[12] =
{
	{ // meditate stat 1
		0,0,0,0,0,0,0,0,
		0,0,750
	},
	{ // meditate stat 2
		0,0,0,0,0,0,0,0,
		0,0,750
	},
	{ // fire attack
		0,0,0,0,0,0,0,0,
		0,0,1000
	},	
	{ // spirit shield stat 1
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{ // spirit shield stat 2
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{ // spirit shield stat 3
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{ // iron skin
		0,0,0,0,0,0,0,0,
		0,0,1000
	},
	{ // body shield stat 1
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{ // body shield stat 2
		0,0,0,0,0,0,0,0,
		0,0,2000
	},	
	{ // body shield stat 3
		0,0,0,0,0,0,0,0,
		0,0,2000
	},
	{ // acension
		0,0,0,0,0,0,0,0,
		0,0,2500
	},
	{ // divine intervention
		0,0,0,0,0,0,0,0,
		0,0,3000
	}
};

T_void PerformNinjaSkill(T_byte8 runenum)
{
	E_Boolean skillsucess = FALSE;
	E_Boolean failpenalty = FALSE;
	T_sword16 spellcost = 0;
	T_sword16 spellpower = 0;
	T_sword16 spellduration = 0;
	T_sword32 manaleft;
	DebugRoutine("PerformNinjaSkill");

	manaleft = StatsGetManaLeft();
	switch (runenum)
	{
		case KEY_SCAN_CODE_KEYPAD_1:
			spellpower = 0;
			spellduration = (75 * StatsGetPlayerSpeedTotal()) + (75 * StatsGetPlayerMagicTotal());
			spellcost = NinjaSkillSpells[0].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Serpent Feet Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_WATER_WALK,
					spellduration,
					spellpower,
					&NinjaSkillSpells[0]);

				skillsucess = TRUE;
			}
			break;

		case KEY_SCAN_CODE_KEYPAD_2:
			spellpower = 0;
			spellduration = 150 * StatsGetPlayerMagicTotal();
			spellcost = NinjaSkillSpells[1].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Night Eyes Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_NIGHT_VISION,
					spellduration,
					spellpower,
					&NinjaSkillSpells[1]);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_3:
			spellpower = 0;
			spellduration = (75 * StatsGetPlayerSpeedTotal()) + (75 * StatsGetPlayerMagicTotal());
			spellcost = NinjaSkillSpells[2].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Ninja Jump Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_FEATHER_FALL,
					spellduration,
					spellpower,
					&NinjaSkillSpells[2]);

				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_LOW_GRAVITY,
					spellduration,
					spellpower,
					&NinjaSkillSpells[3]);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_4:
			spellpower = 1;
			spellduration = 150 * StatsGetPlayerMagicTotal();
			spellcost = NinjaSkillSpells[4].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Snake Form Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_POISON_ATTACK,
					spellduration,
					spellpower,
					&NinjaSkillSpells[4]);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_5:
			spellpower = 0;
			spellduration = 0;
			spellcost = 1000;

			if (manaleft >= spellcost)
			{
				MessageAdd("Smoke Cloud Activated");
				Effect (EFFECT_CREATE_PROJECTILE,
					EFFECT_TRIGGER_CAST,
					EFFECT_MISSILE_CONFUSION,
					spellduration,
					spellpower,
					NULL);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_6:
			spellpower = 0;
			spellduration = (75 * StatsGetPlayerSpeedTotal()) + (75 * StatsGetPlayerMagicTotal());
			spellcost = NinjaSkillSpells[5].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Dragon Feet Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_LAVA_WALK,
					spellduration,
					spellpower,
					&NinjaSkillSpells[5]);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_7:
			spellpower = 25;
			spellduration = (75 * StatsGetPlayerSpeedTotal()) + (75 * StatsGetPlayerMagicTotal());
			spellcost = NinjaSkillSpells[6].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Great Wind Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_SPEED_MOD,
					spellduration,
					spellpower,
					&NinjaSkillSpells[6]);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_8:
			spellpower = 0;
			spellduration = (200 * StatsGetPlayerMagicTotal());
			spellcost = NinjaSkillSpells[7].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Demon Warrior Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_INVISIBLE,
					spellduration,
					spellpower,
					&NinjaSkillSpells[7]);
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_PIERCING_ATTACK,
					spellduration,
					1,
					&NinjaSkillSpells[8]);

				skillsucess = TRUE;	
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_9:
			spellpower = 10 * StatsGetPlayerAttribute(ATTRIBUTE_ACCURACY);
			spellduration = 0;
			spellcost = 1500;

			if (manaleft >= spellcost)
			{
				MessageAdd("Dragon's Breath Activated");
				Effect (EFFECT_CREATE_PROJECTILE,
					EFFECT_TRIGGER_CAST,
					EFFECT_MISSILE_HOMING_FIREBALL,
					spellduration,
					spellpower,
					NULL);

				skillsucess = TRUE;
			}
			break;
	}

	if (skillsucess == TRUE)
	{
		StatsChangePlayerMana (-(spellcost));
		StatsChangePlayerHeartBeat(HEARTRATE_SPELL);
	}
	else if (failpenalty == TRUE && manaleft >= spellcost)
	{
		MessageAdd("Skill failed.");
		StatsChangePlayerMana (-(spellcost / 2));
		StatsChangePlayerHeartBeat(HEARTRATE_SPELL_FAIL);
	}
	else if (spellcost > 0)
	{
		MessageAdd("Too exhausted to perform skill.");
	}
	DebugEnd();
}

T_void PerformMonkSkill(T_byte8 runenum)
{
	E_Boolean skillsucess = FALSE;
	E_Boolean failpenalty = FALSE;
	T_sword16 spellcost = 0;
	T_sword16 spellpower = 0;
	T_sword16 spellduration = 0;
	T_sword32 manaleft;
	DebugRoutine("PerformMonkSkill");

	manaleft = StatsGetManaLeft();
	switch (runenum)
	{
		case KEY_SCAN_CODE_KEYPAD_1:
			spellpower = StatsGetPlayerMagicTotal() * 8;
			spellduration = (100 * StatsGetPlayerMagicTotal());
			spellcost = MonkSkillSpells[0].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Meditation Activated");

				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_HEALTH_REGEN_MOD,
					spellduration,
					spellpower,
					&MonkSkillSpells[0]);

				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_MANA_REGEN_MOD,
					spellduration,
					spellpower,
					&MonkSkillSpells[1]);

				//also put heartrate at rest
				StatsChangePlayerHeartBeat(-(HEARTRATE_FULL));

				skillsucess = TRUE;
			}
			break;

		case KEY_SCAN_CODE_KEYPAD_2:
			spellpower = 1;
			spellduration = 80 * StatsGetPlayerMagicTotal();
			spellcost = MonkSkillSpells[2].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Dragon Fist Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_FIRE_ATTACK,
					spellduration,
					spellpower,
					&MonkSkillSpells[2]);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_3:
			spellpower = 0;
			spellduration = (75 * StatsGetPlayerMagicTotal());
			spellcost = MonkSkillSpells[3].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Spirit Shield Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_RESIST_MANA_DRAIN,
					spellduration,
					spellpower,
					&MonkSkillSpells[3]);

				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_RESIST_PIERCING,
					spellduration,
					spellpower,
					&MonkSkillSpells[4]);

				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_RESIST_POISON,
					spellduration,
					spellpower,
					&MonkSkillSpells[5]);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_4:
			spellpower = (StatsGetPlayerMagicTotal() * 2) / 3;
			spellduration = 150 * StatsGetPlayerMagicTotal();
			spellcost = MonkSkillSpells[6].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Iron Skin Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_SHIELD,
					spellduration,
					spellpower,
					&MonkSkillSpells[6]);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_5:
			spellpower = StatsGetPlayerMagicTotal() * 15;
			spellduration = 0;
			spellcost = 1000;

			if (manaleft >= spellcost)
			{
				MessageAdd("Healing Energy Activated");
				StatsChangePlayerHealth(spellpower);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_6:
			spellpower = 0;
			spellduration = (75 * StatsGetPlayerMagicTotal());
			spellcost = MonkSkillSpells[7].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Body Shield Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_RESIST_FIRE,
					spellduration,
					spellpower,
					&MonkSkillSpells[7]);

				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_RESIST_ELECTRICITY,
					spellduration,
					spellpower,
					&MonkSkillSpells[8]);

				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_RESIST_ACID,
					spellduration,
					spellpower,
					&MonkSkillSpells[9]);

				skillsucess = TRUE;
			}			
			break;
		case KEY_SCAN_CODE_KEYPAD_7:
			spellpower = 10 * StatsGetPlayerMagicTotal();
			spellduration = 0;
			spellcost = 1500;

			if (manaleft >= spellcost)
			{
				MessageAdd("Spirit Blast Activated");
				Effect (EFFECT_CREATE_PROJECTILE,
					EFFECT_TRIGGER_CAST,
					EFFECT_MISSILE_LIGHTNING_BOLT,
					spellduration,
					spellpower,
					NULL);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_8:
			spellpower = 0;
			spellduration = 50 * StatsGetPlayerMagicTotal();
			spellcost = MonkSkillSpells[10].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Acension Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_FLY,
					spellduration,
					spellpower,
					&MonkSkillSpells[10]);

				skillsucess = TRUE;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_9:
			spellpower = 0;
			spellduration = 50 * StatsGetPlayerMagicTotal();
			spellcost = MonkSkillSpells[10].cost;

			if (manaleft >= spellcost)
			{
				MessageAdd("Divine Intervention Activated");
				Effect (EFFECT_ADD_PLAYER_EFFECT,
					EFFECT_TRIGGER_CAST,
					PLAYER_EFFECT_DEATH_WARD,
					spellduration,
					spellpower,
					&MonkSkillSpells[11]);

				skillsucess = TRUE;
			}
			break;
	}

	if (skillsucess == TRUE)
	{
		StatsChangePlayerMana (-(spellcost));
		StatsChangePlayerHeartBeat(HEARTRATE_SPELL);
	}
	else if (failpenalty == TRUE && manaleft >= spellcost)
	{
		MessageAdd("Skill failed.");
		StatsChangePlayerMana (-(spellcost / 2));
		StatsChangePlayerHeartBeat(HEARTRATE_SPELL_FAIL);
	}
	else if (spellcost > 0)
	{
		MessageAdd("Too exhausted to perform skill.");
	}
	DebugEnd();
}

T_void PerformBarbarianSkill(T_byte8 runenum)
{
	E_Boolean skillsucess = FALSE;
	T_sword16 spellcost = 0;
	T_sword16 spellpower = 0;
	T_sword16 spellduration = 0;
	T_sword32 manaleft;
	T_sword16 heartratecost;
	DebugRoutine("PerformBarbarianSkill");

	manaleft = StatsGetManaLeft();
	switch (runenum)
	{
		case KEY_SCAN_CODE_KEYPAD_1:
			//use minimum as spell cost to correctly display exhausted message
			spellcost = BerserkerSkillSpells[0].cost;
			spellduration = StatsGetPlayerMagicTotal() * 100;

			if (manaleft >= BerserkerSkillSpells[0].cost)
			{
				//update spellcost and power
				spellcost = manaleft;
				spellpower = spellcost / 140;
			
				MessageAdd("Battle rage activated");
				//strength boost
				Effect (EFFECT_ADD_PLAYER_EFFECT,
						EFFECT_TRIGGER_CAST,
						PLAYER_EFFECT_STRENGTH_MOD,
						spellduration,
						spellpower,
						&BerserkerSkillSpells[0]);
				//speed boost
				Effect (EFFECT_ADD_PLAYER_EFFECT,
						EFFECT_TRIGGER_CAST,
						PLAYER_EFFECT_SPEED_MOD,
						spellduration,
						spellpower,
						&BerserkerSkillSpells[1]);

				skillsucess = TRUE;
				heartratecost = HEARTRATE_FULL;
			}
			break;

		case KEY_SCAN_CODE_KEYPAD_2:
			spellcost = 500;
			spellpower = 200 * StatsGetPlayerAttribute(ATTRIBUTE_STRENGTH);
			//duration used as range
			spellduration = StatsGetPlayerAttribute(ATTRIBUTE_STRENGTH)*2;

			if (manaleft >= spellcost)
			{
				MessageAdd("Break Free activated");

				Effect(EFFECT_AREA_OF_EFFECT,
						EFFECT_TRIGGER_CAST,
						EFFECT_DAMAGE_SPECIAL | EFFECT_DAMAGE_SPECIAL_PUSH,
						5000,
						5000,
						NULL);

				skillsucess = TRUE;
				heartratecost = HEARTRATE_JUMP;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_3:
			spellcost = 250;
			spellpower = 2000 + StatsGetPlayerAttribute(ATTRIBUTE_STRENGTH) * 100;
			spellduration = 0;

			if (manaleft >= spellcost)
			{
				MessageAdd("Giant Leap activated");

				Effect (EFFECT_PLAYER_LEAP,
						EFFECT_TRIGGER_CAST,
						0,
						0,
						spellpower,
						NULL);

				skillsucess = TRUE;
				heartratecost = HEARTRATE_JUMP;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_4:
			spellcost = 500;
			spellpower = 1;
			spellduration = 0;

			if (manaleft >= spellcost)
			{
				MessageAdd("Attract activated");

				Effect (EFFECT_CREATE_PROJECTILE,
						EFFECT_TRIGGER_CAST,
						EFFECT_MISSILE_PULL,
						spellduration,
						spellpower,
						NULL);

				skillsucess = TRUE;
				heartratecost = HEARTRATE_SPELL;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_5:
			spellcost = 500;
			spellpower = 1;
			spellduration = 0;

			if (manaleft >= spellcost)
			{
				MessageAdd("Hurl Insults activated");

				Effect (EFFECT_CREATE_PROJECTILE,
						EFFECT_TRIGGER_CAST,
						EFFECT_MISSILE_BERSERK,
						spellduration,
						spellpower,
						NULL);

				skillsucess = TRUE;
				heartratecost = HEARTRATE_SPELL;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_6:
			spellcost = BerserkerSkillSpells[2].cost;
			spellpower = 1;
			spellduration = 75 * StatsGetPlayerAttribute(ATTRIBUTE_STRENGTH);

			if (manaleft >= spellcost)
			{
				MessageAdd("Smash Armor activated");

				Effect (EFFECT_ADD_PLAYER_EFFECT,
						EFFECT_TRIGGER_CAST,
						PLAYER_EFFECT_PIERCING_ATTACK,
						spellduration,
						spellpower,
						&BerserkerSkillSpells[2]);

				skillsucess = TRUE;
				heartratecost = HEARTRATE_SPELL;
			}		
			break;
		case KEY_SCAN_CODE_KEYPAD_7:
			spellcost = BerserkerSkillSpells[6].cost;
			spellpower = 2 * StatsGetPlayerMagicTotal();
			spellduration = 25 * StatsGetPlayerAttribute(ATTRIBUTE_STRENGTH);

			if (manaleft >= spellcost)
			{
				MessageAdd("Shout of Courage activated");

				Effect (EFFECT_ADD_PLAYER_EFFECT,
						EFFECT_TRIGGER_CAST,
						PLAYER_EFFECT_SHIELD,
						spellduration,
						spellpower,
						&BerserkerSkillSpells[6]);

				skillsucess = TRUE;
				heartratecost = HEARTRATE_SPELL;
			}	
			break;
		case KEY_SCAN_CODE_KEYPAD_8:
			spellcost = 750;
			spellpower = StatsGetPlayerAttribute(ATTRIBUTE_STRENGTH) * 7;
			spellduration = 0;

			if (manaleft >= spellcost)
			{
				MessageAdd("Rush activated");

				Effect (EFFECT_JUMP_FORWARD,
						EFFECT_TRIGGER_CAST,
						0,
						0,
						spellpower,
						NULL);

				skillsucess = TRUE;
				heartratecost = HEARTRATE_FULL;
			}
			break;
		case KEY_SCAN_CODE_KEYPAD_9:
			spellcost = BerserkerSkillSpells[3].cost;
			spellpower = StatsGetPlayerAttribute(ATTRIBUTE_STRENGTH) * 7;
			spellduration = 0;

			if (manaleft >= spellcost)
			{
				MessageAdd("Fury of the Gods activated");

				Effect (EFFECT_ADD_PLAYER_EFFECT,
						EFFECT_TRIGGER_CAST,
						PLAYER_EFFECT_FIRE_ATTACK,
						0,
						spellpower,
						&BerserkerSkillSpells[3]);
				Effect (EFFECT_ADD_PLAYER_EFFECT,
						EFFECT_TRIGGER_CAST,
						PLAYER_EFFECT_ELECTRICITY_ATTACK,
						0,
						spellpower,
						&BerserkerSkillSpells[4]);
				Effect (EFFECT_ADD_PLAYER_EFFECT,
						EFFECT_TRIGGER_CAST,
						PLAYER_EFFECT_ACID_ATTACK,
						0,
						spellpower,
						&BerserkerSkillSpells[5]);

				skillsucess = TRUE;
				heartratecost = HEARTRATE_SPELL;
			}
			break;
	}

	if (skillsucess == TRUE)
	{
		StatsChangePlayerMana (-(spellcost));
		StatsChangePlayerHeartBeat(heartratecost);
	}
	else if (spellcost > 0 && spellcost > manaleft)
	{
		MessageAdd("Too exhausted to perform skill.");
	}
	DebugEnd();
}

T_void SkillsCastSpell (T_void)
{
	T_byte8 runenum;

	DebugRoutine ("SpellsCastSpell");

	//get first rune
	runenum = G_curspell[0];

	//ariticial leveling for debugging
	//if (runenum == 0)
	//{
		//StatsChangePlayerExperience(StatsGetPlayerExpNeeded() - StatsGetPlayerExperience());
		//StatsChangePlayerMana(StatsGetPlayerMaxMana());
	//}

	switch(StatsGetPlayerSkillSystem())
	{
	case SKILL_SYSTEM_BARBARIAN:
		PerformBarbarianSkill(runenum);
		break;
	case SKILL_SYSTEM_NINJA:
		PerformNinjaSkill(runenum);
		break;
	case SKILL_SYSTEM_MONK:
		PerformMonkSkill(runenum);
		break;
	default:
		break;
	}

	DebugEnd();
}

T_void SpellsCastSpell (T_buttonID buttonID)
{
	E_Boolean success;
    T_doubleLinkListElement element;
    T_resource res;
    T_spellStruct *p_spell, *p_spell_obj;
    T_word32 spellpower, spellduration;
	T_sword16 spellcost;
    T_sword16 spelldif;
    T_byte8 charlevel, charmagic;
	T_word32 i;

	DebugRoutine ("SpellsCastSpell");

	//reset if synctime resets
	if (SpellTimeout > SyncTimeGet() + SPELL_CAST_TIMEOUT)
		SpellTimeout = 0;
	//if spell was not cast too recently
	if (SpellTimeout < SyncTimeGet())
	{
		SpellTimeout = SyncTimeGet() + SPELL_CAST_TIMEOUT;
	}
	else
	{
		DebugEnd();
		return;
	}

    /* search through spell list and see if any spell */
    /* matches current spell code */

    if (ClientIsPaused())  {
        MessageAdd("Cannot cast spells while paused.") ;
    } else if (ClientIsDead())  {
        MessageAdd("Dead players do not cast spells.") ;
    } else {
        element = DoubleLinkListGetFirst (G_spellsList);

		if (StatsGetPlayerSkillLogic()->UsesSpells == FALSE)
		{
			SkillsCastSpell();

			DebugEnd();
			return;
		}

        while (element != DOUBLE_LINK_LIST_ELEMENT_BAD)
        {
            /* get a pointer to this spell's spell struct */
            res = DoubleLinkListElementGetData(element);
            p_spell = (T_spellStruct *)PictureLockDataQuick (res);
            DebugCheck (p_spell != NULL);

            /* check to see if code matches */
            success=TRUE;
            for (i=0;i<4;i++)
            {
                if (p_spell->code[i]!=G_curspell[i])
                {
                    success=FALSE;
                    break;
                }
            }

            if (success==TRUE)
            {
                /* get level of character */
                charlevel = StatsGetPlayerLevel();
				charmagic = StatsGetPlayerMagicTotal();
				
                /* figure duration of spell */
                spellduration = p_spell->duration + (p_spell->durationmod*charlevel);
                if (spellduration > MAX_EFFECT_DURATION) spellduration = MAX_EFFECT_DURATION;
				if (spellduration > 0)
					spellduration += charmagic * 20;

                /* figure power of spell */
                spellpower = p_spell->power + (p_spell->powermod*charlevel) + (charmagic / 15);
                if (spellpower > MAX_EFFECT_POWER) spellpower = MAX_EFFECT_POWER;

                /* figure casting cost of spell */
                spellcost = (T_sword16)(p_spell->cost + (p_spell->costmod*charlevel) - (charmagic * 2));
				if (spellcost < 100)
					spellcost = 100;

                /* figure difficulty of spell */
                spelldif = charmagic + p_spell->hardness + (2 * charlevel);
                if (spelldif < 0) spelldif = 0;

                /* cast this spell if possible */
                if (((StatsGetManaLeft() > (T_sword32)spellcost) && (charlevel >= p_spell->minlevel)) ||
                    (EffectPlayerEffectIsActive (PLAYER_EFFECT_GOD_MODE) == TRUE))
                {
                    success=TRUE;

                    /* check for a d100 against hardness for sucess */
                    if (EffectPlayerEffectIsActive (PLAYER_EFFECT_GOD_MODE) == FALSE)
                    {
                        if (rand()%100 > spelldif)
                        {
                            /* failed roll */
    //                        success = FALSE;
                            MessageAdd ("Your spell fizzled");
                            ColorAddGlobal (10,10,-10);
                            SoundPlayByNumber (SOUND_SPELL_FIZZLE,255);
                            /* remove half the spell cost */
                            StatsChangePlayerMana (-(spellcost / 2));
                            PictureUnlock(res);

							StatsChangePlayerHeartBeat(HEARTRATE_SPELL_FAIL);
                            break;
                        }
                    }

                    if (success==TRUE)
                    {
                        if (EffectPlayerEffectIsActive(PLAYER_EFFECT_GOD_MODE)==FALSE)
                        {
                            StatsChangePlayerMana (-spellcost);
                        }
                        /* do a color effect */
                        ColorAddGlobal ((T_sbyte8)p_spell->filtr>>1,(T_sbyte8)p_spell->filtg>>1,(T_sbyte8)p_spell->filtb>>1);

                        
						//do not send spell data if projectile
						if (p_spell->type == EFFECT_CREATE_PROJECTILE)
						{
							p_spell_obj = NULL;
							
							//remove bonus power value from all attack spelltypes
							spellpower -= 1;
							//scale back to a reasonable damage modifier
							spellpower *= 30;
						}
						else
							p_spell_obj = p_spell;

						/* create spell effect */
                        Effect (p_spell->type,
                            EFFECT_TRIGGER_CAST,
                            p_spell->subtype,
                            (T_word16)spellduration,
                            (T_word16)spellpower,
                            p_spell_obj);

                        /* trigger the spell sound */
                        if (p_spell->sound != 0)
                        {
                            SoundPlayByNumber (p_spell->sound,255);
                        }

						StatsChangePlayerHeartBeat(HEARTRATE_SPELL);

                        PictureUnlock(res);
                        break;
                    }
                }
                else
                {
                    MessageAdd ("^003You are too exhausted to cast this spell.");
                    success=TRUE;
                    PictureUnlock(res);

					StatsChangePlayerHeartBeat(HEARTRATE_SPELL_FAIL);
                    break;
                }

                /* spell success, break from while loop */
                /* nevermind, check for other spells with same code */
    //			break;
		    }

            PictureUnlock(res);
            element = DoubleLinkListElementGetNext (element);
	    }
        if (success==FALSE && G_curspell[0]!=0)
        {
            MessageAdd ("^003You feel something is just not right.");
            SpellsClearRunes(NULL);
        }
        /* set flag to clear runes on next rune entered */
        else G_clearSpells=TRUE;
    }

	DebugEnd();
}


/*-------------------------------------------------------------------------*
 * Routine:  SpellsStopAll
 *-------------------------------------------------------------------------*/
/**
 *  SpellsStopAll turns off all the spells that are in effect.
 *
 *<!-----------------------------------------------------------------------*/
T_void SpellsStopAll (T_void)
{
    DebugRoutine("SpellsStopAll") ;

//    for (i=0; i<NUM_SPELLS; i++)
//        SpellsStop(G_spells+i) ;

	DebugEnd();
}



T_void SpellsDrawInEffectRunes (T_word16 left,
								T_word16 right,
								T_word16 top,
								T_word16 bottom)
{
	E_Boolean drawed=FALSE;

	DebugRoutine  ("SpellsDrawInEffectRunes");
	DebugEnd();
}

T_void SpellsSetRune (E_spellsRuneType type)
{
	E_spellsSpellSystemType spelltype;
    DebugRoutine ("SpellsSetRune");

	spelltype = StatsGetPlayerSkillLogic()->RuneType;
    /* check to see if this rune is valid for the characters */
    /* current spell system */
	if (spelltype == SPELL_SYSTEM_MAGE)
    {
        /* valid runes are >=RUNE_ARCANE_1, <=RUNE_MAGE_5 */
        if (type >=RUNE_ARCANE_1 && type <= RUNE_MAGE_5)
        {
            /* valid rune, increment counter */
            StatsIncrementRuneCount (type);
        }
	} else if (spelltype == SPELL_SYSTEM_CLERIC)
    {
        if (type >= RUNE_ARCANE_5 && type <= RUNE_CLERIC_4)
        {
            StatsIncrementRuneCount (type - RUNE_ARCANE_5);
        }
	} else if (spelltype == SPELL_SYSTEM_ARCANE)
    {
        /* arcane spell system */
        if (type >= RUNE_ARCANE_1 && type <= RUNE_ARCANE_4)
        {
            StatsIncrementRuneCount (type - RUNE_ARCANE_1);
        }
        else if (type >= RUNE_ARCANE_5 && type <= RUNE_ARCANE_9)
        {
            StatsIncrementRuneCount (type - RUNE_ARCANE_5 + 4);
        }
    }

    DebugEnd();
}


T_void SpellsClearRune (E_spellsRuneType type)
{
    DebugRoutine ("SpellsClearRune");

    /* check to see if this rune is valid for the characters */
    /* current spell system */
	if (StatsGetPlayerSkillLogic()->RuneType == SPELL_SYSTEM_MAGE)
    {
        /* valid runes are >=RUNE_ARCANE_1, <=RUNE_MAGE_5 */
        if (type >=RUNE_ARCANE_1 && type <= RUNE_MAGE_5)
        {
            /* valid rune, increment counter */
            StatsDecrementRuneCount (type);
        }
	} 
	else if (StatsGetPlayerSkillLogic()->RuneType == SPELL_SYSTEM_CLERIC)
    {
        if (type >= RUNE_ARCANE_5 && type <= RUNE_CLERIC_4)
        {
            StatsDecrementRuneCount (type - RUNE_ARCANE_5);
        }
	}else if (StatsGetPlayerSkillLogic()->RuneType == SPELL_SYSTEM_ARCANE)
    {
        /* arcane spell system */
        if (type >= RUNE_ARCANE_1 && type <= RUNE_ARCANE_4)
        {
            StatsDecrementRuneCount (type - RUNE_ARCANE_1);
        }
        else if (type >= RUNE_ARCANE_5 && type <= RUNE_ARCANE_9)
        {
            StatsDecrementRuneCount (type - RUNE_ARCANE_5 + 4);
        }
    }

    DebugEnd();
}

/* @} */
/*-------------------------------------------------------------------------*
 * End of File:  SPELLS.C
 *-------------------------------------------------------------------------*/
