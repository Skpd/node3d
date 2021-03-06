#include "ModelDisplay.h"
#include "RenderSystem.h"
#include "UIDialog.h"
#include "Graphics.h"
#include "Intersect.h"
#include "RenderNodeMgr.h"

CModelDisplay::CModelDisplay()
{
	// 生成摄像机的视角参数
	m_Camera.setYawAngle(PI/4);
	m_Camera.setPitchAngle(-PI/4);

	m_bShowBones		= false;
	m_bShowBounds		= false;
	m_bShowModel		= true;
	m_bShowWireframe	= false;
	m_bShowMaterial		= true;
	m_bShowParticles	= true;

	m_vEye = Vec3D(0,0,-4.0f);

	m_pRenderNode=NULL;
	m_TextRender.load("Data\\Fonts\\l_10646.ttf");
	m_TextRender.setFontSize(12);
}

CModelDisplay::~CModelDisplay()
{
}

#include "CSVFile.h"
void CModelDisplay::LoadModel(const std::string& strFilename)
{
	//S_DEL(m_pRenderNode);
	//m_pRenderNode = (CRenderNode *)CRenderNodeMgr::getInstance().loadRenderNode(strFilename.c_str());
}
void CModelDisplay::loadComplexModel(const std::string& strFilename)
{
	//if (m_pRenderNode&&m_pModelObject->getModelFilename()==strFilename)
	{
		//return;
	}
	//S_DEL(m_pRenderNode);
	//m_pRenderNode = (CRenderNode *)CRenderNodeMgr::getInstance().loadRenderNode(strFilename.c_str());
}

void CModelDisplay::OnFrameMove( double fTime, float fElapsedTime )
{
	// 设置投影矩阵
	//m_Camera.SetProjParams(PI/4, m_width, m_height, 10.0f, 256.0f);
	// 更新视矩阵
	m_Camera.FrameMove(fElapsedTime);
	// 渲染更新
	if (m_pRenderNode)
	{
		m_pRenderNode->frameMove(Matrix::UNIT,fTime,fElapsedTime);
	}
	// ----
	m_TextRender.OnFrameMove();
	CUIDisplay::OnFrameMove(fTime, fElapsedTime);
}

void MyMatrixPerspectiveReal(Matrix& Out,Vec3D& Eye, float width, float height, float fFar)
{
	float fXScale = 1.0f/width;
	float fYScale = 1.0f/height;

	float n = 1.0f /fFar;

	// [ -zEye*fXScale	0				xEye*fXScale	0		]
	// [ 0				-zEye*fYScale	yEye*fYScale	0		]
	// [ 0				0				1				0		]
	// [ 0				0				1				-zEye	]
	Out.Zero();
	Out._11 = -Eye.z*fXScale;
	Out._13 = -Eye.x*fXScale;
	Out._22 = -Eye.z*fYScale;
	Out._32 = -Eye.y*fYScale;
	Out._33 = fFar*n;
	Out._34 = -0*fFar*n;
	Out._43 = 1.0f;
	Out._44 = -Eye.z;
}

