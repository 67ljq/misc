/*
 * This file is part of the misc project.
 *
 * Copyright (C) 2018 by ljq.
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>


#include "configfile.h"


/*Delete the left space.*/
static char * leaf_trim(char * pszOut,const char * pszIn)
{
	assert(pszIn != NULL);
    assert(pszOut != NULL);
    assert(pszOut != pszIn);

	for(;*pszIn !='\0' && isspace(*pszIn); pszIn++)
	{
	}
	return strcpy(pszOut, pszIn);
}

/*Delete spaces on both sides.*/
static char * both_trim(char * pszOut,const char * pszIn)
{
	char *p = NULL;
	assert(pszIn != NULL);
    assert(pszOut != NULL);
    assert(pszOut != pszIn);

	leaf_trim(pszOut, pszIn);
	for(p=pszOut+strlen(pszOut)-1; p>=pszOut && isspace(*p); p--)
	{
	}
	*(++p) = '\0';

	return pszOut;
}

/*
 *Read the corresponding values from the configuration file.
 */
int get_profile_string(char * profile, char *pAppName, char *pKeyName, char *pOutput, int iOutputLen)
{
	FILE *fp = NULL;
	char appname[32],keyname[32];
	char *buf,*c;
	char buf_read[KEYVALLEN], buf_trim[KEYVALLEN];
	int found=0; /* 1 AppName 2 KeyName */
	
	if (profile == NULL || pAppName == NULL || pKeyName == NULL || pOutput == NULL)
    {
        printf("[%s][%d]: input parameter(s) is NULL!\n",__func__, __LINE__);
        return -1;
    }

	fp = fopen(profile, "r");
    if (fp == NULL)
    {
        printf("[%s][%d]: open %s failed!\n",__func__, __LINE__, profile);
        return -1;
    }
	fseek(fp, 0, SEEK_SET);
    memset(appname, 0, sizeof(appname) );
    snprintf(appname, sizeof(appname) ,"[%s]", pAppName);

	while( !feof(fp) && fgets( buf_read, KEYVALLEN, fp )!=NULL )
	{
		leaf_trim(buf_trim, buf_read);
		if(strlen(buf_trim) <= 0)
			continue;
		
		buf = buf_trim;
		if(found == 0)
		{
			//find AppName
			if(buf[0] != '[')
				continue;
			else if(strncmp(buf , appname, strlen(appname)) == 0)
			{
				found = 1;
				continue;
			}
		}
		else if(found == 1)
		{
			//find KeyName
			if(buf[0] == '#')
				continue;
			else if(buf[0] == '[')
				break;
			else
			{
				if((c = (char*)strchr(buf,'=')) == NULL)
					continue;

				memset(keyname, 0, sizeof(keyname));
				sscanf( buf, "%[^=|^ |^\t]", keyname );
				if(strcmp(keyname, pKeyName) == 0)
				{
					sscanf( ++c, "%[^\n]", pOutput);
					char *pOutput_trim = (char *)malloc(iOutputLen);
					if(pOutput_trim != NULL)
					{
						memset(pOutput_trim, 0, iOutputLen);
						both_trim(pOutput_trim, pOutput);
						if(pOutput_trim && strlen(pOutput_trim) > 0)
							strncpy(pOutput, pOutput_trim, iOutputLen);
						free(pOutput_trim);
						pOutput_trim = NULL;
					}
					found = 2;
					break;
				}
				else
					continue;
			}
		}
	}

	fclose(fp);
	if(found == 2)
		return 0;
	else
		return -1;
}

/*
 *Set the corresponding value to the configuration file.
 */
