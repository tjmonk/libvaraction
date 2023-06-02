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
 * @defgroup varstrings varstrings
 * @brief Variable Action Script String Support functions
 * @{
 */

/*============================================================================*/
/*!
@file varcompare.c

    Variable Action Script Comparison Support functions

    The Variable Action Script Comparison Support functions provide functions
    for handling variable comparisons in var/action scripts.

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
#include "varcompare.h"

/*==============================================================================
       Function declarations
==============================================================================*/

/*============================================================================*/
/*  Equals                                                                    */
/*!
    Perform a comparison of two arguments

    The Equals function performs a comparison of its two arguments.

    If the arguments are strings, the strcmp() function is used.
    If the arguments are numbers, they are directly compared.

    result = left == right

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
@retval EOK the comparison was successfully performed

==============================================================================*/
int Equals( VARSERVER_HANDLE hVarServer,
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
                val = ( pLeft->obj.val.ul == pRight->obj.val.ul );
                break;

            case VARTYPE_UINT16:
                val = ( pLeft->obj.val.ui == pRight->obj.val.ui );
                break;

            case VARTYPE_FLOAT:
                val = ( pLeft->obj.val.f == pRight->obj.val.f );
                break;

            case VARTYPE_STR:
                if( ( pLeft->obj.val.str == NULL ) &&
                    ( pRight->obj.val.str == NULL ) )
                {
                    /* both strings are empty */
                    val = true;
                }
                else if ( ( pLeft->obj.val.str == NULL ) ||
                          ( pRight->obj.val.str == NULL ) )
                {
                    /* one of the strings is empty and the other is not */
                    val = false;
                }
                else
                {
                    if (strcmp( pLeft->obj.val.str, pRight->obj.val.str ) == 0 )
                    {
                        /* strings match */
                        val = true;
                    }
                    else
                    {
                        /* strings don't match */
                        val = false;
                    }
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

/*============================================================================*/
/*  NotEquals                                                                 */
/*!
    Perform a comparison of two arguments

    The NotEquals function performs a comparison of its two arguments.

    If the arguments are strings, the strcmp() function is used.
    If the arguments are numbers, they are directly compared.

    result = ! ( left == right )

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
@retval EOK the comparison was successfully performed

==============================================================================*/
int NotEquals( VARSERVER_HANDLE hVarServer,
               Variable *pResult,
               Variable *pLeft,
               Variable *pRight )
{
    int result = EINVAL;
    bool val;

    /* perform equals evaluation and invert the result */
    result = Equals( hVarServer, pResult, pLeft, pRight );
    if( pResult == EOK )
    {
        /* invert the result */
        pResult->obj.val.ui == true ? false : true;
    }

    return result;
}

/*============================================================================*/
/*  GreaterThan                                                               */
/*!
    Perform a comparison of two arguments

    The GreaterThan function performs a comparison of its two arguments.

    If the arguments are strings, the strcmp() function is used.
    If the arguments are numbers, they are directly compared.

    result = ( left > right )

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
@retval EOK the comparison was successfully performed

==============================================================================*/
int GreaterThan( VARSERVER_HANDLE hVarServer,
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
                val = ( pLeft->obj.val.ul > pRight->obj.val.ul );
                break;

            case VARTYPE_UINT16:
                val = ( pLeft->obj.val.ui > pRight->obj.val.ui );
                break;

            case VARTYPE_FLOAT:
                val = ( pLeft->obj.val.f > pRight->obj.val.f );
                break;

            case VARTYPE_STR:
                val = false;
                if( ( pLeft->obj.val.str != NULL ) &&
                    ( pRight->obj.val.str != NULL ) )
                {
                    if (strcmp( pLeft->obj.val.str, pRight->obj.val.str ) > 0 )
                    {
                        val = true;
                    }
                }
                else if ( ( pLeft->obj.val.str != NULL ) &&
                          ( pRight->obj.val.str == NULL ) )
                {
                    val = true;
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

/*============================================================================*/
/*  LessThan                                                                  */
/*!
    Perform a comparison of two arguments

    The LessThan function performs a comparison of its two arguments.

    If the arguments are strings, the strcmp() function is used.
    If the arguments are numbers, they are directly compared.

    result = ( left < right )

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
@retval EOK the comparison was successfully performed

==============================================================================*/
int LessThan( VARSERVER_HANDLE hVarServer,
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
                val = ( pLeft->obj.val.ul < pRight->obj.val.ul );
                break;

            case VARTYPE_UINT16:
                val = ( pLeft->obj.val.ui < pRight->obj.val.ui );
                break;

            case VARTYPE_FLOAT:
                val = ( pLeft->obj.val.f < pRight->obj.val.f );
                break;

            case VARTYPE_STR:
                val = false;
                if( ( pLeft->obj.val.str != NULL ) &&
                    ( pRight->obj.val.str != NULL ) )
                {
                    if (strcmp( pLeft->obj.val.str, pRight->obj.val.str ) < 0 )
                    {
                        val = true;
                    }
                }
                else if ( ( pLeft->obj.val.str == NULL ) &&
                          ( pRight->obj.val.str != NULL ) )
                {
                    val = true;
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

/*============================================================================*/
/*  GreaterThanOrEqual                                                        */
/*!
    Perform a comparison of two arguments

    The GreaterThanOrEqual function performs a comparison of its two arguments.

    If the arguments are strings, the strcmp() function is used.
    If the arguments are numbers, they are directly compared.

    result = ( left >= right )

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
@retval EOK the comparison was successfully performed

==============================================================================*/
int GreaterThanOrEqual( VARSERVER_HANDLE hVarServer,
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
                val = ( pLeft->obj.val.ul >= pRight->obj.val.ul );
                break;

            case VARTYPE_UINT16:
                val = ( pLeft->obj.val.ui >= pRight->obj.val.ui );
                break;

            case VARTYPE_FLOAT:
                val = ( pLeft->obj.val.f >= pRight->obj.val.f );
                break;

            case VARTYPE_STR:
                val = false;
                if( ( pLeft->obj.val.str == NULL ) &&
                    ( pRight->obj.val.str == NULL ) )
                {
                    val = true;
                }
                else if( ( pLeft->obj.val.str != NULL ) &&
                         ( pRight->obj.val.str != NULL ) )
                {
                    if (strcmp( pLeft->obj.val.str, pRight->obj.val.str ) >= 0 )
                    {
                        val = true;
                    }
                }
                else if ( ( pLeft->obj.val.str != NULL ) &&
                          ( pRight->obj.val.str == NULL ) )
                {
                    val = true;
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

/*============================================================================*/
/*  LessThanOrEqual                                                           */
/*!
    Perform a comparison of two arguments

    The LessThanOrEqual function performs a comparison of its two arguments.

    If the arguments are strings, the strcmp() function is used.
    If the arguments are numbers, they are directly compared.

    result = ( left <= right )

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
@retval EOK the comparison was successfully performed

==============================================================================*/
int LessThanOrEqual( VARSERVER_HANDLE hVarServer,
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
                val = ( pLeft->obj.val.ul <= pRight->obj.val.ul );
                break;

            case VARTYPE_UINT16:
                val = ( pLeft->obj.val.ui <= pRight->obj.val.ui );
                break;

            case VARTYPE_FLOAT:
                val = ( pLeft->obj.val.f <= pRight->obj.val.f );
                break;

            case VARTYPE_STR:
                val = false;
                if( ( pLeft->obj.val.str == NULL ) &&
                    ( pRight->obj.val.str == NULL ) )
                {
                    val = true;
                }
                else if( ( pLeft->obj.val.str != NULL ) &&
                         ( pRight->obj.val.str != NULL ) )
                {
                    if (strcmp( pLeft->obj.val.str, pRight->obj.val.str ) <= 0 )
                    {
                        val = true;
                    }
                }
                else if ( ( pLeft->obj.val.str == NULL ) &&
                          ( pRight->obj.val.str != NULL ) )
                {
                    val = true;
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
 * end of varcompare group */

