/**
* @file
* @brief QK/C port example for a generic C compiler.
* @ingroup qk
* @cond
******************************************************************************
* Last updated for version 5.6.0
* Last updated on  2015-12-14
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* http://www.state-machine.com
* mailto:info@state-machine.com
******************************************************************************
* @endcond
*/
#ifndef qk_port_h
#define qk_port_h

/****************************************************************************/

/*! determination if the code executes in the ISR context */
#define QK_ISR_CONTEXT_() (get_IPSR() != (uint32_t)0)

/* QK interrupt entry and exit... */
/*! Define the ISR entry sequence, if the compiler supports writing
* interrupts in C.
*/
/**
* @note This is just an example of #QK_ISR_ENTRY. You need to define
* the macro appropriately for the CPU/compiler you're using. Also, some
* QK ports will not define this macro, but instead will provide ISR
* skeleton code in assembly.
*/
#define QK_ISR_ENTRY() do { \
    QF_INT_DISABLE(); \
    ++QK_intNest_; \
    QF_QS_ISR_ENTRY(QK_intNest_, QK_currPrio_); \
    QF_INT_ENABLE(); \
} while (0)


/*! Define the ISR exit sequence, if the compiler supports writing
* interrupts in C.
*/
/**
* @note
* This is just an example of #QK_ISR_EXIT. You need to define the macro
* appropriately for the CPU/compiler you're using. Also, some QK ports will
* not define this macro, but instead will provide ISR skeleton in assembly.
*/
#define QK_ISR_EXIT() do { \
    QF_INT_DISABLE(); \
    --QK_intNest_; \
    if (QK_intNest_ == (uint_fast8_t)0) { \
        uint_fast8_t nextPrio_ = QK_schedPrio_(); \
        if (nextPrio_ != (uint_fast8_t)0) { \
            QK_sched_(nextPrio_); \
        } \
    } \
    QF_INT_ENABLE(); \
} while (0)

/* QF-specific scheduler locking */
/*! Internal port-specific macro to represent the scheduler lock status
* that needs to be preserved to allow nesting of locks.
*/
#define QF_SCHED_STAT_TYPE_ QMutex

/*! Internal port-specific macro for selective scheduler locking. */
#define QF_SCHED_LOCK_(pLockStat_) do { \
    if (QK_ISR_CONTEXT_()) { \
        (pLockStat_)->lockPrio = (uint_fast8_t)(QF_MAX_ACTIVE + 1); \
    } else { \
        QMutex_lock((pLockStat_)); \
    } \
} while (0)

/*! Internal port-specific macro for selective scheduler unlocking. */
#define QF_SCHED_UNLOCK_(pLockStat_) QMutex_unlock((pLockStat_))


#include "qk.h" /* QK platform-independent public interface */

uint32_t get_IPSR(void);

#endif /* qk_port_h */
