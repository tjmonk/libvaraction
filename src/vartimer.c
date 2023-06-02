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
 * @defgroup vartimer vartimer
 * @brief Variable Action Script Timer Support functions
 * @{
 */

/*============================================================================*/
/*!
@file vartimer.c

    Variable Action Script Timer Support functions

    The Variable Action Script Timer Support functions provide
    functions for creating and deleting timers in var/action
    scripts.

    Operators include:  create timer, create tick, delete timer

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
#include <time.h>
#include <signal.h>
#include "vartimer.h"

/*==============================================================================
       Function declarations
==============================================================================*/

/*==============================================================================
       Definitions
==============================================================================*/

/*! Maximum number of timers allowed */
#define MAX_TIMERS ( 255 )

/*! timer notification */
#define TIMER_NOTIFICATION SIGRTMIN+5

/*==============================================================================
       File Scoped Variables
==============================================================================*/

/*! array of timers */
static timer_t timers[MAX_TIMERS] = {0};

/*! the currently active (fired) timer */
static uint16_t activeTimer = 0;

/*==============================================================================
       Function definitions
==============================================================================*/

/*============================================================================*/
/*  VACreateTimer                                                             */
/*!
    Create a timer

    The Create Timer creates a new timer.

    The left operand is the timer identifier (uint16)

    The right operand is the timer value in milliseconds (uint32)

    The result is a uint16 and is either 1 if the timer was created
    successfully, or 0 if the timer could not be created

@param[in]
    hVarServer
        handle to the variable server (unused)

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
@retval EOK the timer was successfully created

==============================================================================*/
int VACreateTimer( VARSERVER_HANDLE hVarServer,
                   Variable *pResult,
                   Variable *pLeft,
                   Variable *pRight )
{
    int result = EINVAL;
    int id;
    int timeoutms;
    struct sigevent te;
    struct itimerspec its;
    int sigNo = TIMER_NOTIFICATION;
    long secs;
    long msecs;
    timer_t *timerID;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;

        id = pLeft->obj.val.ui;
        timeoutms = pRight->obj.val.ul;

        secs = timeoutms / 1000;
        msecs = timeoutms % 1000;

        if( ( id > 0 ) && ( id < MAX_TIMERS ) )
        {
            if( timers[id] != 0 )
            {
                VADeleteTimer( hVarServer, pResult, pLeft, pRight );
            }

            timerID = &timers[id];

            /* Set and enable alarm */
            te.sigev_notify = SIGEV_SIGNAL;
            te.sigev_signo = sigNo;
            te.sigev_value.sival_int = id;
            timer_create(CLOCK_REALTIME, &te, timerID);

            its.it_interval.tv_sec = 0;
            its.it_interval.tv_nsec = 0;
            its.it_value.tv_sec = secs;
            its.it_value.tv_nsec = msecs * 1000000L;
            timer_settime(*timerID, 0, &its, NULL);

            result = EOK;
        }
        else
        {
            result = ENOENT;
        }

        pResult->obj.val.ui = ( result == EOK ) ? 1 : 0;
        pResult->obj.type = VARTYPE_UINT16;
        pResult->obj.len = sizeof(uint16_t);
    }

    return result;
}

