#ifndef MBOT_EMUL_H
#define MBOT_EMUL_H

struct mBotVisual {
	double x, y;
	double heading;
};

mBotVisual emulatorSetup();
mBotVisual emulatorLoop(double dt);

enum RoomColor {
   RoomWhite, RoomBlack, RoomWall
};
RoomColor GetRoomColor(double x, double y);

#endif

