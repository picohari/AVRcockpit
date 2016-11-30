#include "DataPlugin.hpp"          // corresponding header file
#include "../../../common/datacodes.h"

#include <math.h>                  // for atan2, sqrt
#include <stdio.h>                 // for sample output
#include <winsock.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>


// plugin information
unsigned g_uPluginID          = 0;
char     g_szPluginName[]     = "Data Plugin";
unsigned g_uPluginVersion     = 001;
unsigned g_uPluginObjectCount = 1;
InternalsPluginInfo g_PluginInfo;


// interface to plugin information
extern "C" __declspec(dllexport) const char* __cdecl GetPluginName() { return g_szPluginName; }
extern "C" __declspec(dllexport) unsigned __cdecl GetPluginVersion() { return g_uPluginVersion; }
extern "C" __declspec(dllexport) unsigned __cdecl GetPluginObjectCount() { return g_uPluginObjectCount; }


// get the plugin-info object used to create the plugin.
extern "C" __declspec(dllexport) PluginObjectInfo* __cdecl GetPluginObjectInfo( const unsigned uIndex ) {
  switch(uIndex) {
    case 0:
      return  &g_PluginInfo;
    default:
      return 0;
  }
}


// InternalsPluginInfo class
InternalsPluginInfo::InternalsPluginInfo() {
  // put together a name for this plugin
  sprintf( m_szFullName, "%s - %s", g_szPluginName, InternalsPluginInfo::GetName() );
}

const char*    InternalsPluginInfo::GetName()     const { return ExampleInternalsPlugin::GetName(); }
const char*    InternalsPluginInfo::GetFullName() const { return m_szFullName; }
const char*    InternalsPluginInfo::GetDesc()     const { return "Data Plugin"; }
const unsigned InternalsPluginInfo::GetType()     const { return ExampleInternalsPlugin::GetType(); }
const char*    InternalsPluginInfo::GetSubType()  const { return ExampleInternalsPlugin::GetSubType(); }
const unsigned InternalsPluginInfo::GetVersion()  const { return ExampleInternalsPlugin::GetVersion(); }
void*          InternalsPluginInfo::Create()      const { return new ExampleInternalsPlugin(); }


// InternalsPlugin class
const char ExampleInternalsPlugin::m_szName[] = "Data Plugin";
const char ExampleInternalsPlugin::m_szSubType[] = "Internals";
const unsigned ExampleInternalsPlugin::m_uID = 1;
const unsigned ExampleInternalsPlugin::m_uVersion = 3;		// set to 3 for InternalsPluginV3 functionality and added graphical and vehicle info


PluginObjectInfo *ExampleInternalsPlugin::GetInfo() {
  return &g_PluginInfo;
}


void ExampleInternalsPlugin::log(const char *msg) {
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


void ExampleInternalsPlugin::Startup() {
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
		ptrh = gethostbyname("localhost");	/* Convert host name to equivalent IP address and copy to sad. */
	}
	memset((char *)&sad, 0, sizeof(sad));	/* clear sockaddr structure */
	sad.sin_family = AF_INET;				/* set family to Internet     */
	sad.sin_port = htons((u_short) 6788);
	if (((char *)ptrh) == NULL) {
		log("invalid host name");
		return;
	}
	memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
	log("starting telemetry\n");
}


void ExampleInternalsPlugin::Shutdown() {
	if (s > 0) {
		closesocket(s);
		s = 0;
	}
	log("stopping telemetry\n");
}


void ExampleInternalsPlugin::StartSession() {
}

void ExampleInternalsPlugin::EndSession() {
}

void ExampleInternalsPlugin::EnterRealtime() {
	mET = 0.0f;
}

void ExampleInternalsPlugin::ExitRealtime() {
}






