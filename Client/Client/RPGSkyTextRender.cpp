#include "RPGSkyTextRender.h"
#include "RenderSystem.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// # Made by	:  RPGSky
// # Date		:  04.08.2010
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static WORD s_DrawTextIB[DRAW_TEXT_MAX * 6];
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CNode3DTextRender::CNode3DTextRender()
{
	for(int i = 0 ; i < DRAW_TEXT_MAX ; i++)
	{
		s_DrawTextIB[i * 6]		= i*4;
		s_DrawTextIB[i * 6 + 1]	= i*4+1;
		s_DrawTextIB[i * 6 + 2]	= i*4+2;
		s_DrawTextIB[i * 6 + 3]	= i*4;
		s_DrawTextIB[i * 6 + 4]	= i*4+2;
		s_DrawTextIB[i * 6 + 5]	= i*4+3;
	}
	// ----
	m_pTextTexture = NULL;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CNode3DTextRender::updateTextureBuffer(unsigned char* pBuffer, size_t size, size_t width, size_t height)
{
	if(m_pTextTexture == NULL)
	{
		m_pTextTexture = CRenderSystem::getSingleton().GetTextureMgr().CreateTexture(width, height,1);
		// ----
		//GetRenderSystem().GetTextureMgr().CreateDynamicTexture(width, height);
	}
	if(m_pTextTexture)
	{
		m_pTextTexture->FillLevel(pBuffer, size, width, height, 0);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CNode3DTextRender::drawUBB(int x, int y, const CUBB* pUBB)const
{
	int nVertexCount = pUBB->m_VB.size();
	if (nVertexCount)
	{
		Matrix mWorld = CRenderSystem::getSingleton().getWorldMatrix();
		Matrix mNewWorld = mWorld*Matrix::newTranslation(Vec3D(x, y, 0.0f));
		// ----
		CRenderSystem::getSingleton().setWorldMatrix(mNewWorld);
		// ----
		CRenderSystem & R = CRenderSystem::getSingleton();
		// ----
		R.SetTexture(0, m_pTextTexture);
		// ----
		R.SetFVF(FVF_XYZ|FVF_DIFFUSE|FVF_TEX1);
		// ----
		//if (m_nShadowBorder>0)
		{
			Vec4D vColor = m_uShadowColor;
			R.SetShader("text");
			R.setShaderFloatArray("gColor",&vColor, 3);
		}
		// ----
		R.DrawIndexedPrimitiveUP(VROT_TRIANGLE_LIST, 0, nVertexCount, nVertexCount / 2, & s_DrawTextIB, &pUBB->m_VB[0], sizeof(VERTEX_TEXT));
		// ----
		CRenderSystem::getSingleton().setWorldMatrix(mWorld);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------