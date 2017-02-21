/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Boss_Jeklik
SD%Complete: 85
SDComment: Problem in finding the right flying batriders for spawning and making them fly.
SDCategory: Zul'Gurub
EndScriptData */


#include "def_zulgurub.h"

#define SAY_AGGRO                   -1309002
#define SAY_RAIN_FIRE               -1309003
#define SAY_DEATH                   -1309004

#define SPELL_CHARGE              22911
#define SPELL_SONICBURST          23918
#define SPELL_SCREECH             6605
#define SPELL_SHADOW_WORD_PAIN    23952
#define SPELL_MIND_FLAY           23953
#define SPELL_CHAIN_MIND_FLAY     26044                     //Right ID unknown. So disabled
#define SPELL_GREATERHEAL         23954
#define SPELL_BAT_FORM            23966

// Batriders Spell

#define SPELL_BOMB                40332                     //Wrong ID but Magmadars bomb is not working...

struct boss_jeklikAI : public ScriptedAI
{
    boss_jeklikAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((InstanceScript*)c->GetInstanceScript());
    }

    InstanceScript *pInstance;

    uint32 Charge_Timer;
    uint32 SonicBurst_Timer;
    uint32 Screech_Timer;
    uint32 SpawnBats_Timer;
    uint32 ShadowWordPain_Timer;
    uint32 MindFlay_Timer;
    uint32 ChainMindFlay_Timer;
    uint32 GreaterHeal_Timer;
    uint32 SpawnFlyingBats_Timer;

    bool PhaseTwo;

    void Reset()
    override {
        Charge_Timer = 20000;
        SonicBurst_Timer = 8000;
        Screech_Timer = 13000;
        SpawnBats_Timer = 60000;
        ShadowWordPain_Timer = 6000;
        MindFlay_Timer = 11000;
        ChainMindFlay_Timer = 26000;
        GreaterHeal_Timer = 50000;
        SpawnFlyingBats_Timer = 10000;

        PhaseTwo = false;
    }

    void EnterCombat(Unit *who)
    override {
        DoScriptText(SAY_AGGRO, me);
        DoCast(me,SPELL_BAT_FORM);
    }

    void JustDied(Unit* Killer)
    override {
        DoScriptText(SAY_DEATH, me);

        if(pInstance)
            pInstance->SetData(DATA_JEKLIK_DEATH, 0);
    }

    void UpdateAI(const uint32 diff)
    override {
        if (!UpdateVictim())
            return;

        if( me->GetVictim() && me->IsAlive())
        {
            if ((me->GetHealth()*100 / me->GetMaxHealth() > 50))
            {
                if (Charge_Timer < diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0))
                    {
                        DoCast(target,SPELL_CHARGE);
                        AttackStart(target);
                    }

                    Charge_Timer = 15000 + rand()%15000;
                }else Charge_Timer -= diff;

                if (SonicBurst_Timer < diff)
                {
                    DoCast(me->GetVictim(),SPELL_SONICBURST);
                    SonicBurst_Timer = 8000 + rand()%5000;
                }else SonicBurst_Timer -= diff;

                if (Screech_Timer < diff)
                {
                    DoCast(me->GetVictim(),SPELL_SCREECH);
                    Screech_Timer = 18000 + rand()%8000;
                }else Screech_Timer -= diff;

                if (SpawnBats_Timer < diff)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0);

                    Creature* Bat = nullptr;
                    Bat = me->SummonCreature(11368,-12291.6220,-1380.2640,144.8304,5.483, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if (target && Bat ) Bat ->AI()->AttackStart(target);

                    Bat = me->SummonCreature(11368,-12289.6220,-1380.2640,144.8304,5.483, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(target && Bat ) Bat ->AI()->AttackStart(target);

                    Bat = me->SummonCreature(11368,-12293.6220,-1380.2640,144.8304,5.483, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(target && Bat ) Bat ->AI()->AttackStart(target);

                    Bat = me->SummonCreature(11368,-12291.6220,-1380.2640,144.8304,5.483, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(target && Bat ) Bat ->AI()->AttackStart(target);

                    Bat = me->SummonCreature(11368,-12289.6220,-1380.2640,144.8304,5.483, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(target && Bat ) Bat ->AI()->AttackStart(target);
                    Bat = me->SummonCreature(11368,-12293.6220,-1380.2640,144.8304,5.483, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                    if(target && Bat ) Bat ->AI()->AttackStart(target);

                    SpawnBats_Timer = 60000;
                }else SpawnBats_Timer -= diff;
            }
            else
            {
                if(PhaseTwo)
                {
                    if(PhaseTwo && ShadowWordPain_Timer < diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM,0))
                        {
                            DoCast(target, SPELL_SHADOW_WORD_PAIN);
                            ShadowWordPain_Timer = 12000 + rand()%6000;
                        }
                    }ShadowWordPain_Timer -=diff;

                    if(MindFlay_Timer < diff)
                    {
                        DoCast(me->GetVictim(), SPELL_MIND_FLAY);
                        MindFlay_Timer = 16000;
                    }MindFlay_Timer -=diff;

                    if(ChainMindFlay_Timer < diff)
                    {
                        me->InterruptNonMeleeSpells(false);
                        DoCast(me->GetVictim(), SPELL_CHAIN_MIND_FLAY);
                        ChainMindFlay_Timer = 15000 + rand()%15000;
                    }ChainMindFlay_Timer -=diff;

                    if(GreaterHeal_Timer < diff)
                    {
                        me->InterruptNonMeleeSpells(false);
                        DoCast(me,SPELL_GREATERHEAL);
                        GreaterHeal_Timer = 25000 + rand()%10000;
                    }GreaterHeal_Timer -=diff;

                    if(SpawnFlyingBats_Timer < diff)
                    {
                        Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                        if(!target)
                            return;

                        Creature* FlyingBat = me->SummonCreature(14965, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()+15, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if(FlyingBat)
                            FlyingBat->AI()->AttackStart(target);

                        SpawnFlyingBats_Timer = 10000 + rand()%5000;
                    }else SpawnFlyingBats_Timer -=diff;
                }
                else
                {
                    me->SetUInt32Value(UNIT_FIELD_DISPLAYID,15219);
                    DoResetThreat();
                    PhaseTwo = true;
                }
            }

            DoMeleeAttackIfReady();
        }
    }
};

