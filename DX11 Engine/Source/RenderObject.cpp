#include "RenderObject.h"

#include "Renderer.h"
#include "MathWrapper.inl"
#include "DDSTextureLoader.h"

RenderObject::RenderObject(Mesh* cMesh, RenderContext cContext, const wchar_t* chDiffuseMapPath, const wchar_t* chNormalMapPath, const wchar_t* chSpecularMapPath)
{
	m_cMesh = cMesh;
	m_cContext = cContext;
	IdentityMatrix(m_fTransformMatrix);

	SetDiffuseMap(chDiffuseMapPath);
	SetNormalMap(chNormalMapPath);
	SetSpecularMap(chSpecularMapPath);

	CalculateTangents();
}

void RenderObject::Render(bool bContextOverride)
{
	ID3D11DeviceContext* d3dContext = GRAPHICS->GetContext();

	if (!m_bActive || m_bCulled) // Cull check
		return;

	if (bContextOverride) // In case you need to render on object that's not in a node
		m_cContext.Apply();

	MapBuffers(); // Map constant buffers

	// Set Shader Resource Views //
	d3dContext->PSSetShaderResources(0, 1, &m_d3dDiffuseMap); // t0
	d3dContext->PSSetShaderResources(1, 1, &m_d3dNormalMap); // t1
	d3dContext->PSSetShaderResources(2, 1, &m_d3dSpecularMap); // t2

	d3dContext->DrawIndexed((uint)m_cMesh->m_nIndexArr.size(), m_cMesh->m_nIndexStart, m_cMesh->m_nVertexStart);

	if (bContextOverride)
		m_cContext.Revert();
}

void RenderObject::CalculateTangents()
{
	//Loosely Based Off of Code available at http://www.terathon.com/code/tangent.html 
	//Credit goes to
	//Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html


	std::vector<XMFLOAT4> tan1;
	std::vector<XMFLOAT4> tan2;
	XMFLOAT4 temp = { 0, 0, 0, 0 };
	for (unsigned int i = 0; i < m_cMesh->m_cVertexArr.size(); i++)
	{
		tan1.push_back(temp);
		tan2.push_back(temp);
	}

	for (unsigned int index = 0; index < m_cMesh->m_nIndexArr.size(); index += 3)
	{
		float xyz1[3];
		float xyz2[3];
		float st1[2];
		float st2[2];

		Vertex3D& _v1 = m_cMesh->m_cVertexArr[m_cMesh->m_nIndexArr[index]];
		Vertex3D& _v2 = m_cMesh->m_cVertexArr[m_cMesh->m_nIndexArr[index + 1]];
		Vertex3D& _v3 = m_cMesh->m_cVertexArr[m_cMesh->m_nIndexArr[index + 2]];

		for (unsigned int i = 0; i < 3; i++)
		{
			xyz1[i] = _v2.m_fPosition[i] - _v1.m_fPosition[i];
			xyz2[i] = _v3.m_fPosition[i] - _v1.m_fPosition[i];
		}
		for (unsigned int i = 0; i < 2; i++)
		{
			st1[i] = _v2.m_fTexCoords[i] - _v1.m_fTexCoords[i];
			st2[i] = _v3.m_fTexCoords[i] - _v1.m_fTexCoords[i];
		}

		float r = 1.0f / (float)((st1[0] * st2[1]) - (st2[0] * st1[1]));
		XMVECTOR sdir = { (st2[1] * xyz1[0] - st1[1] * xyz2[0]) * r,
			(st2[1] * xyz1[1] - st1[1] * xyz2[1]) * r,
			(st2[1] * xyz1[2] - st1[1] * xyz2[2]) * r };

		XMVECTOR tdir = { (st1[0] * xyz2[0] - st2[0] * xyz1[0]) * r,
			(st1[0] * xyz2[1] - st2[0] * xyz1[1]) * r,
			(st1[0] * xyz2[2] - st2[0] * xyz1[2]) * r };

		// sdir = XMVector3Normalize(sdir);
		// tdir = XMVector3Normalize(tdir);

		tan1[m_cMesh->m_nIndexArr[index]].x += sdir.m128_f32[0];
		tan1[m_cMesh->m_nIndexArr[index]].y += sdir.m128_f32[1];
		tan1[m_cMesh->m_nIndexArr[index]].z += sdir.m128_f32[2];

		tan2[m_cMesh->m_nIndexArr[index]].x += tdir.m128_f32[0];
		tan2[m_cMesh->m_nIndexArr[index]].y += tdir.m128_f32[1];
		tan2[m_cMesh->m_nIndexArr[index]].z += tdir.m128_f32[2];

		tan1[m_cMesh->m_nIndexArr[index + 1]].x += sdir.m128_f32[0];
		tan1[m_cMesh->m_nIndexArr[index + 1]].y += sdir.m128_f32[1];
		tan1[m_cMesh->m_nIndexArr[index + 1]].z += sdir.m128_f32[2];

		tan2[m_cMesh->m_nIndexArr[index + 1]].x += tdir.m128_f32[0];
		tan2[m_cMesh->m_nIndexArr[index + 1]].y += tdir.m128_f32[1];
		tan2[m_cMesh->m_nIndexArr[index + 1]].z += tdir.m128_f32[2];

		tan1[m_cMesh->m_nIndexArr[index + 2]].x += sdir.m128_f32[0];
		tan1[m_cMesh->m_nIndexArr[index + 2]].y += sdir.m128_f32[1];
		tan1[m_cMesh->m_nIndexArr[index + 2]].z += sdir.m128_f32[2];

		tan2[m_cMesh->m_nIndexArr[index + 2]].x += tdir.m128_f32[0];
		tan2[m_cMesh->m_nIndexArr[index + 2]].y += tdir.m128_f32[1];
		tan2[m_cMesh->m_nIndexArr[index + 2]].z += tdir.m128_f32[2];

	}
	for (unsigned int index = 0; index < m_cMesh->m_cVertexArr.size(); index++)
	{
		XMVECTOR n;
		XMVECTOR t;
		t.m128_f32[0] = tan1[index].x;
		t.m128_f32[1] = tan1[index].y;
		t.m128_f32[2] = tan1[index].z;
		t.m128_f32[3] = tan1[index].w;

		XMVECTOR tang;
		for (unsigned int a = 0; a < 4; a++)
		{
			n.m128_f32[a] = m_cMesh->m_cVertexArr[index].m_fNormal[a];
		}

		n = XMVector3Normalize(n);
		t = XMVector3Normalize(t);
		//Gram-Schmidt orthogonalize
		tang = (t - n * XMVector3Dot(n, t));
		tang = XMVector3Normalize(tang);


		XMVECTOR t2;
		t2.m128_f32[0] = tan2[index].x;
		t2.m128_f32[1] = tan2[index].y;
		t2.m128_f32[2] = tan2[index].z;

		//calc handedness
		tang.m128_f32[3] = (XMVector3Dot(XMVector3Cross(n, t), t2)).m128_f32[0] < 0.0f ? -1.0f : 1.0f;
		for (unsigned int i = 0; i < 4; i++)
		{
			m_cMesh->m_cVertexArr[index].m_fTangent[i] = tang.m128_f32[i];

		}
	}
}

