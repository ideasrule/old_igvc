#include "Camera.h"
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "svs.h"
#include "svsclass.h"
// #include "PerformanceTimer.h"

using namespace Pave_Libraries_Camera;
using namespace std;

struct Pave_Libraries_Camera::CameraVidereInternals {
    const char *id;
    svsVideoImages *video;
    svsStreamImages *stream;
    bool writeStarted;
    bool started;
    svsStereoProcess *process;
    svsTransform *transform;
    svsSP *sp;
};

static void setVideoParams(svsVideoImages *video)
{
	
	// conf = 7
	video->SetThresh(6);
	// unique = 8
	video->GetDP()->unique = 6;
	// speckle = 290
	video->GetDP()->specklesize = 100;
	// x off = 0
	video->GetDP()->offx = 0;

	video->SetRate(30);

	video->SetExposure(40, 50, false, true);
}

#define DEBUG(str) cerr << "DEBUG: " << str << endl

static void testTransform(svsTransform *t) {
    DEBUG("Transform:");
    for (int i = 0; i < 16; i++)
        fprintf(stderr, "%f%s", t->tr[i], (i+1) % 4 ? " " : "\n");
    DEBUG("Test transform of (0.1, 0, 1): should be front center");
    float x = 0.1f, y = 0.0f, z = 1.0f;
    t->doTransform(&x, &y, &z);
    fprintf(stderr, "(%f, %f, %f)\n", x, y, z);
}

static bool readParam(istream &in, const char *key, float &rval) {
	if (!in.good()) return false;
	in.seekg(0, ios::beg);
	char tok[1024];
	while (in >> skipws >> setw(1024) >> tok) {
		if (strcmp(key, tok) == 0) {
			in >> skipws >> rval;
			return true;
		}
	}
	return false;
}

CameraVidere::CameraVidere(const char *id) : Camera() {
    internals = new CameraVidereInternals;
    internals->id = id;
    internals->video = NULL;
    internals->stream = NULL;
    internals->writeStarted = false;
    internals->started = false;
    internals->process = new svsStereoProcess();
	
	ostringstream setupFilename(ostringstream::out);
	setupFilename << "C:/PAVE_Common/Libraries/Camera/config/";
	setupFilename << "Videre" << id << ".txt";
	ifstream setup(setupFilename.str().c_str());
	if (!setup.is_open()) throw "Unable to find camera setup file.";

    AngleFromVertical = 0;
	readParam(setup, "AngleFromVertical", AngleFromVertical);
	DEBUG("AngleFromVertical"); DEBUG(AngleFromVertical);
    float BetweenLenses = 0;
	readParam(setup, "BetweenLenses", BetweenLenses);
	DEBUG("BetweenLenses"); DEBUG(BetweenLenses);
    float CameraHeight = 0;
	readParam(setup, "CameraHeight", CameraHeight);
	DEBUG("CameraHeight"); DEBUG(CameraHeight);

    svsTransform *t = new svsTransform();
    t->setRotX(AngleFromVertical);
    t->setTrans(-BetweenLenses/2.0, 0.0, CameraHeight);
    testTransform(t);
    internals->transform = t;
    internals->sp = NULL;
}

CameraVidere::~CameraVidere() {
	DEBUG("Cleaning up CameraVidere!");
    if (internals->video) {
        if (internals->writeStarted) {
            internals->video->StreamStop();
            internals->video->StreamClose();
        }
        if (internals->started) {
            internals->video->Stop();
        }
        internals->video->Close();
        closeVideoObject();
    }
    if (internals->stream) {
        if (internals->started) {
            internals->stream->Stop();
        }
        internals->stream->Close();
        delete internals->stream;
    }
    delete internals->process;
    delete internals->transform;
    delete internals;
}

void CameraVidere::changeTransformPitch(double pitch)
{
	internals->transform->setRotX(AngleFromVertical + pitch);
	
}

