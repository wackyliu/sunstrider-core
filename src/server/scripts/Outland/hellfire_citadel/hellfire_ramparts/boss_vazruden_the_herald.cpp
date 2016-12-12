/*
boss_vazruden_the_herald: The boss that you pull, controls the summons and loot chest. Renders itself invisible when it creates the two other bosses.
boss_nazan: the dragon
boss_vazruden: the boss on foot
mob_hellfire_sentry: sentries before the boss, pull the boss when both are down

TODO:
nazan animation when casting firewall is wrong whenever he's stationary flying. What I tried so far: Tweaking movement flags, tried every unknown cast flags. In the meanwhile, I'll make it so that Nazan is never stationary.
*/

#include "def_hellfire_ramparts.h"

#define SPELL_FIREBALL              (HeroicMode?36920:34653)
#define SPELL_CONE_OF_FIRE          (HeroicMode?36921:30926)
#define SPELL_SUMMON_LIQUID_FIRE    (HeroicMode?30928:23971)
#define SPELL_BELLOWING_ROAR        39427
#define SPELL_REVENGE               (HeroicMode?40392:19130)
#define SPELL_KIDNEY_SHOT           30621
#define SPELL_FIRE_NOVA_VISUAL      19823

#define SAY_INTRO               -1543017
#define SAY_WIPE                -1543018
#define SAY_AGGRO_1             -1543019
#define SAY_AGGRO_2             -1543020
#define SAY_AGGRO_3             -1543021
#define SAY_KILL_1              -1543022
#define SAY_KILL_2              -1543023
#define SAY_DIE                 -1543024
//descend from the sky
#define EMOTE                   -1543025

#define PATH_ENTRY              2081

const Position VazrudenMiddle(-1406.5, 1746.5, 81.2);

const Position VazrudenRing[2] =
{
    {-1430, 1705, 112},
    {-1377, 1760, 112}
};

class boss_nazan : public CreatureScript
{
public:
    boss_nazan() : CreatureScript("boss_nazan") { }

    enum messages
    {
        MESSAGE_SET_VAZRUDEN_GUID = 0,
    };

    struct boss_nazanAI : public ScriptedAI
    {
        boss_nazanAI(Creature *c) : ScriptedAI(c)
        {
            HeroicMode = me->GetMap()->IsHeroic();
            VazrudenGUID = 0;
        }

        EventMap events;

        enum NazanEvents
        {
            EVENT_FIREBALL = 1,
            EVENT_SWITCH_SIDE,
            EVENT_CONE_OF_FIRE,
            EVENT_BELLOWING_ROAR,
            EVENT_START_DESCENDING,
            EVENT_ENTER_LAND_PHASE, //this exists just as a security
            EVENT_VAZRUDEN_HEALTH_CHECK,
        };

        enum NazanPhases
        {
            PHASE_FLIGHT = 1,
            PHASE_DESCENDING,
            PHASE_LAND,
        };
        
        enum NazanMovementInform
        {
            MOVINFORM_CENTER = 0,
            MOVINFORM_FLIGHT = 1,
        };

        uint8 lastWaypoint;
        uint64 VazrudenGUID;
        bool HeroicMode;

        void Reset()
            override 
        {
            events.Reset();
            SetPhase(PHASE_FLIGHT);
        }

        void MovementInform(uint32 type, uint32 id) override
        {
            if (type == POINT_MOTION_TYPE)
                if(id == MOVINFORM_CENTER)
                    events.RescheduleEvent(EVENT_ENTER_LAND_PHASE, 1);
				else if (id == MOVINFORM_FLIGHT) {
						events.RescheduleEvent(EVENT_SWITCH_SIDE, 1, 0, PHASE_FLIGHT);
				}
        }

