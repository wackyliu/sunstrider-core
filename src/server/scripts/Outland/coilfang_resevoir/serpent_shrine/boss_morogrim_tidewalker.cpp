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
SDName: Boss_Morogrim_Tidewalker
SD%Complete: 90
SDComment: Water globules don't explode properly, remove hacks
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */


#include "def_serpent_shrine.h"

#define SAY_AGGRO                   -1548030
#define SAY_SUMMON1                 -1548031
#define SAY_SUMMON2                 -1548032
#define SAY_SUMMON_BUBL1            -1548033
#define SAY_SUMMON_BUBL2            -1548034
#define SAY_SLAY1                   -1548035
#define SAY_SLAY2                   -1548036
#define SAY_SLAY3                   -1548037
#define SAY_DEATH                   -1548038
#define EMOTE_WATERY_GRAVE          -1548039
#define EMOTE_EARTHQUAKE            -1548040
#define EMOTE_WATERY_GLOBULES       -1548041

#define SPELL_TIDAL_WAVE            37730
#define SPELL_WATERY_GRAVE          38049
#define SPELL_EARTHQUAKE            37764
#define SPELL_WATERY_GRAVE_EXPLOSION 37852

#define WATERY_GRAVE_X1             334.64
#define WATERY_GRAVE_Y1             -728.89
#define WATERY_GRAVE_Z1             -14.42
#define WATERY_GRAVE_X2             365.51
#define WATERY_GRAVE_Y2             -737.14
#define WATERY_GRAVE_Z2             -14.44
#define WATERY_GRAVE_X3             366.19
#define WATERY_GRAVE_Y3             -709.59
#define WATERY_GRAVE_Z3             -14.36
#define WATERY_GRAVE_X4             372.93
#define WATERY_GRAVE_Y4             -690.96
#define WATERY_GRAVE_Z4             -14.44

#define SPELL_WATERY_GRAVE_1    38023
#define SPELL_WATERY_GRAVE_2    38024
#define SPELL_WATERY_GRAVE_3    38025
#define SPELL_WATERY_GRAVE_4    37850

#define SPELL_SUMMON_WATER_GLOBULE_1    37854
#define SPELL_SUMMON_WATER_GLOBULE_2    37858
#define SPELL_SUMMON_WATER_GLOBULE_3    37860
#define SPELL_SUMMON_WATER_GLOBULE_4    37861

/*#define SPELL_SUMMON_MURLOC_A6    39813
#define SPELL_SUMMON_MURLOC_A7  39814
#define SPELL_SUMMON_MURLOC_A8  39815
#define SPELL_SUMMON_MURLOC_A9  39816
#define SPELL_SUMMON_MURLOC_A10 39817

#define SPELL_SUMMON_MURLOC_B6  39818
#define SPELL_SUMMON_MURLOC_B7  39819
#define SPELL_SUMMON_MURLOC_B8  39820
#define SPELL_SUMMON_MURLOC_B9  39821
#define SPELL_SUMMON_MURLOC_B10 39822*/

float MurlocCords[10][5] =
{
    {21920.0f, 424.36f, -715.40f, -7.14f, 0.124f},
    {21920.0f, 425.13f, -719.30f, -7.14f, 0.124f},
    {21920.0f, 425.05f, -724.23f, -7.14f, 0.124f},
    {21920.0f, 424.91f, -728.68f, -7.14f, 0.124f},
    {21920.0f, 424.84f, -732.18f, -7.14f, 0.124f},
    {21920.0f, 321.05f, -734.20f, -13.15f, 0.124f},
    {21920.0f, 321.05f, -729.40f, -13.15f, 0.124f},
    {21920.0f, 321.05f, -724.03f, -13.15f, 0.124f},
    {21920.0f, 321.05f, -718.73f, -13.15f, 0.124f},
    {21920.0f, 321.05f, -714.24f, -13.15f, 0.124f}
};

//Creatures
#define WATER_GLOBULE               21913
#define TIDEWALKER_LURKER           21920

//Morogrim Tidewalker AI

