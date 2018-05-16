#include "../../ClassSpellsDamage.h"
#include "../../ClassSpellsCoeff.h"
#include "SpellHistory.h"

class UnbreakableWillTest : public TestCaseScript
{
public:

    UnbreakableWillTest() : TestCaseScript("talents priest unbreakable_will") { }

    class UnbreakableWillTestImpt : public TestCase
    {
    public:
        /*
        Bugs: Not sure, feels like the special attacks hit is wrong
        */
        UnbreakableWillTestImpt() : TestCase(STATUS_WIP) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            TestPlayer* warlock = SpawnPlayer(CLASS_WARLOCK, RACE_HUMAN);
            TestPlayer* warrior = SpawnPlayer(CLASS_WARRIOR, RACE_HUMAN);
            TestPlayer* enemyPriest = SpawnPlayer(CLASS_PRIEST, RACE_HUMAN);

            LearnTalent(priest, Talents::Priest::UNBREAKABLE_WILL_RNK_5);
            float const talentResistFactor = 15.f;
            float const expectedSpellResist = talentResistFactor + 3.f; // 3% is required to be spell hit capped in PvP
            float const expectedMeleeResist = talentResistFactor + 5.f; // 5% is required to be special attacks capped in PvP

            TEST_SPELL_HIT_CHANCE_CALLBACK(warlock, priest, ClassSpells::Warlock::FEAR_RNK_3, expectedSpellResist, SPELL_MISS_RESIST, [](Unit* caster, Unit* victim) {
                victim->ClearDiminishings();
                victim->RemoveAurasDueToSpell(ClassSpells::Warlock::FEAR_RNK_3);
            });
            TEST_SPELL_HIT_CHANCE_CALLBACK(warrior, priest, Talents::Warrior::CONCUSSION_BLOW_RNK_1, expectedMeleeResist, SPELL_MISS_RESIST, [](Unit* caster, Unit* victim) {
                victim->ClearDiminishings();
                victim->RemoveAurasDueToSpell(Talents::Warrior::CONCUSSION_BLOW_RNK_1);
            });
            TEST_SPELL_HIT_CHANCE_CALLBACK(enemyPriest, priest, ClassSpells::Priest::SILENCE_RNK_1, expectedSpellResist, SPELL_MISS_RESIST, [](Unit* caster, Unit* victim) {
                victim->ClearDiminishings();
                victim->RemoveAurasDueToSpell(ClassSpells::Priest::SILENCE_RNK_1);
            });
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<UnbreakableWillTestImpt>();
    }
};

class PriestWandSpecializationTest : public TestCaseScript
{
public:
    PriestWandSpecializationTest() : TestCaseScript("talents priest wand_specialization") { }

    class WandSpecializationTestImpt : public TestCase
    {
    public:
        WandSpecializationTestImpt() : TestCase(STATUS_KNOWN_BUG) { } // No boost

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_HUMAN);
            Creature* dummy = SpawnCreature();

            EQUIP_NEW_ITEM(priest, 28783); // Eredar Wand of Obliteration, 177 - 330 Shadow Damage

            LearnTalent(priest, Talents::Priest::WAND_SPECIALIZATION_RNK_5);
            float const talentFactor = 1.25f;

            uint32 const expectedWandMin = 177 * talentFactor;
            uint32 const expectedWandMax = 330 * talentFactor;
            TEST_DIRECT_SPELL_DAMAGE(priest, dummy, ClassSpells::Priest::WAND, expectedWandMin, expectedWandMax, false);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<WandSpecializationTestImpt>();
    }
};

class SilentResolve : public TestCaseScript
{
public:
    SilentResolve() : TestCaseScript("talents priest silent_resolve") { }

    class WandSpecializationTestImpt : public TestCase
    {
    public:
        /*
        Bugs:
            - Mind Flay and Vampiric Embrace not affected by the dispell resist.
        */
        WandSpecializationTestImpt() : TestCase(STATUS_WIP) { } // Waiting TEST_DISPEL_RESIST_CHANCE to be able to handle other dispel types -> DONE

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_HUMAN);
            _location.MoveInFront(_location, 5.0f);
            TestPlayer* enemy = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            Creature* dummy = SpawnCreature();

            LearnTalent(priest, Talents::Priest::SILENT_RESOLVE_RNK_5);
            float const threatReductionFactor = 1.0f - 0.2f;
            float const healThreatFactor = 0.5f * threatReductionFactor;
            const float dispelTalentFactor = 20.f;

            // Threat of Discipline and Holy
            // TODO: Power Word: Shield
            //TEST_DIRECT_HEAL_THREAT(priest, dummy, ClassSpells::Priest::BINDING_HEAL_RNK_1, 0.5f * healThreatFactor);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::STARSHARDS_RNK_8, threatReductionFactor, false);
            ////TEST_DIRECT_SPELL_THREAT(priest, dummy, ClassSpells::Priest::CHASTISE_RNK_6, threatFactor);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::CIRCLE_OF_HEALING_RNK_5, healThreatFactor, true);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::DESPERATE_PRAYER_RNK_8, healThreatFactor, true);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::FLASH_HEAL_RNK_9, healThreatFactor, true);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::GREATER_HEAL_RNK_7, healThreatFactor, true);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::HEAL_RNK_4, healThreatFactor, true);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::HOLY_FIRE_RNK_9, threatReductionFactor, false);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::LESSER_HEAL_RNK_3, healThreatFactor, true);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::PRAYER_OF_HEALING_RNK_6, healThreatFactor, true);
            ////TEST_DIRECT_HEAL_THREAT(priest, dummy, ClassSpells::Priest::PRAYER_OF_MENDING_RNK_1, healThreatFactor);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::RENEW_RNK_12, healThreatFactor, true);
            //TEST_THREAT(priest, dummy, ClassSpells::Priest::SMITE_RNK_10, threatReductionFactor, false);