        void SetPhase(NazanPhases phase)
        {
            switch (phase)
            {
            case PHASE_FLIGHT:
                me->SetCanFly(true, true);
                lastWaypoint = 1;
                events.RescheduleEvent(EVENT_FIREBALL, 3000);
                events.RescheduleEvent(EVENT_SWITCH_SIDE, 1, 0, PHASE_FLIGHT);
                events.RescheduleEvent(EVENT_START_DESCENDING, 45 * SECOND * IN_MILLISECONDS, 0, PHASE_FLIGHT);
                events.RescheduleEvent(EVENT_VAZRUDEN_HEALTH_CHECK, 2000, 0, PHASE_FLIGHT);
                break;
            case PHASE_DESCENDING:
                me->GetMotionMaster()->Clear();
                DoScriptText(EMOTE, me);
                events.CancelEvent(EVENT_FIREBALL);
                //EVENT_ENTER_LAND_PHASE is triggered when MOVINFORM_CENTER is reached
                me->GetMotionMaster()->MovePoint(MOVINFORM_CENTER, POSITION_GET_X_Y_Z(&VazrudenMiddle), 0.0f, false, true);
                //this shouldn't be needed but lets add this event here so that we still go to land phase if movement inform fails for some reason
                events.RescheduleEvent(EVENT_ENTER_LAND_PHASE, 10000, 0, PHASE_DESCENDING); 
                break;
            case PHASE_LAND:
                DoResetThreat();
                me->SetCanFly(false, true);
                DoStartMovement(me->GetVictim()); //re enable combat movement
                events.RescheduleEvent(EVENT_FIREBALL, urand(5000, 10000));
                events.RescheduleEvent(EVENT_CONE_OF_FIRE, urand(10000, 15000), 0, PHASE_LAND);
                if (HeroicMode)
                    events.RescheduleEvent(EVENT_BELLOWING_ROAR, 6000, 0, PHASE_LAND);

                break;
            }

            //reset other phases
            events.SetPhase(0);
            events.SetPhase(phase);
        }

        uint64 message(uint32 id, uint64 data) override
        {
            switch (id)
            {
            case MESSAGE_SET_VAZRUDEN_GUID:
                VazrudenGUID = data;
                break;
            }
            return 0;
        }

        void SpellHitTarget(Unit* target, const SpellInfo* entry)
            override {
            if (entry->Id == SPELL_FIREBALL)
            {
                Creature* summoned = me->SummonCreature(NPC_LIQUID_FIRE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 30000);
                if (!summoned)
                    return;

                summoned->SetFaction(me->GetFaction());
                summoned->CastSpell(summoned, SPELL_SUMMON_LIQUID_FIRE, true); //this should be handled in 34653 && 36920 spells script effects
                summoned->CastSpell(summoned, SPELL_FIRE_NOVA_VISUAL, true);
            }
        }