bool CameraVidere::ensureSetupCamera() {
    if (setupDone) return true;
    const char *sn = NULL;

    if (getReadFilename() && (sn = streamName())) {
        // streaming from file
        DEBUG("Streaming from file:");
        DEBUG(sn);
        svsStreamImages *stream = new svsStreamImages();
        if (stream->Open((char *)sn)) {
            DEBUG("Opened stream");
        } else {
            DEBUG("Could not open stream");
            DEBUG(stream->Error());
            return false;
        }

        stream->SetColor(true, true);
        stream->SetRect(true);

        if (stream->Start()) {
            DEBUG("Started stream");
            internals->started = true;
        } else {
            DEBUG("Could not start stream");
            DEBUG(stream->Error());
            return false;
        }

        internals->stream = stream;
        internals->sp = stream->GetSP();

    } else {
        // open device
        svsVideoImages *video = getVideoObject();
	    if (!video) {
		    DEBUG("Could not get video object");
            return false;
	    }

	    DEBUG("Using live images");
	    DEBUG(svsVideoIdent);

	    DEBUG("Enumerating devices");
	    int numDevices = video->Enumerate();
	    char **devices = video->DeviceIDs();
	    char *foundDevice = NULL;
		for (int i = 0; i < numDevices; i++) {
		    DEBUG(devices[i]);
	    }

		if (!video->Open((char*)internals->id)) {
		    DEBUG("Can't open stereo device");
		    DEBUG(video->Error());
		    closeVideoObject();
            return false;
	    }

	    video->SetSize(640, 480);

		setVideoParams(video);

        if (isPointCloudNeeded()) {
            if (video->is_proc_capable) {
		        video->SetProcMode(PROC_MODE_DISPARITY);
                DEBUG("Set Proc mode to Disparity");
            } else {
                DEBUG("Not Proc Capable WTF?");
            }
        }
    	
        if (getWriteFilename() && (sn = streamName())) {
            DEBUG("Trying to write to streaming file:");
            DEBUG(sn);
            if (video->StreamOpen((char *)sn)) {
                DEBUG("Opened streaming file");
                if (video->StreamStart()) {
                    DEBUG("Started streaming");
                    internals->writeStarted = true;
                } else {
                    DEBUG("Unable to start streaming");
                }
            } else {
                DEBUG("Unable to open streaming file");
            }
        }

	    if (!video->Start()) {
		    DEBUG("Can't start continuous capture");
		    DEBUG(video->Error());
		    closeVideoObject();
		    return false;
	    }

	    if (video->SetRect(true)) {
		    DEBUG("Images will be rectified");
	    } else {
		    DEBUG("Can't set rectification");
	    }

	    for (int i = 0; i < 3; i++)
		    video->GetImage(500);

	    if (video->is_color_capable) {
		    video->SetColor(true);
            video->SetColorAlg(COLOR_ALG_BEST);
		    DEBUG("Set color true, false with best color algorithm");
	    }

	    //Throw out the first three frames so we start with good data!
	    for (int i = 0; i < 3; i++)
		    video->GetImage(500);

        internals->video = video;
        internals->sp = video->GetSP();
    }

	setupDone = true;
	return true;
}

