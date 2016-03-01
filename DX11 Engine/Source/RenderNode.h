#pragma once
#include "stdafx.h"
#include "RenderObject.h"
#include "RenderContext.h"

// Forward Declarations //
class RenderObject;

class RenderNode
{
	RenderContext m_cContext;
	vector<RenderObject*> m_cObjectArr;

public:
	RenderNode() = default;
	~RenderNode() = default;

	RenderNode(RenderObject* cObject);

	void Render();
	void AddObject(RenderObject* cObject) { m_cObjectArr.push_back(cObject); }

	void Clear() { m_cObjectArr.clear(); }
	void Remove(RenderObject* cObject) { VECTOR_REMOVE(m_cObjectArr, cObject); }

	bool operator==(RenderObject* cObject) { return (m_cContext == cObject->GetContext()); }

	// Accessors //
	RenderContext& GetContext() { return m_cContext; }
	vector<RenderObject*> GetObjectArr() { return m_cObjectArr; }
};