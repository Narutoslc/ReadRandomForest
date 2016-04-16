#include "stdafx.h"
//#include <CL/cl.h>
#include "RandomForest.h"
//#include "clUtil.h"

typedef struct  
{
	UINT ind;
	UCHAR v[5];
} tmpNodeValueStorage;


RandomForest::RandomForest()
	: m_pValue(NULL)
	//m_cl_tree(NULL),
	//m_context(NULL)
{
}


RandomForest::~RandomForest()
{
	//if (m_ppTree)
	//{
	//	for (int i = 0; i < TREE_COUNT; i ++)
	//	{
	//		if (m_ppTree[i])
	//		{
	//			delete [] m_ppTree[i];
	//			m_ppTree[i] = NULL;
	//		}
	//	}
	//	delete [] m_ppTree;
	//	// m_ppTree = NULL;
	//}

	delete [] m_ppTree[0];
	if (m_pValue)
	{
		delete [] m_pValue;
		m_pValue = NULL;
	}
}


bool RandomForest::BuildForest(const  char * pszData, const int size)
{
	// check input
	if (!pszData || FOREST_DATA_SIZE != size)
	{
		return FALSE;
	}

	memset(m_ppTree, 0, TREE_COUNT * sizeof(TreeNode *));//@xu-li: sizeof(TreeNode *)�ڵ��С������m_ppTree�Ǹ����飬��������ָ���������ĸ��ڵ��ָ�룬��ֻ�������ڵ�Ĵ�С

	 //for (int i = 0; i < TREE_COUNT; i ++)
	 //{
	TreeNode * t = new TreeNode[NODE_COUNT * 3];
	if (t == NULL)
	{
		return FALSE;
	}

	m_ppTree[0] = t;
	m_ppTree[1] = t + NODE_COUNT;
	m_ppTree[2] = t + NODE_COUNT * 2;

	//}
	//}

	if (m_pValue == NULL)
	{
		m_pValue = new NodeValue[VALUE_COUNT];
		if (m_pValue == NULL)
		{
			return FALSE;
		}
	}


	const char * pForestData = pszData;
	for (int i=0; i<TREE_COUNT; i++)
	{
		TreeNode * n = m_ppTree[i];
		int ind = -1;
		while (++ind < NODE_COUNT)
		{
			TreeNode * src = (TreeNode *)pForestData;//@xu-li:ǿ��ת�����ı�ָ�����ͣ�ֻ�Ը�ֵ������Ӱ��
			n[ind].left = src->left;
			n[ind].right = src->right;
			n[ind].ux = src->ux;
			n[ind].uy = src->uy;
			n[ind].vx = src->vx;
			n[ind].vy = src->vy;
			n[ind].c  = src->c;
			pForestData += 0x10;					//@xu-li:pForestData��const char����ָ�룬һ���ƶ�һ���ֽڣ������ǿ��ת�������ı�ָ������
		}											//@xu-li:�ڵ����ݽṹ������ʵ����ֻ��14���ֽڣ���pForestDataÿ�μ�16���ֽڣ�˵��ģ�����ݴ洢ʱ�п���
		ASSERT(ind == NODE_COUNT);
		if (ind != NODE_COUNT)
		{
			return FALSE;
		}
	}

	{
		NodeValue * v = m_pValue;
		int ind = -1;
		while (++ind < VALUE_COUNT)
		{
			tmpNodeValueStorage * src = (tmpNodeValueStorage *)pForestData;
			UINT value_indices = src->ind;
			v[ind].v[0].id  = (unsigned char)(value_indices & 0x0000001F);//@xu-li:5λ��id
			v[ind].v[0].cnt = src->v[0];
			value_indices >>= 5;
			v[ind].v[1].id  = (unsigned char)(value_indices & 0x0000001F);
			v[ind].v[1].cnt = src->v[1];
			value_indices >>= 5;
			v[ind].v[2].id  = (unsigned char)(value_indices & 0x0000001F);
			v[ind].v[2].cnt = src->v[2];
			value_indices >>= 5;
			v[ind].v[3].id  = (unsigned char)(value_indices & 0x0000001F);
			v[ind].v[3].cnt = src->v[3];
			value_indices >>= 5;
			v[ind].v[4].id  = (unsigned char)(value_indices & 0x0000001F);
			v[ind].v[4].cnt = src->v[4];
			pForestData += 9;
		}
		ASSERT(ind == VALUE_COUNT);
		if (ind != VALUE_COUNT)
		{
			return FALSE;
		}
	}
	if (pForestData != pszData + FOREST_DATA_SIZE)
	{
		return FALSE;
	}



	return TRUE;
}

