#include <stdlib.h>
#include <crtdbg.h>

#include "Camera.h"
#include "Frame.h"
#include "FrameLoggerText.h"
#include "FrameLoggerBin.h"
#include "commandArgs.h"

#include <conio.h> // for kbdhit

using namespace std;
using namespace Pave_Libraries_Common;

int main(int argc, char **argv)
{
	commandArgs args(argc, argv);
	
	shared_ptr<Frame> frm;
	FrameLoggerBin *olog = new FrameLoggerBin(args.readLogDir, (string(args.readLogBasename) + "-bin").c_str());
	FrameLoggerText *ilog = new FrameLoggerText(args.readLogDir, args.readLogBasename);

    if (!ilog) {
        cerr << "Need input and output directories." << endl;
	}

	cerr << "=====Hit any key to stop capturing.=====" << endl;

	while (!_kbhit()) {
		frm = ilog->readLog(NULL);
		if (!frm)
			break;

        olog->log(frm, NULL);
	}

	if (olog) delete olog;
	if (ilog) delete ilog;

	return 0;
}