#ifndef _REC_JOYSTICK_DEFINES_H_
#define _REC_JOYSTICK_DEFINES_H_

#ifdef WIN32
#  ifdef rec_joystick_EXPORTS
#    define REC_JOYSTICK_EXPORT __declspec(dllexport)
#  else
#    define REC_JOYSTICK_EXPORT __declspec(dllimport)
#  endif // rec_joystick_EXPORTS
#else // WIN32
#  define REC_JOYSTICK_EXPORT __attribute__ ((visibility ("default")))
#endif // WIN32

#endif //_REC_JOYSTICK_DEFINES_H_
