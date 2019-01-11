//  Copyright (C) 2004-2009, Robotics Equipment Corporation GmbH

#ifndef WIN32
#ifdef USE_RTAI

#include "RTAI.h"

RTAI::RTAI( unsigned long long period )
{
  if( !( task = rt_task_init( nam2num("RTAI"), 1, 0, 0) ) )
	{
		throw RTAIException("Couldn't init rtai task");
	}

  rt_set_oneshot_mode();
  start_rt_timer( nano2count( period ) );
  mlockall( MCL_CURRENT | MCL_FUTURE );

  rt_make_hard_real_time();

  rt_task_make_periodic(task, rt_get_time() + 5 * nano2count(period), nano2count(period));
}

RTAI::~RTAI()
{
  rt_make_soft_real_time();
  if(task != NULL)
    rt_task_delete(task);
}

void RTAI::waitPeriod()
{
  rt_task_wait_period();
}

#endif
#endif
