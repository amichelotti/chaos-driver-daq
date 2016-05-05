/*
 * Copyright (c) 2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: thread_controller.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ____THREAD_CONTROLLER_H
#define ____THREAD_CONTROLLER_H

#include <thread>
#include <condition_variable>
#include <string>


typedef std::function<void()>   LoopFnc;
typedef std::function<void()>   CancelLoopFnc;

namespace istd {

class ThreadController
{
public:
    enum State_e {
       eStopped     = 0,   /* thread is stopped                 */
       eStarting    = 1,   /* thread is going to be started     */
       eRunning     = 2,   /* thread is running                 */
       eTerminating = 3    /* thread is announced to be stopped */
    };

    ThreadController(const std::string  a_id,
                 LoopFnc            a_loop_fnc,
                 CancelLoopFnc      a_cancel_loop_fnc);
    ~ThreadController();

    bool   Start();
    bool   Stop();
    void   Restart();

    void   AsyncStart();
    void   AsyncStop();
    void   AsyncRestart();

    State_e GetState() { return m_state; }

private:
    void   Run();
    bool   SetState(State_e a_state);
    bool   WaitOnStateEnter(const State_e &a_state, std::unique_lock<std::mutex> &a_lock);
    void   WaitOnStateExit (const State_e &a_state, std::unique_lock<std::mutex> &a_lock);

private:
    const std::string               m_id;
    State_e                         m_state;
    mutable std::condition_variable m_state_cond;
    mutable std::mutex              m_state_x;
    LoopFnc                         m_loop_fnc;
    CancelLoopFnc                   m_cancel_loop_fnc;
    uint32_t                        m_msec;
};
}

#endif // ____THREAD_CONTROLLER_H
