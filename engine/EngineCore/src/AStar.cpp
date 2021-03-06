#include "AStar.h"

void AStar::addNode(int x,int y,int dir,int level,int father)
{
	if (!checkHash(x,y))
	{
		return;
	}

	m_nNodeCount++;
	int p = m_nNodeCount;
	// 当前状态到目标状态的估计代价
	int h = sqrtf((x-m_tx)*(x-m_tx)+(y-m_ty)*(y-m_ty))*5;//(7*abs(x-m_tx)+7*abs(y-m_ty)+3*abs(abs(x-m_tx)-abs(y-m_ty))) / 2;
	if (m_nMinH > h)
	{
		m_nMinH = h;
		m_nMinHNode = m_nAllNodeCount;
	}
	int f = level + h;//启发函数定义
	while( p > 1 )
	{
		int q = p >> 1;
		if( f < node[q].f )
			node[p] = node[q];
		else
			break;
		p = q;
	}
	node[p].x = x;
	node[p].y = y;
	node[p].f = f;
	node[p].level = level;
	node[p].n = m_nAllNodeCount;

	m_allnode[m_nAllNodeCount].dir = dir;
	m_allnode[m_nAllNodeCount].father = father;
	m_nAllNodeCount++;

	add2Hash(x,y);
}

AStar::Node AStar::getNode()
{
	Node Top = node[1];
	int p,q;
	Node a = node[m_nNodeCount];
	m_nNodeCount--;
	p = 1;
	q = p * 2;
	while( q <= m_nNodeCount )
	{
		if( (node[q+1].f < node[q].f) && (q < m_nNodeCount) )
			q++;
		if( node[q].f < a.f )
			node[p] = node[q];
		else
			break;
		p = q;
		q = p * 2;
	}
	node[p] = a;	
	return Top;
}