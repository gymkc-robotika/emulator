#ifndef MBOT_EMUL_H
#define MBOT_EMUL_H

struct Pos {
	double x = 0, y = 0;
};

struct MBotPos {
	// coordinates
	Pos pos;
	double heading = 0;

};

MBotPos emulatorSetup();

MBotPos emulatorLoop(double dt);

enum RoomColor {
	RoomWhite, RoomBlack, RoomWall
};

RoomColor GetRoomColor(Pos pos);

#endif

