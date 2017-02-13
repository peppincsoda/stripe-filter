#include "sf_camera.h"

#include <sfcore.h>

#include <pylonc/PylonC.h>

#include <string.h>
#include <assert.h>

#define NUM_BUFFERS 2

#define GOTO_ERROR_IF_FAILED(res) { if ((res) != GENAPI_E_OK) { log_pylon_error (res); goto error; } }

struct _SfCamera
{
  PYLON_DEVICE_HANDLE         hDev;                     /* Handle for the pylon camera device. */
  PYLON_STREAMGRABBER_HANDLE  hGrabber;                 /* Handle for the pylon stream grabber. */
  PYLON_WAITOBJECT_HANDLE     hWait;                    /* Handle used for waiting for a grab to be finished. */
  uint8_t                    *buffers[NUM_BUFFERS];     /* Buffers used for grabbing. */
  PYLON_STREAMBUFFER_HANDLE   bufHandles[NUM_BUFFERS];  /* Handles for the buffers. */
};

static void
log_pylon_error (GENAPIC_RESULT res)
{
  char *message;
  size_t length;

  /* Get the last error message from GenApi. */
  GenApiGetLastErrorMessage (NULL, &length);
  message = (char *) sf_malloc (length);
  GenApiGetLastErrorMessage (message, &length);
  sf_log_error ("%s (#%08x)", message, (unsigned int) res);
  sf_clear_pointer (&message, sf_free);

  /* Get also the details of the error. */
  GenApiGetLastErrorDetail (NULL, &length);
  message = (char *) sf_malloc (length);
  GenApiGetLastErrorDetail (message, &length);
  sf_log_error ("%s", message);
  sf_clear_pointer (&message, sf_free);
}

static char *
pylon_device_get_model_name (PYLON_DEVICE_HANDLE hDev)
{
  GENAPIC_RESULT res;
  char *buf = NULL;
  size_t buf_len;

  if (!PylonDeviceFeatureIsReadable (hDev, "DeviceModelName")) {
    sf_log_error ("DeviceModelName is not readable");
    goto error;
  }
  res = PylonDeviceFeatureToString (hDev, "DeviceModelName", NULL, &buf_len);
  GOTO_ERROR_IF_FAILED (res);
  buf = (char *) sf_malloc (buf_len);
  res = PylonDeviceFeatureToString (hDev, "DeviceModelName", buf, &buf_len);
  GOTO_ERROR_IF_FAILED (res);

  return buf;

error:
  sf_clear_pointer (&buf, sf_free);

  return NULL;
}

