/*****************************************************************************
 * Represents the camera. Supports functions to initialize the camera and get
 * the next image.
 *
 * The Camera class itself is virtual, so should never be instantiated.
 *
 * The mechanics of getting images is implemented by its subclasses.
 *
 * Author: Brenton Partridge '12
 *****************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#include "Frame.h"

#include "boost/tr1/memory.hpp"
using std::tr1::shared_ptr;

namespace Pave_Libraries_Camera {

	// In general for the Camera class, note that the different camera models
	// store files differently, so the way we do it now is that the CameraX
	// subclass implements reading and/or writing to files as well as the physical camera.
	// To use, construct the CameraX instance, set the variables you need
	// (note that the implementation can read and write from files at the same time
	// so we can test by diff) and then call hasNextImage and getNextFrame;
	// initialization is done on the first call to either.
	// Typically, this initialization will read from a setup file, not from 
	// constructor arguments or command-line arguments, although
	// this is not required; subclasses should document this.
	class Camera {
	protected:
		// Base class constructor. Should be called by subclass constructors.
		Camera() {
			setupDone = false;
			pointCloudNeeded = false;
			directory = NULL;
			readFilename = NULL;
			writeFilename = NULL;
		}
	public:

		virtual ~Camera() {};

		// Specifies if a point cloud should be generated, either in FPGA or in-process. 
		bool isPointCloudNeeded() {
			return pointCloudNeeded;
		}
		virtual void setPointCloudNeeded(bool pointCloudNeeded) {
			if (setupDone) throw "Already setup.";
			this->pointCloudNeeded = pointCloudNeeded; 
		}
		
		// The directory where files are read and written.
		const char *getDirectory() {
			return directory;
		}
		virtual void setDirectory(const char *directory) {
			if (setupDone) throw "Already setup.";
			this->directory = directory;
		}

		virtual void changeTransformPitch(double pitch) = 0;

		// The base file name (no directory, left/disparity/whatever suffix or file extension)
		// for written files.
		const char *getWriteFilename() {
			return writeFilename;
		}
		virtual void setWriteFilename(char *writeFilename) {
			if (setupDone) throw "Already setup.";
			this->writeFilename = writeFilename;
		}

		// The base file name (no directory, left/disparity/whatever suffix or file extension)
		// for read files.
		const char *getReadFilename() {
			return readFilename;
		}
		virtual void setReadFilename(const char *readFilename) {
			if (setupDone) throw "Already setup.";
			this->readFilename = readFilename;
		}

        // Ensures the camera is setup and returns true iff it is.
        // Returns false on error.
		virtual bool ensureSetupCamera() = 0;

		// Pass an existing frame, which may just be the uninitialized default-constructor frame.
		// Caller is responsible for ensuring the frame passed in is destroyed.
		// (Of course, it will automatically be destroyed if it's on the stack,
		// but if it's a pointer Frame *frm, pass it in as *frm, and remember to delete it.)
		// Will set frame size if necessary, thereby allocating memory, then write to the frame.
		// Return true if successful. False if no image is available or something went wrong.
		virtual shared_ptr<Frame> getNextImage(void) = 0;

		// Independent of any image, transform to world coordinates in meters from pixels.
		virtual bool toXYZ(float row, float col, float disp, float* x, float* y, float* z) = 0;

		// Independent of any image, transform from world coordinates in meters to pixels.
		virtual bool toRCD(float x, float y, float z, float* row, float* col, float* disp) = 0;

		// Independent of any image, gives the expected depth of a ground pixel at the given row.
		virtual float groundPixelDepthAtRow(float row) {return 0.1f;}

		// Independent of any image, gives the expected width of a ground pixel at the given row.
		virtual float groundPixelWidthAtRow(float row) {return 0.1f;}

	protected:
		// Subclasses should set if setup already done.
		bool setupDone;
		bool pointCloudNeeded;
		float AngleFromVertical;
	private:
		const char *readFilename, *writeFilename, *directory;
		
		// Prevent copy construction and assignment
        Camera(const Camera &other);
        Camera & operator = (const Camera &other);
	};
/*
    struct CameraTriclopsInternals;

	class CameraTriclops : Camera {
	public:
		CameraTriclops(const char *id);
        bool getNextImage(Frame &frm);
        bool ensureSetupCamera();
        void toXYZ(float row, float col, float disp, float* x, float* y, float* z);
        void toRCD(float x, float y, float z, float* row, float* col, float* disp);
	private:
		CameraTriclopsInternals *internals;
	};
*/
    struct CameraVidereInternals;

	class CameraVidere : public Camera {
	public:
		CameraVidere(const char *id);
		~CameraVidere();
        shared_ptr<Frame> getNextImage(void);
        bool ensureSetupCamera();
        bool toXYZ(float row, float col, float disp, float* x, float* y, float* z);
        bool toRCD(float x, float y, float z, float* row, float* col, float* disp);
		void changeTransformPitch(double pitch);

	private:
        const char *streamName();
        struct CameraVidereInternals *internals;
	};
}

#endif