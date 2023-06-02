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
 * @defgroup varassign varassign
 * @brief Variable Action Script Assignment Operation Support functions
 * @{
 */

/*============================================================================*/
/*!
@file varassign.c

    Variable Action Script Assignment Operation Support functions

    The Variable Action Script Assigment Operation Support functions provide
    functions for handling assignment operations in var/action
    scripts.

    Operators include:  +=, -=, *=, /=

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
#include "varassign.h"
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
/*  Assign                                                                    */
/*!
    Perform a variable assignment

    The Assign function performs the following operation:

    result <= left <= right

    If any of the variables is a sysvar, then the variable is set
    via the VAR_Set() function.

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
@retval EOK the assignment was successfully performed

==============================================================================*/
int Assign( VARSERVER_HANDLE hVarServer,
            Variable *pResult,
            Variable *pLeft,
            Variable *pRight )
{
    int result = EINVAL;
    size_t len;
    size_t bufsize;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pResult->obj.val.ul = pLeft->obj.val.ul = pRight->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pResult->obj.val.ui = pLeft->obj.val.ui = pRight->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pResult->obj.val.f = pLeft->obj.val.f = pRight->obj.val.f;
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            case VARTYPE_STR:
                result = AssignString( pResult, pLeft, pRight );
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if( result == EOK )
        {
            if ( pLeft->operation == VA_SYSVAR )
            {
                /* set the sysvar */
                result = VAR_Set( hVarServer,
                                  pLeft->hVar,
                                  &(pLeft->obj) );
            }
        }
    }

    return result;
}

/*============================================================================*/
/*  PlusEquals                                                                */
/*!
    Add one variable to another

    The PlusEquals function performs the following operation:

    result <= left <= left + right

    If the left variable is a sysvar, then the variable is set
    via the VAR_Set() function.

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
@retval EOK the addition was successfully performed

==============================================================================*/
int PlusEquals( VARSERVER_HANDLE hVarServer,
                Variable *pResult,
                Variable *pLeft,
                Variable *pRight )
{
    int result = EINVAL;
    size_t len;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pLeft->obj.val.ul += pRight->obj.val.ul;
                pResult->obj.val.ul = pLeft->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pLeft->obj.val.ui += pRight->obj.val.ui;
                pResult->obj.val.ui = pLeft->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pLeft->obj.val.f += pRight->obj.val.f;
                pResult->obj.val.f = pLeft->obj.val.f;
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            case VARTYPE_STR:
                result = ConcatString( pResult, pLeft, pRight );
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if( result == EOK )
        {
            if ( pLeft->operation == VA_SYSVAR )
            {
                /* set the sysvar */
                result = VAR_Set( hVarServer,
                                  pLeft->hVar,
                                  &(pLeft->obj) );
            }
        }
    }

    return result;
}

/*============================================================================*/
/*  MinusEquals                                                               */
/*!
    Subtract one variable from another

    The MinusEquals function performs the following operation:

    result <= left <= left - right

    If the left variable is a sysvar, then the variable is set
    via the VAR_Set() function.

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
@retval EOK the subtraction was successfully performed

==============================================================================*/
int MinusEquals( VARSERVER_HANDLE hVarServer,
                 Variable *pResult,
                 Variable *pLeft,
                 Variable *pRight )
{
    int result = EINVAL;
    size_t len;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pLeft->obj.val.ul -= pRight->obj.val.ul;
                pResult->obj.val.ul = pLeft->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pLeft->obj.val.ui -= pRight->obj.val.ui;
                pResult->obj.val.ui = pLeft->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pLeft->obj.val.f -= pRight->obj.val.f;
                pResult->obj.val.f = pLeft->obj.val.f;
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if( result == EOK )
        {
            if ( pLeft->operation == VA_SYSVAR )
            {
                /* set the sysvar */
                result = VAR_Set( hVarServer,
                                  pLeft->hVar,
                                  &(pLeft->obj) );
            }
        }
    }

    return result;
}

/*============================================================================*/
/*  TimesEquals                                                               */
/*!
    Multiply one variable with another

    The TimesEquals function performs the following operation:

    result <= left <= left * right

    If the left variable is a sysvar, then the variable is set
    via the VAR_Set() function.

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
@retval EOK the multiplication was successfully performed

==============================================================================*/
int TimesEquals( VARSERVER_HANDLE hVarServer,
                 Variable *pResult,
                 Variable *pLeft,
                 Variable *pRight )
{
    int result = EINVAL;
    size_t len;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pLeft->obj.val.ul *= pRight->obj.val.ul;
                pResult->obj.val.ul = pLeft->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pLeft->obj.val.ui *= pRight->obj.val.ui;
                pResult->obj.val.ui = pLeft->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pLeft->obj.val.f *= pRight->obj.val.f;
                pResult->obj.val.f = pLeft->obj.val.f;
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if( result == EOK )
        {
            if ( pLeft->operation == VA_SYSVAR )
            {
                /* set the sysvar */
                result = VAR_Set( hVarServer,
                                  pLeft->hVar,
                                  &(pLeft->obj) );
            }
        }
    }

    return result;
}