//Water Globule AI
#define SPELL_GLOBULE_EXPLOSION 37871


class boss_morogrim_tidewalker : public CreatureScript
{
public:
    boss_morogrim_tidewalker() : CreatureScript("boss_morogrim_tidewalker")
    { }

    class boss_morogrim_tidewalkerAI : public ScriptedAI
    {
        public:
        boss_morogrim_tidewalkerAI(Creature *c) : ScriptedAI(c)
        {
            pInstance = ((InstanceScript*)c->GetInstanceScript());
        }
    
        InstanceScript* pInstance;
        Map::PlayerList const *PlayerList;
    
        uint32 TidalWave_Timer;
        uint32 WateryGrave_Timer;
        uint32 Earthquake_Timer;
        uint32 WateryGlobules_Timer;
        uint32 globulespell[4];
        int8 Playercount;
        int8 counter;
    
        bool Earthquake;
        bool Phase2;
    
        void Reset()
        override {
            TidalWave_Timer = 10000;
            WateryGrave_Timer = 30000;
            Earthquake_Timer = 40000;
            WateryGlobules_Timer = 0;
            globulespell[0] = SPELL_SUMMON_WATER_GLOBULE_1;
            globulespell[1] = SPELL_SUMMON_WATER_GLOBULE_2;
            globulespell[2] = SPELL_SUMMON_WATER_GLOBULE_3;
            globulespell[3] = SPELL_SUMMON_WATER_GLOBULE_4;
    
            Earthquake = false;
            Phase2 = false;
    
            if (pInstance)
                pInstance->SetData(DATA_MOROGRIMTIDEWALKEREVENT, NOT_STARTED);
        }
    
        void StartEvent()
        {
            DoScriptText(SAY_AGGRO, me);
    
            if (pInstance)
                pInstance->SetData(DATA_MOROGRIMTIDEWALKEREVENT, IN_PROGRESS);
        }
    
