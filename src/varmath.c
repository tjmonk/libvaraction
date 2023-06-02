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
 * @defgroup varmath varmath
 * @brief Variable Action Script Math Operation Support functions
 * @{
 */

/*============================================================================*/
/*!
@file varboolean.c

    Variable Action Script Match Operation Support functions

    The Variable Action Script Math Operation Support functions provide
    functions for handling mathematical operations in var/action
    scripts.

    Operators include:  +, -, *, /

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
#include "varmath.h"
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
/*  Multiply                                                                  */
/*!
    Perform a variable multiplication

    The Multiply function performs the following operation:

    result = left * right

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
int Multiply( VARSERVER_HANDLE hVarServer,
              Variable *pResult,
              Variable *pLeft,
              Variable *pRight )
{
    int result = EINVAL;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pResult->obj.val.ul = pLeft->obj.val.ul * pRight->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pResult->obj.val.ui = pLeft->obj.val.ui * pRight->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pResult->obj.val.f = pLeft->obj.val.f * pRight->obj.val.f;
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
/*  Divide                                                                    */
/*!
    Perform a variable division

    The Divide function performs the following operation:

    result = left / right

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
int Divide( VARSERVER_HANDLE hVarServer,
            Variable *pResult,
            Variable *pLeft,
            Variable *pRight )
{
    int result = EINVAL;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pResult->obj.val.ul = pLeft->obj.val.ul / pRight->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pResult->obj.val.ui = pLeft->obj.val.ui / pRight->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pResult->obj.val.f = pLeft->obj.val.f / pRight->obj.val.f;
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
/*  Add                                                                       */
/*!
    Perform a variable division

    The Add function performs the following operation:

    result = left + right

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
int Add( VARSERVER_HANDLE hVarServer,
         Variable *pResult,
         Variable *pLeft,
         Variable *pRight )
{
    int result = EINVAL;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pResult->obj.val.ul = pLeft->obj.val.ul + pRight->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pResult->obj.val.ui = pLeft->obj.val.ui + pRight->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pResult->obj.val.f = pLeft->obj.val.f + pRight->obj.val.f;
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            case VARTYPE_STR:
                result = AddString( pResult, pLeft, pRight );
                break;

            default:
                result = ENOTSUP;
                break;
        }
    }

    return result;
}


/*============================================================================*/
/*  Sub                                                                       */
/*!
    Perform a variable subtraction

    The Sub function performs the following operation:

    result = left - right

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
int Sub( VARSERVER_HANDLE hVarServer,
         Variable *pResult,
         Variable *pLeft,
         Variable *pRight )
{
    int result = EINVAL;

    if ( ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pResult->obj.val.ul = pLeft->obj.val.ul - pRight->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pResult->obj.val.ui = pLeft->obj.val.ui - pRight->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pResult->obj.val.f = pLeft->obj.val.f - pRight->obj.val.f;
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

/*! @}
 * end of varmath group */

