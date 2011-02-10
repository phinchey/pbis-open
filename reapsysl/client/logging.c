/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the license, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.  You should have received a copy
 * of the GNU Lesser General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * LESSER GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        logging.c
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (RSYS)
 *
 *        Group Lookup and Management API
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 *          Kyle Stemen (kstemen@likewisesoftware.com)
 */
#include "includes.h"

RSYS_API
DWORD
RSysSetLogLevel(
    HANDLE      hUnnamedConnection,
    RSysLogLevel logLevel
    )
{
    PRSYS_LOG_INFO pLogInfo = {0};
    DWORD dwError = 0;

    dwError = RSysGetLogInfo(
                    hUnnamedConnection,
                    &pLogInfo);
    BAIL_ON_RSYS_ERROR(dwError);

    pLogInfo->maxAllowedLogLevel = logLevel;

    dwError = RSysSetLogInfo(
                    hUnnamedConnection,
                    pLogInfo);
    BAIL_ON_RSYS_ERROR(dwError);

cleanup:
    if (pLogInfo)
    {
        RSysFreeLogInfo(pLogInfo);
    }
    return dwError;

error:
    goto cleanup;
}

RSYS_API
DWORD
RSysSetLogInfo(
    HANDLE hUnnamedConnection,
    PRSYS_LOG_INFO pLogInfo
    )
{
    DWORD dwError = 0;
    PRSYS_CLIENT_CONNECTION_CONTEXT pContext =
                     (PRSYS_CLIENT_CONNECTION_CONTEXT)hUnnamedConnection;
    PRSYS_IPC_ERROR pError = NULL;

    LWMsgMessage request = LWMSG_MESSAGE_INITIALIZER;
    LWMsgMessage response = LWMSG_MESSAGE_INITIALIZER;

    request.tag = RSYS_Q_SET_LOGINFO;
    request.object = pLogInfo;

    dwError = MAP_LWMSG_ERROR(lwmsg_assoc_send_message_transact(
                              pContext->pAssoc,
                              &request,
                              &response));
    BAIL_ON_RSYS_ERROR(dwError);

    switch (response.tag)
    {
        case RSYS_R_SET_LOGINFO_SUCCESS:
            // response.object == NULL
            break;
        case RSYS_R_SET_LOGINFO_FAILURE:
            pError = (PRSYS_IPC_ERROR) response.object;
            dwError = pError->dwError;
            BAIL_ON_RSYS_ERROR(dwError);
            break;
        default:
            dwError = EINVAL;
            BAIL_ON_RSYS_ERROR(dwError);
    }

cleanup:

    return dwError;

error:
    if (response.object)
    {
        lwmsg_assoc_free_message(pContext->pAssoc, &response);
    }

    goto cleanup;
}

RSYS_API
DWORD
RSysGetLogInfo(
    HANDLE         hUnnamedConnection,
    PRSYS_LOG_INFO* ppLogInfo
    )
{
    DWORD dwError = 0;
    PRSYS_CLIENT_CONNECTION_CONTEXT pContext =
                     (PRSYS_CLIENT_CONNECTION_CONTEXT)hUnnamedConnection;
    PRSYS_IPC_ERROR pError = NULL;

    LWMsgMessage request = LWMSG_MESSAGE_INITIALIZER;
    LWMsgMessage response = LWMSG_MESSAGE_INITIALIZER;

    request.tag = RSYS_Q_GET_LOGINFO;
    request.object = NULL;

    dwError = MAP_LWMSG_ERROR(lwmsg_assoc_send_message_transact(
                              pContext->pAssoc,
                              &request,
                              &response));
    BAIL_ON_RSYS_ERROR(dwError);

    switch (response.tag)
    {
        case RSYS_R_GET_LOGINFO_SUCCESS:
            *ppLogInfo = (PRSYS_LOG_INFO) response.object;
            break;
        case RSYS_R_GET_LOGINFO_FAILURE:
            pError = (PRSYS_IPC_ERROR) response.object;
            dwError = pError->dwError;
            BAIL_ON_RSYS_ERROR(dwError);
            break;
        default:
            dwError = EINVAL;
            BAIL_ON_RSYS_ERROR(dwError);
    }

cleanup:

    return dwError;

error:
    if (response.object)
    {
        lwmsg_assoc_free_message(pContext->pAssoc, &response);
    }

    goto cleanup;
}

RSYS_API
VOID
RSysFreeLogInfo(
    PRSYS_LOG_INFO pLogInfo
    )
{
    RtlCStringFree(&pLogInfo->pszPath);
    RtlMemoryFree(pLogInfo);
}