        void KilledUnit(Unit *victim)
        override {
            DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3), me);
        }
    
        void JustDied(Unit *victim)
        override {
            DoScriptText(SAY_DEATH, me);
    
            if (pInstance)
                pInstance->SetData(DATA_MOROGRIMTIDEWALKEREVENT, DONE);
        }
    
        void EnterCombat(Unit *who)
        override {
            PlayerList = &((InstanceMap*)me->GetMap())->GetPlayers();
            Playercount = PlayerList->getSize();
            StartEvent();
        }
    
        void ApplyWateryGrave(Unit *player, uint8 i)
        {
            switch(i)
            {
            case 0: player->CastSpell(player, SPELL_WATERY_GRAVE_1, true); break;
            case 1: player->CastSpell(player, SPELL_WATERY_GRAVE_2, true); break;
            case 2: player->CastSpell(player, SPELL_WATERY_GRAVE_3, true); break;
            case 3: player->CastSpell(player, SPELL_WATERY_GRAVE_4, true); break;
            }
        }
    
        void UpdateAI(const uint32 diff)
        override {
            //Return since we have no target
            if (!UpdateVictim() )
                return;
    
            //Earthquake_Timer
            if (Earthquake_Timer < diff)
            {
                if (!Earthquake)
                {
                    DoCast(me->GetVictim(), SPELL_EARTHQUAKE);
                    Earthquake = true;
                    Earthquake_Timer = 10000;
                }
                else
                {
                    DoScriptText(RAND(SAY_SUMMON1, SAY_SUMMON2), me);
    
                    for(auto & MurlocCord : MurlocCords)
                    {
                        Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0);
                        Creature* Murloc = me->SummonCreature(MurlocCord[0],MurlocCord[1],MurlocCord[2],MurlocCord[3],MurlocCord[4], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                        if(target && Murloc)
                            Murloc->AI()->AttackStart(target);
                    }
                    DoScriptText(EMOTE_EARTHQUAKE, me);
                    Earthquake = false;
                    Earthquake_Timer = 40000+rand()%5000;
                }
            }else Earthquake_Timer -= diff;
    
            //TidalWave_Timer
            if (TidalWave_Timer < diff)
            {
                DoCast(me->GetVictim(), SPELL_TIDAL_WAVE);
                TidalWave_Timer = 20000;
            }else TidalWave_Timer -= diff;
    
            if (!Phase2)
            {
                //WateryGrave_Timer
                if (WateryGrave_Timer < diff)
                {
                    //Teleport 4 players under the waterfalls
                    Unit *target;
                    using std::set;
                    set<int>list;
                    set<int>::iterator itr;
                    for(uint8 i = 0; i < 4; i++)
                    {
                        counter = 0;
                        do
                        {
                            target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0, true, true);    //target players only and no tank
                            if(counter < Playercount)
                                break;
                            if(target) itr = list.find(target->GetGUID());
                            counter++;
                        } while(itr != list.end());
                        if(target){list.insert(target->GetGUID());
                        ApplyWateryGrave(target, i);
                        }
                    }
                    
                    DoScriptText(RAND(SAY_SUMMON_BUBL1, SAY_SUMMON_BUBL2), me);
                    DoScriptText(EMOTE_WATERY_GRAVE, me);
                    WateryGrave_Timer = 30000;
                }else WateryGrave_Timer -= diff;
    
                //Start Phase2
                if ((me->GetHealthPct()) < 25)
                    Phase2 = true;
            }
            else
            {
                //WateryGlobules_Timer
                if (WateryGlobules_Timer < diff)
                {
                    Unit* globuletarget;
                    using std::set;
                    set<int>globulelist;
                    set<int>::iterator itr;
                    for (uint32 g : globulespell)  //one unit cant cast more than one spell per update, so some players have to cast for us XD
                    {
                        counter = 0;
                        do {globuletarget = SelectTarget(SELECT_TARGET_RANDOM, 0,50,true);
                        if(globuletarget) itr = globulelist.find(globuletarget->GetGUID());
                        if (counter > Playercount)
                            break;
                        counter++;
                        } while (itr != globulelist.end());
                        if(globuletarget)globulelist.insert(globuletarget->GetGUID());
                        globuletarget->CastSpell(globuletarget, g, true);
                    }
                    DoScriptText(EMOTE_WATERY_GLOBULES, me);
                    WateryGlobules_Timer = 25000;
                }else WateryGlobules_Timer -= diff;
            }
    
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_morogrim_tidewalkerAI(creature);
    }
};

class mob_water_globule : public CreatureScript
{
public:
    mob_water_globule() : CreatureScript("mob_water_globule")
    { }

    class mob_water_globuleAI : public ScriptedAI
    {
        public:
        mob_water_globuleAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 Check_Timer;
        uint32 DespawnTimer;
    
        void Reset()
        override {
            Check_Timer = 1000;
            DespawnTimer = 40000;
    
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFaction(FACTION_MONSTER);
        }
    
        void EnterCombat(Unit *who) override {}
    
        void MoveInLineOfSight(Unit *who)
        override {
            if (!who || me->GetVictim())
                return;
    
            if (me->CanAttack(who) == CAN_ATTACK_RESULT_OK && who->isInAccessiblePlaceFor(me) && me->IsHostileTo(who))
            {
                //no attack radius check - it attacks the first target that moves in his los
                //who->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                AttackStart(who);
            }
        }
    
        void UpdateAI(const uint32 diff)
        override {
            //Return since we have no target
            if (!UpdateVictim() )
                return;
                
            if (DespawnTimer <= diff) {
                me->DisappearAndDie();
                return;
            } else DespawnTimer -= diff;
    
            if (Check_Timer <= diff) {
                if (me->IsWithinDistInMap(me->GetVictim(), 5))
                {
                    DoCast(me->GetVictim(), SPELL_GLOBULE_EXPLOSION);
    
                    //despawn
                    me->DealDamage(me, me->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
                }
                Check_Timer = 500;
            }else Check_Timer -= diff;
    
            //do NOT deal any melee damage to the target.
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mob_water_globuleAI(creature);
    }
};


void AddSC_boss_morogrim_tidewalker()
{

    new boss_morogrim_tidewalker();

    new mob_water_globule();
}
