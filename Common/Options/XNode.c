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

// ----

XNode_t *GetXmlChild(XNode_t *node, char *trgName, int ignoreCase)
{
	XNode_t *child;
	uint index;

	foreach(node->Children, child, index)
		if(!(ignoreCase ? mbs_stricmp : strcmp)(child->Name, trgName))
			return child;

	return NULL;
}
XNode_t *ne_GetXmlChild(XNode_t *node, char *trgName, int ignoreCase)
{
	XNode_t *child = GetXmlChild(node, trgName, ignoreCase);

	errorCase(!child);
	return child;
}