/*============================================================================*/
/*  DivEquals                                                                 */
/*!
    Divide one variable by another

    The DivEquals function performs the following operation:

    result <= left <= left / right

    If the left variable is a sysvar, then the variable is set
    via the VAR_Set() function.

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
@retval EOK the division was successfully performed

==============================================================================*/
int DivEquals( VARSERVER_HANDLE hVarServer,
               Variable *pResult,
               Variable *pLeft,
               Variable *pRight )
{
    int result = EINVAL;
    size_t len;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pLeft->obj.val.ul /= pRight->obj.val.ul;
                pResult->obj.val.ul = pLeft->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pLeft->obj.val.ui /= pRight->obj.val.ui;
                pResult->obj.val.ui = pLeft->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            case VARTYPE_FLOAT:
                pLeft->obj.val.f /= pRight->obj.val.f;
                pResult->obj.val.f = pLeft->obj.val.f;
                pResult->obj.type = VARTYPE_FLOAT;
                pResult->obj.len = sizeof(float);
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if( result == EOK )
        {
            if ( pLeft->operation == VA_SYSVAR )
            {
                /* set the sysvar */
                result = VAR_Set( hVarServer,
                                  pLeft->hVar,
                                  &(pLeft->obj) );
            }
        }
    }

    return result;
}

/*============================================================================*/
/*  AndEquals                                                                 */
/*!
    Bitwise AND one variable with another

    The AndEquals function performs the following operation:

    result <= left <= left & right

    If the left variable is a sysvar, then the variable is set
    via the VAR_Set() function.

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
@retval EOK the AND operation was successfully performed

==============================================================================*/
int AndEquals( VARSERVER_HANDLE hVarServer,
               Variable *pResult,
               Variable *pLeft,
               Variable *pRight )
{
    int result = EINVAL;
    size_t len;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pLeft->obj.val.ul &= pRight->obj.val.ul;
                pResult->obj.val.ul = pLeft->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pLeft->obj.val.ui &= pRight->obj.val.ui;
                pResult->obj.val.ui = pLeft->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if( result == EOK )
        {
            if ( pLeft->operation == VA_SYSVAR )
            {
                /* set the sysvar */
                result = VAR_Set( hVarServer,
                                  pLeft->hVar,
                                  &(pLeft->obj) );
            }
        }
    }

    return result;
}

/*============================================================================*/
/*  OrEquals                                                                  */
/*!
    Bitwise OR one variable with another

    The OrEquals function performs the following operation:

    result <= left <= left | right

    If the left variable is a sysvar, then the variable is set
    via the VAR_Set() function.

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
@retval EOK the OR operation was successfully performed

==============================================================================*/
int OrEquals( VARSERVER_HANDLE hVarServer,
              Variable *pResult,
              Variable *pLeft,
              Variable *pRight )
{
    int result = EINVAL;
    size_t len;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pLeft->obj.val.ul |= pRight->obj.val.ul;
                pResult->obj.val.ul = pLeft->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pLeft->obj.val.ui |= pRight->obj.val.ui;
                pResult->obj.val.ui = pLeft->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if( result == EOK )
        {
            if ( pLeft->operation == VA_SYSVAR )
            {
                /* set the sysvar */
                result = VAR_Set( hVarServer,
                                  pLeft->hVar,
                                  &(pLeft->obj) );
            }
        }
    }

    return result;
}

