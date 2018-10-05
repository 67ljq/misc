#include <stdio.h>

#include "zlog.h"
#include "configfile.h"

#define ZLOG_PROFILE_PATH "../../usrcfg/zlog.conf"
#define ZLOG_APP_MSG "zlog_config"


int main()
{
	int rc = 0;
	char LOG_PROFILE_ERROR[120] = {0};
	char LOG_PROFILE_DEBUG[120] = {0};
	char zlog_dbg_level[20];
	
	printf("hi, 67\n");

	rc = get_profile_string(ZLOG_PROFILE_PATH, ZLOG_APP_MSG, "ZLOG_PROFILE_ERROR", LOG_PROFILE_ERROR, sizeof(LOG_PROFILE_ERROR));
	rc = get_profile_string(ZLOG_PROFILE_PATH, ZLOG_APP_MSG, "ZLOG_PROFILE_DEBUG", LOG_PROFILE_DEBUG, sizeof(LOG_PROFILE_DEBUG));
	if(rc)
	{
		printf("\nReadConfig error\n");
		return -1;
	}
	printf("======error env:%s======\n",LOG_PROFILE_ERROR);
	printf("======debug env:%s======\n",LOG_PROFILE_DEBUG);

	rc = set_profile_string("../../usrcfg/clog.conf", "zlog_config", "dbg_level", "Â½ÇÙ");
	if(rc < 0)
	{
		printf("\nReadConfig error\n");
		return -1;
	}

	
	setenv("ZLOG_PROFILE_ERROR", LOG_PROFILE_ERROR, 1);
	setenv("ZLOG_PROFILE_DEBUG", LOG_PROFILE_DEBUG, 1);
	rc = zlog_init(NULL);
	if(rc)
	{
		printf("\n---syntax error, see error message above\n");
		return -1;
	}
	
	return 0;
}