            // Resist dispell of all spells
            // Disc
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::DIVINE_SPIRIT_RNK_5, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::ELUNES_GRACE_RNK_1, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::FEAR_WARD_RNK_1, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::INNER_FIRE_RNK_7, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, Talents::Priest::INNER_FOCUS_RNK_1, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::LEVITATE_RNK_1, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::PAIN_SUPPRESSION_RNK_1, dispelTalentFactor + 65.f);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::POWER_INFUSION_RNK_1, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::POWER_WORD_FORTITUDE_RNK_7, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE_CALLBACK(priest, priest, enemy, ClassSpells::Priest::POWER_WORD_SHIELD_RNK_12, dispelTalentFactor); // need callback to remove aura
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::PRAYER_OF_FORTITUDE_RNK_3, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::PRAYER_OF_SPIRIT_RNK_2, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::STARSHARDS_RNK_8, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::SYMBOL_OF_HOPE_RNK_1, dispelTalentFactor);
            // Holy
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::HOLY_FIRE_RNK_9, dispelTalentFactor);
            // Shadow
            TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::DEVOURING_PLAGUE_RNK_7, dispelTalentFactor); // Disease
            TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::HEX_OF_WEAKNESS_RNK_7, dispelTalentFactor); // Curse
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::MIND_FLAY_RNK_7, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::PRAYER_OF_SHADOW_PROTECTION_RNK_2, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::SHADOW_PROTECTION_RNK_4, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::PSYCHIC_SCREAM_RNK_4, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::SHADOW_WORD_PAIN_RNK_10, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::SHADOW_GUARD_RNK_7, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, ClassSpells::Priest::TOUCH_OF_WEAKNESS_RNK_7, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::VAMPIRIC_EMBRACE_RNK_1, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::VAMPIRIC_TOUCH_RNK_3, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, ClassSpells::Priest::SILENCE_RNK_1, dispelTalentFactor);
            // Talents procs - We suppose that this talent affects other talents procs such as:
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, Talents::Priest::MARTYRDOM_RNK_2_TRIGGER, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, Talents::Priest::INSPIRATION_RNK_3_TRIGGER, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, Talents::Priest::FOCUSED_WILL_RNK_3_TRIGGER, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, priest, enemy, Talents::Priest::HOLY_CONCENTRATION_RNK_3_TRIGGER, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, Talents::Priest::SHADOW_WEAVING_RNK_5_PROC, dispelTalentFactor);
            //TEST_DISPEL_RESIST_CHANCE(priest, enemy, enemy, Talents::Priest::BLACKOUT_RNK_5_TRIGGER, dispelTalentFactor);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<WandSpecializationTestImpt>();
    }
};

class ImprovedPowerWordFortitudeTest : public TestCaseScript
{
public:

    ImprovedPowerWordFortitudeTest() : TestCaseScript("talents priest improved_power_word_fortitude") { }

    class ImprovedPowerWordFortitudeTestImpt : public TestCase
    {
    public:
        ImprovedPowerWordFortitudeTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            TestPlayer* warrior = SpawnPlayer(CLASS_WARRIOR, RACE_ORC);

            LearnTalent(priest, Talents::Priest::IMPROVED_POWER_WORD_FORTITUDE_RNK_2);
            float const talentFactor = 1.3f;

            uint32 powerWordFortitudeBonus = 79 * talentFactor;
            uint32 expectedMaxHealth = warrior->GetMaxHealth() + powerWordFortitudeBonus * 10;

            // Power Word: Fortitude
            TEST_CAST(priest, warrior, ClassSpells::Priest::POWER_WORD_FORTITUDE_RNK_7);
            TEST_ASSERT(warrior->GetMaxHealth() == expectedMaxHealth);

            // Prayer of Fortitude
            GroupPlayer(priest, warrior);
            TEST_CAST(priest, warrior, ClassSpells::Priest::PRAYER_OF_FORTITUDE_RNK_3, SPELL_CAST_OK, TRIGGERED_FULL_MASK);
            TEST_ASSERT(warrior->GetMaxHealth() == expectedMaxHealth);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<ImprovedPowerWordFortitudeTestImpt>();
    }
};

class ImprovedPowerWordShieldTest : public TestCaseScript
{
public:

    ImprovedPowerWordShieldTest() : TestCaseScript("talents priest improved_power_word_shield") { }

    class ImprovedPowerWordShieldTestImpt : public TestCase
    {
    public:
        ImprovedPowerWordShieldTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            TestPlayer* rogue = SpawnPlayer(CLASS_ROGUE, RACE_ORC);

            LearnTalent(priest, Talents::Priest::IMPROVED_POWER_WORD_SHIELD_RNK_3);
            float const talentFactor = 1.15f;

            int32 expectedAbsorb = ClassSpellsDamage::Priest::POWER_WORD_SHIELD_RNK_12 * talentFactor;
            TEST_CAST(priest, priest, ClassSpells::Priest::POWER_WORD_SHIELD_RNK_12);

            // Step 1 - test theoritical amount
            Aura const* shield = priest->GetAura(ClassSpells::Priest::POWER_WORD_SHIELD_RNK_12);
            TEST_ASSERT(shield != nullptr);
            AuraEffect const* absorbEffect = shield->GetEffect(EFFECT_0);
            int32 const absorbAmount = absorbEffect->GetAmount();
            ASSERT_INFO("absorbAmount %u != expectedAbsorb %u", absorbAmount, expectedAbsorb);
            TEST_ASSERT(absorbAmount == expectedAbsorb);

            // Step 2 - Test with real damage
            priest->SetFullHealth();
            auto AI = rogue->GetTestingPlayerbotAI();
            rogue->ForceMeleeHitResult(MELEE_HIT_NORMAL);
            int32 totalDamage = 0;
            for (uint32 i = 0; i < 150; i++)
            {
                rogue->AttackerStateUpdate(priest, BASE_ATTACK);
                auto damageToTarget = AI->GetMeleeDamageDoneInfo(priest);

                TEST_ASSERT(damageToTarget->size() == i + 1);
                auto& data = damageToTarget->back();

                for (uint8 j = 0; j < MAX_ITEM_PROTO_DAMAGES; j++)
                {
                    totalDamage += data.damageInfo.Damages[j].Damage;
                    totalDamage += data.damageInfo.Damages[j].Resist;
                    totalDamage += data.damageInfo.Damages[j].Absorb;
                }
                totalDamage += data.damageInfo.Blocked;

                if (totalDamage < expectedAbsorb)
                {
                    TEST_HAS_AURA(priest, ClassSpells::Priest::POWER_WORD_SHIELD_RNK_12); //if this fails, shield did not absorb enough 
                    continue;
                }

                TEST_HAS_NOT_AURA(priest, ClassSpells::Priest::POWER_WORD_SHIELD_RNK_12);
                uint32 expectedHealth = priest->GetMaxHealth() - (totalDamage - expectedAbsorb);
                TEST_ASSERT(priest->GetHealth() == expectedHealth);
                break;
            }
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<ImprovedPowerWordShieldTestImpt>();
    }
};

