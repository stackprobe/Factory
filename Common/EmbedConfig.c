#include "all.h"

#define HEADER "{fa2b2611-7ff1-4e17-a692-5d99b415aacc}" // shared_uuid:2
#define HEADER_SIZE 38

static char *EmbedConfig =
	HEADER
	"E" // EMBCFG_FACTORY_DIR_DISABLED
	"E" // EMBCFG_CHANGE_COLOR_DISABLED
	;

int isFactoryDirDisabled(void)
{
	return EmbedConfig[HEADER_SIZE + EMBCFG_FACTORY_DIR_DISABLED] == 'D';
}
int isFactoryDirEnabled(void)
{
	return !isFactoryDirDisabled();
}
int isChangeColorDisabled(void)
{
	return EmbedConfig[HEADER_SIZE + EMBCFG_CHANGE_COLOR_DISABLED] == 'D';
}
