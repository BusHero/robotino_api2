//  Copyright (C) 2004-2008, Robotics Equipment Corporation GmbH

#ifndef _REC_ROBOTINO_API2_C_FACTORY4_H_
#define _REC_ROBOTINO_API2_C_FACTORY4_H_

#include "rec/robotino/api2/c/globals.h"
#include "rec/robotino/api2/c/Com.h"

/** \file Factory4.h
\brief In "rec/robotino/api2/c/Factory4.h" you can find functions for reading Robotino's bumper.

Use Factory4_construct() to create a new Factory4 object. Associate the Factory4 object with a com object using Factory4_setComId().
Use Factory4_setSetPointSpeed() to set the Factory4's speed.
*/

/** Factory4Id */
typedef int Factory4Id;

/** Invalid Factory4Id is -1 */
#define INVALID_FACTORY4ID -1

/**
Construct an Factory4 object
@return Returns the ID of the newly constructed Factory4 object.
*/
DLLEXPORT Factory4Id Factory4_construct();

/**
Destroy the Factory4 object assigned to id
@param id The id of the Factory4 object to be destroyed
@return Returns TRUE (1) on success. Returns FALSE (0) if the given Factory4Id is invalid.
*/
DLLEXPORT BOOL Factory4_destroy( Factory4Id id );

/**
Associated a Factory4 object with a communication interface, i.e. binding the Factory4 to a specific Robotino
@return Returns TRUE (1) on success. Returns FALSE (0) if the given Factory4Id or ComId is invalid.
*/
DLLEXPORT BOOL Factory4_setComId( Factory4Id id, ComId comId );

DLLEXPORT BOOL Factory4_processEvents(Factory4Id id);

/*
For a list of possoble messages see http://wiki.openrobotino.org/index.php?title=SmartFestoFleetCom
*/
DLLEXPORT BOOL Factory4_sendMessage(Factory4Id id, const char* message);

/**
* Send a modified version of the navigation map to the robot. Provide data as png image.
*/
DLLEXPORT BOOL Factory4_setMapPlanner(Factory4Id id, const char* data, unsigned int dataSize);

DLLEXPORT BOOL Factory4_mapDirRequest(Factory4Id id, const char* infoData, const unsigned int infoDataSize, const char* data, const unsigned int dataSize);

DLLEXPORT BOOL Factory4_savePathNetwork(Factory4Id id, const char* data, const unsigned int dataSize);

DLLEXPORT BOOL Factory4_setSmartLocations(Factory4Id id, const char* data, const unsigned int dataSize);
DLLEXPORT BOOL Factory4_setSmartStations(Factory4Id id, const char* data, const unsigned int dataSize);

/*Events*/

typedef void(*Factory4_data1Cb_t)(const char* data, int dataSize);
typedef void(*Factory4_data2Cb_t)(const char* data, int dataSize, const char* data2, int data2Size);

DLLEXPORT BOOL Factory4_setMclayoutEventCb(Factory4Id id, Factory4_data1Cb_t func);

DLLEXPORT BOOL Factory4_setMcstatusEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setPathnetworkEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setLocalizationModeEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setSmartlogEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setSmartnavigationplanEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setSmartlocationsEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setSmartstationsEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setSmartrobotinfoEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setSmartmyrobotidEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setSmartjoblistEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setMapDirEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setMapDirWithDataEventCb(Factory4Id id, Factory4_data2Cb_t func);
DLLEXPORT BOOL Factory4_setFleetcom_responseEventCb(Factory4Id id, Factory4_data1Cb_t func);
DLLEXPORT BOOL Factory4_setMapDir_responseEventCb(Factory4Id id, Factory4_data2Cb_t func);
DLLEXPORT BOOL Factory4_setSmartlicenseEventCb(Factory4Id id, Factory4_data1Cb_t func);

/**
* The current map used for localization and navigation as jpg image.
*/
typedef void(*Factory4_mapEventCb_t)(const char* data, int dataSize, int width, int height, int resolution, int offsetx, int offsety, int sequence);
DLLEXPORT BOOL Factory4_setMapEventCb(Factory4Id id, Factory4_mapEventCb_t func);

/**
* The navigation map as png image
*/
DLLEXPORT BOOL Factory4_setMapPlannerEventCb(Factory4Id id, Factory4_mapEventCb_t func);


#endif //_REC_ROBOTINO_API2_C_FACTORY4_H_
