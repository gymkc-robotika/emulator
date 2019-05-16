#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-return-braced-init-list"
#ifndef MBOT_EMUL_H
#define MBOT_EMUL_H

struct Pos {
	double x, y;

	Pos(double x = 0, double y = 0):x(x),y(y){}
};

struct MBotPos {
	// coordinates
	Pos pos;
	double heading = 0;

	Pos local(double x, double y) {
		double sh = sin(heading);
		double ch = cos(heading);
		double px = pos.x + ch * x + sh * y;
		double py = pos.y - sh * x + ch * y;
		return Pos(px, py);
	}
};

MBotPos emulatorSetup();

MBotPos emulatorLoop(double dt);

enum RoomColor {
	RoomWhite, RoomBlack, RoomWall
};

RoomColor GetRoomColor(Pos pos);

#endif


#pragma clang diagnostic pop