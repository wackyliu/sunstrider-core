/*
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
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

#include "OutdoorPvP.h"
#include "OutdoorPvPMgr.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Map.h"
#include "MapManager.h"
#include "OutdoorPvPObjectiveAI.h"
#include "Group.h"
#include "WorldPacket.h"

OutdoorPvPObjective::OutdoorPvPObjective(OutdoorPvP * pvp)
: m_PvP(pvp), m_ShiftPhase(0), m_ShiftMaxPhase(0), m_OldPhase(0),
m_State(0), m_OldState(0), m_CapturePoint(0), m_NeutralValue(0),
m_ShiftMaxCaptureSpeed(0), m_CapturePointCreature(0)
{
}

bool OutdoorPvPObjective::HandlePlayerEnter(Player * plr)
{
    uint32 team = (plr->GetTeam() == TEAM_HORDE) ? 1 : 0;
    // only called if really entered, so no use in the return value anymore
    // player distance and activity state was checked already in the AI
    std::pair<std::set<uint64>::iterator,bool> newinsert = m_ActivePlayerGuids[team].insert(plr->GetGUID());
    return true;
}

void OutdoorPvPObjective::HandlePlayerLeave(Player * plr)
{
    uint32 team = (plr->GetTeam() == TEAM_HORDE) ? 1 : 0;
    // only decrease the count if the player is in the active list
    m_ActivePlayerGuids[team].erase(plr->GetGUID());
}

void OutdoorPvPObjective::HandlePlayerActivityChanged(Player * plr)
{
    if(m_CapturePointCreature)
        if(Creature * c = HashMapHolder<Creature>::Find(m_CapturePointCreature))
            if(c->IsAIEnabled)
                c->AI()->MoveInLineOfSight(plr);
}

bool OutdoorPvPObjective::AddObject(uint32 type, uint32 entry, uint32 map, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3)
{
    GameObjectTemplate const* goinfo = sObjectMgr->GetGameObjectTemplate(entry);
    if (!goinfo)
        return false;

    uint32 guid = sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT,true);

    GameObjectData& data = sObjectMgr->NewGOData(guid);

    data.id             = entry;
    data.mapid          = map;
    data.posX           = x;
    data.posY           = y;
    data.posZ           = z;
    data.orientation    = o;
	data.rotation       = G3D::Quat(rotation0, rotation1, rotation2, rotation3);
    data.spawntimesecs  = 0;
    data.animprogress   = 100;
    data.spawnMask      = 1;
    data.go_state       = 1;

    sObjectMgr->AddGameobjectToGrid(guid, &data);

    // 2 way registering
    m_Objects[type] = MAKE_NEW_GUID(guid, entry, HIGHGUID_GAMEOBJECT);
    m_ObjectTypes[m_Objects[type]]=type;

    Map * pMap = sMapMgr->FindBaseNonInstanceMap(map);
    if(!pMap)
        return true;
    auto  go = new GameObject;
    if(!go->Create(guid,entry, pMap,Position(x,y,z,o),G3D::Quat(rotation0,rotation1,rotation2,rotation3), 255, GO_STATE_READY))
    {
        TC_LOG_ERROR("FIXME","Gameobject template %u not found in database.", entry);
        delete go;
        return true;
    }

    go->SetRespawnTime(0);
    sObjectMgr->SaveGORespawnTime(go->GetDBTableGUIDLow(),0,0,0);
    pMap->Add(go);

    return true;
}

bool OutdoorPvPObjective::AddCreature(uint32 type, uint32 entry, uint32 map, float x, float y, float z, float o, uint32 spawntimedelay, bool setActive)
{
    CreatureTemplate const *cInfo = sObjectMgr->GetCreatureTemplate(entry);
    if(!cInfo)
    {
        return false;
    }

    uint32 displayId = sObjectMgr->ChooseDisplayId(cInfo, nullptr);
    CreatureModelInfo const *minfo = sObjectMgr->GetCreatureModelRandomGender(displayId);
    if (!minfo)
        return false;

    uint32 level = cInfo->minlevel == cInfo->maxlevel ? cInfo->minlevel : urand(cInfo->minlevel, cInfo->maxlevel); // Only used for extracting creature base stats
    CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(level, cInfo->unit_class);

    uint32 guid = sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT,true);
    CreatureData& data = sObjectMgr->NewOrExistCreatureData(guid);
    data.id = entry;
    data.mapid = map;
    data.displayid = displayId;
    data.equipmentId = cInfo->equipmentId;
    data.posX = x;
    data.posY = y;
    data.posZ = z;
    data.orientation = o;
    data.spawntimesecs = spawntimedelay;
    data.spawndist = 0;
    data.currentwaypoint = 0;
    data.curhealth = stats->GenerateHealth(cInfo);
    data.curmana = stats->GenerateMana(cInfo);
    data.movementType = cInfo->MovementType;
    data.spawnMask = 1;

    sObjectMgr->AddCreatureToGrid(guid, &data);

    m_Creatures[type] = MAKE_NEW_GUID(guid, entry, HIGHGUID_UNIT);
    m_CreatureTypes[m_Creatures[type]] = type;

    Map * pMap = sMapMgr->FindBaseNonInstanceMap(map);
    if(!pMap)
        return true;
    auto  pCreature = new Creature;
    if (!pCreature->Create(guid, pMap, entry))
    {
        TC_LOG_ERROR("FIXME","Can't create creature entry: %u",entry);
        delete pCreature;
        return true;
    }

    pCreature->AIM_Initialize();

    pCreature->Relocate(x, y, z, o);

    if(!pCreature->IsPositionValid())
    {
        TC_LOG_ERROR("FIXME","ERROR: Creature (guidlow %d, entry %d) not added to opvp. Suggested coordinates isn't valid (X: %f Y: %f)",pCreature->GetGUIDLow(),pCreature->GetEntry(),pCreature->GetPositionX(),pCreature->GetPositionY());
        delete pCreature;
        return false;
    }

    if(spawntimedelay)
        pCreature->SetRespawnDelay(spawntimedelay);

    pMap->Add(pCreature);
    pCreature->SetHomePosition(x, y, z, o);
    
    if (setActive)
        pCreature->SetKeepActive(true);

    return true;
}

bool OutdoorPvPObjective::AddCapturePoint(uint32 entry, uint32 map, float x, float y, float z, float o, float rotation0, float rotation1, float rotation2, float rotation3)
{
    // check info existence
    GameObjectTemplate const* goinfo = sObjectMgr->GetGameObjectTemplate(entry);
    if (!goinfo)
        return false;

    CreatureTemplate const *cInfo = sObjectMgr->GetCreatureTemplate(OPVP_TRIGGER_CREATURE_ENTRY);
    if(!cInfo)
        return false;

    // create capture point creature
    uint32 displayId = sObjectMgr->ChooseDisplayId(cInfo, nullptr);

    uint32 creature_guid = sObjectMgr->GenerateLowGuid(HIGHGUID_UNIT,true);

    CreatureData& cdata = sObjectMgr->NewOrExistCreatureData(creature_guid);

    uint32 level = cInfo->minlevel == cInfo->maxlevel ? cInfo->minlevel : urand(cInfo->minlevel, cInfo->maxlevel); // Only used for extracting creature base stats
    CreatureBaseStats const* stats = sObjectMgr->GetCreatureBaseStats(level, cInfo->unit_class);

    cdata.id = OPVP_TRIGGER_CREATURE_ENTRY;
    cdata.mapid = map;
    cdata.displayid = displayId;
    cdata.equipmentId = cInfo->equipmentId;
    cdata.posX = x;
    cdata.posY = y;
    cdata.posZ = z;
    cdata.orientation = o;
    cdata.spawntimesecs = 1;
    cdata.spawndist = 0;
    cdata.currentwaypoint = 0;
    cdata.curhealth = stats->GenerateHealth(cInfo);
    cdata.curmana = stats->GenerateMana(cInfo);
    cdata.movementType = cInfo->MovementType;
    cdata.spawnMask = 1;

    sObjectMgr->AddCreatureToGrid(creature_guid, &cdata);
    m_CapturePointCreature = MAKE_NEW_GUID(creature_guid, OPVP_TRIGGER_CREATURE_ENTRY, HIGHGUID_UNIT);

    // create capture point go
    uint32 guid = sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT,true);

    GameObjectData& data = sObjectMgr->NewGOData(guid);

    data.id             = entry;
    data.mapid          = map;
    data.posX           = x;
    data.posY           = y;
    data.posZ           = z;
    data.orientation    = o;
    data.rotation       = G3D::Quat(rotation0, rotation1, rotation2, rotation3);
    data.spawntimesecs  = 1;
    data.animprogress   = 100;
    data.spawnMask      = 1;
    data.go_state       = 1;

    sObjectMgr->AddGameobjectToGrid(guid, &data);

    m_CapturePoint = MAKE_NEW_GUID(guid, entry, HIGHGUID_GAMEOBJECT);

    // get the needed values from goinfo
    m_ShiftMaxPhase = goinfo->raw.data[17];
    m_ShiftMaxCaptureSpeed = m_ShiftMaxPhase / float(goinfo->raw.data[16]);
    m_NeutralValue = goinfo->raw.data[12];

    // add to map if map is already loaded
    Map * pMap = sMapMgr->FindBaseNonInstanceMap(map);
    if(!pMap)
        return true;
    // add GO...
    auto  go = new GameObject;
    if(!go->Create(guid,entry, pMap, Position(x,y,z,o), G3D::Quat(rotation0,rotation1,rotation2,rotation3), 255, GO_STATE_READY))
    {
        TC_LOG_ERROR("FIXME","Gameobject template %u not found in database.", entry);
        delete go;
    }
    else
    {
        go->SetRespawnTime(0);
        sObjectMgr->SaveGORespawnTime(go->GetDBTableGUIDLow(), 0, 0, 0);
        pMap->Add(go);
    }
    // add creature...
    auto  pCreature = new Creature;
    if (!pCreature->Create(creature_guid, pMap, OPVP_TRIGGER_CREATURE_ENTRY))
    {
        TC_LOG_ERROR("FIXME","Can't create creature entry: %u",entry);
        delete pCreature;
    }
    else
    {
        pCreature->AIM_Initialize();

        pCreature->Relocate(x, y, z, o);

        if(!pCreature->IsPositionValid())
        {
            TC_LOG_ERROR("FIXME","ERROR: Creature (guidlow %d, entry %d) not added to opvp. Suggested coordinates isn't valid (X: %f Y: %f)",pCreature->GetGUIDLow(),pCreature->GetEntry(),pCreature->GetPositionX(),pCreature->GetPositionY());
            delete pCreature;
            return false;
        }

        pMap->Add(pCreature);
    }
    return true;
}

bool OutdoorPvPObjective::DelCreature(uint32 type)
{
    if(!m_Creatures[type])
    {
        TC_LOG_ERROR("FIXME","opvp creature type %u was already deleted",type);
        return false;
    }

    Creature *cr = HashMapHolder<Creature>::Find(m_Creatures[type]);
    if(!cr)
    {
        // can happen when closing the core
        m_Creatures[type] = 0;
        return false;
    }

    uint32 guid = cr->GetDBTableGUIDLow();
    // Don't save respawn time
    cr->SetRespawnTime(0);
    cr->RemoveCorpse();
    // explicit removal from map
    // beats me why this is needed, but with the recent removal "cleanup" some creatures stay in the map if "properly" deleted
    // so this is a big fat workaround, if AddObjectToRemoveList and DoDelayedMovesAndRemoves worked correctly, this wouldn't be needed
    if(Map * map = sMapMgr->FindBaseNonInstanceMap(cr->GetMapId()))
        map->Remove(cr,false);
    // delete respawn time for this creature
    CharacterDatabase.PExecute("DELETE FROM creature_respawn WHERE guid = '%u'", guid);
    cr->AddObjectToRemoveList();
    sObjectMgr->DeleteCreatureData(guid);
    m_CreatureTypes[m_Creatures[type]] = 0;
    m_Creatures[type] = 0;
    return true;
}

bool OutdoorPvPObjective::DelObject(uint32 type)
{
    if(!m_Objects[type])
        return false;

    GameObject *obj = HashMapHolder<GameObject>::Find(m_Objects[type]);
    if(!obj)
    {
        m_Objects[type] = 0;
        return false;
    }
    uint32 guid = obj->GetDBTableGUIDLow();
    obj->SetRespawnTime(0);                                 // not save respawn time
    obj->Delete();
    sObjectMgr->DeleteGOData(guid);
    m_ObjectTypes[m_Objects[type]] = 0;
    m_Objects[type] = 0;
    return true;
}

bool OutdoorPvPObjective::DelCapturePoint()
{
    if(m_CapturePoint)
    {
        GameObject *obj = HashMapHolder<GameObject>::Find(m_CapturePoint);
        if(obj)
        {
            uint32 guid = obj->GetDBTableGUIDLow();
            obj->SetRespawnTime(0);                                 // not save respawn time
            obj->Delete();
            sObjectMgr->DeleteGOData(guid);
        }
        m_CapturePoint = 0;
    }
    if(m_CapturePointCreature)
    {
        Creature *cr = HashMapHolder<Creature>::Find(m_CapturePointCreature);
        if(cr)
        {
            uint32 guid = cr->GetDBTableGUIDLow();
            // Don't save respawn time
            cr->SetRespawnTime(0);
            cr->RemoveCorpse();
            cr->CleanupsBeforeDelete();
            // explicit removal from map
            // beats me why this is needed, but with the recent removal "cleanup" some creatures stay in the map if "properly" deleted
            // so this is a big fat workaround, if AddObjectToRemoveList and DoDelayedMovesAndRemoves worked correctly, this wouldn't be needed
            if(Map * map = sMapMgr->FindBaseNonInstanceMap(cr->GetMapId()))
                map->Remove(cr,false);
            // delete respawn time for this creature
            CharacterDatabase.PExecute("DELETE FROM creature_respawn WHERE guid = '%u'", guid);
            cr->AddObjectToRemoveList();
            sObjectMgr->DeleteCreatureData(guid);
        }
        m_CapturePointCreature = 0;
    }
    return true;
}

void OutdoorPvPObjective::DeleteSpawns()
{
    for(auto & m_Object : m_Objects)
        DelObject(m_Object.first);
    for(auto & m_Creature : m_Creatures)
        DelCreature(m_Creature.first);
    DelCapturePoint();
}

void OutdoorPvP::DeleteSpawns()
{
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
        m_OutdoorPvPObjective->DeleteSpawns();
}

OutdoorPvP::OutdoorPvP()
{
}

OutdoorPvP::~OutdoorPvP()
{
    DeleteSpawns();
    for (OutdoorPvPObjective* itr : m_OutdoorPvPObjectives)
        delete itr;
}

void OutdoorPvP::HandlePlayerEnterZone(Player * plr, uint32 zone)
{
    if(plr->GetTeam()==TEAM_ALLIANCE)
        m_PlayerGuids[0].insert(plr->GetGUID());
    else
        m_PlayerGuids[1].insert(plr->GetGUID());
}

void OutdoorPvP::HandlePlayerLeaveZone(Player * plr, uint32 zone)
{
    // inform the objectives of the leaving
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
        m_OutdoorPvPObjective->HandlePlayerLeave(plr);
    // remove the world state information from the player (we can't keep everyone up to date, so leave out those who are not in the concerning zones)
    if(zone != plr->GetZoneId())
        SendRemoveWorldStates(plr);
    if(plr->GetTeam()==TEAM_ALLIANCE)
        m_PlayerGuids[0].erase(plr->GetGUID());
    else
        m_PlayerGuids[1].erase(plr->GetGUID());
}

bool OutdoorPvP::Update(uint32 diff)
{
    bool objective_changed = false;
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
        objective_changed |= m_OutdoorPvPObjective->Update(diff);
    return objective_changed;
}

void OutdoorPvPObjective::UpdateActivePlayerProximityCheck()
{
    if(GameObject *cp = HashMapHolder<GameObject>::Find(m_CapturePoint))
    {
        for(auto & m_ActivePlayerGuid : m_ActivePlayerGuids)
        {
            std::set<uint64>::iterator itr, next;
            for(itr = m_ActivePlayerGuid.begin(); itr != m_ActivePlayerGuid.end(); itr = next)
            {
                next = itr;
                ++next;
                // if the player is online
                if(Player * pl = sObjectMgr->GetPlayer(*itr))
                {
                    if(!cp->IsWithinDistInMap(pl,cp->GetGOInfo()->raw.data[0]))
                        HandleCapturePointEvent(pl, cp->GetGOInfo()->raw.data[9]); //i_objective->HandlePlayerLeave(u->ToPlayer());
                }
                else
                {
                    TC_LOG_ERROR("FIXME","Player (" UI64FMTD ") offline, bit still in outdoor pvp, this should never happen.",(*itr));
                } 
            }
        }
    }
}

bool OutdoorPvPObjective::Update(uint32 diff)
{
    UpdateActivePlayerProximityCheck();

    uint32 Challenger = 0;

        // get the difference of numbers
        float fact_diff = ((float)m_ActivePlayerGuids[0].size() - (float)m_ActivePlayerGuids[1].size());

        if(fact_diff<0)
        {
            if(fact_diff < - m_ShiftMaxCaptureSpeed)
                fact_diff = - m_ShiftMaxCaptureSpeed;
            Challenger = TEAM_HORDE;
            // horde is in majority, but it's already horde-controlled -> no change
            if(m_State == OBJECTIVESTATE_HORDE && m_ShiftPhase == - m_ShiftMaxPhase)
                return false;
        }
        else if(fact_diff>0)
        {
            if(fact_diff > m_ShiftMaxCaptureSpeed)
                fact_diff = m_ShiftMaxCaptureSpeed;
            Challenger = TEAM_ALLIANCE;
            // ally is in majority, but it's already ally-controlled -> no change
            if(m_State == OBJECTIVESTATE_ALLIANCE && m_ShiftPhase == m_ShiftMaxPhase)
                return false;
        }
        else /*if(fact_diff==0)*/ // no change
            return false;

        m_OldPhase = m_ShiftPhase;

        m_OldState = m_State;

        m_ShiftPhase += fact_diff;

        // check limits, these are over the grey part
        if(m_ShiftPhase <= - m_ShiftMaxPhase * (float)(m_NeutralValue) / 100.0f)
        {
            if(m_ShiftPhase <= - m_ShiftMaxPhase)
                m_ShiftPhase = - m_ShiftMaxPhase;
            m_State = OBJECTIVESTATE_HORDE;
            return true;
        }
        else if(m_ShiftPhase >= m_ShiftMaxPhase * (float)(m_NeutralValue) / 100.0f)
        {
            if(m_ShiftPhase >= m_ShiftMaxPhase)
                m_ShiftPhase = m_ShiftMaxPhase;
            m_State = OBJECTIVESTATE_ALLIANCE;
            return true;
        }

        if(m_OldPhase*m_ShiftPhase <=0)
        {
            // gone through neutral
            // if challenger is ally, then n->a challenge
            if(Challenger == TEAM_ALLIANCE)
                m_State = OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE;
            // if challenger is horde, then n->h challenge
            else if(Challenger == TEAM_HORDE)
                m_State = OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE;
        }
        else
        {
            // old phase and current are on the same side, so one team challenges the other
            if(Challenger == TEAM_ALLIANCE && (m_OldState == OBJECTIVESTATE_HORDE || m_OldState == OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE))
                m_State = OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE;
            else if(Challenger == TEAM_HORDE && (m_OldState == OBJECTIVESTATE_ALLIANCE || m_OldState == OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE))
                m_State = OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE;
        }

        return true;
}

