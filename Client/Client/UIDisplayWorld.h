//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma once /* UIDisaplyWorld.h */
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "RenderSystem.h"
#include "Scene.h"
#include "Shader.h"
#include "UIDialog.h"
#include "Graphics.h"
#include "TextRender.h"
#include "PlayerMe.h"
#include "World.h"
#include "UIDisplayModel.h"
#include "Terrain.h"
#include "SceneEffect.h"
#include "Role.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CUIDisplayWorld: public CUIDisplayRenderNode
{
protected:
	POINT				m_ptLastMousePosition;
	iRenderNode*		m_pRenderNodeProps;
	CRole*				m_pMouseRole;
public:
	CUIDisplayWorld();
	~CUIDisplayWorld();
	// ----
	void			SetFog			(float fFogStart, float fFogEnd, float fFogDensity, DWORD dwFogColor);
	void			MoveCamera		(int x,int y);
	void			Render();
	// ----
	virtual void	OnFrameMove		(double fTime, float fElapsedTime);
	virtual void	OnFrameRender	(const Matrix& mTransform, double fTime, float fElapsedTime);
	// ----
	virtual bool	HandleMouse		(UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam);
	virtual bool	HandleKeyboard	(UINT uMsg, WPARAM wParam, LPARAM lParam);
	// ----
	virtual void OnMouseMove		(POINT point);
	virtual void OnMouseWheel		(POINT point, short wheelDelta);
	virtual void OnLButtonDown		(POINT point);
	virtual void OnLButtonUp		(POINT point);
	virtual void OnRButtonDown		(POINT point);
	virtual void OnRButtonUp		(POINT point);
	virtual void OnMButtonDown		(POINT point);
	virtual void OnMButtonUp		(POINT point);
	// ----
	virtual void OnFocusOut			();
	// ----
	virtual bool CanHaveFocus		() { return (m_bVisible && m_bEnabled); }
	// ----
	virtual void OnSize				(const CRect<int> & rc);
	// ----
	CSceneEffect&	getSceneEffect	() {return m_SceneEffect;}
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------