        void UpdateAI(const uint32 diff)
            override 
        {
            events.Update(diff);

            if (!UpdateVictim())
                return;

            switch (events.GetEvent())
            {
            case EVENT_FIREBALL:
            {
                bool flight = events.IsInPhase(PHASE_FLIGHT);
                if (Unit *victim = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(victim, SPELL_FIREBALL, flight ? true : false);

                uint32 nextTimer = flight ? 3000 : urand(10000, 15000);
                events.RescheduleEvent(EVENT_FIREBALL, nextTimer);
                break;
            }
            case EVENT_SWITCH_SIDE:
                lastWaypoint = lastWaypoint == 1 ? 0 : 1;
                me->GetMotionMaster()->MovePoint(MOVINFORM_FLIGHT, POSITION_GET_X_Y_Z(&(VazrudenRing[lastWaypoint])), 0.0f, false, true);

                events.RescheduleEvent(EVENT_SWITCH_SIDE, 15 * SECOND * IN_MILLISECONDS, 0, PHASE_FLIGHT);
                break;
            case EVENT_CONE_OF_FIRE:
                DoCast(me->GetVictim(), SPELL_CONE_OF_FIRE);
                events.RescheduleEvent(EVENT_CONE_OF_FIRE, urand(10000, 15000), 0, PHASE_LAND);
                break;
            case EVENT_BELLOWING_ROAR:
                DoCast(me->GetVictim(), SPELL_BELLOWING_ROAR);
                events.RescheduleEvent(EVENT_BELLOWING_ROAR, 45000, 0, PHASE_LAND);
                break;
            case EVENT_START_DESCENDING:
                SetPhase(PHASE_DESCENDING);
                events.CancelEvent(EVENT_START_DESCENDING);
                break;
            case EVENT_ENTER_LAND_PHASE:
                SetPhase(PHASE_LAND);
                events.CancelEvent(EVENT_ENTER_LAND_PHASE);
                break;
            case EVENT_VAZRUDEN_HEALTH_CHECK:
            {
                Creature *Vazruden = me->GetMap()->GetCreature(VazrudenGUID);
                bool VazrudenBelow40percent = !Vazruden || Vazruden->GetHealthPct() <= 40.0f;
                if (VazrudenBelow40percent)
                    events.RescheduleEvent(EVENT_START_DESCENDING, 1, 0, PHASE_FLIGHT);
                else
                    events.RescheduleEvent(EVENT_VAZRUDEN_HEALTH_CHECK, 2000, 0, PHASE_FLIGHT);

                break;
            }
            }

            if (events.IsInPhase(PHASE_LAND))
            {
                DoMeleeAttackIfReady();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
        override {
        return new boss_nazanAI(creature);
    }
};

class boss_vazruden : public CreatureScript
{
public:
    boss_vazruden() : CreatureScript("boss_vazruden") { }

    struct boss_vazrudenAI : public ScriptedAI
    {
        boss_vazrudenAI(Creature *c) : ScriptedAI(c)
        {
            HeroicMode = me->GetMap()->IsHeroic();
        }

        uint32 Revenge_Timer;
        bool HeroicMode;
        bool WipeSaid;
        uint32 UnsummonCheck;

        void Reset()
            override {
            Revenge_Timer = 4000;
            UnsummonCheck = 2000;
            WipeSaid = false;
        }

        bool CheckEvadeIfOutOfCombatArea() const
            override
        {
            return me->GetDistance2d(VazrudenMiddle.GetPositionX(), VazrudenMiddle.GetPositionY()) > 150.0f;
        }

        void EnterCombat(Unit *who)
            override {
            DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
        }

        void KilledUnit(Unit* who)
            override {
            if (who && who->GetEntry() != NPC_VAZRUDEN)
                DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2), me);
        }

        void JustDied(Unit* who)
            override {
            if (who && who != me)
                DoScriptText(SAY_DIE, me);
        }

        void UpdateAI(const uint32 diff)
            override {
            if (!UpdateVictim())
            {
                if (UnsummonCheck < diff && me->IsAlive())
                {
                    if (!WipeSaid)
                    {
                        DoScriptText(SAY_WIPE, me);
                        WipeSaid = true;
                    }
                    me->DisappearAndDie();
                }
                else UnsummonCheck -= diff;

                return;
            }

            if (Revenge_Timer < diff)
            {
                if (Unit *victim = me->GetVictim())
                    DoCast(victim, SPELL_REVENGE);

                Revenge_Timer = 5000;
            }
            else Revenge_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };


    CreatureAI* GetAI(Creature* creature) const
        override {
        return new boss_vazrudenAI(creature);
    }
};

class boss_vazruden_the_herald : public CreatureScript
{
public:
    boss_vazruden_the_herald() : CreatureScript("boss_vazruden_the_herald") { }

    enum messages
    {
        MESSAGE_SENTRY_DIED = 0,
    };

    enum phases
    {
        PHASE_CIRCLING, // circle around the platform
        PHASE_INITIATING_FIGHT, // go to the middle and begin the fight
        PHASE_INVISIBLE_CONTROLLER,  // adds do the job now
    };

    struct boss_vazruden_the_heraldAI : public ScriptedAI
    {
        boss_vazruden_the_heraldAI(Creature *c) : ScriptedAI(c)
        {
            summoned = false;
            NazanGUID = 0;
            VazrudenGUID = 0;
            HeroicMode = me->GetMap() ? me->GetMap()->IsHeroic() : true;
        }

        uint32 phase;
        uint32 checkTimer;
        uint64 NazanGUID;
        uint64 VazrudenGUID;
        bool summoned;
        bool HeroicMode;
        bool lootSpawned;

        void Reset()
            override {
            phase = PHASE_CIRCLING;
            checkTimer = 0;
            UnsummonAdds();
            me->SetDisableGravity(true);
            me->SetSpeedRate(MOVE_FLIGHT, 2.5);
            me->GetMotionMaster()->MovePath(PATH_ENTRY);
            lootSpawned = false;
        }

        void UnsummonAdds()
        {
            if (summoned) 
            {
                Creature *Nazan = me->GetMap()->GetCreature(NazanGUID);
                Creature *Vazruden = me->GetMap()->GetCreature(VazrudenGUID);
                if (Nazan || (Nazan = FindCreature(NPC_NAZAN, 500.0f, me))) 
                {
                    Nazan->DisappearAndDie();
                    NazanGUID = 0;
                }
                if (Vazruden || (Vazruden = FindCreature(NPC_VAZRUDEN, 500.0f, me))) 
                {
                    Vazruden->DisappearAndDie();
                    VazrudenGUID = 0;
                }
                summoned = false;
                me->ClearUnitState(UNIT_STATE_ROOT);
                me->SetVisibility(VISIBILITY_ON);
            }
        }

        void SummonAdds()
        {
            if (!summoned) {
                if(Creature* Vazruden = me->SummonCreature(NPC_VAZRUDEN, POSITION_GET_X_Y_Z(&VazrudenMiddle), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1 * HOUR * IN_MILLISECONDS))
                    VazrudenGUID = Vazruden->GetGUID();

                if(Creature* Nazan = me->SummonCreature(NPC_NAZAN, POSITION_GET_X_Y_Z(&VazrudenMiddle), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1 * HOUR * IN_MILLISECONDS))
                    NazanGUID = Nazan->GetGUID();

                summoned = true;
                me->SetVisibility(VISIBILITY_OFF);
                me->AddUnitState(UNIT_STATE_ROOT);
            }
        }

        void EnterCombat(Unit *who)
            override {
            if (phase == PHASE_CIRCLING) {
                phase = PHASE_INITIATING_FIGHT;
                checkTimer = 0;
                DoScriptText(SAY_INTRO, me);
            }
        }

        void JustSummoned(Creature *summoned)
            override {
            if (!summoned) return;

            Unit *victim = me->GetVictim();
            if (summoned->GetEntry() == NPC_NAZAN) 
            {
                summoned->AI()->message(boss_nazan::MESSAGE_SET_VAZRUDEN_GUID, VazrudenGUID);
                summoned->SetDisableGravity(true);
                summoned->SetSpeedRate(MOVE_FLIGHT, 2.5);
                if (victim)
                    ((ScriptedAI*)summoned->AI())->AttackStart(victim, false);
            }
            else if (victim)
                summoned->AI()->AttackStart(victim);
        }

        uint64 message(uint32 id, uint64 data) override
        {
            switch (id)
            {
                case MESSAGE_SENTRY_DIED:
                {
                    bool aliveSentry = me->FindCreatureInGrid(NPC_HELLFIRE_SENTRY, 150.0f, true);
                    if (!aliveSentry)
                        if (Unit* killer = me->GetMap()->GetPlayer(data))
                            AttackStart(killer);
                } break;
            }
            return 0;
        }

        void UpdateAI(const uint32 diff)
            override {
            switch (phase)
            {
            case PHASE_CIRCLING: 
                return;
            case PHASE_INITIATING_FIGHT:
                if (checkTimer < diff) {
                    if (me->GetDistance(POSITION_GET_X_Y_Z(&VazrudenMiddle)) > 5.0f)
                    {
                        me->GetMotionMaster()->Clear();
                        me->SetWalk(false);
                        me->GetMotionMaster()->MovePoint(0, POSITION_GET_X_Y_Z(&VazrudenMiddle), 0.0f, false, true);
                        checkTimer = 1000;
                    }
                    else {
                        SummonAdds();
                        phase = PHASE_INVISIBLE_CONTROLLER;
                        return;
                    }
                }
                else 
                    checkTimer -= diff;
                break;
            case PHASE_INVISIBLE_CONTROLLER:
            default:
                if (checkTimer < diff) {
                    Creature *Nazan = me->GetMap()->GetCreature(NazanGUID);
                    Creature *Vazruden = me->GetMap()->GetCreature(VazrudenGUID);
                    if ((Nazan && Nazan->IsAlive()) || (Vazruden && Vazruden->IsAlive())) 
                    {
                        if ((Nazan && Nazan->GetVictim()) || (Vazruden && Vazruden->GetVictim()))
                            return;
                        else
                        {
                            UnsummonAdds();
                            EnterEvadeMode();
                            return;
                        }
                    }
                    else if (!lootSpawned) {
                        me->SummonGameObject(GOBJECT_REINFORCED_FEL_IRON_CHEST, VazrudenMiddle, G3D::Quat(), 0);
                        me->DisappearAndDie();
                        lootSpawned = true;
                    }
                    checkTimer = 2000;
                }
                else checkTimer -= diff;
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
        override {
        return new boss_vazruden_the_heraldAI(creature);
    }
};

class mob_hellfire_sentry : public CreatureScript
{
public:
    mob_hellfire_sentry() : CreatureScript("mob_hellfire_sentry") { }

    struct mob_hellfire_sentryAI : public ScriptedAI
    {
        mob_hellfire_sentryAI(Creature *c) : ScriptedAI(c) {}

        uint32 KidneyShot_Timer;

        void Reset()
            override {
            KidneyShot_Timer = urand(3000, 7000);
        }

        void JustDied(Unit* who)
            override 
        {
            who = who->GetCharmerOrOwnerPlayerOrPlayerItself();
            if (Creature *herald = me->FindNearestCreature(NPC_VAZRUDEN_HERALD, 150.0f, true))
                herald->AI()->message(boss_vazruden_the_herald::MESSAGE_SENTRY_DIED, who->GetGUID());
        }

        void UpdateAI(const uint32 diff)
            override 
        {
            if (!UpdateVictim())
                return;

            if (KidneyShot_Timer < diff) {
                if (Unit *victim = me->GetVictim())
                    DoCast(victim, SPELL_KIDNEY_SHOT);
                KidneyShot_Timer = 20000;
            }
            else KidneyShot_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature *pCreature) const 
        override
    {
        return new mob_hellfire_sentryAI(pCreature);
    }
};

void AddSC_boss_vazruden_the_herald()
{
    new boss_vazruden_the_herald();
    new mob_hellfire_sentry();
    new boss_nazan();
    new boss_vazruden();
}