void ExampleInternalsPlugin::UpdateTelemetry(const TelemInfoV2 &info) {
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
	//StreamData((char *)&info.mHeadlights, sizeof(bool),			HEADLIGHTS,		BOOL_T);

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


void ExampleInternalsPlugin::UpdateScoring(const ScoringInfoV2 &info) {
	//log("starting update\n");
	StartStream();
	/**
	StreamData((char *)&type_scoring, sizeof(char));

	// session data (changes mostly with changing sessions)
	StreamString((char *)&info.mTrackName, 64);
	StreamData((char *)&info.mSession, sizeof(long));

	// event data (changes continuously)
	StreamData((char *)&info.mCurrentET, sizeof(float));
	StreamData((char *)&info.mEndET, sizeof(float));
	StreamData((char *)&info.mLapDist, sizeof(float));
	StreamData((char *)&info.mNumVehicles, sizeof(long));

	StreamData((char *)&info.mGamePhase, sizeof(byte));
	StreamData((char *)&info.mYellowFlagState, sizeof(byte));
	StreamData((char *)&info.mSectorFlag[0], sizeof(byte));
	StreamData((char *)&info.mSectorFlag[1], sizeof(byte));
	StreamData((char *)&info.mSectorFlag[2], sizeof(byte));
	StreamData((char *)&info.mStartLight, sizeof(byte));
	StreamData((char *)&info.mNumRedLights, sizeof(byte));

	StreamData((char *)&info.mOnPathWetness, sizeof(float));
	StreamData((char *)&info.mOffPathWetness, sizeof(float));
	StreamData((char *)&info.mRaining, sizeof(float));
	StreamData((char *)&info.mDarkCloud, sizeof(float));
	StreamData((char *)&info.mAmbientTemp, sizeof(float));
	StreamData((char *)&info.mWind.x, sizeof(float));
	StreamData((char *)&info.mWind.y, sizeof(float));
	StreamData((char *)&info.mWind.z, sizeof(float));

	// scoring data (changes with new sector times)
	for (long i = 0; i < info.mNumVehicles; i++) {
		VehicleScoringInfoV2 &vinfo = info.mVehicle[i];
		StreamData((char *)&vinfo.mPos.x, sizeof(float));
		StreamData((char *)&vinfo.mPos.z, sizeof(float));
		StreamData((char *)&vinfo.mPlace, sizeof(char));
		StreamData((char *)&vinfo.mLapDist, sizeof(float));
		StreamData((char *)&vinfo.mPathLateral, sizeof(float));
		const float metersPerSec = sqrtf( ( vinfo.mLocalVel.x * vinfo.mLocalVel.x ) +
                                      ( vinfo.mLocalVel.y * vinfo.mLocalVel.y ) +
                                      ( vinfo.mLocalVel.z * vinfo.mLocalVel.z ) );
		StreamData((char *)&metersPerSec, sizeof(float));
		StreamString((char *)&vinfo.mVehicleName, 64);
		StreamString((char *)&vinfo.mDriverName, 32);
		StreamString((char *)&vinfo.mVehicleClass, 32);
		StreamData((char *)&vinfo.mTotalLaps, sizeof(short));
		StreamData((char *)&vinfo.mBestSector1, sizeof(float));
		StreamData((char *)&vinfo.mBestSector2, sizeof(float));
		StreamData((char *)&vinfo.mBestLapTime, sizeof(float));
		StreamData((char *)&vinfo.mLastSector1, sizeof(float));
		StreamData((char *)&vinfo.mLastSector2, sizeof(float));
		StreamData((char *)&vinfo.mLastLapTime, sizeof(float));
		StreamData((char *)&vinfo.mCurSector1, sizeof(float));
		StreamData((char *)&vinfo.mCurSector2, sizeof(float));
		StreamData((char *)&vinfo.mTimeBehindLeader, sizeof(float));
		StreamData((char *)&vinfo.mLapsBehindLeader, sizeof(long));
		StreamData((char *)&vinfo.mTimeBehindNext, sizeof(float));
		StreamData((char *)&vinfo.mLapsBehindNext, sizeof(long));
		StreamData((char *)&vinfo.mNumPitstops, sizeof(short));
		StreamData((char *)&vinfo.mNumPenalties, sizeof(short));
		StreamData((char *)&vinfo.mInPits, sizeof(bool));
		StreamData((char *)&vinfo.mSector, sizeof(char));
		StreamData((char *)&vinfo.mFinishStatus, sizeof(char));
	}
	StreamVarString((char *) info.mResultsStream);
	**/
	EndStream();
	//log("ending update\n");
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

void ExampleInternalsPlugin::StartStream() {
	data[0] = BEGIN_TRANSMISSION;							// STX
	memcpy(&data[1], &data_sequence, sizeof(short));		// PACKET NUMBER
	data_sequence++;
	data_offset = 3;
}


void ExampleInternalsPlugin::StreamData(char *data_ptr, int length, unsigned short what, unsigned char type) {

	data[data_offset] = RECORD_SEPARATOR;					// RS Record Separator
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


void ExampleInternalsPlugin::EndStream() {

	data[data_offset] = END_TRANSMISSION;					// DATA
	data_offset += 1;

	if (data_offset > 4) {
		sendto(s, data, data_offset, 0, (struct sockaddr *) &sad, sizeof(struct sockaddr));
	}
}






void ExampleInternalsPlugin::StreamVarString(char *data_ptr) {

}

void ExampleInternalsPlugin::StreamString(char *data_ptr, int length) {

}

















/**

void ExampleInternalsPlugin::StartStream() {
	data_packet = 0;
	data_sequence++;
	data[0] = data_startbyte;
	data[1] = data_packet;
	memcpy(&data[2], &data_sequence, sizeof(short));
	data_offset = 4;
}

void ExampleInternalsPlugin::EndStream() {
	if (data_offset > 4) {
		sendto(s, data, data_offset, 0, (struct sockaddr *) &sad, sizeof(struct sockaddr));
	}
}

void ExampleInternalsPlugin::StreamData(char *data_ptr, int length) {
	int i;

	for (i = 0; i < length; i++) {
		if (data_offset + i == 512) {
			sendto(s, data, 512, 0, (struct sockaddr *) &sad, sizeof(struct sockaddr));
			data_packet++;
			data[0] = data_startbyte;
			data[1] = data_packet;
			memcpy(&data[2], &data_sequence, sizeof(short));
			data_offset = 4;
			length = length - i;
			data_ptr += i;
			i = 0;
		}
		data[data_offset + i] = data_ptr[i];
	}
	data_offset = data_offset + length;
}









void ExampleInternalsPlugin::StreamVarString(char *data_ptr) {
	int i = 0;
	while (data_ptr[i] != 0) {
		i++;
	}
	StreamData((char *)&i, sizeof(int));
	StreamString(data_ptr, i);
}

void ExampleInternalsPlugin::StreamString(char *data_ptr, int length) {
	int i;

	for (i = 0; i < length; i++) {
		if (data_offset + i == 512) {
			sendto(s, data, 512, 0, (struct sockaddr *) &sad, sizeof(struct sockaddr));
			data_packet++;
			data[0] = data_startbyte;
			data[1] = data_packet;
			memcpy(&data[2], &data_sequence, sizeof(short));
			data_offset = 4;
			length = length - i;
			data_ptr += i;
			i = 0;
		}
		data[data_offset + i] = data_ptr[i];
		if (data_ptr[i] == 0) {
			// found end of string, so this is where we stop
			data_offset = data_offset + i + 1;
			return;
		}
	}
	data_offset = data_offset + length;
}

**/