/*============================================================================*/
/*  XorEquals                                                                 */
/*!
    Bitwise XOR one variable with another

    The XorEquals function performs the following operation:

    result <= left <= left ^ right

    If the left variable is a sysvar, then the variable is set
    via the VAR_Set() function.

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
@retval EOK the XOR operation was successfully performed

==============================================================================*/
int XorEquals( VARSERVER_HANDLE hVarServer,
               Variable *pResult,
               Variable *pLeft,
               Variable *pRight )
{
    int result = EINVAL;
    size_t len;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;
        switch( pLeft->obj.type )
        {
            case VARTYPE_UINT32:
                pLeft->obj.val.ul ^= pRight->obj.val.ul;
                pResult->obj.val.ul = pLeft->obj.val.ul;
                pResult->obj.type = VARTYPE_UINT32;
                pResult->obj.len = sizeof(uint32_t);
                break;

            case VARTYPE_UINT16:
                pLeft->obj.val.ui ^= pRight->obj.val.ui;
                pResult->obj.val.ui = pLeft->obj.val.ui;
                pResult->obj.type = VARTYPE_UINT16;
                pResult->obj.len = sizeof(uint16_t);
                break;

            default:
                result = ENOTSUP;
                break;
        }

        if( result == EOK )
        {
            if ( pLeft->operation == VA_SYSVAR )
            {
                /* set the sysvar */
                result = VAR_Set( hVarServer,
                                  pLeft->hVar,
                                  &(pLeft->obj) );
            }
        }
    }

    return result;
}

/*============================================================================*/
/*  Inc                                                                       */
/*!
    Perform a pre or post increment

    The Inc function performs one of the following operations depending
    on if the argument is in the left or right tree:

    result = left++

    OR

    result = ++right

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
int Inc( VARSERVER_HANDLE hVarServer,
         Variable *pResult,
         Variable *pLeft,
         Variable *pRight )
{
    int result = EINVAL;
    Variable *pVar = NULL;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( ( pLeft != NULL ) ||
           ( pRight != NULL ) ) )
    {
        result = EOK;

        if( pLeft != NULL )
        {
            /* post increment */
            switch( pLeft->obj.type )
            {
                case VARTYPE_UINT32:
                    pResult->obj.val.ul = pLeft->obj.val.ul++;
                    break;

                case VARTYPE_UINT16:
                    pResult->obj.val.ui = pLeft->obj.val.ui++;
                    break;

                default:
                    result = ENOTSUP;
                    break;
            }

            if ( result == EOK )
            {
                pVar = pLeft;
            }
        }
        else if ( pRight != NULL )
        {
            /* pre increment */
            switch( pRight->obj.type )
            {
                case VARTYPE_UINT32:
                    pResult->obj.val.ul = ++pRight->obj.val.ul;
                    break;

                case VARTYPE_UINT16:
                    pResult->obj.val.ui = ++pRight->obj.val.ui;
                    break;

                default:
                    result = ENOTSUP;
                    break;
            }

            if ( result == EOK )
            {
                pVar = pRight;
            }
        }

        if ( ( pVar != NULL ) &&
             ( pVar->operation == VA_SYSVAR ) )
        {
            /* update the sysvar */
            result = VAR_Set( hVarServer, pVar->hVar, &(pVar->obj) );
        }
    }

    return result;
}


/*============================================================================*/
/*  Dec                                                                       */
/*!
    Perform a pre or post decrement

    The Dec function performs one of the following operations depending
    on if the argument is in the left or right tree:

    result = left--

    OR

    result = --right

@param[in]
    pActions
        pointer to the Actions context

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
int Dec( VARSERVER_HANDLE hVarServer,
         Variable *pResult,
         Variable *pLeft,
         Variable *pRight )
{
    int result = EINVAL;
    Variable *pVar = NULL;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( ( pLeft != NULL ) ||
           ( pRight != NULL ) ) )
    {
        result = EOK;

        if( pLeft != NULL )
        {
            /* post increment */
            switch( pLeft->obj.type )
            {
                case VARTYPE_UINT32:
                    pResult->obj.val.ul = pLeft->obj.val.ul--;
                    break;

                case VARTYPE_UINT16:
                    pResult->obj.val.ui = pLeft->obj.val.ui--;
                    break;

                default:
                    result = ENOTSUP;
                    break;
            }

            if ( result == EOK )
            {
                pVar = pLeft;
            }
        }
        else if ( pRight != NULL )
        {
            /* pre increment */
            switch( pRight->obj.type )
            {
                case VARTYPE_UINT32:
                    pResult->obj.val.ul = --pRight->obj.val.ul;
                    break;

                case VARTYPE_UINT16:
                    pResult->obj.val.ui = --pRight->obj.val.ui;
                    break;

                default:
                    result = ENOTSUP;
                    break;
            }

            if ( result == EOK )
            {
                pVar = pRight;
            }
        }

        if ( ( pVar != NULL ) &&
             ( pVar->operation == VA_SYSVAR ) )
        {
            /* update the sysvar */
            result = VAR_Set( hVarServer, pVar->hVar, &(pVar->obj) );
        }
    }

    return result;
}

/*! @}
 * end of varassign group */

