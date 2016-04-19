#ifndef _RANDOMFOREST_
#define _RANDOMFOREST_
#include "fstream"
#include "iostream"

using namespace std;



// �������ڵ�
struct TreeNode			//14�ֽ�
{
    int  left;			//4�ֽ�
    int  right;			//4�ֽ�
    signed char ux, uy;
    signed char vx, vy;
    signed short c;
};


// Ҷ�ӽڵ�Value: ��λ���ĸ���
struct ClassDist
{
    unsigned char id;
    unsigned char cnt;
};


// Ҷ�ӽڵ�Value, 5���������������
struct NodeValue
{
    ClassDist v[5];
	bool operator != (const NodeValue &p1) const
	{
		for (int idx = 0; idx < 5; idx++)
		{
			if (this->v[idx].cnt != p1.v[idx].cnt || this->v[idx].id != p1.v[idx].id)
			{
				cout << (int) this->v[idx].cnt << ' ' << (int) p1.v[idx].cnt << ' '
					<< (int) this->v[idx].id << ' ' << (int) p1.v[idx].id << endl;
				cout << "...." << idx << "..." << endl;
				return true;
			}
		}
		return false;
	}

};


// ���ɭ��
class RandomForest
{
public:
    static const int TREE_COUNT = 3;						//@xu-li:3����
    static const int TREE_DEPTH = 18;						//@xu-li:18��?
    static const int NODE_COUNT = (1<<(TREE_DEPTH+1)) - 1;	//@xu-li:��TREE_DEPTH+1���Ƿ���Ҫ��1?
    static const int VALUE_COUNT = 835114;
    static const int FOREST_DATA_SIZE = 0x01f2af4a;

    RandomForest();
    ~RandomForest();
  //  BOOL BuildForest(const  char * pszData, const int size, cl_context & context, cl_command_queue &commandQueue);
	bool BuildForest(const char * pszData, const int size);
	const TreeNode  * Tree(int treeID) const;
    const TreeNode  * Node(int treeID, int nodeID) const;
    const NodeValue * Value(int valueID) const;
//private:
    TreeNode * m_ppTree[3]; //[TREE_COUNT][NODE_COUNT];
    NodeValue * m_pValue; //[VALUE_COUNT];
	//cl_mem m_cl_tree;
	//cl_mem m_cl_value;
	//cl_context  m_context;
	//cl_command_queue m_commandQueue;

	unsigned int m_TreeNumber;
	unsigned int m_TreeDepth;
	unsigned int m_NodeNumber;
	unsigned int m_ValueNumber;
	TreeNode** m_ppTree2;
	NodeValue* m_pValue2;

	bool WriteForest(char* pwirtefilename);
	bool BuildRandomForestFromUnzipfile(char* pfilename);
	bool cmpRandomForest(void);

	const TreeNode  * GetTree(int treeID) const;
	const TreeNode  * GetNode(int treeID, int nodeID) const;
	const NodeValue * GetValue(int valueID) const;

	int TreeNumber(void) { return m_TreeNumber; }
};


#endif// _RANDOMFOREST_
