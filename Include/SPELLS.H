/****************************************************************************/
/*    FILE:  SPELLS.H                                                       */
/****************************************************************************/
#ifndef _SPELLS_H_
#define _SPELLS_H_
#define MAX_RUNE_COUNT 99

#include "BUTTON.H"
#include "SPELTYPE.H"

#define SPELL_CAST_TIMEOUT 36
static T_word32 SpellTimeout;
static T_word32 WandTimeout;

T_void SpellsBackspace (T_buttonID buttonID);


T_void SpellsInitSpells (T_void);
T_void SpellsAddRune (T_buttonID buttonID);
T_void SpellsClearRunes (T_buttonID buttonID);
T_void SpellsCastSpell (T_buttonID buttonID);
T_void SpellsFinishSpell (T_void);
T_void SpellsDrawRuneBox (T_void);
T_void SpellsLeap (T_spellID spell);
T_void SpellsFast (T_spellID spell);
T_void SpellsShockAbsorb (T_spellID spell);
T_void SpellsHeal (T_spellID spell);
T_void SpellsToggle (T_spellID spell);
T_void SpellsFireball (T_spellID spell);
T_void SpellsLavaWalk (T_spellID spell);
T_void SpellsWaterWalk (T_spellID spell);
T_void SpellsInvulnerable (T_spellID spell);
T_void SpellsDuration (T_spellID spell);
T_void SpellsRegenerate (T_spellID spell);
T_void SpellsFeatherFall (T_spellID spell);
T_void SpellsLoGrav (T_spellID spell);
T_void SpellsBeaconSet (T_spellID spell);
T_void SpellsBeaconReturn (T_spellID spell);
T_void SpellsStopAll (T_void) ;
T_void SpellsDrawInEffectIcons (T_word16 left,
								T_word16 right,
								T_word16 top,
								T_word16 bottom);
T_void SpellsFinish(T_void) ; /* LES */
T_void SpellsSetRune (E_spellsRuneType type);
T_void SpellsClearRune (E_spellsRuneType type);
T_void SpellsFinishSpell (T_void);
T_void SpellsMakeNextSound (T_void* p_data);
T_byte8* GetRuneText(T_byte8 runecode);
#endif
