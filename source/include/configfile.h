/*
 * This file is part of the misc project.
 *
 * Copyright (C) 2018 by ljq.
 */

#ifndef __confilefile_h
#define __confilefile_h

#ifdef __cplusplus
extern "C" {
#endif

#define KEYVALLEN 256


/*******************************************************************************/
/*										function prototype													 */
/*******************************************************************************/

int get_profile_string(char * profile, char *pAppName, char *pKeyName, char *pOutput, int iOutputLen);

int set_profile_string(char * profile, char *pAppName, char *pKeyName, char *pInput);


#ifdef __cplusplus
}
#endif

#endif
