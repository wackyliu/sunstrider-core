
/* ScriptData
SDName: Generic_Creature
SD%Complete: 80
SDComment: Should be replaced with core based AI
SDCategory: Creatures
EndScriptData */



#define GENERIC_CREATURE_COOLDOWN   5000

class generic_creature : public CreatureScript
{
public:
    generic_creature() : CreatureScript("generic_creature")
    { }

    class generic_creatureAI : public ScriptedAI
    {
        public:
        generic_creatureAI(Creature *c) : ScriptedAI(c) {}
    
        uint32 GlobalCooldown;      //This variable acts like the global cooldown that players have (1.5 seconds)
        uint32 BuffTimer;           //This variable keeps track of buffs
        bool IsSelfRooted;
    
        void Reset()
        override {
            GlobalCooldown = 0;
            BuffTimer = 0;          //Rebuff as soon as we can
            IsSelfRooted = false;
        }
    
        void EnterCombat(Unit *who)
        override {
            if (!me->IsWithinMeleeRange(who))
            {
                IsSelfRooted = true;
            }
        }
    
        void UpdateAI(const uint32 diff)
        override {
            //Always decrease our global cooldown first
            if (GlobalCooldown > diff)
                GlobalCooldown -= diff;
            else GlobalCooldown = 0;
    
            //Buff timer (only buff when we are alive and not in combat
            if (!me->IsInCombat() && me->IsAlive())
            {
                if (BuffTimer < diff )
                {
                    //Find a spell that targets friendly and applies an aura (these are generally buffs)
                    SpellInfo const *info = SelectSpell(me, 0xFFFFFFF, 0xFFFFFFF, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_AURA);
    
                    if (info && !GlobalCooldown)
                    {
                        //Cast the buff spell
                        DoCastSpell(me, info);
    
                        //Set our global cooldown
                        GlobalCooldown = GENERIC_CREATURE_COOLDOWN;
    
                        //Set our timer to 10 minutes before rebuff
                        BuffTimer = 600000;
                    }//Try agian in 30 seconds
                    else BuffTimer = 30000;
                }else BuffTimer -= diff;
            }
    
            //Return since we have no target
            if (!UpdateVictim() )
                return;
    
            //If we are within range melee the target
            if( me->IsWithinMeleeRange(me->GetVictim()))
            {
                //Make sure our attack is ready and we arn't currently casting
                if( me->IsAttackReady() && !me->IsNonMeleeSpellCast(false))
                {
                    bool Healing = false;
                    SpellInfo const *info = nullptr;
    
                    //Select a healing spell if less than 30% hp
    				if (me->GetHealthPct() < 30)
                        info = SelectSpell(me, 0xFFFFFFF, 0xFFFFFFF, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_HEALING);
    
                    //No healing spell available, select a hostile spell
                    if (info) 
    					Healing = true;
                    else 
    					info = SelectSpell(me->GetVictim(), 0xFFFFFFF, 0xFFFFFFF, SELECT_TARGET_ANY_ENEMY, 0, 0, 0, 0, SELECT_EFFECT_DONTCARE);
    
                    //50% chance if elite or higher, 20% chance if not, to replace our white hit with a spell
                    if (info && (rand() % (me->GetCreatureTemplate()->rank > 1 ? 2 : 5) == 0) && !GlobalCooldown)
                    {
                        //Cast the spell
                        if (Healing)
    						DoCastSpell(me, info);
                        else 
    						DoCastSpell(me->GetVictim(), info);
    
                        //Set our global cooldown
                        GlobalCooldown = GENERIC_CREATURE_COOLDOWN;
                    }
                    else me->AttackerStateUpdate(me->GetVictim());
    
                    me->ResetAttackTimer();
                }
            }
            else
            {
                //Only run this code if we arn't already casting
                if (!me->IsNonMeleeSpellCast(false))
                {
                    bool Healing = false;
                    SpellInfo const *info = nullptr;
    
                    //Select a healing spell if less than 30% hp ONLY 33% of the time
                    if (me->GetHealthPct() < 30 && rand() % 3 == 0)
                        info = SelectSpell(me, 0xFFFFFFF, 0xFFFFFFF, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_HEALING);
    
                    //No healing spell available, See if we can cast a ranged spell (Range must be greater than ATTACK_DISTANCE)
                    if (info) 
    					Healing = true;
    
                    else info = SelectSpell(me->GetVictim(), 0xFFFFFFF, 0xFFFFFFF, SELECT_TARGET_ANY_ENEMY, 0, 0, NOMINAL_MELEE_RANGE, 0, SELECT_EFFECT_DONTCARE);
    
                    //Found a spell, check if we arn't on cooldown
                    if (info && !GlobalCooldown)
                    {
                        //If we are currently moving stop us and set the movement generator
                        if (!IsSelfRooted)
                        {
                            IsSelfRooted = true;
                        }
    
                        //Cast spell
                        if (Healing) 
    						DoCastSpell(me,info);
                        else 
    						DoCastSpell(me->GetVictim(),info);
    
                        //Set our global cooldown
                        GlobalCooldown = GENERIC_CREATURE_COOLDOWN;
    
    
                    }//If no spells available and we arn't moving run to target
                    else if (IsSelfRooted)
                    {
                        //Cancel our current spell and then allow movement agian
                        me->InterruptNonMeleeSpells(false);
                        IsSelfRooted = false;
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new generic_creatureAI(creature);
    }
};



void AddSC_generic_creature()
{
    new generic_creature();
}
