#include "BcpReader.h"

autoList_t *SqlBcpReader(char *bcpFile)
{
	autoList_t *lines;
	char *line;
	uint index;
	autoList_t *table = newList();

	errorCase(m_isEmpty(bcpFile));

	lines = readLines(bcpFile);

	foreach(lines, line, index)
	{
		line2JLine(line, 1, 0, 1, 1);

		addElement(table, (uint)tokenize(line, '\t'));
	}
	releaseDim(lines, 1);

	// check
	{
		if(getCount(table)) // ? 行が１つ以上ある。
		{
			uint colcnt = getCount(getList(table, 0));
			autoList_t * row;
			uint rowidx;
			char *cell;
			uint colidx;

			errorCase(colcnt < 1); // ? 列が１つも無い。

			foreach(table, row, rowidx)
			{
				errorCase(getCount(row) != colcnt); // ? 列数が一定ではない。

				foreach(row, cell, colidx)
				{
					errorCase(cell == NULL); // ? cell == NULL
				}
			}
		}
	}
	return table;
}