bool OutdoorPvPObjective::HandleCaptureCreaturePlayerMoveInLos(Player * p, Creature * c)
{
    // check if guid matches
    if(c->GetGUID() != m_CapturePointCreature)
        return false;

    // check if capture point go is spawned
    GameObject * cp = HashMapHolder<GameObject>::Find(m_CapturePoint);
    if(!cp)
        return false;

    // check range and activity
    if(cp->IsWithinDistInMap(p,cp->GetGOInfo()->raw.data[0]) && p->IsOutdoorPvPActive())
    {
        // data[8] will be used for player enter
        return HandleCapturePointEvent(p, cp->GetGOInfo()->raw.data[8]); //i_objective->HandlePlayerEnter(u->ToPlayer());
    }
    else
    {
        // data[9] will be used for player leave
        return HandleCapturePointEvent(p, cp->GetGOInfo()->raw.data[9]); //i_objective->HandlePlayerLeave(u->ToPlayer());
    }
}

void OutdoorPvP::SendUpdateWorldState(uint32 field, uint32 value)
{
    // send to both factions
    for(auto & m_PlayerGuid : m_PlayerGuids)
    {
        // send to all players present in the area
        for(auto itr = m_PlayerGuid.begin(); itr != m_PlayerGuid.end(); ++itr)
        {
            Player * plr = sObjectMgr->GetPlayer(*itr);
            if(plr)
            {
                plr->SendUpdateWorldState(field,value);
            }
        }
    }
}

