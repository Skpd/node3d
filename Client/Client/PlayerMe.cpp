#include "PlayerMe.h"
#include "MainRoot.h"
#include "protocol.h"
#include "World.h"
#include "Audio.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPlayerMe::CPlayerMe()
{
	m_uAttackTarget = 0;
	m_fWalkSpeed = 2;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPlayerMe::~CPlayerMe()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPlayerMe::playWalkSound() 
{
	if(this->getRoleID() == CPlayerMe::getInstance().getRoleID())
	{
		if (CWorld::getInstance().getSceneData())
		{
			unsigned char uTileID = CWorld::getInstance().getSceneData()->getCellTileID(m_vPos.x, m_vPos.z ,0);
			// ----
			if (uTileID==0)
			{
				GetAudio().playSound("Data\\Sound\\pWalk(Grass).wav");
			}
			else
			{
				GetAudio().playSound("Data\\Sound\\pWalk(Soil).wav");
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern bool g_bLocal;
void CPlayerMe::frameMoveRole(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	// ----
	// # If has the attack target, player should walk around the target, then attack the target.
	// ----
	if(m_uAttackTarget > 0)
	{
		CRole * pRole = CWorld::getInstance().getRole(m_uAttackTarget);
		// ----
		if(pRole == NULL)
		{
			m_uAttackTarget = 0;
		}
		else if (pRole->getCurRoleCmd().nType != RoleCmd::DEAD)
		{
			if (m_CurRoleCmd.nType == RoleCmd::STAND)
			{
				float fDistance = (pRole->getPos() - getPos()).length();
				// ----
				// Can Attact
				// ----
				if (fDistance > 2.0f)
				{
					// ----
					// Add Move Cmd
					// ----
					RoleCmd cmd;
					cmd.nType = RoleCmd::MOVE;
					cmd.nParam1 = pRole->getCellPosX();
					cmd.nParam2 = pRole->getCellPosY();
					cmd.nParam3	= 2;
					addRoleCmd(cmd);
				}
				else
				{
					// ----
					// Add Attact Cmd
					// ----
					RoleCmd cmd;
					cmd.nType = RoleCmd::ATTACT;
					cmd.nParam1 =  m_uAttackTarget;
					addRoleCmd(cmd);
					// Local Robot
					if (g_bLocal)
					{
						pRole->damage(rand()%30,0,0);
						// ----
						if (rand()%5==0)
						{
							RoleCmd cmd;
							cmd.nType = RoleCmd::DEAD;
							pRole->addRoleCmd(cmd);
						}
					}
				}
			}
		}
	}
	// ----
	CRole::frameMoveRole(mWorld, fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------