class MartyrdomTest : public TestCaseScript
{
public:

    MartyrdomTest() : TestCaseScript("talents priest martyrdom") { }

    class MartyrdomTestImpt : public TestCase
    {
    public:
        MartyrdomTestImpt() : TestCase(STATUS_WIP) { } // Waiting for TestCase::_GetPercentApproximationParams to handle an expected chance of 100%

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            TestPlayer* shaman = SpawnPlayer(CLASS_SHAMAN, RACE_DRAENEI);

            LearnTalent(priest, Talents::Priest::MARTYRDOM_RNK_2);
            float const talentResistPushbackFactor = 100.f;
            float const talentResistInterruptFactor = 20.f;
            float const expectedResist = talentResistInterruptFactor + 3.f; // 3% is required to be spell hit capped in PvP

            shaman->ForceMeleeHitResult(MELEE_HIT_CRIT);
            shaman->AttackerStateUpdate(priest, BASE_ATTACK);
            shaman->AttackStop();
            TEST_AURA_MAX_DURATION(priest, Talents::Priest::MARTYRDOM_RNK_2_TRIGGER, Seconds(6));
            shaman->ResetForceMeleeHitResult();

            // No pushback
            TEST_PUSHBACK_RESIST_CHANCE(priest, shaman, ClassSpells::Priest::GREATER_HEAL_RNK_7, talentResistPushbackFactor);

            // 20% resist to interrupt
            TEST_SPELL_HIT_CHANCE_CALLBACK(shaman, priest, ClassSpells::Shaman::EARTH_SHOCK_RNK_8, expectedResist, SPELL_MISS_RESIST, [](Unit* caster, Unit* victim) {
                victim->AddAura(Talents::Priest::MARTYRDOM_RNK_2_TRIGGER, victim);
                victim->CastSpell(victim, ClassSpells::Priest::GREATER_HEAL_RNK_7, TRIGGERED_IGNORE_POWER_AND_REAGENT_COST);
            });
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<MartyrdomTestImpt>();
    }
};

class AbsolutionTest : public TestCaseScript
{
public:

    AbsolutionTest() : TestCaseScript("talents priest absolution") { }

    class AbsolutionTestImpt : public TestCase
    {
    public:
        AbsolutionTestImpt() : TestCase(STATUS_WIP) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);

            LearnTalent(priest, Talents::Priest::ABSOLUTION_RNK_3);
            float const talentFactor = 1 - 0.15f;

            TEST_POWER_COST(priest, ClassSpells::Priest::DISPEL_MAGIC_RNK_2, POWER_MANA, 366 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::CURE_DISEASE_RNK_1, POWER_MANA, 314 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::ABOLISH_DISEASE_RNK_1, POWER_MANA, 314 * talentFactor);
            uint32 const baseMana = priest->GetMaxPower(POWER_MANA) - priest->GetManaBonusFromIntellect();
            float const massDispelMana = baseMana * 0.33f;
            TEST_POWER_COST(priest, ClassSpells::Priest::MASS_DISPEL_RNK_1, POWER_MANA, massDispelMana * talentFactor);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<AbsolutionTestImpt>();
    }
};

class InnerFocusTest : public TestCaseScript
{
public:

    InnerFocusTest() : TestCaseScript("talents priest inner_focus") { }

    class InnerFocusTestImpt : public TestCase
    {
    public:
        InnerFocusTestImpt() : TestCase(STATUS_KNOWN_BUG) { } // Crit chance fails

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            Creature* dummy = SpawnCreature();
            float const talentFactor = 25.f;
            float const expectedSpellCritChance = priest->GetFloatValue(PLAYER_CRIT_PERCENTAGE) + talentFactor;

            // CD
            TEST_CAST(priest, priest, ClassSpells::Priest::INNER_FOCUS_RNK_1);
            TEST_HAS_AURA(priest, ClassSpells::Priest::INNER_FOCUS_RNK_1);
            TEST_POWER_COST(priest, ClassSpells::Priest::SHADOW_WORD_PAIN_RNK_10, POWER_MANA, uint32(0));
            TEST_CAST(priest, dummy, ClassSpells::Priest::SHADOW_WORD_PAIN_RNK_10);
            TEST_HAS_COOLDOWN(priest, ClassSpells::Priest::INNER_FOCUS_RNK_1, Minutes(3));

            // Crit chance
            TEST_SPELL_CRIT_CHANCE_CALLBACK(priest, dummy, ClassSpells::Priest::SMITE_RNK_10, expectedSpellCritChance, [](Unit* caster, Unit* target) {
                caster->AddAura(ClassSpells::Priest::INNER_FOCUS_RNK_1, caster);
            });
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<InnerFocusTestImpt>();
    }
};

class MeditationTest : public TestCaseScript
{
public:

    MeditationTest() : TestCaseScript("talents priest meditation") { }

    class MeditationTestImpt : public TestCase
    {
    public:
        MeditationTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);

            float const talentFactor = 0.3f;
            float const expectedManaRegen = priest->GetFloatValue(PLAYER_FIELD_MOD_MANA_REGEN) * talentFactor;

            LearnTalent(priest, Talents::Priest::MEDITATION_RNK_3);
            ASSERT_INFO("Priest should have %f mana regen but has %f.", expectedManaRegen, priest->GetFloatValue(PLAYER_FIELD_MOD_MANA_REGEN_INTERRUPT));
            TEST_ASSERT(Between<float>(priest->GetFloatValue(PLAYER_FIELD_MOD_MANA_REGEN_INTERRUPT), expectedManaRegen - 0.1f, expectedManaRegen + 0.1));
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<MeditationTestImpt>();
    }
};

class ImprovedInnerFireTest : public TestCaseScript
{
public:

    ImprovedInnerFireTest() : TestCaseScript("talents priest improved_inner_fire") { }

    class ImprovedInnerFireTestImpt : public TestCase
    {
    public:
        ImprovedInnerFireTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);

            float const talentFactor = 1.3f;
            uint32 const innerFireBonus = 1580 * talentFactor;
            uint32 const expectedArmor = priest->GetArmor() + innerFireBonus;

