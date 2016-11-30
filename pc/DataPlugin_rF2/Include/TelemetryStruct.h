#ifndef _TELEMETRY_STRUCT_H
#define _TELEMETRY_STRUCT_H

typedef struct {

	int state;
	int carid;
	int gear;
	int lap;
	int totallaps;
	int position;
	int pitlimiter;
	int sessiontype;
	int carinfront;

	double timestamp;
	double speed;
	double rpm;
	double fuel;
	double prevlap;
	double tracklen;
	double maxrpm;

	double trackpositions[64];

	char carname[64];
	char trackname[64];

	void TelemetryData() {
		int i;
		for(i=0; i < sizeof(trackpositions); i++)
			trackpositions[i] = -1.0;
	}

} TelemetryData;

#endif