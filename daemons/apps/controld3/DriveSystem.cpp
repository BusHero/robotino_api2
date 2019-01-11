#include "DriveSystem.h"

#define _USE_MATH_DEFINES
#include <cmath>

void projectVelocity( float* m1, float* m2, float* m3, float vx, float vy, float omega, const DriveLayout& layout )
{
  //Projection matrix
  static const double v0[2] = { -0.5 * sqrt( 3.0 ),  0.5 };
  static const double v1[2] = {  0.0              , -1.0 };
  static const double v2[2] = {  0.5 * sqrt( 3.0 ),  0.5 };

  //Scale omega with the radius of the robot
  double vOmegaScaled = layout.rb * (double)omega ;

  //Convert from m/s to RPM
  const double k = 60.0 * layout.gear / ( 2.0 * M_PI * layout.rw );

  //Compute the desired velocity
  *m1 = static_cast<float>( ( v0[0] * (double)vx + v0[1] * (double)vy + vOmegaScaled ) * k );
  *m2 = static_cast<float>( ( v1[0] * (double)vx + v1[1] * (double)vy + vOmegaScaled ) * k );
  *m3 = static_cast<float>( ( v2[0] * (double)vx + v2[1] * (double)vy + vOmegaScaled ) * k );
}


void unprojectVelocity( float* vx, float* vy, float* omega, float m1, float m2, float m3, const DriveLayout& layout )
{
	//std::cout << m1 << " " << m2 << " " << m3 << std::endl;

	//Convert from RPM to mm/s
	const double k = 60.0 * layout.gear / ( 2.0 * M_PI * layout.rw );

	*vx = static_cast<float>( ( (double)m3 - (double)m1 ) / sqrt( 3.0 ) / k );
	*vy = static_cast<float>( 2.0 / 3.0 * ( (double)m1 + 0.5 * ( (double)m3 - (double)m1 ) - (double)m2 ) / k );

	double vw = (double)*vy + (double)m2 / k;

	*omega = static_cast<float>( vw / layout.rb );
}