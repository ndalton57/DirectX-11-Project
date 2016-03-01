#include "RenderNode.h"

RenderNode::RenderNode(RenderObject* cObject)
{
	m_cContext = cObject->GetContext();
	m_cObjectArr.push_back(cObject);
}

void RenderNode::Render()
{
	m_cContext.Apply();

	for (uint i = 0; i < m_cObjectArr.size(); i++)
		m_cObjectArr[i]->Render();

	m_cContext.Revert();
}