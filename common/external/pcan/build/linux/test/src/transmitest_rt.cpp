//****************************************************************************
// Copyright (C) 2001-2006  PEAK System-Technik GmbH
//
// linux@peak-system.com 
// www.peak-system.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// Maintainer(s): Klaus Hitschler (klaus.hitschler@gmx.de)
//****************************************************************************

//****************************************************************************
// realtime part
// transmitest_rt.cpp - a simple program to test CAN transmits
//
// $Id: transmitest_rt.cpp 517 2007-07-09 09:40:42Z edouard $
//
//****************************************************************************


//****************************************************************************
// INCLUDES

#include <sys/mman.h>

#ifdef XENOMAI
  #include <native/task.h>
  #include <native/timer.h>
#endif

#ifdef RTAI
  #include <sys/poll.h>
  #include <rtai_lxrt.h>
#endif

//****************************************************************************
// DEFINES

#define STATE_FILE_OPENED         1
#define STATE_TASK_CREATED        2
#define SET_INIT_STATE(new_state) current_state |= new_state
#define RESET_INIT_STATE(new_state) current_state &= ~new_state
//****************************************************************************
// GLOBALS
unsigned int current_state = 0;
int shutdownnow = 0;

#ifdef XENOMAI
  RT_TASK writing_task;
#endif

#ifdef RTAI
  RT_TASK *writing_task;
  RT_TASK *maint;
  static pthread_t writing_thr;
#endif

//****************************************************************************
// SPECIFICS FUNCTIONS FOR REALTIME 

void do_exit(int error)
{
  shutdownnow = 1;
  if (current_state & STATE_FILE_OPENED) {
    print_diag("transmitest");
    CAN_Close(h);
    RESET_INIT_STATE(STATE_FILE_OPENED);
  }

#ifdef XENOMAI
  if (current_state & STATE_TASK_CREATED) {
    rt_task_delete(&writing_task);
    RESET_INIT_STATE(STATE_TASK_CREATED);
  }
#endif

#ifdef RTAI
  pthread_join(writing_thr, NULL);
  if (current_state & STATE_TASK_CREATED) {
    RESET_INIT_STATE(STATE_TASK_CREATED);
  }
  rt_task_delete(maint);
#endif
}

void signal_handler(int unused)
{
  /* re-install signal handler */
  /* let main() executes after pause() */
  signal(SIGTERM, signal_handler);
  signal(SIGINT, signal_handler);
}

//----------------------------------------------------------------------------
// real time task
void writing_task_proc(void *arg)
{
#ifdef RTAI
  writing_task = rt_task_init_schmod(nam2num("WRTSK"),2, 0, 0, SCHED_FIFO, 0xF);
  rt_make_hard_real_time();
#endif
  
  while (1) {
    std::list<TPCANMsg>::iterator iter;
    for (iter = List->begin(); iter != List->end(); iter++) {
      // test for standard frames only
      if ((nExtended == CAN_INIT_TYPE_EX) || !(iter->MSGTYPE & MSGTYPE_EXTENDED)) {
        // send the message
        if ((errno = CAN_Write(h, &(*iter))))
          shutdownnow = 1;
      }
      if (shutdownnow == 1) break;
    }
    if (shutdownnow == 1) break;
  }
  #ifdef RTAI
    rt_make_soft_real_time();
    rt_task_delete(writing_task);
  #endif
}

int init()
{
  mlockall(MCL_CURRENT | MCL_FUTURE);
  /* Initialize LXRT */
  #ifdef RTAI
    maint = rt_task_init_schmod(nam2num("MAINT"), 3, 0, 0, SCHED_FIFO, 0xF);
  #endif 
}

// start writing task
int write_loop()
{
  #ifdef XENOMAI
    errno = rt_task_create(&writing_task,NULL,0,50,0);
    if (errno) {
      printf("transmitest: Failed to create rt task, code %d\n",errno);
      return errno;
    }
    errno = rt_task_start(&writing_task,&writing_task_proc,NULL);
    if (errno) {
      printf("transmitest: Failed to start rt task, code %d\n",errno);
      return errno;
    }
  #endif

  #ifdef RTAI
    errno = pthread_create(&writing_thr, NULL, (void *(*)(void *))&writing_task_proc, NULL);
    if (errno) {
      printf("transmitest: Failed to create and start rt task, code %d\n",errno);
      return errno;
    }
  #endif 

  SET_INIT_STATE(STATE_TASK_CREATED);
  printf("transmitest: writing data to CAN ... (press Ctrl-C to exit)\n");

  pause();

  printf("transmitest: finishing\n");
  do_exit(0);
 
  return 0;
}