void CModelDisplay::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime )
{
	if (!IsVisible()&&!isStyleVisible())
	{
		return;
	}
	CRect<int> rcViewport = getViewport();
	CRenderSystem& R = GetRenderSystem();
	CShader* pShader = R.GetShaderMgr().getSharedShader();
	pShader->setFloat("g_fTime",fTime);
	pShader->setMatrix("g_mViewProj",m_Camera.GetProjXView());
	pShader->setMatrix("g_mView",m_Camera.getViewMatrix());
	pShader->setVec3D("g_vLightDir",Vec3D(1,1,1));
	pShader->setVec3D("g_vEyePot",m_Camera.getEyePt());
	// 设置投影矩阵&视矩阵
	//m_vEye = Vec3D(sin(fTime*0.3f), cos(fTime*0.5f),cos(fTime*0.8f)-2);
	Matrix mProj;
	float fWidth = rcViewport.getWidth()*0.01f;
	float fheight = rcViewport.getHeight()*.01f;
	MyMatrixPerspectiveReal(mProj, m_vEye, fWidth/fWidth, fheight/fWidth, 256.0f);
	m_Camera.SetProjParams(PI/4, rcViewport.getWidth(), rcViewport.getHeight(), 0.1f, 256.0f);
	R.setProjectionMatrix(/*mProj*/m_Camera.getProjMatrix());
	R.setViewMatrix(m_Camera.getViewMatrix());

	DirectionalLight	light(Vec4D(0.6f,0.6f,0.6f,0.6f),Vec4D(1.0f,1.0f,1.0f,1.0f),Vec4D(1.0f,1.0f,1.0f,1.0f),m_Camera.GetViewDir());
	R.SetDirectionalLight(0,light);
	// 设置视窗大小
	R.setViewport(rcViewport);
	// 背屏清0
	R.ClearBuffer(true,true,0xFF112244);
	R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
	R.SetSamplerFilter(1, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
	// 渲染天空盒
	// Coordinate
	{
		m_MeshCoordinate.setPos(Vec3D(0,0,0));
		m_MeshCoordinate.setScale(1);
		m_MeshCoordinate.render(Vec3D(0,0,0));
	}
	// Gird
	if (R.prepareMaterial("grid"))
	{
		GetGraphics().begin(VROT_TRIANGLE_FAN,4)
			.c(0xFF555555)
			.v(-10,0,-10).t( 0,20)
			.v(-10,0, 10).t( 0, 0)
			.v( 10,0, 10).t(20, 0)
			.v( 10,0,-10).t(20,20)
			.end();
		R.finishMaterial();
	}
	// 渲染模型
	if (m_pRenderNode)
	{
		if (m_bShowModel)
		{
			if (m_bShowMaterial)
			{
				m_pRenderNode->render(Matrix::UNIT);
				/*CTexture* pSceneTexture = m_SceneEffect.getSceneTexture();
				pShader->setTexture("g_texScene",pSceneTexture);
				R.SetBlendFunc(false);
				m_pRenderNode->render(MATERIAL_RENDER_BUMP,MATERIAL_RENDER_BUMP);*/
			}
			else
			{
				R.SetAlphaTestFunc(false);
				R.SetBlendFunc(false);
				R.SetDepthBufferFunc(true, true);
				R.SetLightingEnabled(true);
				R.SetTextureColorOP(0,TBOP_SOURCE2);
				R.SetTextureAlphaOP(0,TBOP_DISABLE);
				m_pRenderNode->render(Matrix::UNIT, MATERIAL_RENDER_NO_MATERIAL);
			}
		}
		if (!(m_bShowModel||m_bShowBones)||m_bShowWireframe)
		{
			R.SetFillMode( FILL_WIREFRAME );
			R.SetAlphaTestFunc(false);
			R.SetBlendFunc(false);
			R.SetDepthBufferFunc(true, true);
			R.SetLightingEnabled(false);
			R.SetTextureFactor(0xFF00FF00);
			R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
			R.SetTextureAlphaOP(0,TBOP_DISABLE);
			m_pRenderNode->render(Matrix::UNIT, MATERIAL_RENDER_NO_MATERIAL);
			R.SetFillMode( FILL_SOLID );
		}
		if (m_bShowBounds)
		{
			//m_pRenderNode->getModelData()->m_BoundMesh.draw();
		}
		if (m_bShowBones)
		{
			//m_pRenderNode->drawSkeleton(&m_TextRender);
		}
	}

	//m_SceneEffect.RenderBloom();
	// 重置渲染状态，利于UI绘制
	R.SetupRenderState();
	// 重置视窗大小
	R.setViewport(GetParentDialog()->GetBoundingBox());
	CUIDisplay::OnFrameRender(mTransform,fTime, fElapsedTime );
}

void CModelDisplay::OnMouseMove(POINT point)
{
	Vec3D vTargetPos;
	Vec3D vRayPos, vRayDir;
	m_Camera.GetPickRay( vRayPos, vRayDir, point.x, point.y,m_rcBoundingBox.getRECT());
	if (IsPressed())
	{
		if (GetKeyState(VK_RBUTTON)<0)
		{
			m_Camera.addMouseDelta(Vec3D(m_ptLastMousePosition.x-point.x,m_ptLastMousePosition.y-point.y,0));
		}
		else if (GetKeyState(VK_MBUTTON)<0)
		{
			Vec3D vPos = m_Camera.getTargetPos();
			// 基于摄像机的 yaw 创建旋转矩阵
			Matrix mCameraRot;
			mCameraRot.rotationYawPitchRoll(m_Camera.getYawAngle(), m_Camera.getPitchAngle(), 0);
			vPos += mCameraRot * Vec3D(-point.x+m_ptLastMousePosition.x, point.y-m_ptLastMousePosition.y, 0)*0.001f*m_Camera.GetRadius();
			m_Camera.setTargetPos(vPos);
		}
		else if (GetKeyState(VK_LBUTTON)<0)
		{
		}

	}
	m_ptLastMousePosition=point;
}

void CModelDisplay::OnMouseWheel(POINT point,short wheelDelta)
{
	if(ContainsPoint(point))
	{
		if (wheelDelta!=0)
		{
			m_Camera.addMouseDelta(Vec3D(0,0,-wheelDelta/12.0f*m_Camera.GetRadius()));
		}
	}
}

void CModelDisplay::OnLButtonDown(POINT point)
{
	SetPressed(true);
}

void CModelDisplay::OnLButtonUp(POINT point)
{
	if (IsPressed())
	{
		SetPressed(false);
	}
}

void CModelDisplay::OnRButtonDown(POINT point)
{
	SetPressed(true);
	m_ptLastMousePosition=point;
}

void CModelDisplay::OnRButtonUp(POINT point)
{
	if (IsPressed())
	{
		ReleaseCapture();
	}
}

void CModelDisplay::OnMButtonDown(POINT point)
{
	SetPressed(true);
	m_ptLastMousePosition=point;
}

void CModelDisplay::OnMButtonUp(POINT point)
{
	if (IsPressed())
	{
		SetPressed(false);
	}
}

void CModelDisplay::OnSize(const CRect<int>& rc)
{
	CUIDisplay::OnSize(rc);
	m_SceneEffect.Reset(GetBoundingBox());
}

CRenderNode* CModelDisplay::getModelObject()
{
	return m_pRenderNode;
}