void OutdoorPvPObjective::SendUpdateWorldState(uint32 field, uint32 value)
{
    for(auto & m_ActivePlayerGuid : m_ActivePlayerGuids)
    {
        // send to all players present in the area
        for(auto itr = m_ActivePlayerGuid.begin(); itr != m_ActivePlayerGuid.end(); ++itr)
        {
            Player * plr = sObjectMgr->GetPlayer(*itr);
            if(plr)
            {
                plr->SendUpdateWorldState(field,value);
            }
        }
    }
}

void OutdoorPvPObjective::SendObjectiveComplete(uint32 id,uint64 guid)
{
    uint32 team;
    switch(m_State)
    {
    case OBJECTIVESTATE_ALLIANCE:
        team = 0;
        break;
    case OBJECTIVESTATE_HORDE:
        team = 1;
        break;
    default:
        return;
        break;
    }

    // send to all players present in the area
    for(uint64 itr : m_ActivePlayerGuids[team])
    {
        Player * plr = sObjectMgr->GetPlayer(itr);
        if(plr)
        {
            plr->KilledMonsterCredit(id,guid);
        }
    }
}

void OutdoorPvP::HandlePlayerActivityChanged(Player * plr)
{
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
        m_OutdoorPvPObjective->HandlePlayerActivityChanged(plr);
}

