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
 *        dlinkedlist.c
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (LSASS)
 * 
 *        Doubly linked list
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */

#include "includes.h"


static
VOID
LwDLinkedListNodeCounter(
    PVOID pData,
    PVOID pUserData
    );


DWORD
LwDLinkedListPrepend(
    PLW_DLINKED_LIST* ppList,
    PVOID        pItem
    )
{
    DWORD dwError = 0;
    PLW_DLINKED_LIST pList = NULL;
    
    dwError = LwAllocateMemory(sizeof(LW_DLINKED_LIST), (PVOID*)&pList);
    BAIL_ON_LW_ERROR(dwError);
    
    pList->pItem = pItem;
    
    if (*ppList) {
       (*ppList)->pPrev = pList;
       pList->pNext = *ppList;
       *ppList = pList;
    } else {
       *ppList = pList;
    }
    
cleanup:

    return dwError;
    
error:

    if (pList) {
        LwFreeMemory(pList);
    }

    goto cleanup;
}

DWORD
LwDLinkedListAppend(
    PLW_DLINKED_LIST* ppList,
    PVOID        pItem
    )
{
    DWORD dwError = 0;
    PLW_DLINKED_LIST pList = NULL;
    
    dwError = LwAllocateMemory(sizeof(LW_DLINKED_LIST), (PVOID*)&pList);
    BAIL_ON_LW_ERROR(dwError);
    
    pList->pItem = pItem;
    
    if (*ppList) {
       PLW_DLINKED_LIST pLast = NULL;
       PLW_DLINKED_LIST pCur = *ppList;
       while (pCur) {
             pLast = pCur;
             pCur = pCur->pNext;
       } 
       pLast->pNext = pList;
       pList->pPrev = pLast;
    } else {
       *ppList = pList;
    }
    
cleanup:

    return dwError;
    
error:

    if (pList) {
        LwFreeMemory(pList);
    }

    goto cleanup;    
}

BOOLEAN
LwDLinkedListDelete(
    PLW_DLINKED_LIST* ppList,
    PVOID        pItem
    )
{
    BOOLEAN bFound = FALSE;
    PLW_DLINKED_LIST pList = (ppList ? *ppList : NULL);
    PLW_DLINKED_LIST pCandidate = NULL;
    
    while (pList)
    {
        if (pList->pItem == pItem)
        {
            pCandidate = pList;
            bFound = TRUE;
            break;
        }
        pList = pList->pNext;
    }
    
    if (bFound) {
       if (pCandidate->pNext) {
          // Connect the next neighbor to our previous neighbor
          pCandidate->pNext->pPrev = pCandidate->pPrev;
       }
       if (pCandidate->pPrev) {
          // Connect the previous neighbor to our next neighbor
          pCandidate->pPrev->pNext = pCandidate->pNext;
       }
       if (*ppList == pCandidate) {
          *ppList = pCandidate->pNext;
       }
       pCandidate->pItem = NULL;
       LwFreeMemory(pCandidate);
    }
    
    return bFound;
}

VOID
LwDLinkedListForEach(
    PLW_DLINKED_LIST          pList,
    PFN_LW_DLINKED_LIST_FUNC pFunc,
    PVOID                pUserData
    )
{
    while (pList) {
        pFunc(pList->pItem, pUserData);
        pList = pList->pNext;
    }
}

DWORD
LwDLinkedListLength(
    PLW_DLINKED_LIST pList
    )
{
    DWORD dwCount = 0;
    LwDLinkedListForEach(pList,
                          LwDLinkedListNodeCounter,
                          &dwCount);

    return dwCount;
}


VOID
LwDLinkedListFree(
    PLW_DLINKED_LIST pList
    )
{
    while (pList)
    {
        PLW_DLINKED_LIST pTmp = pList;
        pList = pList->pNext;
        LwFreeMemory(pTmp);
    }
}


static
VOID
LwDLinkedListNodeCounter(
    PVOID pData,
    PVOID pUserData
    )
{
    PDWORD pdwCount = (PDWORD)pUserData;
    (*pdwCount)++;
}


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/