            LearnTalent(priest, Talents::Priest::IMPROVED_INNER_FIRE_RNK_3);
            TEST_CAST(priest, priest, ClassSpells::Priest::INNER_FIRE_RNK_7);
            ASSERT_INFO("Priest should have %f armor but has %f.", expectedArmor, priest->GetArmor());
            TEST_ASSERT(priest->GetArmor() == expectedArmor);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<ImprovedInnerFireTestImpt>();
    }
};

class MentalAgilityTest : public TestCaseScript
{
public:

    MentalAgilityTest() : TestCaseScript("talents priest mental_agility") { }

    class MentalAgilityTestImpt : public TestCase
    {
    public:
        MentalAgilityTestImpt() : TestCase(STATUS_WIP) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);

            float const talentFactor = 1 - 0.1f;
            uint32 const baseMana = priest->GetMaxPower(POWER_MANA) - priest->GetManaBonusFromIntellect();

            LearnTalent(priest, Talents::Priest::MENTAL_STRENGTH_RNK_5);
            WaitNextUpdate();
            // Discipline
            TEST_POWER_COST(priest, ClassSpells::Priest::DISPEL_MAGIC_RNK_2, POWER_MANA, baseMana * 0.14f * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::DIVINE_SPIRIT_RNK_5, POWER_MANA, 680 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::ELUNES_GRACE_RNK_1, POWER_MANA, baseMana * 0.03f * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::FEAR_WARD_RNK_1, POWER_MANA, baseMana * 0.03f * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::FEEDBACK_RNK_6, POWER_MANA, 705 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::INNER_FIRE_RNK_7, POWER_MANA, 375 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::LEVITATE_RNK_1, POWER_MANA, 100 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::MANA_BURN_RNK_7, POWER_MANA, 355 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::MASS_DISPEL_RNK_1, POWER_MANA, baseMana * 0.33f * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::POWER_INFUSION_RNK_1, POWER_MANA, baseMana * 0.16f * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::POWER_WORD_FORTITUDE_RNK_7, POWER_MANA, 700 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::POWER_WORD_SHIELD_RNK_12, POWER_MANA, 600 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::PRAYER_OF_FORTITUDE_RNK_3, POWER_MANA, 1800 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::PRAYER_OF_SPIRIT_RNK_2, POWER_MANA, 1800 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::SYMBOL_OF_HOPE_RNK_1, POWER_MANA, 15 * talentFactor);
            // Holy
            TEST_POWER_COST(priest, ClassSpells::Priest::ABOLISH_DISEASE_RNK_1, POWER_MANA, baseMana * 0.12f * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::CHASTISE_RNK_6, POWER_MANA, 300 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::CIRCLE_OF_HEALING_RNK_5, POWER_MANA, 450 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::CURE_DISEASE_RNK_1, POWER_MANA, baseMana * 0.12f * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::HOLY_NOVA_RNK_7, POWER_MANA, 875 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::PRAYER_OF_MENDING_RNK_1, POWER_MANA, 390 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::RENEW_RNK_12, POWER_MANA, 450 * talentFactor);
            // Shadow
            TEST_POWER_COST(priest, ClassSpells::Priest::DEVOURING_PLAGUE_RNK_7, POWER_MANA, 1145 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::FADE_RNK_7, POWER_MANA, 330 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::HEX_OF_WEAKNESS_RNK_7, POWER_MANA, 295 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::MIND_SOOTHE_RNK_4, POWER_MANA, 120 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::PRAYER_OF_SHADOW_PROTECTION_RNK_2, POWER_MANA, 1620 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::PSYCHIC_SCREAM_RNK_4, POWER_MANA, 210 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::SHADOW_PROTECTION_RNK_4, POWER_MANA, 810 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::SHADOW_WORD_DEATH_RNK_2, POWER_MANA, 309 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::SHADOW_WORD_PAIN_RNK_10, POWER_MANA, 575 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::SHADOWFIEND_RNK_1, POWER_MANA, baseMana * 0.06f * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::SHADOW_GUARD_RNK_7, POWER_MANA, 270 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::TOUCH_OF_WEAKNESS_RNK_7, POWER_MANA, 235 * talentFactor);
            TEST_POWER_COST(priest, ClassSpells::Priest::VAMPIRIC_EMBRACE_RNK_1, POWER_MANA, baseMana * 0.02f * talentFactor);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<MentalAgilityTestImpt>();
    }
};

class ImprovedManaBurnTest : public TestCaseScript
{
public:

    ImprovedManaBurnTest() : TestCaseScript("talents priest improved_mana_burn") { }

    class ImprovedManaBurnTestImpt : public TestCase
    {
    public:
        ImprovedManaBurnTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);

            LearnTalent(priest, Talents::Priest::IMPROVED_MANA_BURN_RNK_2);
            TEST_CAST_TIME(priest, ClassSpells::Priest::MANA_BURN_RNK_7, 2000);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<ImprovedManaBurnTestImpt>();
    }
};

class MentalStrengthTest : public TestCaseScript
{
public:

    MentalStrengthTest() : TestCaseScript("talents priest mental_strength") { }

    class MentalStrengthTestImpt : public TestCase
    {
    public:
        MentalStrengthTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            float const talentFactor = 1.1f;

            uint32 expectedMaxMana = priest->GetMaxPower(POWER_MANA) * talentFactor;

            LearnTalent(priest, Talents::Priest::MENTAL_STRENGTH_RNK_5);
            ASSERT_INFO("Priest has %u max MP, should have %u.", priest->GetMaxPower(POWER_MANA), expectedMaxMana);
            TEST_ASSERT(Between<uint32>(priest->GetMaxPower(POWER_MANA), expectedMaxMana - 1, expectedMaxMana + 1));
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<MentalStrengthTestImpt>();
    }
};

class DivineSpiritTest : public TestCaseScript
{
public:

    DivineSpiritTest() : TestCaseScript("talents priest divine_spirit") { }

    class DivineSpiritTestImpt : public TestCase
    {
    public:
        DivineSpiritTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);

            uint32 expectedSpirit = priest->GetStat(STAT_SPIRIT) + 50;

            TEST_CAST(priest, priest, ClassSpells::Priest::DIVINE_SPIRIT_RNK_5);
            TEST_ASSERT(priest->GetStat(STAT_SPIRIT) == expectedSpirit);

            TEST_AURA_MAX_DURATION(priest, ClassSpells::Priest::DIVINE_SPIRIT_RNK_5, Minutes(30));