/*============================================================================*/
/*  VACreateTick                                                              */
/*!
    Create a tick

    The VACreateTick function creates a new repeating tick timer.

    The left operand is the timer identifier (uint16)

    The right operand is the timer value in milliseconds (uint32)

    The result is a uint16 and is either 1 if the tick timer was created
    successfully, or 0 if the timer could not be created

@param[in]
    hVarServer
        handle to the variable server (unused)

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
@retval EOK the timer was successfully created

==============================================================================*/
int VACreateTick( VARSERVER_HANDLE hVarServer,
                  Variable *pResult,
                  Variable *pLeft,
                  Variable *pRight )
{
    int result = EINVAL;
    int id;
    int timeoutms;
    struct sigevent te;
    struct itimerspec its;
    int sigNo = TIMER_NOTIFICATION;
    long secs;
    long msecs;
    timer_t *timerID;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) &&
         ( pRight != NULL ) )
    {
        result = EOK;

        id = pLeft->obj.val.ui;
        timeoutms = pRight->obj.val.ul;

        secs = timeoutms / 1000;
        msecs = timeoutms % 1000;

        if( ( id > 0 ) && ( id < MAX_TIMERS ) )
        {
            if( timers[id] != 0 )
            {
                VADeleteTimer( hVarServer, pResult, pLeft, pRight );
            }

            timerID = &timers[id];

            /* Set and enable alarm */
            te.sigev_notify = SIGEV_SIGNAL;
            te.sigev_signo = sigNo;
            te.sigev_value.sival_int = id;
            timer_create(CLOCK_REALTIME, &te, timerID);

            its.it_interval.tv_sec = secs;
            its.it_interval.tv_nsec = msecs * 1000000L;
            its.it_value.tv_sec = secs;
            its.it_value.tv_nsec = msecs * 1000000L;
            timer_settime(*timerID, 0, &its, NULL);

            result = EOK;
        }
        else
        {
            result = ENOENT;
        }

        pResult->obj.val.ui = ( result == EOK ) ? 1 : 0;
        pResult->obj.type = VARTYPE_UINT16;
        pResult->obj.len = sizeof(uint16_t);
    }

    return result;
}

/*============================================================================*/
/*  VADeleteTimer                                                             */
/*!
    Delete a timer or a tick

    The VADeleteTimer function deletes an existing tick or timer.

    The left operand is the timer identifier (uint16)

    The result is a uint16 and is either 1 if the tick/timer was deleted
    successfully, or 0 if the tick/timer could not be deleted.

@param[in]
    hVarServer
        handle to the variable server (unused)

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
@retval EOK the timer was successfully deleted

==============================================================================*/
int VADeleteTimer( VARSERVER_HANDLE hVarServer,
                   Variable *pResult,
                   Variable *pLeft,
                   Variable *pRight )
{
    int result = EINVAL;
    int id;
    timer_t *timerID;

    if ( ( hVarServer != NULL ) &&
         ( pResult != NULL ) &&
         ( pLeft != NULL ) )
    {
        result = EOK;

        id = pLeft->obj.val.ui;
        if( ( id > 0 ) && ( id < MAX_TIMERS ) )
        {
            timerID = timers[id];

            if ( timer_delete( timerID ) == 0 )
            {
                result = EOK;
            }
            else
            {
                result = errno;
            }
        }
        else
        {
            result = ENOENT;
        }

        pResult->obj.val.ui = ( result == EOK ) ? 1 : 0;
        pResult->obj.type = VARTYPE_UINT16;
        pResult->obj.len = sizeof(uint16_t);
    }

    return result;
}


/*============================================================================*/
/*  VAGetActiveTimer                                                          */
/*!
    Get the active (fired) timer

    The VAGetActiveTimer function gets the active timer and
    stores in the variable uint16_t value.

@param[in]
    hVarServer
        handle to the variable server (unused)

@param[in]
    pResult
        pointer to the result node

@param[in]
    pLeft
        pointer to the left node (unused)

@param[in]
    pRight
        pointer to the right node (unused)

@retval EINVAL invalid argument
@retval EOK the active timer was successfully retrieved

==============================================================================*/
int VAGetActiveTimer( VARSERVER_HANDLE hVarServer,
                      Variable *pResult,
                      Variable *pLeft,
                      Variable *pRight )
{
    int result = EINVAL;
    int id;
    timer_t *timerID;

    if( pResult != NULL )
    {
        pResult->obj.val.ui = activeTimer;
        pResult->obj.type = VARTYPE_UINT16;
        pResult->obj.len = sizeof(uint16_t);
        result = EOK;
    }

    return result;
}

/*============================================================================*/
/*  VASetActiveTimer                                                          */
/*!
    Set the active (fired) timer

    The VASetActiveTimer function sets the active timer id.
    (use 0 to indicate no active timers)

@param[in]
    id
        identifier of the active timer

==============================================================================*/
void VASetActiveTimer( uint16_t id )
{
    activeTimer = id;
}

/*! @}
 * end of vartimer group */

