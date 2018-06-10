#pragma once

#include "C:\Factory\Common\all.h"

/*
	pub_member
*/
typedef struct XNode_st
{
	char *Name; // ���̃m�[�h�̖��O
	char *Text; // ���̃m�[�h�̒l
	autoList_t *Children; // (XNode_t *)�̃��X�g
}
XNode_t;

void NormalizeXNode(XNode_t *root, char *(*strFltr)(char *));
void ReleaseXNode(XNode_t *root);

// ----

XNode_t *GetXmlChild(XNode_t *node, char *trgName, int ignoreCase);
XNode_t *ne_GetXmlChild(XNode_t *node, char *trgName, int ignoreCase);