shared_ptr<Frame> CameraVidere::getNextImage(void) {
	// CPerformanceTimer timer;

	shared_ptr<Frame> frm(new Frame());

    if (!ensureSetupCamera()) {
        DEBUG("Camera was not set up");
        return shared_ptr<Frame>();
    }

    // Acquire image
    svsAcquireImages *acquire;
	if (internals->video) acquire = internals->video;
    else if (internals->stream) acquire = internals->stream;
	else {
        DEBUG("No acquisition device or stream");
        return shared_ptr<Frame>();
    }
    svsStereoImage *si = acquire->GetImage(500);
    if (!si) {
        DEBUG("No image acquired!");
        return shared_ptr<Frame>();
    }

	frm->framenum = acquire->acqFrame;

    // Ensure frame is correct size
    //frm->setSize(si->ip.width, si->ip.height);
	frm->setSize(320, 240);

    frm->camera = this;
    if (si->haveRect)
		frm->focalLength = si->rp.left.f;
    else 
		frm->focalLength = 0;

	frm->setObstacle((unsigned char)0);

    // Name after live time OR stream time
    _ftime_s(&frm->time);
    stringstream sstream;
    sstream << static_cast<long int>(frm->time.time);
    sstream.fill('0');
    sstream.width(3);
    sstream << static_cast<int>(frm->time.millitm);
    frm->name = sstream.str();

    // Calculate depth map and maxima
    if (isPointCloudNeeded()) {
        if (!si->haveDisparity) {
            DEBUG("Need to compute disparity WTF?");
            internals->process->CalcStereo(si);
        }

		si->have3D = false;
        svs3Dpoint min, max; 
        internals->process->Calc3D(si, 0, 0, 0, 0,
            /*dest*/ NULL, internals->transform->tr,
            /*z cutoff*/ 0, &min, &max);
        frm->min.x = min.X;
        frm->min.y = min.Y;
        frm->min.z = min.Z;
        frm->max.x = max.X;
        frm->max.y = max.Y;
        frm->max.z = max.Z;
		int idx = 0;

		// timer.Start();

		if (FRAME_IMAGE_HEIGHT == 240 && FRAME_IMAGE_WIDTH == 320) {
			for (int r = 0; r < frm->height; r++) {
				for (int c = 0; c < frm->width; c++) {
					int index = (r*2*IMAGE_FULL_WIDTH) + c*2;
					svs3Dpoint *svsPt = si->pts3D + index;
					cv::Point3f& pt = frm->transformedCloud(r, c);
					pt = cv::Point3f(svsPt->X, svsPt->Y, svsPt->Z);
					frm->validArr[r][c] = (svsPt->A > 0);
				}
			}
		} else {
			// 640 x 480
			for (int r = 0; r < frm->height; r++) {
				for (int c = 0; c < frm->width; c++) {
					int index = (r*IMAGE_FULL_WIDTH) + c;
					svs3Dpoint *svsPt = si->pts3D + index;
					cv::Point3f& pt = frm->transformedCloud(r, c);
					pt = cv::Point3f(svsPt->X, svsPt->Y, svsPt->Z);
					frm->validArr[r][c] = (svsPt->A > 0);
				}
			}
		}

		// timer.Stop();
		// cout << "acquisition time = " << timer.Interval_mS() << "ms\n";

    } // if point cloud needed

    // Copy picture data using OpenCV. Autostep (no gap between rows)
	cv::Mat nativeColor(IMAGE_FULL_HEIGHT, IMAGE_FULL_WIDTH, CV_8UC4, si->Color());
    const int channelMap[] = { 0,2 , 1,1 , 2,0 }; // RGBX to BGR
    const unsigned int channelMapPairs = 3;
	cv::mixChannels(&nativeColor, 1, &frm->colorFull, 1, channelMap, channelMapPairs);
    
	cv::Mat nativeMono(IMAGE_FULL_HEIGHT, IMAGE_FULL_WIDTH, CV_8UC1, si->Left());
    //nativeMono.convertTo(frm->mono, frm->mono.type());
	cv::resize(frm->colorFull, frm->color, cv::Size(320, 240));
    //cv::cvtColor(frm->color, frm->mono, CV_RGB2GRAY);

    return frm;
}

const char *CameraVidere::streamName() {
    if (getDirectory() && (getWriteFilename() || getReadFilename())) {
        string streamName(getDirectory());
        streamName += "\\";
        if (getWriteFilename())
            streamName += getWriteFilename();
        else
            streamName += getReadFilename();
        streamName += ".ssf";
        return strdup(streamName.c_str());
    } else {
        return NULL;
    }
}

bool CameraVidere::toXYZ(float row, float col, float disp, float* x, float* y, float* z) {
    if (!ensureSetupCamera()) {
        DEBUG("Camera was not set up");
        return false;
    }
    
    svsReconstruct3D(x, y, z, col, row, disp, internals->sp, internals->transform);
    return true;
}

bool CameraVidere::toRCD(float x, float y, float z, float* row, float* col, float* disp) {
    if (!ensureSetupCamera()) {
        DEBUG("Camera was not set up");
        return false;
    }
    
    svsProject3D(col, row, x, y, z, internals->sp, svsLEFT, internals->transform);
    *disp = 0;
    return true;
}