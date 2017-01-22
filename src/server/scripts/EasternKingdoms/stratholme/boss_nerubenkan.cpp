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
SDName: Boss_Nerubenkan
SD%Complete: 70
SDComment:
SDCategory: Stratholme
EndScriptData */


#include "def_stratholme.h"

#define SPELL_ENCASINGWEBS          4962
#define SPELL_PIERCEARMOR           6016
#define SPELL_CRYPT_SCARABS         31602
#define SPELL_RAISEUNDEADSCARAB     17235

struct boss_nerubenkanAI : public ScriptedAI
{
    boss_nerubenkanAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = (InstanceScript*)me->GetInstanceScript();
    }

    InstanceScript* pInstance;

    uint32 EncasingWebs_Timer;
    uint32 PierceArmor_Timer;
   uint32 CryptScarabs_Timer;
    uint32 RaiseUndeadScarab_Timer;
    int Rand;
    int RandX;
    int RandY;
    Creature* Summoned;

    void Reset()
    override {
       CryptScarabs_Timer = 3000;
        EncasingWebs_Timer = 7000;
        PierceArmor_Timer = 19000;
        RaiseUndeadScarab_Timer = 3000;
    }

    void EnterCombat(Unit *who)
    override {
    }

    void JustDied(Unit* Killer)
    override {
        if (pInstance)
            pInstance->SetData(TYPE_NERUB,IN_PROGRESS);
    }

    void RaiseUndeadScarab(Unit* victim)
    {
        Rand = rand()%10;
        switch (rand()%2)
        {
        case 0: RandX = 0 - Rand; break;
        case 1: RandX = 0 + Rand; break;
        }
        Rand = 0;
        Rand = rand()%10;
        switch (rand()%2)
        {
        case 0: RandY = 0 - Rand; break;
        case 1: RandY = 0 + Rand; break;
        }
        Rand = 0;
        Summoned = DoSpawnCreature(10876, RandX, RandY, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 180000);
        if(Summoned)
            ((CreatureAI*)Summoned->AI())->AttackStart(victim);
    }

    void UpdateAI(const uint32 diff)
    override {
        if (!UpdateVictim())
            return;

        //EncasingWebs
        if (EncasingWebs_Timer < diff)
        {
            DoCast(me->GetVictim(),SPELL_ENCASINGWEBS);
            EncasingWebs_Timer = 30000;
        }else EncasingWebs_Timer -= diff;

        //PierceArmor
        if (PierceArmor_Timer < diff)
        {
            if (rand()%100 < 75)
                DoCast(me->GetVictim(),SPELL_PIERCEARMOR);
            PierceArmor_Timer = 35000;
        }else PierceArmor_Timer -= diff;

        //CryptScarabs_Timer
        if (CryptScarabs_Timer < diff)
        {
            DoCast(me->GetVictim(),SPELL_CRYPT_SCARABS);
            CryptScarabs_Timer = 20000;
        }else CryptScarabs_Timer -= diff;

        //RaiseUndeadScarab
        if (RaiseUndeadScarab_Timer < diff)
        {
            RaiseUndeadScarab(me->GetVictim());
            RaiseUndeadScarab_Timer = 16000;
        }else RaiseUndeadScarab_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_nerubenkan(Creature *_Creature)
{
    return new boss_nerubenkanAI (_Creature);
}

void AddSC_boss_nerubenkan()
{
    OLDScript *newscript;
    newscript = new OLDScript;
    newscript->Name="boss_nerubenkan";
    newscript->GetAI = &GetAI_boss_nerubenkan;
    sScriptMgr->RegisterOLDScript(newscript);
}
