#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>

// #include "control_chain.h"

extern "C"
{
  void (*timerCallback)(void);

  timer_t posixTimerID = (void *)0xBAADF00D;

  void PosixTimerHandler(int, siginfo_t *, void *)
  {
    //printf("PosixTimerHandler\n");
    if(timerCallback)
      timerCallback();
  }

  bool CreatePosixTimer(void)
  {
    struct sigevent         sigEvent;
    struct sigaction        sigAction;
    int                     sigNum = SIGRTMIN;

    sigAction.sa_flags = SA_SIGINFO;
    sigAction.sa_sigaction = PosixTimerHandler;
    sigemptyset(&sigAction.sa_mask);

    if (sigaction(sigNum, &sigAction, NULL) == -1)
    {
      printf("Error sigaction() failed\n");
      return false;
    }

    sigEvent.sigev_notify = SIGEV_SIGNAL;
    sigEvent.sigev_signo = sigNum;

    if (timer_create(CLOCK_REALTIME, &sigEvent, &posixTimerID) == -1)
    {
      printf("Error timer_create() failed\n");
      return false;
    }

    return true;
  }

  bool SetPosixTimer(uint32_t timeUS)
  {
    struct itimerspec       intervalTimerSpec;

    time_t seconds = timeUS / 1000000;
    time_t ns = 1000* (timeUS - (seconds * 1000000));
    intervalTimerSpec.it_value.tv_sec = seconds;
    intervalTimerSpec.it_value.tv_nsec = ns;
    intervalTimerSpec.it_interval.tv_sec = 0;
    intervalTimerSpec.it_interval.tv_nsec = 0;

    if (timer_settime(posixTimerID, 0, &intervalTimerSpec, NULL) == -1)
    {
      printf("Error timer_settime() failed\n");
      return false;
    }

    return true;
  }

  void timer_init(void (*callback)(void))
  {
    timerCallback = callback;
    CreatePosixTimer();
  }

  void timer_set(uint32_t time_us)
  {
    SetPosixTimer(time_us);
  }

  void delay_us(uint32_t time_us)
  {
    struct timespec td = {
        .tv_sec = 0,
        .tv_nsec = time_us*1000 
    };
    nanosleep(&td, NULL);   
  }
}
