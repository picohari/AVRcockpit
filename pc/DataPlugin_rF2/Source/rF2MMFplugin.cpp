//‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
//›                                                                         ﬁ
//› Module: Internals Example Source File                                   ﬁ
//›                                                                         ﬁ
//› Description: Declarations for the Internals Example Plugin              ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› This source code module, and all information, data, and algorithms      ﬁ
//› associated with it, are part of CUBE technology (tm).                   ﬁ
//›                 PROPRIETARY AND CONFIDENTIAL                            ﬁ
//› Copyright (c) 1996-2008 Image Space Incorporated.  All rights reserved. ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› Change history:                                                         ﬁ
//›   tag.2005.11.30: created                                               ﬁ
//›                                                                         ﬁ
//ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

#include "rF2MMFplugin.hpp"          // corresponding header file

#include "../../../common/datacodes.h"

#include <math.h>                  // for atan2, sqrt
#include <stdio.h>                 // for sample output
#include <winsock.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

// plugin information

extern "C" __declspec( dllexport )
const char * __cdecl GetPluginName()                   { return( "rF2MMFplugin" ); }

extern "C" __declspec( dllexport )
PluginObjectType __cdecl GetPluginType()               { return( PO_INTERNALS ); }

extern "C" __declspec( dllexport )
int __cdecl GetPluginVersion()                         { return( 1 ); } // InternalsPluginV01 functionality

extern "C" __declspec( dllexport )
PluginObject * __cdecl CreatePluginObject()            { return( (PluginObject *) new rF2MMFplugin ); }

extern "C" __declspec( dllexport )
void __cdecl DestroyPluginObject( PluginObject *obj )  { delete( (rF2MMFplugin *) obj ); }



void rF2MMFplugin::log(const char *msg) {
	FILE *logFile;
	time_t curtime;
	struct tm *loctime;

	logFile = fopen("DataPlugin.log", "a");
	if (logFile != NULL) {
		curtime = time(NULL);
		loctime = localtime(&curtime);
		fprintf(logFile, "[%s] %s\n", asctime (loctime), msg);
		fclose(logFile);
	}
}


void rF2MMFplugin::Startup( long version )
{

	FILE *settings;
	struct hostent *ptrh;
	data_startbyte = 0x02;		// Start Marker: Send 0x02 = STX: Start of transmission

	// open socket
	s = socket(PF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		log("could not create datagram socket");
		return;
	}
	settings = fopen("DataSettings.ini", "r");
	if (settings != NULL) {
		fscanf(settings, "%s", hostname);
		ptrh = gethostbyname(hostname);
		fclose(settings);
	}
	else {
		ptrh = gethostbyname("localhost");	// Convert host name to equivalent IP address and copy to sad.
	}
	memset((char *)&sad, 0, sizeof(sad));	// clear sockaddr structure
	sad.sin_family = AF_INET;				// set family to Internet
	sad.sin_port = htons((u_short) 6788);
	if (((char *)ptrh) == NULL) {
		log("invalid host name");
		return;
	}
	memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

	log("starting telemetry\n");
}

void rF2MMFplugin::Shutdown()
{
	if (s > 0) {
		closesocket(s);
		s = 0;
	}
	log("stopping telemetry\n");
}

void rF2MMFplugin::StartSession()
{

}

void rF2MMFplugin::EndSession()
{

}

void rF2MMFplugin::EnterRealtime()
{

}

void rF2MMFplugin::ExitRealtime()
{

}