const TreeNode * RandomForest::Tree(int treeID) const
{
	if (treeID < 0 || treeID >= TREE_COUNT)
	{
		return NULL;
	}
	else
	{
		return m_ppTree[treeID];
	}
}


const TreeNode * RandomForest::Node(int treeID, int nodeID) const
{
	if (treeID < 0 || treeID >= TREE_COUNT ||
		nodeID < 0 || nodeID >= NODE_COUNT)
	{
		return NULL;
	}
	else
	{
		return &(m_ppTree[treeID][nodeID]);
	}
}


const NodeValue * RandomForest::Value(int valueID) const
{
	if (valueID < 0 || valueID >= VALUE_COUNT)
	{
		return NULL;
	}
	else
	{
		return &(m_pValue[valueID]);
	}
}

bool RandomForest::WriteForest(char* pwirtefilename)
{
	ASSERT(pwirtefilename != '\0', );

	ofstream ofileout;
	ofileout.open(pwirtefilename, ios::binary | ios::out);
	
	ASSERT(ofileout.is_open() != false);

	printf("writing random forest structure...");
	ofileout.write((char*)(&TREE_COUNT),sizeof(TREE_COUNT));
	ofileout.write((char*)(&TREE_DEPTH), sizeof(TREE_DEPTH));

	const TreeNode* pnode;
	int idxnode;
	for (int idxtree = 0; idxtree < TREE_COUNT; idxtree++)
	{
		for (idxnode = 0; idxnode < NODE_COUNT; idxnode++)
		{
			pnode = this->Node(idxtree, idxnode);
			ASSERT(pnode != NULL);
			ofileout.write((char*)(pnode),sizeof(TreeNode));
		}

		ASSERT(idxnode == NODE_COUNT);
	}

	printf(" done!\n");

	printf("writing random forest node value...");
	ofileout.write((char*)(&VALUE_COUNT),sizeof(VALUE_COUNT));
	
	tmpNodeValueStorage nodevalue;
	for (idxnode = 0; idxnode < VALUE_COUNT; idxnode++)
	{
		nodevalue.ind = m_pValue[idxnode].v[0].id;
		nodevalue.v[0] = m_pValue[idxnode].v[0].cnt;

		nodevalue.ind <<= 5;
		nodevalue.ind += m_pValue[idxnode].v[1].id;
		nodevalue.v[1] = m_pValue[idxnode].v[1].cnt;

		nodevalue.ind <<= 5;
		nodevalue.ind += m_pValue[idxnode].v[2].id;
		nodevalue.v[2] = m_pValue[idxnode].v[2].cnt;

		nodevalue.ind <<= 5;
		nodevalue.ind += m_pValue[idxnode].v[3].id;
		nodevalue.v[3] = m_pValue[idxnode].v[3].cnt;

		nodevalue.ind <<= 5;
		nodevalue.ind += m_pValue[idxnode].v[4].id;
		nodevalue.v[4] = m_pValue[idxnode].v[4].cnt;

		nodevalue.ind <<= 5;
		nodevalue.ind += m_pValue[idxnode].v[5].id;
		nodevalue.v[5] = m_pValue[idxnode].v[5].cnt;

		ofileout.write((char*)(&nodevalue),sizeof(nodevalue));
	}

	ofileout.close();

	printf("done!\n");

	return true;
}

bool RandomForest::BuildRandomForestFromUnzipfile(char* pfilename)
{
	ASSERT(pfilename != '\0');
	if (pfilename == NULL || pfilename == '\0')
		return false;

	ifstream ifile;
	ifile.open(pfilename, ios::binary | ios::in);
	return true;
}