//Flying Bat
struct mob_batriderAI : public ScriptedAI
{
    mob_batriderAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((InstanceScript*)c->GetInstanceScript());
    }

    InstanceScript *pInstance;

    uint32 Bomb_Timer;
    uint32 Check_Timer;

    void Reset()
    override {
        Bomb_Timer = 2000;
        Check_Timer = 1000;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void EnterCombat(Unit *who) override {}

    void UpdateAI (const uint32 diff)
    override {
        if (!UpdateVictim() )
            return;

        //Bomb_Timer
        if(Bomb_Timer < diff)
        {
            if (Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 0))
            {
                DoCast(target, SPELL_BOMB);
                Bomb_Timer = 5000;
            }
        }else Bomb_Timer -= diff;

        //Check_Timer
        if(Check_Timer < diff)
        {
            if(pInstance)
            {
                if(pInstance->GetData(DATA_JEKLIKISDEAD))
                {
                    me->DespawnOrUnsummon();
                }
            }

            Check_Timer = 1000;
        }else Check_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jeklik(Creature *_Creature)
{
    return new boss_jeklikAI (_Creature);
}

CreatureAI* GetAI_mob_batrider(Creature *_Creature)
{
    return new mob_batriderAI (_Creature);
}

void AddSC_boss_jeklik()
{
    OLDScript *newscript;
    newscript = new OLDScript;
    newscript->Name="boss_jeklik";
    newscript->GetAI = &GetAI_boss_jeklik;
    sScriptMgr->RegisterOLDScript(newscript);

    newscript = new OLDScript;
    newscript->Name="mob_batrider";
    newscript->GetAI = &GetAI_mob_batrider;
    sScriptMgr->RegisterOLDScript(newscript);
}