void rF2MMFplugin::UpdateTelemetry( const TelemInfoV01 &info )
{
	//log("starting telemetry\n");
	StartStream();
	//StreamData((char *)&type_telemetry,		sizeof(char),	TELEMETRY,		CHAR_T);
	StreamData((char *)&info.mGear,				sizeof(long),	GEAR,			LONG_T);
	StreamData((char *)&info.mEngineRPM,		sizeof(float),	ENGRPM,			FLOAT_T);
	StreamData((char *)&info.mEngineMaxRPM,		sizeof(float),	ENGMAXRPM,		FLOAT_T);
	StreamData((char *)&info.mEngineWaterTemp,	sizeof(float),	ENGWATERTEMP,	FLOAT_T);
	StreamData((char *)&info.mEngineOilTemp,	sizeof(float),	ENGOILTEMP,		FLOAT_T);
	//StreamData((char *)&info.mClutchRPM, sizeof(float));
	StreamData((char *)&info.mOverheating, sizeof(bool),		ENGOVERHEAT,	BOOL_T);
	StreamData((char *)&info.mFuel, sizeof(float),				FUELTANK,		FLOAT_T);



	//StreamData((char *)&info.mPos.x, sizeof(float));
	//StreamData((char *)&info.mPos.y, sizeof(float));
	//StreamData((char *)&info.mPos.z, sizeof(float));
	
	const float metersPerSec = sqrtf( ( info.mLocalVel.x * info.mLocalVel.x ) +
                                      ( info.mLocalVel.y * info.mLocalVel.y ) +
                                      ( info.mLocalVel.z * info.mLocalVel.z ) );
	StreamData((char *)&metersPerSec, sizeof(float),			METERSPERSEC,	FLOAT_T);
	StreamData((char *)&info.mHeadlights, sizeof(bool),			HEADLIGHTS,		BOOL_T);

	/**
	StreamData((char *)&info.mLapStartET, sizeof(float));
	StreamData((char *)&info.mLapNumber, sizeof(long));

	StreamData((char *)&info.mUnfilteredThrottle, sizeof(float));
	StreamData((char *)&info.mUnfilteredBrake, sizeof(float));
	StreamData((char *)&info.mUnfilteredSteering, sizeof(float));
	StreamData((char *)&info.mUnfilteredClutch, sizeof(float));

	StreamData((char *)&info.mLastImpactET, sizeof(float));
	StreamData((char *)&info.mLastImpactMagnitude, sizeof(float));
	StreamData((char *)&info.mLastImpactPos.x, sizeof(float));
	StreamData((char *)&info.mLastImpactPos.y, sizeof(float));
	StreamData((char *)&info.mLastImpactPos.z, sizeof(float));
	for (long i = 0; i < 8; i++) {
		StreamData((char *)&info.mDentSeverity[i], sizeof(byte));
	}

	for (long i = 0; i < 4; i++) {
		const TelemWheelV2 &wheel = info.mWheel[i];
		StreamData((char *)&wheel.mDetached, sizeof(bool));
		StreamData((char *)&wheel.mFlat, sizeof(bool));
		StreamData((char *)&wheel.mBrakeTemp, sizeof(float));
		StreamData((char *)&wheel.mPressure, sizeof(float));
		StreamData((char *)&wheel.mRideHeight, sizeof(float));
		StreamData((char *)&wheel.mTemperature[0], sizeof(float));
		StreamData((char *)&wheel.mTemperature[1], sizeof(float));
		StreamData((char *)&wheel.mTemperature[2], sizeof(float));
		StreamData((char *)&wheel.mWear, sizeof(float));
	}
	**/

	EndStream();
	//log("ending telemetry\n");
}

void rF2MMFplugin::UpdateScoring( const ScoringInfoV01 &info )
{

}

bool rF2MMFplugin::RequestCommentary( CommentaryRequestInfoV01 &info )
{
  return( false );
}



/**
        // send a trame containing 12 bytes.
        memcpy(&identificationData[0], &mSession, sizeof(long));
        memcpy(&identificationData[4], &mCurrentET, sizeof(double));
        sendto(senderSocket, identificationData, 12, 0, (struct sockaddr *) &sadSender, sizeof(struct sockaddr));

		StreamData((char *)&info.mGear, sizeof(long), GEAR, LONG_T);
**/


/*
	STX	| NR > RS |	WHAT | TYPE | VALUE < ETX
*/

void rF2MMFplugin::StartStream() {
	data[0] = data_startbyte;								// STX
	memcpy(&data[1], &data_sequence, sizeof(short));		// PACKET NUMBER
	data_sequence++;
	data_offset = 3;
}


void rF2MMFplugin::StreamData(char *data_ptr, int length, unsigned short what, unsigned char type) {

	data[data_offset] = 0x1E;								// RS Record Separator
	data_offset += 1;

	memcpy(&data[data_offset], &what, sizeof(short));		// WHAT to transmit
	data_offset += sizeof(short);

	memcpy(&data[data_offset], &type, sizeof(char));		// TYPE of transmitted data
	data_offset += sizeof(char);

	int i;

	for (i = 0; i < length; i++) {
		
		data[data_offset + i] = data_ptr[i];				// DATA
	}

	data_offset += length;
}


void rF2MMFplugin::EndStream() {

	data[data_offset] = 0x03;								// DATA
	data_offset += 1;

	if (data_offset > 4) {
		sendto(s, data, data_offset, 0, (struct sockaddr *) &sad, sizeof(struct sockaddr));
	}
}
