#include "SolutionOrder.h"

typedef struct SourceInfo_st
{
	char *Source;
	autoList_t *RefSolutions;
}
SourceInfo_t;

autoList_t *GetReferenceSolutions(char *source)
{
	autoList_t *solutions = newList();

	cout("< %s\n", source);

	if(!_stricmp(getExt(source), "sln"))
	{
		char *projName = changeExt(getLocal(source), "");
		char *projFile;

		projFile = changeLocal(source, "");
		projFile = addLocal(projFile, projName);
		projFile = addLocal(projFile, projName);
		projFile = addExt(projFile, "csproj");

		if(existFile(projFile))
		{
			autoList_t *lines = readLines(projFile);
			char *line;
			uint index;
			char *projFileDir = changeLocal(projFile, "");

			ucTrimEdgeAllLine(lines);

			foreach(lines, line, index)
			{
				if(lineExpICase("/<HintPath><1,,>\\<1,,>\\bin\\Release\\<1,,>.dll/<//HintPath>", line))
				//                                 ~~~~~                ~~~~~
				//                                   ^                    ^
				//                                   |                    |
				//                                   +--------------------+---- プロジェクト名 (ソリューション名)
				{
					char *solution = line + 10;
					char *p;

					p = mbs_stristr(solution, "\\bin\\Release\\"); // fixme: 最後のパターンを探すべき。
					errorCase(!p); // 2bs
					strcpy(p, ".sln");

					addCwd(projFileDir);
					{
						solution = makeFullPath(solution); // ここで solution 複製
					}
					unaddCwd();

					if(existFile(solution))
					{
						cout("> %s\n", solution);
						addElement(solutions, (uint)strx(solution));
					}
					memFree(solution);
				}
			}
			releaseDim(lines, 1);
			memFree(projFileDir);
		}
		memFree(projName);
		memFree(projFile);
	}
	return solutions;
}
static uint FindLastReference(autoList_t *infos, SourceInfo_t *info, uint index)
{
	uint lastRefIndex = 0;

	for(; index < getCount(infos); index++)
	{
		SourceInfo_t *info = (SourceInfo_t *)getElement(infos, index);
		uint i;

		for(i = 0; i < getCount(info->RefSolutions); i++)
		{
			if(!_stricmp(info->Source, getLine(info->RefSolutions, i)))
			{
				LOGPOS();
				lastRefIndex = index;
				break;
			}
		}
	}
	return lastRefIndex;
}
void SolutionOrder(autoList_t *sources) // sources: 全てフルパスを想定する。
{
	autoList_t *infos = newList();
	char *source;
	uint index;

	LOGPOS();

	foreach(sources, source, index)
	{
		SourceInfo_t *info = nb(SourceInfo_t);

		info->Source = source;
		info->RefSolutions = GetReferenceSolutions(source);

		addElement(infos, (uint)info);
	}
	LOGPOS();

	for(index = getCount(infos); index; index--)
	{
		SourceInfo_t *info = (SourceInfo_t *)getElement(infos, index - 1);
		uint lastRefIndex;

		lastRefIndex = FindLastReference(infos, info, index + 1);

		if(lastRefIndex)
		{
			LOGPOS();
			insertElement(infos, lastRefIndex, desertElement(infos, index));
		}
	}
	LOGPOS();

	for(index = 0; index < getCount(infos); index++)
	{
		SourceInfo_t *info = (SourceInfo_t *)getElement(infos, index);

		cout("+ %s\n", info->Source);

		setElement(sources, index, (uint)info->Source);

		releaseDim(info->RefSolutions, 1);
		memFree(info);
	}
	LOGPOS();

	releaseAutoList(infos);
}
