#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-return-braced-init-list"
#ifndef MBOT_EMUL_H
#define MBOT_EMUL_H

struct Pos {
	double x, y;

	explicit Pos(double x = 0, double y = 0):x(x),y(y){}

	void operator += (Pos that) {
		x += that.x;
		y += that.y;
	}
	void operator -= (Pos that) {
		x -= that.x;
		y -= that.y;
	}

	Pos operator + (Pos that) const {
		Pos ret = *this;
		ret += that;
		return ret;
	}
	Pos operator - (Pos that) const {
		Pos ret = *this;
		ret -= that;
		return ret;
	}

	void operator *= (double f) {
		x *= f;
		y *= f;
	}

	void operator /= (double f) {
		*this *= 1 / f;
	}

	Pos operator * (double f) const {
		Pos ret = *this;
		ret *= f;
		return ret;
	}

	Pos operator / (double f) const {
		Pos ret = *this;
		ret /= f;
		return ret;
	}

	double dist(Pos that) const {
		return sqrt((x - that.x) * (x - that.x) + (y - that.y) * (y - that.y));
	}
};
enum RoomColor {
	RoomWhite, RoomBlack, RoomWall
};

static const double botScale = 0.1;

struct MBotPos {
	// coordinates
	Pos pos;
	double heading = 0;
	RoomColor sensorLeft = RoomWhite, sensorRight = RoomWhite;
	double ultrasonicDistance = 4.0;

	COLORREF ledLeft = 0, ledRight = 0;

	Pos local(double x, double y) const {
		double sh = sin(heading);
		double ch = cos(heading);
		double px = pos.x + ch * x + sh * y;
		double py = pos.y - sh * x + ch * y;
		return Pos(px, py);
	}

	Pos body(double x, double y) const {
		return local(x * botScale, y * botScale);
	}
};

struct MBotConfig {
	static constexpr double lineSensorPosFront = 0.1;
	static constexpr double lineSensorPosL = +0.005;
	static constexpr double lineSensorPosR = -0.005;
};

MBotPos emulatorSetup();

MBotPos emulatorLoop(double dt);

void placeMBot(double xPos, double yPos);

void rotateMBot(double heading);

RoomColor GetRoomColor(Pos pos);

double RoomRayCast(Pos beg, Pos end);



#endif


#pragma clang diagnostic pop