void OutdoorPvP::HandleKill(Player *killer, Unit * killed)
{
    if(Group * pGroup = killer->GetGroup())
    {
        for(GroupReference *itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player *pGroupGuy = itr->GetSource();

            if(!pGroupGuy)
                continue;

            // skip if too far away
            if(!pGroupGuy->IsAtGroupRewardDistance(killed))
                continue;

            // creature kills must be notified, even if not inside objective / not outdoor pvp active
            // player kills only count if active and inside objective
            if(( pGroupGuy->IsOutdoorPvPActive() && IsInsideObjective(pGroupGuy) ) || killed->GetTypeId() == TYPEID_UNIT)
            {
                HandleKillImpl(pGroupGuy, killed);
            }
        }
    }
    else
    {
        // creature kills must be notified, even if not inside objective / not outdoor pvp active
        if(killer && (( killer->IsOutdoorPvPActive() && IsInsideObjective(killer) ) || killed->GetTypeId() == TYPEID_UNIT))
        {
            HandleKillImpl(killer, killed);
        }
    }
}

bool OutdoorPvP::IsInsideObjective(Player *plr)
{
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
    {
        if(m_OutdoorPvPObjective->IsInsideObjective(plr))
            return true;
    }
    return false;
}

bool OutdoorPvPObjective::IsInsideObjective(Player *plr)
{
    uint32 team = (plr->GetTeam() == TEAM_HORDE) ? 1 : 0;
    auto itr = m_ActivePlayerGuids[team].find(plr->GetGUID());
    return itr != m_ActivePlayerGuids[team].end();
}

