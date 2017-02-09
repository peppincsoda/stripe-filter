#ifndef _SF_CAMERA_H_
#define _SF_CAMERA_H_

#include <stdint.h>

typedef struct _SfCamera SfCamera;

typedef enum _SfCameraGrabStatus SfCameraGrabStatus;

enum _SfCameraGrabStatus
{
  SfCameraGrabStatus_Grabbed,
  SfCameraGrabStatus_Timeout,
  SfCameraGrabStatus_Failed,
};

typedef struct _SfCameraGrabResult SfCameraGrabResult;

struct _SfCameraGrabResult
{
  int width;
  int height;
  int stride;
  uint8_t *buffer;
};

typedef void (*SfCameraFrameFn) (void *context, SfCameraGrabResult *result);

SfCamera *
sf_camera_new (void);

void
sf_camera_free (SfCamera *camera);

SfCameraGrabStatus
sf_camera_grab_image (SfCamera *camera,
                      SfCameraFrameFn frame_fn,
                      void *context);

#endif  //  _SF_CAMERA_H_
