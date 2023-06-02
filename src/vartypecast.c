/*==============================================================================
MIT License

Copyright (c) 2023 Trevor Monk

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
==============================================================================*/

/*!
 * @defgroup vartypecast vartypecast
 * @brief Variable Action Script Typecast Operation Support functions
 * @{
 */

/*============================================================================*/
/*!
@file varmath.c

    Variable Action Script Typecast Operation Support functions

    The Variable Action Script Typecast Operation Support functions provide
    functions for handling type conversion between types.

    Operators include: (float), (int), (string), (short)

*/
/*============================================================================*/

/*==============================================================================
        Includes
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <syslog.h>
#include "vartypecast.h"
#include "varstrings.h"

/*==============================================================================
       Function declarations
==============================================================================*/

/*==============================================================================
       Definitions
==============================================================================*/

/*==============================================================================
       Function definitions
==============================================================================*/

/*============================================================================*/
/*  ToFloat                                                                   */
/*!
    Convert a variable to a floating point value

    The ToFloat function performs the following operation:

    result = (float)left

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    pResult
        pointer to the result node

@param[in]
    pLeft
        pointer to the left node

@param[in]
    pRight
        pointer to the right node

@retval EINVAL invalid argument
@retval EOK the script was successfully processed

==============================================================================*/
int ToFloat( VARSERVER_HANDLE hVarServer,
             Variable *pResult,
             Variable *pLeft,
             Variable *pRight )
{
    int result = EINVAL;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pResult->obj.val.f = (float)(pLeft->obj.val.ul);
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            case VARTYPE_UINT16:
                pResult->obj.val.f = (float)(pLeft->obj.val.ui);
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            case VARTYPE_FLOAT:
                pResult->obj.val.f = (float)(pLeft->obj.val.f);
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            case VARTYPE_STR:
                if( pLeft->obj.val.str != NULL )
                {
                    pResult->obj.val.f = atof(pLeft->obj.val.str);
                }
                else
                {
                    pResult->obj.val.f = 0.0;
                }
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            default:
                result = ENOTSUP;
                break;
        }
    }

    return result;
}

/*============================================================================*/
/*  ToShort                                                                   */
/*!
    Convert a variable to a short value

    The ToShort function performs the following operation:

    result = (short)left

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    pResult
        pointer to the result node

@param[in]
    pLeft
        pointer to the left node

@param[in]
    pRight
        pointer to the right node (unused)

@retval EINVAL invalid argument
@retval EOK the script was successfully processed

==============================================================================*/
int ToShort( VARSERVER_HANDLE hVarServer,
             Variable *pResult,
             Variable *pLeft,
             Variable *pRight )
{
    int result = EINVAL;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pResult->obj.val.ui = (short)(pLeft->obj.val.ul);
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_UINT16:
                pResult->obj.val.ui = pLeft->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pResult->obj.val.ui = (short)(pLeft->obj.val.f);
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_STR:
                if( pLeft->obj.val.str != NULL )
                {
                    pResult->obj.val.ui = atoi(pLeft->obj.val.str);
                }
                else
                {
                    pResult->obj.val.ui = 0;
                }
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            default:
                result = ENOTSUP;
                break;
        }
    }

    return result;
}

/*============================================================================*/
/*  ToInt                                                                     */
/*!
    Convert a variable to an int value

    The ToInt function performs the following operation:

    result = (int)left

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    pResult
        pointer to the result node

@param[in]
    pLeft
        pointer to the left node

@param[in]
    pRight
        pointer to the right node (unused)

@retval EINVAL invalid argument
@retval EOK the script was successfully processed

==============================================================================*/
int ToInt( VARSERVER_HANDLE hVarServer,
           Variable *pResult,
           Variable *pLeft,
           Variable *pRight )
{
    int result = EINVAL;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pResult->obj.val.ul = pLeft->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pResult->obj.val.ul = pLeft->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_FLOAT:
                pResult->obj.val.ul = (int)(pLeft->obj.val.f);
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_STR:
                if( pLeft->obj.val.str != NULL )
                {
                    pResult->obj.val.ul = atol(pLeft->obj.val.str);
                }
                else
                {
                    pResult->obj.val.ul = 0;
                }
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            default:
                result = ENOTSUP;
                break;
        }
    }

    return result;
}

/*============================================================================*/
/*  ToString                                                                  */
/*!
    Convert a variable to a string value

    The ToString function performs the following operation:

    result = (string)left

@param[in]
    hVarServer
        handle to the variable server

@param[in]
    pResult
        pointer to the result node

@param[in]
    pLeft
        pointer to the left node

@param[in]
    pRight
        pointer to the right node

@retval EINVAL invalid argument
@retval EOK the script was successfully processed

==============================================================================*/
int ToString( VARSERVER_HANDLE hVarServer,
              Variable *pResult,
              Variable *pLeft,
              Variable *pRight )
{
    int result = EINVAL;
    char *fmtspec;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) )
    {
        pResult->obj.type = VARTYPE_STR;

        if ( ( pRight != NULL ) &&
             ( pRight->obj.type == VARTYPE_STR ) )
        {
            fmtspec = pRight->obj.val.str;
        }

        /* allocate space for the number */
        result = AllocateString( pResult, 64 );

        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                snprintf( pResult->obj.val.str,
                          pResult->bufsize,
                          fmtspec ? fmtspec : "%d",
                          pLeft->obj.val.ul );
                pResult->obj.len = strlen( pResult->obj.val.str );
                break;

            case VARTYPE_UINT16:
                snprintf( pResult->obj.val.str,
                          pResult->bufsize,
                          fmtspec ? fmtspec : "%d",
                          pLeft->obj.val.ui );
                pResult->obj.len = strlen( pResult->obj.val.str );
                break;

            case VARTYPE_FLOAT:
                snprintf( pResult->obj.val.str,
                          pResult->bufsize,
                          fmtspec ? fmtspec : "%f",
                          pLeft->obj.val.f );
                pResult->obj.len = strlen( pResult->obj.val.str );
                break;

            default:
                result = ENOTSUP;
                break;
        }
    }

    return result;
}

/*! @}
 * end of vartypecast group */

