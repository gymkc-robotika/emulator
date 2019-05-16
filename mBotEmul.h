#ifndef MBOT_EMUL_H
#define MBOT_EMUL_H

struct mBotVisual {
	double x, y;
	double heading;
};

mBotVisual emulatorSetup();
mBotVisual emulatorLoop(double dt);

#endif