            TEST_POWER_COST(priest, ClassSpells::Priest::DIVINE_SPIRIT_RNK_5, POWER_MANA, 680);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<DivineSpiritTestImpt>();
    }
};

class ImprovedDivineSpiritTest : public TestCaseScript
{
public:

    ImprovedDivineSpiritTest() : TestCaseScript("talents priest improved_divine_spirit") { }

    class ImprovedDivineSpiritTestImpt : public TestCase
    {
    public:
        ImprovedDivineSpiritTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);

            float const spirit = priest->GetStat(STAT_SPIRIT) + 50;

            EQUIP_NEW_ITEM(priest, 34335); // Hammer of Sanctification -- 183 SP & 550 BH
            uint32 const bonusHeal = priest->GetInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS);
            TEST_ASSERT(bonusHeal == 550);
            uint32 const spellPower = priest->GetInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW);
            TEST_ASSERT(spellPower == 183);

            float const talentFactor = 0.1f;
            uint32 expectedBH = bonusHeal + spirit * talentFactor;
            uint32 expectedSP = spellPower + spirit * talentFactor;

            LearnTalent(priest, Talents::Priest::IMPROVED_DIVINE_SPIRIT_RNK_2);
            TEST_CAST(priest, priest, ClassSpells::Priest::DIVINE_SPIRIT_RNK_5);
            TEST_ASSERT(priest->GetInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS) == int32(expectedBH));
            TEST_ASSERT(priest->GetInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW) == int32(expectedSP));

            TEST_CAST(priest, priest, ClassSpells::Priest::PRAYER_OF_SPIRIT_RNK_2, SPELL_CAST_OK, TRIGGERED_FULL_MASK);
            TEST_ASSERT(priest->GetInt32Value(PLAYER_FIELD_MOD_HEALING_DONE_POS) == int32(expectedBH));
            TEST_ASSERT(priest->GetInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW) == int32(expectedSP));
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<ImprovedDivineSpiritTestImpt>();
    }
};

class FocusedPowerTest : public TestCaseScript
{
public:

    FocusedPowerTest() : TestCaseScript("talents priest focused_power") { }

    class FocusedPowerTestImpt : public TestCase
    {
    public:
        FocusedPowerTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            Creature* dummy = SpawnCreature(12); // Boss

            LearnTalent(priest, Talents::Priest::FOCUSED_POWER_RNK_2);
            float const hitTalentFactor = 4.0f;
            float const hitChance = 16.0f - hitTalentFactor;

            // Hit chance
            TEST_SPELL_HIT_CHANCE(priest, dummy, ClassSpells::Priest::SMITE_RNK_10, hitChance, SPELL_MISS_RESIST);
            TEST_SPELL_HIT_CHANCE(priest, dummy, ClassSpells::Priest::MIND_BLAST_RNK_11, hitChance, SPELL_MISS_RESIST);
            TEST_SPELL_HIT_CHANCE_CALLBACK(priest, dummy, ClassSpells::Priest::MASS_DISPEL_RNK_1, hitChance, SPELL_MISS_RESIST, [](Unit* caster, Unit* target) {
                target->AddAura(ClassSpells::Priest::POWER_WORD_FORTITUDE_RNK_7, target);
            });

            // Mass Dispel Cast Time
            TEST_CAST_TIME(priest, ClassSpells::Priest::MASS_DISPEL_RNK_1, 500);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<FocusedPowerTestImpt>();
    }
};

class ForceOfWillTest : public TestCaseScript
{
public:

    ForceOfWillTest() : TestCaseScript("talents priest force_of_will") { }

    class ForceOfWillTestImpt : public TestCase
    {
    public:
        /*
        Bugs:
            - Damages: Holy Fire's Dot, Shadow Word: Death
            - Crit: Smite, Holy Fire, Holy Nova, Mind Blast and Shadow Word: Death
        */
        ForceOfWillTestImpt() : TestCase(STATUS_WIP) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            Creature* dummy = SpawnCreature();

            float const talentDamageFactor = 1.05f;
            float const talentCritFactor = 5.f;

            float const expectedSpellCritChance = priest->GetFloatValue(PLAYER_CRIT_PERCENTAGE) + talentCritFactor;

            LearnTalent(priest, Talents::Priest::FORCE_OF_WILL_RNK_5);
            // SP
            // Smite
            uint32 const expectedSmiteMin = ClassSpellsDamage::Priest::SMITE_RNK_10_MIN * talentDamageFactor;
            uint32 const expectedSmiteMax = ClassSpellsDamage::Priest::SMITE_RNK_10_MAX * talentDamageFactor;
            TEST_DIRECT_SPELL_DAMAGE(priest, dummy, ClassSpells::Priest::SMITE_RNK_10, expectedSmiteMin, expectedSmiteMax, false);
            // Holy Fire -- Direct
            uint32 const expectedHolyFireMin = ClassSpellsDamage::Priest::HOLY_FIRE_RNK_9_MIN * talentDamageFactor;
            uint32 const expectedHolyFireMax = ClassSpellsDamage::Priest::HOLY_FIRE_RNK_9_MAX * talentDamageFactor;
            //TEST_DIRECT_SPELL_DAMAGE(priest, dummy, ClassSpells::Priest::HOLY_FIRE_RNK_9, expectedHolyFireMin, expectedHolyFireMax, false);
            // Holy Fire -- Dot
            uint32 const holyFireDoT = ClassSpellsDamage::Priest::HOLY_FIRE_RNK_9_TOTAL * talentDamageFactor;
            //TEST_DOT_DAMAGE(priest, dummy, ClassSpells::Priest::HOLY_FIRE_RNK_9, holyFireDoT, true);
            // Holy Nova
            uint32 const expectedHolyNovaMin = ClassSpellsDamage::Priest::HOLY_NOVA_RNK_7_MIN_LVL_70 * talentDamageFactor;
            uint32 const expectedHolyNovaMax = ClassSpellsDamage::Priest::HOLY_NOVA_RNK_7_MAX_LVL_70 * talentDamageFactor;
            //TEST_DIRECT_SPELL_DAMAGE(priest, dummy, ClassSpells::Priest::HOLY_NOVA_RNK_7, expectedHolyNovaMin, expectedHolyNovaMax, false);
            // Mind Blast
            uint32 const expectedMindBlastMin = ClassSpellsDamage::Priest::MIND_BLAST_RNK_11_MIN * talentDamageFactor;
            uint32 const expectedMindBlastMax = ClassSpellsDamage::Priest::MIND_BLAST_RNK_11_MAX * talentDamageFactor;
            //TEST_DIRECT_SPELL_DAMAGE(priest, dummy, ClassSpells::Priest::MIND_BLAST_RNK_11, expectedMindBlastMin, expectedMindBlastMax, false);
            // SwD
            uint32 const expectedSwDMin = ClassSpellsDamage::Priest::SHADOW_WORD_DEATH_RNK_2_MIN * talentDamageFactor;
            uint32 const expectedSwDMax = ClassSpellsDamage::Priest::SHADOW_WORD_DEATH_RNK_2_MAX * talentDamageFactor;
            //TEST_DIRECT_SPELL_DAMAGE(priest, dummy, ClassSpells::Priest::SHADOW_WORD_DEATH_RNK_2, expectedSwDMin, expectedSwDMax, false);
            // SwP
            uint32 const expectedSwPDoT = ClassSpellsDamage::Priest::SHADOW_WORD_PAIN_RNK_10_TOTAL * talentDamageFactor;
            //TEST_DOT_DAMAGE(priest, dummy, ClassSpells::Priest::SHADOW_WORD_PAIN_RNK_10, expectedSwPDoT, false);
            // TODO: Vampiric Touch
            // TODO: Mind Flay

