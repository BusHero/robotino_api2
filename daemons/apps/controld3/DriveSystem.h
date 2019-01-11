#ifndef _DRIVESYSTEM_H_
#define _DRIVESYSTEM_H_

#include "DriveLayout.h"

/**
@param m1 Speed set-point motor 1 in rpm
@param m2 Speed set-point motor 2 in rpm
@param m3 Speed set-point motor 3 in rpm
@param vx Speed set-point in x-direction in m/s
@param vy Speed set-point in x-direction in m/s
@param omega Rotational speed set-point (position is counter-clockwise ssen from the top) in rad/s
*/
void projectVelocity( float* m1, float* m2, float* m3, float vx, float vy, float omega, const DriveLayout& layout );

/**
@param vx Speed set-point in x-direction in m/s
@param vy Speed set-point in x-direction in m/s
@param m1 Speed set-point motor 1 in rpm
@param m2 Speed set-point motor 2 in rpm
@param m3 Speed set-point motor 3 in rpm
*/
void unprojectVelocity( float* vx, float* vy, float* omega, float m1, float m2, float m3, const DriveLayout& layout );

#endif //_DRIVESYSTEM_H_
