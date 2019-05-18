/*
	GitHisRemove.exe リポジトリ名
*/

#include "C:\Factory\Common\all.h"

#define REDIR_FILE "C:\\temp\\1.tmp"

static void Main2(char *repositoryName)
{
	char *repositoryDir = combine("C:\\huge\\GitHub", repositoryName);
	autoList_t *files = newList();
	char *file;
	uint index;

	errorCase_m(!existDir(repositoryDir), "そんなリポジトリありません。");

	addCwd(repositoryDir);
	{
		coExecute_x(xcout("C:\\var\\bat\\go log --name-status > " REDIR_FILE));

		{
			autoList_t *lines = readLines(REDIR_FILE);
			char *line;
			uint index;

			foreach(lines, line, index)
			{
				if(strchr(line, '\t'))
				{
					autoList_t *tokens;
					uint token_index;

					errorCase(
						!lineExp("<1,,09AZaz>\t<1,, ~>", line) &&
						!lineExp("<1,,09AZaz>\t<1,, ~>\t<1,, ~>", line)
						);

					tokens = tokenize(line, '\t');

					for(token_index = 1; token_index < getCount(tokens); token_index++)
					{
						addElement(files, (uint)strx(getLine(tokens, token_index)));
					}
					releaseDim(tokens, 1);
				}
			}
			releaseDim(lines, 1);
		}

		files = autoDistinctLines(files);
		files = selectLines_x(files);

		foreach(files, file, index)
			cout("削除対象 ⇒ %s\n", file);

		cout("続行？\n");

		if(clearGetKey() == 0x1b)
			termination(0);

		cout("続行します。\n");

		foreach(files, file, index)
		{
			cout("削除 ⇒ %s\n", file);

			coExecute_x(xcout("C:\\var\\bat\\go filter-branch -f --tree-filter \"rm -f '%s'\" HEAD", file));
			coExecute("C:\\var\\bat\\go push -f");

			LOGPOS();
		}
		LOGPOS();
	}
	unaddCwd();

	memFree(repositoryDir);
	releaseDim(files, 1);
}
int main(int argc, char **argv)
{
	Main2(nextArg());
}