            // Crit
            //TEST_SPELL_CRIT_CHANCE(priest, dummy, ClassSpells::Priest::SMITE_RNK_10, expectedSpellCritChance);
            //TEST_SPELL_CRIT_CHANCE(priest, dummy, ClassSpells::Priest::HOLY_FIRE_RNK_9, expectedSpellCritChance);
            //TEST_SPELL_CRIT_CHANCE(priest, dummy, ClassSpells::Priest::HOLY_NOVA_RNK_7, expectedSpellCritChance);
            //TEST_SPELL_CRIT_CHANCE(priest, dummy, ClassSpells::Priest::MIND_BLAST_RNK_11, expectedSpellCritChance);
            //TEST_SPELL_CRIT_CHANCE(priest, dummy, ClassSpells::Priest::SHADOW_WORD_DEATH_RNK_2, expectedSpellCritChance);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<ForceOfWillTestImpt>();
    }
};

class FocusedWillTest : public TestCaseScript
{
public:

    FocusedWillTest() : TestCaseScript("talents priest focused_will") { }

    class FocusedWillTestImpt : public TestCase
    {
    public:
        FocusedWillTestImpt() : TestCase(STATUS_WIP) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            TestPlayer* shaman = SpawnPlayer(CLASS_SHAMAN, RACE_DRAENEI);

            LearnTalent(priest, Talents::Priest::FOCUSED_WILL_RNK_3);
            float const talentDamageTakenFactor = 0.96f;
            float const talentHealingBoostPerStack = 0.1f;

            shaman->ForceMeleeHitResult(MELEE_HIT_CRIT);
            for (uint8 i = 0; i < 5; i++)
                shaman->AttackerStateUpdate(priest, BASE_ATTACK);
            shaman->AttackStop();
            shaman->ResetForceMeleeHitResult();
            TEST_AURA_MAX_DURATION(priest, Talents::Priest::FOCUSED_WILL_RNK_3_TRIGGER, Seconds(8));
            Aura* aura = priest->GetAura(Talents::Priest::FOCUSED_WILL_RNK_3_TRIGGER);
            TEST_ASSERT(aura != nullptr);
            TEST_ASSERT(aura->GetStackAmount() == 3);

            // Damage reduced: melee MH & Earth Shock
            EQUIP_NEW_ITEM(shaman, 34165); // Fang of Kalecgos
            WaitNextUpdate();
            uint32 const weaponMinDmg = 113;
            uint32 const weaponMaxDmg = 211;
            float const weaponSpeed = 1.5f;
            float const AP = shaman->GetTotalAttackPowerValue(BASE_ATTACK);
            float const armorFactor = 1 - (priest->GetArmor() / (priest->GetArmor() + 10557.5f));
            uint32 const minMelee = floor(weaponMinDmg + AP / 14.f * weaponSpeed) * armorFactor * talentDamageTakenFactor;
            uint32 const maxMelee = floor(weaponMaxDmg + AP / 14.f * weaponSpeed) * armorFactor * talentDamageTakenFactor;
            TEST_MELEE_DAMAGE(shaman, priest, BASE_ATTACK, minMelee, maxMelee, false);
            uint32 const minEarthShock = ClassSpellsDamage::Shaman::EARTH_SHOCK_RNK_8_MIN * talentDamageTakenFactor;
            uint32 const maxEarthShock = ClassSpellsDamage::Shaman::EARTH_SHOCK_RNK_8_MAX * talentDamageTakenFactor;
            TEST_DIRECT_SPELL_DAMAGE(shaman, priest, ClassSpells::Shaman::EARTH_SHOCK_RNK_8, minEarthShock, maxEarthShock, false);

            // Healing increased
            uint8 auraStack = 0;
            if (priest->HasAura(Talents::Priest::FOCUSED_WILL_RNK_3_TRIGGER))
            {
                aura = priest->GetAura(Talents::Priest::FOCUSED_WILL_RNK_3_TRIGGER);
                TEST_ASSERT(aura != nullptr);
                auraStack = aura->GetStackAmount();
            }
            else
            {
                priest->AddAura(Talents::Priest::FOCUSED_WILL_RNK_3_TRIGGER, priest);
                auraStack = 1;
            }
            float const talentBoost = 1 + auraStack * talentHealingBoostPerStack;
            uint32 const minGreaterHeal = ClassSpellsDamage::Priest::GREATER_HEAL_RNK_7_MIN * talentBoost;
            uint32 const maxGreaterHeal = ClassSpellsDamage::Priest::GREATER_HEAL_RNK_7_MAX * talentBoost;
            TEST_DIRECT_HEAL(priest, priest, ClassSpells::Priest::GREATER_HEAL_RNK_7, minGreaterHeal, maxGreaterHeal, false);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<FocusedWillTestImpt>();
    }
};

class PowerInfusionTest : public TestCaseScript
{
public:

    PowerInfusionTest() : TestCaseScript("talents priest power_infusion") { }

    class PowerInfusionTestImpt : public TestCase
    {
    public:
        PowerInfusionTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);

            float const castingSpeedIncrease = 1.2f;
            float const manaCostReduction = 0.8f;

