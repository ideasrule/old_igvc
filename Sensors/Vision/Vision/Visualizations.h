
#include "Frame.h"
#include "cv.h"
#include "highgui.h"
#include <vector>
#include "Spline.h"

#include "Memory.h"

class ImageLane;

using namespace Pave_Libraries_Camera;

void showDisparity(const shared_ptr<Frame> frm);
void showOverlay(const shared_ptr<Frame> frm);
void showLaneOverlay(const shared_ptr<Frame> frm, const std::vector<Spline> &splines, bool save = false);
void showTopDown(const shared_ptr<Frame> frm);