// --- Helper Functions --- //
void RenderObject::MapBuffers()
{
	ID3D11DeviceContext* d3dContext = GRAPHICS->GetContext();
	ID3D11Buffer* d3dTransformBuffer = GRAPHICS->GetTransformBuffer();
	ID3D11Buffer* d3dPropertyBuffer = GRAPHICS->GetPropertyBuffer();

	D3D11_MAPPED_SUBRESOURCE d3dResource;
	
	// Send Transform Matrix To GPU //
	ZeroMemory(&d3dResource, sizeof(d3dResource));
	d3dContext->Map(d3dTransformBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &d3dResource);
	memcpy(d3dResource.pData, &m_fTransformMatrix, sizeof(m_fTransformMatrix));
	d3dContext->Unmap(d3dTransformBuffer, 0);

	// Send Properties To GPU // - Color and transparency level are the only properties right now
	ZeroMemory(&d3dResource, sizeof(d3dResource));
	d3dContext->Map(d3dPropertyBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &d3dResource);
	memcpy(d3dResource.pData, &m_fColorOverride, sizeof(m_fColorOverride));
	d3dContext->Unmap(d3dPropertyBuffer, 0);
}

void RenderObject::SetTexture(ID3D11ShaderResourceView*& d3dTexture, const wchar_t* chFilePath, MapType tMap)
{
	const wchar_t* chErrorPath = nullptr;
	ID3D11Device* d3dDevice = GRAPHICS->GetDevice();
	unordered_map<wstring, ID3D11ShaderResourceView*>& d3dTextureList = GRAPHICS->GetTextureList();

	// Grab Correct Error Path //
	if (tMap == Diffuse)
		chErrorPath = DIFFUSE_ERROR_PATH;
	else if (tMap == Normal)
		chErrorPath = NORMAL_ERROR_PATH;
	else if (tMap == Specular)
		chErrorPath = SPECULAR_ERROR_PATH;

	if (chFilePath == nullptr) // No file path
		chFilePath = chErrorPath;

	if (d3dTextureList.find(chFilePath) != d3dTextureList.end()) // Check if texture's already been created
	{
		d3dTexture = d3dTextureList[chFilePath]; // Load existing texture
		OUTPUT("Texture Load Successful\n");
		return;
	}
	else if (SUCCEEDED(CreateDDSTextureFromFile(d3dDevice, chFilePath, nullptr, &d3dTexture))) // Texture load failed, create new texture
	{
		d3dTextureList[chFilePath] = d3dTexture; // Save new texture
		OUTPUT("Texture Creation Successful\n");
		return;
	}
	else if (d3dTextureList.find(chErrorPath) != d3dTextureList.end()) // Texture creation failed, check if error texture's already been created
	{
		d3dTexture = d3dTextureList[chErrorPath]; // Load existing error texture
		OUTPUT("Texture Creation Failed. Error Texture Loaded\n");
	}
	else if (SUCCEEDED(CreateDDSTextureFromFile(d3dDevice, chErrorPath, nullptr, &d3dTexture))) // Error texture load failed, create new error texture
	{
		d3dTextureList[chErrorPath] = d3dTexture; // Save new error texture
		OUTPUT("Error Texture Creation Successful\n");
	}
	else // You messed up. Check the error texture's path
		assert(!"Error texture not found");
}