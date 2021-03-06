#pragma once
#include "D3D9Shader.h"
#include "D3D9Texture.h"
#include "RenderSystem.h"
#include "D3D9RenderSystemCommon.h"
#include "IORead.h"
#include "FileSystem.h"
#include "D3D9RenderSystem.h"

ID3DXEffectPool* CD3D9Shader::ms_pEffectPool=NULL;

CD3D9Shader::CD3D9Shader()
{
	m_pEffect=NULL;
}

CD3D9Shader::~CD3D9Shader()
{
	S_REL(m_pEffect);
}

void CD3D9Shader::OnResetDevice()
{
	m_pEffect->OnResetDevice();
}

void CD3D9Shader::OnLostDevice()
{
	m_pEffect->OnLostDevice();
}

void CD3D9Shader::OnDestroyDevice()
{
	D3D9S_REL(m_pEffect);
}
class CShaderIncludeManager:public ID3DXInclude
{
public:
	void setParentFilename(const char* szFilename){m_strParentFilename = szFilename;}
	const char* getParentFilename(){return m_strParentFilename.c_str();}
protected:
	STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
	{
		std::string strFilename = GetParentPath(m_strParentFilename)+pFileName;
		IOReadBase* pRead = IOReadBase::autoOpen(strFilename);
		if (pRead)
		{
			size_t uFilesize = pRead->GetSize();
			char* pBuf = new char[uFilesize];
			if (pBuf)
			{
				pRead->Read(pBuf, uFilesize);
				*ppData = pBuf;
				*pBytes = uFilesize;
			}
			IOReadBase::autoClose(pRead);
		}
		return S_OK;
	}
	STDMETHOD(Close)(THIS_ LPCVOID pData)
	{
		if (pData)
		{
			delete[] pData;
		}
		return S_OK;
	}
private:
	std::string m_strParentFilename;
};

bool CD3D9Shader::create(const char* szFilename)
{
	bool bRet = false;
	IOReadBase* pRead = IOReadBase::autoOpen(szFilename);
	if (pRead)
	{
		size_t uFilesize = pRead->GetSize();
		char* pBuf = new char[uFilesize];
		if (pBuf)
		{
			pRead->Read(pBuf, uFilesize);
			CShaderIncludeManager shaderIncludeManager;
			shaderIncludeManager.setParentFilename(szFilename);
			bRet = createFromMemory(pBuf,uFilesize,&shaderIncludeManager);
			delete[] pBuf;
		}
		IOReadBase::autoClose(pRead);
	}
	return bRet;
}

bool CD3D9Shader::createFromMemory(void* pBuf, int nSize,LPD3DXINCLUDE pInclude)
{
	IDirect3DDevice9* pD3D9Device = GetD3D9RenderSystem().GetD3D9Device();
	if (ms_pEffectPool==NULL)
	{
		D3DXCreateEffectPool( & ms_pEffectPool);
	}

	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
	ID3DXBuffer* errorBuffer = 0;
	D3D9HR(D3DXCreateEffect(pD3D9Device,pBuf,nSize, NULL, pInclude, dwShaderFlags, 
		ms_pEffectPool, &m_pEffect, &errorBuffer));
	// output any error messages
	if(errorBuffer)
	{
		const char* szFilenam = NULL;
		if (pInclude)
		{
			szFilenam = ((CShaderIncludeManager*)pInclude)->getParentFilename();
		}
		::MessageBoxA(0, (char*)errorBuffer->GetBufferPointer(), szFilenam, 0);
		S_REL(errorBuffer);
	}
	return m_pEffect!=NULL;
}

ID3DXEffect* CD3D9Shader::getD3DXEffect()
{
	return m_pEffect;
}

void CD3D9Shader::setFloat(const char* szName, float val)
{
	m_pEffect->SetFloat(szName,val);
}

void CD3D9Shader::setFloatArray(const char* szName, const float* pVal, int nCount)
{
	m_pEffect->SetFloatArray(szName,pVal,nCount);
}

void CD3D9Shader::setMatrix(const char* szName, const Matrix& mat)
{
	Matrix mDx=mat;mDx.transpose();
	m_pEffect->SetMatrix(szName,(D3DXMATRIX*)&mDx);
}

void CD3D9Shader::setTexture(const char* szName, const CTexture* pTexture)
{
	LPDIRECT3DTEXTURE9 pD3DTexture = NULL;
	if (pTexture)
	{
		pD3DTexture=static_cast<const CD3D9Texture*>(pTexture)->GetD3D9Texture();
	}
	m_pEffect->SetTexture(szName, pD3DTexture);
}

void CD3D9Shader::setRenderType(int nRenderType)
{
	m_nRenderType = nRenderType;
}

int CD3D9Shader::getRenderType()
{
	return m_nRenderType;
}

bool CD3D9Shader::begin(const char* szTec)
{
	m_pEffect->SetTechnique(szTec);
	UINT cPasses;
	m_pEffect->Begin(&cPasses, D3DXFX_DONOTSAVESTATE);
	m_pEffect->BeginPass(0);
	//m_pEffect->CommitChanges();
	return true;
}

void CD3D9Shader::end()
{
	m_pEffect->EndPass();
	m_pEffect->End();
}
