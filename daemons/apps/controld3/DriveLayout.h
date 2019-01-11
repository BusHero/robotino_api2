#ifndef _DRIVELAYOUT_H_
#define _DRIVELAYOUT_H_

class DriveLayout
{
public:
	DriveLayout()
		: rb( 0.17 )
		, rw( 0.06 )
		, gear( 32.0 )
	{
	}

	/**
	* distance from center to wheel center
	* Set by stated from robotino.xml.
	*/
	double rb;

	/**
	* radius wheel in m
	* Set by stated from robotino.xml.
	*/
	double rw;

	/**
	* gear
	* Set by stated from robotino.xml.
	*/
	double gear;
};

#endif //_DRIVELAYOUT_H_
