#include "XNode.h"

static char *DefStrFltr(char *str)
{
	line2JLine(str, 1, 1, 1, 1);
	return str;
}
static char *StrFltrWrap(char *str, char *(*strFltr)(char *))
{
	if(!str)
		str = strx("");

	return strFltr(str);
}
void NormalizeXNode(XNode_t *root, char *(*strFltr)(char *))
{
	autoList_t *nodes = newList();

	errorCase(!root);

	if(!strFltr)
		strFltr = DefStrFltr;

	addElement(nodes, (uint)root);

	while(getCount(nodes))
	{
		XNode_t *node = (XNode_t *)unaddElement(nodes);

		node->Name = StrFltrWrap(node->Name, strFltr);
		node->Text = StrFltrWrap(node->Text, strFltr);

		if(!node->Children)
			node->Children = newList();

		removeZero(node->Children);
		addElements(nodes, node->Children);
	}
	releaseAutoList(nodes);
}
void ReleaseXNode(XNode_t *root)
{
	if(!root)
		return;

	memFree(root->Name);
	memFree(root->Text);

	if(root->Children)
		callAllElement_x(root->Children, (void (*)(uint))ReleaseXNode);

	memFree(root);
}

// ---- accessor ----

static autoList_t *CXN_PTkns;
static autoList_t *CXN_Dest;

static void CXN_Main(XNode_t *root, uint pTknIndex)
{
	if(pTknIndex < getCount(CXN_PTkns))
	{
		XNode_t *node;
		uint index;

		foreach(root->Children, node, index)
			if(!strcmp(node->Name, getLine(CXN_PTkns, pTknIndex)))
				CXN_Main(node, pTknIndex + 1);
	}
	else
	{
		addElement(CXN_Dest, (uint)root);
	}
}
autoList_t *CollectXNode(XNode_t *root, char *path)
{
	errorCase(!root);
	errorCase(!path);

	CXN_PTkns = tokenize(path, '/');
	CXN_Dest = newList();

	CXN_Main(root, 0);

	return CXN_Dest;
}
XNode_t *GetXNode(XNode_t *root, char *path)
{
	autoList_t *nodes = CollectXNode(root, path);
	XNode_t *node;

	errorCase(!getCount(nodes));

	node = (XNode_t *)getElement(nodes, 0);

	releaseAutoList(nodes);
	return node;
}
XNode_t *RefXNode(XNode_t *root, char *path)
{
	autoList_t *nodes = CollectXNode(root, path);
	XNode_t *node;

	if(!getCount(nodes))
	{
		static XNode_t *dmyNode;

		if(!dmyNode)
		{
			dmyNode = (XNode_t *)memAlloc(sizeof(XNode_t));

			dmyNode->Name = "Dummy";
			dmyNode->Text = "Dummy";
			dmyNode->Children = newList();
		}
		node = dmyNode;
	}
	else
		node = (XNode_t *)getElement(nodes, 0);

	releaseAutoList(nodes);
	return node;
}

// ----