            TEST_CAST(priest, priest, ClassSpells::Priest::POWER_INFUSION_RNK_1);
            TEST_AURA_MAX_DURATION(priest, ClassSpells::Priest::POWER_INFUSION_RNK_1, Seconds(15));
            TEST_HAS_COOLDOWN(priest, ClassSpells::Priest::POWER_INFUSION_RNK_1, Minutes(3));

            // Haste: X% haste means to cast X additional spells in the time it would normally take to cast 100 spells
            uint32 const greaterHealCastTime = 3 * IN_MILLISECONDS;
            uint32 const expectedGreaterHealCastTime = greaterHealCastTime * 100 / uint32(100 * castingSpeedIncrease);
            TEST_CAST_TIME(priest, ClassSpells::Priest::GREATER_HEAL_RNK_7, expectedGreaterHealCastTime);

            // Mana reduction
            uint32 const expectedGreaterHealManaCost = 825 * manaCostReduction;
            TEST_POWER_COST(priest, ClassSpells::Priest::GREATER_HEAL_RNK_7, POWER_MANA, expectedGreaterHealManaCost);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<PowerInfusionTestImpt>();
    }
};

class ReflectiveShieldTest : public TestCaseScript
{
public:

    ReflectiveShieldTest() : TestCaseScript("talents priest reflective_shield") { }

    class ReflectiveShieldTestImpt : public TestCase
    {
    public:
        ReflectiveShieldTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            Position spawn;
            spawn.MoveInFront(_location, 5.f);
            TestPlayer* mage = SpawnPlayer(CLASS_MAGE, RACE_TROLL, 70, spawn);
            spawn.MoveInFront(spawn, 5.f);
            TestPlayer* enemyMage = SpawnPlayer(CLASS_MAGE, RACE_HUMAN, 70, spawn);
            EnableCriticals(enemyMage, false);
            enemyMage->DisableRegeneration(true);

            LearnTalent(priest, Talents::Priest::REFLECTIVE_SHIELD_RNK_5);

            // Patch 2.4: Reflective Shield will no longer break crowd control effects.
            // Enemy casts on Priest and gets polymorphed before the spell hits the reflective shield
            TEST_CAST(priest, priest, ClassSpells::Priest::POWER_WORD_SHIELD_RNK_12, SPELL_CAST_OK, TRIGGERED_FULL_MASK);
            TEST_HAS_AURA(priest, ClassSpells::Priest::POWER_WORD_SHIELD_RNK_12);
            FORCE_CAST(enemyMage, priest, ClassSpells::Mage::ICE_LANCE_RNK_1, SPELL_MISS_NONE, TRIGGERED_FULL_MASK);
            FORCE_CAST(mage, enemyMage, ClassSpells::Mage::POLYMORPH_RNK_4, SPELL_MISS_NONE, TriggerCastFlags(TRIGGERED_FULL_MASK | TRIGGERED_IGNORE_SPEED));
            WaitNextUpdate();
            TEST_HAS_AURA(enemyMage, ClassSpells::Mage::POLYMORPH_RNK_4);
            enemyMage->RemoveAurasDueToSpell(ClassSpells::Mage::POLYMORPH_RNK_4);

            // Reflects 50% of damage
            TEST_HAS_AURA(priest, ClassSpells::Priest::POWER_WORD_SHIELD_RNK_12);
            uint32 enemyMageStartingHealth = enemyMage->GetHealth();
            FORCE_CAST(enemyMage, priest, ClassSpells::Mage::SCORCH_RNK_9, SPELL_MISS_NONE, TriggerCastFlags(TRIGGERED_FULL_MASK | TRIGGERED_IGNORE_SPEED));
            auto[dealtMin, dealtMax] = GetDamagePerSpellsTo(enemyMage, priest, ClassSpells::Mage::SCORCH_RNK_9, false, 1);
            TEST_ASSERT(dealtMin == dealtMax);
            uint32 enemyMageExpectedHealth = enemyMageStartingHealth - floor(dealtMin * 0.5f);
            ASSERT_INFO("Mage should have %u HP but has %u HP.", enemyMageExpectedHealth, enemyMage->GetHealth());
            TEST_ASSERT(enemyMage->GetHealth() == enemyMageExpectedHealth);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<ReflectiveShieldTestImpt>();
    }
};

class EnlightmentTest : public TestCaseScript
{
public:

    EnlightmentTest() : TestCaseScript("talents priest enlightment") { }

    class EnlightmentTestImpt : public TestCase
    {
    public:
        EnlightmentTestImpt() : TestCase(STATUS_PASSING) { }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);

            float const talentFactor = 1.05f;

            uint32 expectedSpirit = priest->GetStat(STAT_SPIRIT) * talentFactor;
            uint32 expectedStamina = priest->GetStat(STAT_STAMINA) * talentFactor;
            uint32 expectedIntellect = priest->GetStat(STAT_INTELLECT) * talentFactor;

            LearnTalent(priest, Talents::Priest::ENLIGHTENMENT_RNK_5);

            TEST_ASSERT(priest->GetStat(STAT_SPIRIT) == expectedSpirit);
            TEST_ASSERT(priest->GetStat(STAT_STAMINA) == expectedStamina);
            TEST_ASSERT(priest->GetStat(STAT_INTELLECT) == expectedIntellect);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<EnlightmentTestImpt>();
    }
};

class PainSuppressionTest : public TestCaseScript
{
public:

    PainSuppressionTest() : TestCaseScript("talents priest pain_suppression") { }

    class PainSuppressionTestImpt : public TestCase
    {
    public:
        /*
        Bugs:
            - Threat is not reduced by 5%.
        */
        PainSuppressionTestImpt() : TestCase(STATUS_WIP) { } // Waiting on max health in TestCase::_TestDirectValue, should pass afterwards -> DONE

        void RefreshPainSuppression(TestPlayer* priest)
        {
            priest->RemoveAurasDueToSpell(ClassSpells::Priest::PAIN_SUPPRESSION_RNK_1);
            priest->AddAura(ClassSpells::Priest::PAIN_SUPPRESSION_RNK_1, priest);
        }