static GENAPIC_RESULT
sf_camera_create_device (PYLON_DEVICE_HANDLE *phDev)
{
  GENAPIC_RESULT res;
  size_t numDevices;    /* Number of available devices. */

  /* Enumerate all camera devices. You must call
     PylonEnumerateDevices() before creating a device. */
  res = PylonEnumerateDevices (&numDevices);
  GOTO_ERROR_IF_FAILED (res);
  if (numDevices == 0) {
    sf_log_error ("No camera devices found");
    res = GENAPI_E_FAIL;
    goto error;
  }

  /* Get a handle for the first device found.  */
  res = PylonCreateDeviceByIndex (0, phDev);
  GOTO_ERROR_IF_FAILED (res);

  /* Before using the device, it must be opened. Open it for configuring
     parameters and for grabbing images. */
  res = PylonDeviceOpen (*phDev, PYLONC_ACCESS_MODE_CONTROL | PYLONC_ACCESS_MODE_STREAM);
  GOTO_ERROR_IF_FAILED (res);

  /* Print out the name of the camera we are using. */
  {
    char *modelName = NULL;
    if ((modelName = pylon_device_get_model_name (*phDev)) == NULL) {
      res = GENAPI_E_FAIL;
      goto error;
    }
    sf_log_info ("Using camera %s", modelName);
    sf_clear_pointer (&modelName, sf_free);
  }
  
  /* Set the pixel format to Mono8, where gray values will be output as 8 bit values for each pixel. */
  /* ... Check first to see if the device supports the Mono8 format. */
  if (!PylonDeviceFeatureIsAvailable (*phDev, "EnumEntry_PixelFormat_Mono8")) {
    sf_log_error ("Device doesn't support the Mono8 pixel format");
    res = GENAPI_E_FAIL;
    goto error;
  }
  /* ... Set the pixel format to Mono8. */
  res = PylonDeviceFeatureFromString (*phDev, "PixelFormat", "Mono8");
  GOTO_ERROR_IF_FAILED (res);

  /* Disable acquisition start trigger if available. */
  if (PylonDeviceFeatureIsAvailable (*phDev, "EnumEntry_TriggerSelector_AcquisitionStart")) {
    res = PylonDeviceFeatureFromString (*phDev, "TriggerSelector", "AcquisitionStart");
    GOTO_ERROR_IF_FAILED (res);
    res = PylonDeviceFeatureFromString (*phDev, "TriggerMode", "Off");
    GOTO_ERROR_IF_FAILED (res);
  }

  /* Disable frame burst start trigger if available. */
  if (PylonDeviceFeatureIsAvailable (*phDev, "EnumEntry_TriggerSelector_FrameBurstStart")) {
    res = PylonDeviceFeatureFromString (*phDev, "TriggerSelector", "FrameBurstStart");
    GOTO_ERROR_IF_FAILED (res);
    res = PylonDeviceFeatureFromString (*phDev, "TriggerMode", "Off");
    GOTO_ERROR_IF_FAILED (res);
  }

  /* Disable frame start trigger if available. */
  if (PylonDeviceFeatureIsAvailable (*phDev, "EnumEntry_TriggerSelector_FrameStart")) {
    res = PylonDeviceFeatureFromString (*phDev, "TriggerSelector", "FrameStart");
    GOTO_ERROR_IF_FAILED (res);
    res = PylonDeviceFeatureFromString (*phDev, "TriggerMode", "Off");
    GOTO_ERROR_IF_FAILED (res);
  }

  /* We will use the Continuous frame acquisition mode, i.e., the camera delivers
     images continuously. */
  res = PylonDeviceFeatureFromString (*phDev, "AcquisitionMode", "Continuous");
  GOTO_ERROR_IF_FAILED (res);

  /* For GigE cameras, increasing the packet size is recommended for better
     performance. When the network adapter supports jumbo frames, the packet
     size shall be set to a value > 1500, e.g., to 8192. Here, we only set the packet size
     to 1500. */
  if (PylonDeviceFeatureIsWritable (*phDev, "GevSCPSPacketSize")) {
    res = PylonDeviceSetIntegerFeature (*phDev, "GevSCPSPacketSize", 1500);
    GOTO_ERROR_IF_FAILED (res);
  }

  return GENAPI_E_OK;

error:
  return res;
}

static GENAPIC_RESULT
sf_camera_destroy_device (PYLON_DEVICE_HANDLE hDev)
{
  GENAPIC_RESULT res;

  /* ... Close and release the pylon device. The stream grabber becomes invalid
     after closing the pylon device. Don't call stream grabber related methods after
     closing or releasing the device. */
  res = PylonDeviceClose (hDev);
  GOTO_ERROR_IF_FAILED (res);

  /* ...The device is no longer used, destroy it. */
  res = PylonDestroyDevice (hDev);
  GOTO_ERROR_IF_FAILED (res);

  return GENAPI_E_OK;

error:
  return res;
}

static void
sf_camera_shutdown (SfCamera *camera)
{
  size_t i;

  /* ... Shut down the pylon runtime system. Don't call any pylon method after
     calling PylonTerminate(). */
  PylonTerminate();

  for (i = 0; i < NUM_BUFFERS; ++i) {
    sf_clear_pointer (&camera->buffers[i], sf_free);
  }

  sf_free (camera);
}