int set_profile_string(char * profile, char *pAppName, char *pKeyName, char *pInput)
{
	FILE *fp = NULL;
	char appname[32],keyname[32];
	char *buf,*c;
	char buf_read[KEYVALLEN], buf_trim[KEYVALLEN];
	int found=0, done = 0; /* 1 AppName 2 KeyName */
	int file_lenth;
	char *buf_write;
	
	if (profile == NULL || pAppName == NULL || pKeyName == NULL || pInput == NULL)
    {
        printf("[%s][%d]: input parameter(s) is NULL!\n",__func__, __LINE__);
        return -1;
    }

	fp = fopen(profile, "r+");
    if (fp == NULL)
    {
		fp = fopen(profile, "w+");
		if(fp == NULL)
		{
        	printf("[%s][%d]: open %s failed!\n",__func__, __LINE__, profile);
        	return -1;
		}
    }
	fseek(fp, 0, SEEK_END);
	file_lenth = ftell(fp);
	fseek(fp, 0, SEEK_SET);
    memset(appname, 0, sizeof(appname) );
    snprintf(appname, sizeof(appname) ,"[%s]", pAppName);

	if(file_lenth == 0)
	{
		snprintf(buf_trim, sizeof(buf_trim), "%s\r\n%s = %s",appname ,pKeyName, pInput);
		fwrite(buf_trim, 1, strlen(buf_trim), fp);
		fclose(fp);
		return 0;
	}

	buf_write = (char *)malloc(file_lenth + KEYVALLEN);
	if(buf_write == NULL)
	{
		printf("[%s][%d]:MALLOC failed!\n",__func__, __LINE__);
		fclose(fp);
		return -1;
	}
	memset(buf_write, 0, (file_lenth + KEYVALLEN));
	//find position
	while(fgets( buf_read, KEYVALLEN, fp )!=NULL )
	{
		leaf_trim(buf_trim, buf_read);
		if(strlen(buf_trim) <= 0)
		{
			strcat(buf_write,buf_read);
			continue;
		}

		buf = buf_trim;
		if(found == 0)
		{
			//find AppName
			if(buf[0] != '[')
			{
				strcat(buf_write,buf_read);
				continue;
			}
			else if(strncmp(buf , appname, strlen(appname)) == 0)
			{
				found = 1;
				strcat(buf_write,buf_read);
				continue;
			}
		}
		else if(found == 1)
		{
			//find KeyName
			if(buf[0] == '#')
			{
				strcat(buf_write,buf_read);
				continue;
			}
			else if(buf[0] == '[')
			{
				memset(buf_trim, 0, sizeof(buf_trim));
				snprintf(buf_trim, sizeof(buf_trim), "%s = %s\r\n", pKeyName, pInput);
				strcat(buf_write, buf_trim);
				strcat(buf_write,buf_read);
				fread(buf_write+strlen(buf_write), 1, file_lenth, fp);
				done = 1;
				break;
			}
			else
			{
				if((c = (char*)strchr(buf,'=')) == NULL)
				{
					strcat(buf_write,buf_read);
					continue;
				}

				memset(keyname, 0, sizeof(keyname));
				sscanf( buf, "%[^=|^ |^\t]", keyname );
				if(strcmp(keyname, pKeyName) == 0)
				{
					memset(buf_trim, 0, sizeof(buf_trim));
					snprintf(buf_trim, sizeof(buf_trim), "%s = %s\r\n", pKeyName, pInput);
					strcat(buf_write, buf_trim);
					fread(buf_write+strlen(buf_write), 1, file_lenth, fp);
					found = 2;
					done = 1;
					break;
				}
			}
		}

		strcat(buf_write,buf_read);
	}

	if(done == 0)
	{
		memset(buf_trim, 0, sizeof(buf_trim));
		if(found == 1)
		{
			if(buf_read[strlen(buf_read)-1] == '\n')
				snprintf(buf_trim, sizeof(buf_trim), "%s = %s", pKeyName, pInput);
			else
				snprintf(buf_trim, sizeof(buf_trim), "\r\n%s = %s", pKeyName, pInput);
		}
		else
		{
			if(buf_read[strlen(buf_read)-1] == '\n')
				snprintf(buf_trim, sizeof(buf_trim), "%s\r\n%s = %s", appname, pKeyName, pInput);
			else
				snprintf(buf_trim, sizeof(buf_trim), "\r\n%s\r\n%s = %s", appname, pKeyName, pInput);
		}
		strcat(buf_write, buf_trim);
	}
	
	remove(profile);
	fclose(fp);
	fp = NULL;
	fp = fopen(profile,"w+");
	if(fp == NULL)
	{
		printf("[%s][%d]: open %s failed!\n",__func__, __LINE__, profile);
		free(buf_write);
		return -1;
	}
	fseek(fp,0,SEEK_SET);
	fputs(buf_write,fp);
	free(buf_write);
	fclose(fp);

	return 0;
}