        void Test() override
        {
            TestPlayer* priest = SpawnPlayer(CLASS_PRIEST, RACE_BLOODELF);
            TestPlayer* shaman = SpawnPlayer(CLASS_SHAMAN, RACE_DRAENEI);

            float const talentDamageTakenFactor = 0.6f;
            float const talentThreatFactor = 0.95f;
            float const talentDispelResistFactor = 65.f;

            TEST_CAST(priest, priest, ClassSpells::Priest::PAIN_SUPPRESSION_RNK_1);
            TEST_AURA_MAX_DURATION(priest, ClassSpells::Priest::PAIN_SUPPRESSION_RNK_1, Seconds(8));
            TEST_HAS_COOLDOWN(priest, ClassSpells::Priest::PAIN_SUPPRESSION_RNK_1, Minutes(2));
            priest->RemoveAurasDueToSpell(ClassSpells::Priest::PAIN_SUPPRESSION_RNK_1);
            priest->GetSpellHistory()->ResetAllCooldowns();

            // Reduces threat by 5%
            //Creature* dummy = SpawnCreature();
            //FORCE_CAST(priest, dummy, ClassSpells::Priest::SMITE_RNK_10, SPELL_MISS_NONE, TRIGGERED_FULL_MASK);
            //float const expectedThreat = dummy->GetThreatManager().GetThreat(priest) * talentThreatFactor;
            //TEST_CAST(priest, priest, ClassSpells::Priest::PAIN_SUPPRESSION_RNK_1);
            //WaitNextUpdate();
            //ASSERT_INFO("Priest should have %f threat but has %f.", expectedThreat, dummy->GetThreatManager().GetThreat(priest));
            //TEST_ASSERT(Between<float>(dummy->GetThreatManager().GetThreat(priest), expectedThreat - 0.1f, expectedThreat + 0.1f));

            // Reduces all damage taken by 40%
            EQUIP_NEW_ITEM(shaman, 34165); // Fang of Kalecgos
            WaitNextUpdate();
            uint32 const weaponMinDmg = 113;
            uint32 const weaponMaxDmg = 211;
            float const weaponSpeed = 1.5f;
            float const AP = shaman->GetTotalAttackPowerValue(BASE_ATTACK);
            float const armorFactor = 1 - (priest->GetArmor() / (priest->GetArmor() + 10557.5f));
            uint32 const minMelee = floor(weaponMinDmg + AP / 14.f * weaponSpeed) * armorFactor * talentDamageTakenFactor;
            uint32 const maxMelee = floor(weaponMaxDmg + AP / 14.f * weaponSpeed) * armorFactor * talentDamageTakenFactor;
            RefreshPainSuppression(priest);
            TEST_MELEE_DAMAGE(shaman, priest, BASE_ATTACK, minMelee, maxMelee, false);
            uint32 const minEarthShock = ClassSpellsDamage::Shaman::EARTH_SHOCK_RNK_8_MIN * talentDamageTakenFactor;
            uint32 const maxEarthShock = ClassSpellsDamage::Shaman::EARTH_SHOCK_RNK_8_MAX * talentDamageTakenFactor;
            RefreshPainSuppression(priest);
            //TEST_DIRECT_SPELL_DAMAGE(shaman, priest, ClassSpells::Shaman::EARTH_SHOCK_RNK_8, minEarthShock, maxEarthShock, false);

            // Increases resistance to dispell by 65%
            TEST_DISPEL_RESIST_CHANCE(priest, priest, shaman, ClassSpells::Priest::PAIN_SUPPRESSION_RNK_1, talentDispelResistFactor);
        }
    };

    std::shared_ptr<TestCase> GetTest() const override
    {
        return std::make_shared<PainSuppressionTestImpt>();
    }
};

class SearingLightTest : public TestCaseScript
{
public:

	SearingLightTest() : TestCaseScript("talents priest searing_light") { }

	class SearingLightTestImpt : public TestCase
	{
	public:
		SearingLightTestImpt() : TestCase(STATUS_PASSING) { }

		void Test() override
		{
			TestPlayer* priest = SpawnRandomPlayer(CLASS_PRIEST);
            Creature* dummy = SpawnCreature();

            LearnTalent(priest, Talents::Priest::SEARING_LIGHT_RNK_2);
            float const talentFactor = 1.1f;

			// Smite
			uint32 const expectedSmiteMin = ClassSpellsDamage::Priest::SMITE_RNK_10_MIN * talentFactor;
			uint32 const expectedSmiteMax = ClassSpellsDamage::Priest::SMITE_RNK_10_MAX * talentFactor;
            TEST_DIRECT_SPELL_DAMAGE(priest, dummy, ClassSpells::Priest::SMITE_RNK_10, expectedSmiteMin, expectedSmiteMax, false);

			// Holy Fire
			uint32 const expectedHolyFireMin = ClassSpellsDamage::Priest::HOLY_FIRE_RNK_9_MIN * talentFactor;
			uint32 const expectedHolyFireMax = ClassSpellsDamage::Priest::HOLY_FIRE_RNK_9_MAX * talentFactor;
            TEST_DIRECT_SPELL_DAMAGE(priest, dummy, ClassSpells::Priest::HOLY_FIRE_RNK_9, expectedHolyFireMin, expectedHolyFireMax, false);
            uint32 const expectedHolyFireDot = 5.0f * floor(ClassSpellsDamage::Priest::HOLY_FIRE_RNK_9_TOTAL * talentFactor / 5.0f);
            TEST_DOT_DAMAGE(priest, dummy, ClassSpells::Priest::HOLY_FIRE_RNK_9, expectedHolyFireDot, false);
		}
	};

	std::shared_ptr<TestCase> GetTest() const override
	{
		return std::make_shared<SearingLightTestImpt>();
	}
};

void AddSC_test_talents_priest()
{
    // Discipline
    new UnbreakableWillTest();
	new PriestWandSpecializationTest();
    new SilentResolve();
    new ImprovedPowerWordFortitudeTest();
    new ImprovedPowerWordShieldTest();
    new MartyrdomTest();
    new AbsolutionTest();
    new InnerFocusTest();
    new MeditationTest();
    new ImprovedInnerFireTest();
    new MentalAgilityTest();
    new ImprovedManaBurnTest();
    new MentalStrengthTest();
    new DivineSpiritTest();
    new ImprovedDivineSpiritTest();
    new FocusedPowerTest();
    new ForceOfWillTest();
    new FocusedWillTest();
    new PowerInfusionTest();
    new ReflectiveShieldTest();
    new EnlightmentTest();
    new PainSuppressionTest();
    // Holy
	new SearingLightTest();
    // Shadow
}