SfCamera *
sf_camera_new (void)
{
  SfCamera       *camera;
  GENAPIC_RESULT  res;
  int32_t         payloadSize;    /* Size of an image frame in bytes. */
  size_t          nStreams;       /* The number of streams the device provides. */
  size_t          i;

  camera = (SfCamera *) sf_malloc (sizeof (SfCamera));
  memset (camera, 0, sizeof (*camera));

  /* Before using any pylon methods, the pylon runtime must be initialized. */
  res = PylonInitialize ();
  GOTO_ERROR_IF_FAILED (res);

  res = sf_camera_create_device (&camera->hDev);
  if (res != GENAPI_E_OK)
    goto error;

  /* Determine the required size of the grab buffer. */
  res = PylonDeviceGetIntegerFeatureInt32 (camera->hDev, "PayloadSize", &payloadSize);
  GOTO_ERROR_IF_FAILED (res);

  /* Image grabbing is done using a stream grabber.
     A device may be able to provide different streams. A separate stream grabber must
     be used for each stream. Here, we create a stream grabber for the default
     stream, i.e., the first stream ( index == 0 ).
    */

  /* Get the number of streams supported by the device and the transport layer. */
  res = PylonDeviceGetNumStreamGrabberChannels (camera->hDev, &nStreams);
  GOTO_ERROR_IF_FAILED (res);
  if (nStreams < 1) {
    sf_log_error ("The transport layer doesn't support image streams");
    goto error;
  }

  /* Create and open a stream grabber for the first channel. */
  res = PylonDeviceGetStreamGrabber (camera->hDev, 0, &camera->hGrabber);
  GOTO_ERROR_IF_FAILED (res);
  res = PylonStreamGrabberOpen (camera->hGrabber);
  GOTO_ERROR_IF_FAILED (res);

  /* Get a handle for the stream grabber's wait object. The wait object
     allows waiting for buffers to be filled with grabbed data. */
  res = PylonStreamGrabberGetWaitObject (camera->hGrabber, &camera->hWait);
  GOTO_ERROR_IF_FAILED (res);

  /* Allocate memory for grabbing.  */
  for (i = 0; i < NUM_BUFFERS; ++i) {
    camera->buffers[i] = (uint8_t *) sf_malloc (payloadSize);
    if (camera->buffers[i] == NULL) {
      sf_log_error ("Out of memory");
      goto error;
    }
  }

  /* We must tell the stream grabber the number and size of the buffers
     we are using. */
  /* .. We will not use more than NUM_BUFFERS for grabbing. */
  res = PylonStreamGrabberSetMaxNumBuffer (camera->hGrabber, NUM_BUFFERS);
  GOTO_ERROR_IF_FAILED (res);
  /* .. We will not use buffers bigger than payloadSize bytes. */
  res = PylonStreamGrabberSetMaxBufferSize (camera->hGrabber, payloadSize);
  GOTO_ERROR_IF_FAILED (res);

  /* Allocate the resources required for grabbing. After this, critical parameters
     that impact the payload size must not be changed until FinishGrab() is called. */
  res = PylonStreamGrabberPrepareGrab (camera->hGrabber);
  GOTO_ERROR_IF_FAILED (res);

  /* Before using the buffers for grabbing, they must be registered at
     the stream grabber. For each registered buffer, a buffer handle
     is returned. After registering, these handles are used instead of the
     raw pointers. */
  for (i = 0; i < NUM_BUFFERS; ++i) {
    res = PylonStreamGrabberRegisterBuffer (camera->hGrabber, camera->buffers[i], payloadSize, &camera->bufHandles[i]);
    GOTO_ERROR_IF_FAILED (res);
  }

  /* Feed the buffers into the stream grabber's input queue. For each buffer, the API
     allows passing in a pointer to additional context information. This pointer
     will be returned unchanged when the grab is finished. Here, we use the index of the
     buffer as context information. */
  for (i = 0; i < NUM_BUFFERS; ++i) {
    res = PylonStreamGrabberQueueBuffer (camera->hGrabber, camera->bufHandles[i], (void *) i);
    GOTO_ERROR_IF_FAILED (res);
  }

  /* Now the stream grabber is prepared. As soon as the camera starts to acquire images,
     the image data will be grabbed into the buffers provided.  */

  /* Let the camera acquire images. */
  res = PylonDeviceExecuteCommandFeature (camera->hDev, "AcquisitionStart");
  GOTO_ERROR_IF_FAILED (res);

  return camera;

error:
  sf_camera_shutdown (camera);

  return NULL;
}

