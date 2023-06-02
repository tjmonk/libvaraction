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

/*============================================================================*/
/*!
@file varboolean.c

    Variable Action Script Logical Operation Support functions

    The Variable Action Script Logical Operation Support functions provide
    functions for handling boolean or logical operations in var/action
    scripts.

    Operators include:  AND, OR, and NOT operators.

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
#include "varboolean.h"

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
/*  And                                                                       */
/*!
    Perform a logical AND of two arguments

    The And function performs a logical AND of two arguments as follows:

    result = left && right

    This only applies to 16 and 32 bit data types.

    The result is true (1) if left and right are both true (non-zero),
    otherwise the result is false (0)

    The result is stored in a 16-bit value

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
@retval EOK the AND was successfully performed

==============================================================================*/
int And( VARSERVER_HANDLE hVarServer,
         Variable *pResult,
         Variable *pLeft,
         Variable *pRight )
{
    int result = EINVAL;
    bool val;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                val = pLeft->obj.val.ul && pRight->obj.val.ul;
                break;

            case VARTYPE_UINT16:
                val = pLeft->obj.val.ui && pRight->obj.val.ui;
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if ( result == EOK )
        {
            pResult->obj.val.ui = val;
            pResult->obj.type = VARTYPE_UINT16;
            pResult->obj.len = sizeof(uint16_t);
        }
    }

    return result;
}

/*============================================================================*/
/*  Or                                                                        */
/*!
    Perform a logical OR of two arguments

    The Or function performs a logical OR of two arguments as follows:

    result = left || right

    This only applies to 16 and 32 bit data types.

    The result is true (1) if either the left or the right are true (non-zero),
    otherwise the result is false (0)

    The result is stored in a 16-bit value

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
@retval EOK the OR was successfully performed

==============================================================================*/
int Or( VARSERVER_HANDLE hVarServer,
        Variable *pResult,
        Variable *pLeft,
        Variable *pRight )
{
    int result = EINVAL;
    bool val;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                val = pLeft->obj.val.ul || pRight->obj.val.ul;
                break;

            case VARTYPE_UINT16:
                val = pLeft->obj.val.ui || pRight->obj.val.ui;
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if ( result == EOK )
        {
            pResult->obj.val.ui = val;
            pResult->obj.type = VARTYPE_UINT16;
            pResult->obj.len = sizeof(uint16_t);
        }
    }

    return result;
}

/*============================================================================*/
/*  Not                                                                       */
/*!
    Perform a logical NOT of a single argument

    The Not function performs a logical NOT of its argument as follows:

    result = !left

    If the type of the argument is a 16 bit, 32 bit, or float value,
    the result is true if the value is zero

    If the type of the argument is a string, the result is true
    if the string is empty, and false if the string has content.

    The result is stored in a 16-bit value

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
@retval EOK the invert operation was successfully performed

==============================================================================*/
int Not( VARSERVER_HANDLE hVarServer,
         Variable *pResult,
         Variable *pLeft,
         Variable *pRight )
{
    int result = EINVAL;
    bool val;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                val = ( pLeft->obj.val.ul == 0 ) ? true : false;
                break;

            case VARTYPE_UINT16:
                val = ( pLeft->obj.val.ui == 0 ) ? true : false;
                break;

            case VARTYPE_FLOAT:
                val = ( pLeft->obj.val.f == 0.0 ) ? true : false;
                break;

            case VARTYPE_STR:
                if( pLeft->obj.val.str == NULL )
                {
                    val = true;
                }
                else
                {
                    val = strlen( pLeft->obj.val.str ) == 0 ? true : false;
                }
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if ( result == EOK )
        {
            pResult->obj.val.ui = val;
            pResult->obj.type = VARTYPE_UINT16;
            pResult->obj.len = sizeof(uint16_t);
        }
    }

    return result;
}

/*! @}
 * end of varboolean group */