bool OutdoorPvP::HandleCustomSpell(Player *plr, uint32 spellId, GameObject * go)
{
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
    {
        if(m_OutdoorPvPObjective->HandleCustomSpell(plr,spellId,go))
            return true;
    }
    return false;
}

bool OutdoorPvPObjective::HandleCustomSpell(Player *plr, uint32 spellId, GameObject * go)
{
    if(!plr->IsOutdoorPvPActive())
        return false;
    return false;
}

bool OutdoorPvP::HandleOpenGo(Player *plr, uint64 guid)
{
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
    {
        if(m_OutdoorPvPObjective->HandleOpenGo(plr,guid) >= 0)
            return true;
    }
    return false;
}

bool OutdoorPvP::HandleCaptureCreaturePlayerMoveInLos(Player * p, Creature * c)
{
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
    {
        if(m_OutdoorPvPObjective->HandleCaptureCreaturePlayerMoveInLos(p, c))
            return true;
    }
    return false;
}

bool OutdoorPvP::HandleGossipOption(Player * plr, uint64 guid, uint32 id)
{
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
    {
        if(m_OutdoorPvPObjective->HandleGossipOption(plr, guid, id))
            return true;
    }
    return false;
}

bool OutdoorPvP::CanTalkTo(Player * plr, Creature * c, GossipMenuItems const& gso)
{
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
    {
        if(m_OutdoorPvPObjective->CanTalkTo(plr, c, gso))
            return true;
    }
    return false;
}

bool OutdoorPvP::HandleDropFlag(Player * plr, uint32 id)
{
    for(auto & m_OutdoorPvPObjective : m_OutdoorPvPObjectives)
    {
        if(m_OutdoorPvPObjective->HandleDropFlag(plr, id))
            return true;
    }
    return false;
}

bool OutdoorPvPObjective::HandleGossipOption(Player * plr, uint64 guid, uint32 id)
{
    return false;
}

bool OutdoorPvPObjective::CanTalkTo(Player * plr, Creature * c, GossipMenuItems const& gso)
{
    return false;
}

bool OutdoorPvPObjective::HandleDropFlag(Player * plr, uint32 id)
{
    return false;
}

int32 OutdoorPvPObjective::HandleOpenGo(Player *plr, uint64 guid)
{
    auto itr = m_ObjectTypes.find(guid);
    if(itr != m_ObjectTypes.end())
    {
        return itr->second;
    }
    return -1;
}

bool OutdoorPvP::HandleAreaTrigger(Player *plr, uint32 trigger)
{
    return false;
}