void
sf_camera_free (SfCamera *camera)
{
  GENAPIC_RESULT    res;
  PylonGrabResult_t grabResult;
  _Bool             isReady;
  size_t            i;

  /* Clean up. */

  /*  ... Stop the camera. */
  res = PylonDeviceExecuteCommandFeature (camera->hDev, "AcquisitionStop");
  GOTO_ERROR_IF_FAILED (res);

  /* ... We must issue a cancel call to ensure that all pending buffers are put into the
     stream grabber's output queue. */
  res = PylonStreamGrabberCancelGrab (camera->hGrabber);
  GOTO_ERROR_IF_FAILED (res);

  /* ... The buffers can now be retrieved from the stream grabber. */
  do {
    res = PylonStreamGrabberRetrieveResult (camera->hGrabber, &grabResult, &isReady);
    GOTO_ERROR_IF_FAILED (res);
  } while (isReady);

  /* ... When all buffers have been retrieved from the stream grabber, they can be deregistered.
         After that, it is safe to free the memory. */
  for (i = 0; i < NUM_BUFFERS; ++i) {
    res = PylonStreamGrabberDeregisterBuffer (camera->hGrabber, camera->bufHandles[i]);
    GOTO_ERROR_IF_FAILED (res);
  }

  /* ... Release grabbing related resources. */
  res = PylonStreamGrabberFinishGrab (camera->hGrabber);
  GOTO_ERROR_IF_FAILED (res);

  /* After calling PylonStreamGrabberFinishGrab(), parameters that impact the payload size (e.g.,
  the AOI width and height parameters) are unlocked and can be modified again. */

  /* ... Close the stream grabber. */
  res = PylonStreamGrabberClose (camera->hGrabber);
  GOTO_ERROR_IF_FAILED (res);

  res = sf_camera_destroy_device (camera->hDev);
  if (res != GENAPI_E_OK)
    goto error;

error:
  sf_camera_shutdown (camera);
}

SfCameraGrabStatus
sf_camera_grab_image (SfCamera *camera,
                      SfCameraFrameFn frame_fn,
                      void *context)
{
  GENAPIC_RESULT     res;
  PylonGrabResult_t  grabResult;   /* Stores the result of a grab operation. */
  _Bool              isReady;
  size_t             bufferIndex;  /* Index of the buffer */
  SfCameraGrabStatus status;

  /* Wait for the next buffer to be filled. Wait up to 5000 ms. */
  res = PylonWaitObjectWait (camera->hWait, 5000, &isReady);
  GOTO_ERROR_IF_FAILED (res);
  if (!isReady) {
    /* Timeout occurred. */
    sf_log_error ("Grab timeout occurred");
    return SfCameraGrabStatus_Timeout;
  }

  /* Since the wait operation was successful, the result of at least one grab
     operation is available. Retrieve it. */
  res = PylonStreamGrabberRetrieveResult (camera->hGrabber, &grabResult, &isReady);
  GOTO_ERROR_IF_FAILED (res);
  if (!isReady) {
    /* Oops. No grab result available? We should never have reached this point.
       Since the wait operation above returned without a timeout, a grab result
       should be available. */
    sf_log_error ("Failed to retrieve a grab result");
    return SfCameraGrabStatus_Failed;
  }

  /* Get the buffer index from the context information. */
  bufferIndex = (size_t) grabResult.Context;

  /* Check to see if the image was grabbed successfully. */
  if (grabResult.Status == Grabbed) {
    /*  Success. Perform image processing. Since we passed more than one buffer
    to the stream grabber, the remaining buffers are filled while
    we do the image processing. The processed buffer won't be touched by
    the stream grabber until we pass it back to the stream grabber. */
    
    SfCameraGrabResult result;
    result.width = grabResult.SizeX;
    result.height = grabResult.SizeY;
    result.stride = grabResult.SizeX + grabResult.PaddingX;
    result.buffer = camera->buffers[bufferIndex];

    (*frame_fn) (context, &result);

    status = SfCameraGrabStatus_Grabbed;

#ifdef GENAPIC_WIN_BUILD
    /* Display image */
    res = PylonImageWindowDisplayImageGrabResult (0, &grabResult);
    if (res != GENAPI_E_OK) {
      log_pylon_error (res);
      status = SfCameraGrabStatus_Failed;
    }
#endif
  }
  else if (grabResult.Status == Failed) {
    sf_log_error ("Frame wasn't grabbed successfully.  Error code = 0x%08X\n",
      grabResult.ErrorCode);

    status = SfCameraGrabStatus_Failed;
  }

  /* Once finished with the processing, requeue the buffer to be filled again. */
  res = PylonStreamGrabberQueueBuffer (camera->hGrabber, grabResult.hBuffer, (void *) bufferIndex);
  GOTO_ERROR_IF_FAILED (res);

  return status;

error:
  return SfCameraGrabStatus_Failed;
}
