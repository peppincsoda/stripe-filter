
#include <Windows.h>

#include <stdint.h>
#include <string.h>
#include <assert.h>

#define PYLONC_API __declspec(dllexport)
#define GENAPIC_RESULT HRESULT
#define PYLONC_CC _stdcall
#define PINVOKE
#define PINVOKE_DOC
#define RETVAL_PAR
#define PINVOKE_RETVAL_PAR
#define STRING_PAR

typedef unsigned char _Bool;
typedef void *PYLON_DEVICE_HANDLE;
typedef void *PYLON_STREAMGRABBER_HANDLE;
typedef void *PYLON_WAITOBJECT_HANDLE;
typedef void *PYLON_STREAMBUFFER_HANDLE;

typedef enum
{
    waitex_timeout    =  0, /*!< \brief The time-out interval elapsed */
    waitex_signaled   =  1, /*!< \brief The wait operation completed successfully */
    waitex_abandoned  =  2, /*!< \brief Windows only (see MSDN for more information) */
    waitex_alerted    = -1 /*!< \brief The wait was interrupted (Windows: queued APC or I/O completion routine; UNIX: signal) */
} EPylonWaitExResult;

/**
 * \brief The grab status.
 */
typedef enum
{
    UndefinedGrabStatus = -1,   /*!< \brief status not defined */
    Idle,                       /*!< \brief currently not used */
    Queued,                     /*!< \brief in the input queue */
    Grabbed,                    /*!< \brief filled with data */
    Canceled,                   /*!< \brief request was canceled */
    Failed                      /*!< \brief request failed */
} EPylonGrabStatus;

/**
  * \brief PayloadType for PylonGrabResult_t
  */
typedef enum
{
    PayloadType_Undefined = -1,          /*!< \brief The buffer content is undefined. */
    PayloadType_Image,                   /*!< \brief The buffer contains image data. */
    PayloadType_RawData,                 /*!< \brief The buffer contains raw data. */
    PayloadType_File,                    /*!< \brief The buffer contains file data. */
    PayloadType_ChunkData,               /*!< \brief The buffer contains chunk data. */
    PayloadType_DeviceSpecific = 0x8000  /*!< \brief The buffer contains device specific data. */
} EPylonPayloadType;

#define PIXEL_MONO          0x01000000
#define PIXEL_COLOR         0x02000000
#define PIXEL_BIT_COUNT(n)  ((n) << 16)

/*! \brief List all possible pixel formats. See the camera User's Manual for a detailed description of the available pixel formats. */
typedef enum
{
    PixelType_Undefined         = -1, /*!< \brief Indicates that the pixel format is undefined or not valid. */
    PixelType_Mono8             = PIXEL_MONO | PIXEL_BIT_COUNT(8)  | 0x0001, /*!< \brief Indicates the Mono 8 pixel format. */
    PixelType_Mono8signed       = PIXEL_MONO | PIXEL_BIT_COUNT(8)  | 0x0002, /*!< \brief Indicates the Mono 8 Signed pixel format. */
    PixelType_Mono10            = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x0003, /*!< \brief Indicates the Mono 10 pixel format. */
    PixelType_Mono10packed      = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x0004, /*!< \brief Indicates the Mono 10 Packed pixel format. The memory layout of PixelType_Mono10packed and PixelType_Mono10p is different*/
    PixelType_Mono10p           = PIXEL_MONO | PIXEL_BIT_COUNT(10) | 0x0046, /*!< \brief Indicates the Mono10p pixel format. The memory layout of PixelType_Mono10packed and PixelType_Mono10p is different*/
    PixelType_Mono12            = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x0005, /*!< \brief Indicates the Mono 12 pixel format. */
    PixelType_Mono12packed      = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x0006, /*!< \brief Indicates the Mono 12 Packed pixel format. The memory layout of PixelType_Mono12packed and PixelType_Mono12p is different.*/
    PixelType_Mono12p           = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x0047, /*!< \brief Indicates the Mono12p pixel format. The memory layout of PixelType_Mono12packed and PixelType_Mono12p is different.*/
    PixelType_Mono16            = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x0007, /*!< \brief Indicates the Mono 16 pixel format. */

    PixelType_BayerGR8          = PIXEL_MONO | PIXEL_BIT_COUNT(8)  | 0x0008, /*!< \brief Indicates the Bayer GR 8 pixel format. */
    PixelType_BayerRG8          = PIXEL_MONO | PIXEL_BIT_COUNT(8)  | 0x0009, /*!< \brief Indicates the Bayer RG 8 pixel format. */
    PixelType_BayerGB8          = PIXEL_MONO | PIXEL_BIT_COUNT(8)  | 0x000a, /*!< \brief Indicates the Bayer GB 8 pixel format. */
    PixelType_BayerBG8          = PIXEL_MONO | PIXEL_BIT_COUNT(8)  | 0x000b, /*!< \brief Indicates the Bayer BG 8 pixel format. */

    PixelType_BayerGR10         = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x000c, /*!< \brief Indicates the Bayer GR 10 pixel format. */
    PixelType_BayerRG10         = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x000d, /*!< \brief Indicates the Bayer RG 10 pixel format. */
    PixelType_BayerGB10         = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x000e, /*!< \brief Indicates the Bayer GB 10 pixel format. */
    PixelType_BayerBG10         = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x000f, /*!< \brief Indicates the Bayer BG 10 pixel format. */

    PixelType_BayerGR12         = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x0010, /*!< \brief Indicates the Bayer GR 12 pixel format. */
    PixelType_BayerRG12         = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x0011, /*!< \brief Indicates the Bayer RG 12 pixel format. */
    PixelType_BayerGB12         = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x0012, /*!< \brief Indicates the Bayer GB 12 pixel format. */
    PixelType_BayerBG12         = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x0013, /*!< \brief Indicates the Bayer BG 12 pixel format. */

    PixelType_RGB8packed        = PIXEL_COLOR | PIXEL_BIT_COUNT(24) | 0x0014, /*!< \brief Indicates the RGB 8 Packed pixel format. */
    PixelType_BGR8packed        = PIXEL_COLOR | PIXEL_BIT_COUNT(24) | 0x0015, /*!< \brief Indicates the BGR 8 Packed pixel format. */

    PixelType_RGBA8packed       = PIXEL_COLOR | PIXEL_BIT_COUNT(32) | 0x0016, /*!< \brief Indicates the RGBA 8 Packed pixel format. */
    PixelType_BGRA8packed       = PIXEL_COLOR | PIXEL_BIT_COUNT(32) | 0x0017, /*!< \brief Indicates the BGRA 8 Packed pixel format. */

    PixelType_RGB10packed       = PIXEL_COLOR | PIXEL_BIT_COUNT(48) | 0x0018, /*!< \brief Indicates the RGB 10 Packed pixel format. */
    PixelType_BGR10packed       = PIXEL_COLOR | PIXEL_BIT_COUNT(48) | 0x0019, /*!< \brief Indicates the BGR 10 Packed pixel format. */

    PixelType_RGB12packed       = PIXEL_COLOR | PIXEL_BIT_COUNT(48) | 0x001a, /*!< \brief Indicates the RGB 12 Packed pixel format. */
    PixelType_BGR12packed       = PIXEL_COLOR | PIXEL_BIT_COUNT(48) | 0x001b, /*!< \brief Indicates the BGR 12 Packed pixel format. */

    PixelType_RGB16packed       = PIXEL_COLOR | PIXEL_BIT_COUNT(48) | 0x0033, /*!< \brief Indicates the RGB 16 Packed pixel format. */

    PixelType_BGR10V1packed     = PIXEL_COLOR | PIXEL_BIT_COUNT(32) | 0x001c, /*!< \brief Indicates the BGR 10 V1 Packed pixel format. */
    PixelType_BGR10V2packed     = PIXEL_COLOR | PIXEL_BIT_COUNT(32) | 0x001d, /*!< \brief Indicates the BGR 10 V2 Packed pixel format. */

    PixelType_YUV411packed      = PIXEL_COLOR | PIXEL_BIT_COUNT(12) | 0x001e, /*!< \brief Indicates the YUV 411 Packed pixel format. */
    PixelType_YUV422packed      = PIXEL_COLOR | PIXEL_BIT_COUNT(16) | 0x001f, /*!< \brief Indicates the YUV 422 Packed pixel format. */
    PixelType_YUV444packed      = PIXEL_COLOR | PIXEL_BIT_COUNT(24) | 0x0020, /*!< \brief Indicates the YUV 444 Packed pixel format. */

    PixelType_RGB8planar        = PIXEL_COLOR | PIXEL_BIT_COUNT(24) | 0x0021, /*!< \brief Indicates the RGB 8 Planar pixel format. */
    PixelType_RGB10planar       = PIXEL_COLOR | PIXEL_BIT_COUNT(48) | 0x0022, /*!< \brief Indicates the RGB 10 Planar pixel format. */
    PixelType_RGB12planar       = PIXEL_COLOR | PIXEL_BIT_COUNT(48) | 0x0023, /*!< \brief Indicates the RGB 12 Planar pixel format. */
    PixelType_RGB16planar       = PIXEL_COLOR | PIXEL_BIT_COUNT(48) | 0x0024, /*!< \brief Indicates the RGB 16 Planar pixel format. */

    PixelType_YUV422_YUYV_Packed = PIXEL_COLOR | PIXEL_BIT_COUNT(16) | 0x0032, /*!< \brief Indicates the YUV 422 (YUYV) Packed pixel format. */

    PixelType_BayerGR12Packed = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x002A, /*!< \brief Indicates the Bayer GR 12 Packed pixel format. The memory layout of PixelType_BayerGR12Packed and PixelType_BayerGR12p is different.*/
    PixelType_BayerRG12Packed = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x002B, /*!< \brief Indicates the Bayer RG 12 Packed pixel format. The memory layout of PixelType_BayerRG12Packed and PixelType_BayerRG12p is different.*/
    PixelType_BayerGB12Packed = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x002C, /*!< \brief Indicates the Bayer GB 12 Packed pixel format. The memory layout of PixelType_BayerGB12Packed and PixelType_BayerGB12p is different.*/
    PixelType_BayerBG12Packed = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x002D, /*!< \brief Indicates the Bayer BG 12 Packed pixel format. The memory layout of PixelType_BayerBG12Packed and PixelType_BayerBG12p is different.*/

    PixelType_BayerGR10p      = PIXEL_MONO | PIXEL_BIT_COUNT(10) | 0x0056,  /*!< \brief Indicates the BayerGR10p pixel format. */
    PixelType_BayerRG10p      = PIXEL_MONO | PIXEL_BIT_COUNT(10) | 0x0058,  /*!< \brief Indicates the BayerRG10p pixel format. */
    PixelType_BayerGB10p      = PIXEL_MONO | PIXEL_BIT_COUNT(10) | 0x0054,  /*!< \brief Indicates the BayerGB10p pixel format. */
    PixelType_BayerBG10p      = PIXEL_MONO | PIXEL_BIT_COUNT(10) | 0x0052,  /*!< \brief Indicates the BayerBG10p pixel format. */

    PixelType_BayerGR12p      = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x0057, /*!< \brief Indicates the BayerGR12p pixel format. The memory layout of PixelType_BayerGR12Packed and PixelType_BayerGR12p is different.*/
    PixelType_BayerRG12p      = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x0059, /*!< \brief Indicates the BayerRG12p pixel format. The memory layout of PixelType_BayerRG12Packed and PixelType_BayerRG12p is different.*/
    PixelType_BayerGB12p      = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x0055, /*!< \brief Indicates the BayerGB12p pixel format. The memory layout of PixelType_BayerGB12Packed and PixelType_BayerGB12p is different.*/
    PixelType_BayerBG12p      = PIXEL_MONO | PIXEL_BIT_COUNT(12) | 0x0053, /*!< \brief Indicates the BayerBG12p pixel format. The memory layout of PixelType_BayerBG12Packed and PixelType_BayerBG12p is different.*/

    PixelType_BayerGR16       = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x002E, /*!< \brief Indicates the Bayer GR 16 pixel format. */
    PixelType_BayerRG16       = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x002F, /*!< \brief Indicates the Bayer RG 16 pixel format. */
    PixelType_BayerGB16       = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x0030, /*!< \brief Indicates the Bayer GB 16 pixel format. */
    PixelType_BayerBG16       = PIXEL_MONO | PIXEL_BIT_COUNT(16) | 0x0031, /*!< \brief Indicates the Bayer BG 16 pixel format. */

    PixelType_RGB12V1packed   = PIXEL_COLOR | PIXEL_BIT_COUNT(36) | 0X0034, /*!< \brief Indicates the RGB 12 V1 packed pixel format. */

    /* custom codes */
    PixelType_Double            =  0x80000000 | PIXEL_MONO | PIXEL_BIT_COUNT(48) | 0x100 /*!< \brief Indicates the Double pixel format. */
} EPylonPixelType;

typedef struct tag_PylonGrabResult_t
{
    const void *Context;                /**< Application specific context specified when calling PylonStreamGrabberRegisterBuffer() */
    PYLON_STREAMBUFFER_HANDLE hBuffer;  /**< Data buffer handle returned when calling PylonStreamGrabberRegisterBuffer() */
    const void *pBuffer;                /**< Pointer to the raw data */
    EPylonGrabStatus Status;            /**< Result status. check this to check whether the result contains valid data. */
    unsigned int FrameNr;               /**< Deprecated: FrameNr has been deprecated. Use BlockID instead. */
    EPylonPayloadType PayloadType;      /**< Type of data contained in buffer */
    EPylonPixelType PixelType;          /**< Pixel type (if the result contains pixel data) */
    uint64_t TimeStamp;                 /**< Time stamp generated by the device when filling the result (optional)*/
    int SizeX;                          /**< Number of columns, in pixels (only if payload is image data) */
    int SizeY;                          /**< Number of rows, in pixels (only if payload is image data) */
    int OffsetX;                        /**< Starting column, in pixels (only if payload is image data) */
    int OffsetY;                        /**< Starting row, in pixels (only if payload is image data) */
    int PaddingX;                       /**< Number of extra data bytes at end of row (only if payload is image data) */
    int PaddingY;                       /**< Number of extra data bytes at end of image (only if payload is image data) */
    uint64_t PayloadSize;               /**< Total payload size, in bytes, pointed to by pBuffer */
    unsigned int ErrorCode;             /**< Additional error code in case Status is gst_Failed (optional) */
    uint64_t BlockID;                   /**< The block ID of the grabbed frame (camera device specific).
                                            \par IEEE 1394 Camera Devices
                                            The value of block ID is always UINT64_MAX.

                                            \par GigE Camera Devices
                                            The sequence number starts with 1 and
                                            wraps at 65535. The value 0 has a special meaning and indicates
                                            that this feature is not supported by the camera.

                                            \par USB Camera Devices
                                            The sequence number starts with 0 and uses the full 64 Bit range.

                                            \attention A block ID of value UINT64_MAX indicates that the Block ID is invalid and must not be used. */
} PylonGrabResult_t;

static _Bool isGrabberPrepared = 0;

static HRESULT
ReturnStringToBuffer(const char* s, char* pBuf, size_t* pBufLen)
{
  size_t buf_len;

  assert (pBufLen != NULL);

  buf_len = strlen (s) + 1;

  if (pBuf == NULL) {
    *pBufLen = buf_len;
    return S_OK;
  } else if (*pBufLen < buf_len) {
    *pBufLen = buf_len;
    return E_NOT_SUFFICIENT_BUFFER;
  } else {
    strcpy_s (pBuf, *pBufLen, s);
    return S_OK;
  }
}

PYLONC_API GENAPIC_RESULT PYLONC_CC
PylonInitialize(void)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC
PylonTerminate(void)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC
GenApiGetLastErrorMessage( char* pBuf, size_t *pBufLen )
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC
GenApiGetLastErrorDetail( char* pBuf, size_t *pBufLen )
{ return S_OK; }

PYLONC_API _Bool PYLONC_CC
PylonDeviceFeatureIsAvailable(PYLON_DEVICE_HANDLE hDev, const char* pName)
{ return 1; }

PYLONC_API _Bool PYLONC_CC
PylonDeviceFeatureIsReadable(PYLON_DEVICE_HANDLE hDev, const char* pName)
{ return 1; }

PYLONC_API _Bool PYLONC_CC
PylonDeviceFeatureIsWritable(PYLON_DEVICE_HANDLE hDev, const char* pName)
{ return 1; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonDeviceFeatureFromString(PYLON_DEVICE_HANDLE hDev, STRING_PAR const char* pName, STRING_PAR const char* pValue)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC
PylonDeviceFeatureToString(PYLON_DEVICE_HANDLE hDev, const char *pName, char* pBuf, size_t* pBufLen)
{
  return ReturnStringToBuffer ("FeatureString", pBuf, pBufLen);
}

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonDeviceSetIntegerFeature(PYLON_DEVICE_HANDLE hDev, const char *pName, int64_t value)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonEnumerateDevices(RETVAL_PAR size_t *numDevices)
{
  assert (numDevices != NULL);

  *numDevices = 0;

  return S_OK;
}

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonCreateDeviceByIndex( size_t index, RETVAL_PAR PYLON_DEVICE_HANDLE *phDev)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonDeviceOpen(PYLON_DEVICE_HANDLE hDev, int accessMode)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonDeviceClose(PYLON_DEVICE_HANDLE hDev)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE_DOC
PylonDestroyDevice(PYLON_DEVICE_HANDLE hDev)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC
PylonDeviceGetIntegerFeatureInt32(PYLON_DEVICE_HANDLE dev, const char *name, int32_t *value)
{
  assert (name != NULL);
  assert (value != NULL);

  if (_strcmpi (name, "PayloadSize") == 0) {
    *value = 16 * 1024;
    return S_OK;
  }

  return E_FAIL;
}

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonDeviceGetNumStreamGrabberChannels(PYLON_DEVICE_HANDLE hDev, RETVAL_PAR size_t *pNumChannels)
{
  assert (pNumChannels != NULL);

  *pNumChannels = 1;

  return S_OK;
}

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonDeviceGetStreamGrabber(PYLON_DEVICE_HANDLE hDev, size_t index, RETVAL_PAR PYLON_STREAMGRABBER_HANDLE *phStg)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonStreamGrabberOpen(PYLON_STREAMGRABBER_HANDLE hStg)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonStreamGrabberClose(PYLON_STREAMGRABBER_HANDLE hStg)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonStreamGrabberGetWaitObject(PYLON_STREAMGRABBER_HANDLE hStg, RETVAL_PAR PYLON_WAITOBJECT_HANDLE *phWobj)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonStreamGrabberSetMaxNumBuffer(PYLON_STREAMGRABBER_HANDLE hStg, size_t numBuffers )
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonStreamGrabberSetMaxBufferSize(PYLON_STREAMGRABBER_HANDLE hStg, size_t maxSize )
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonStreamGrabberPrepareGrab(PYLON_STREAMGRABBER_HANDLE hStg)
{
  isGrabberPrepared = 1;
  return S_OK;
}

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonStreamGrabberCancelGrab(PYLON_STREAMGRABBER_HANDLE hStg)
{
  isGrabberPrepared = 0;
  return S_OK;
}

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonStreamGrabberFinishGrab(PYLON_STREAMGRABBER_HANDLE hStg)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC
PylonStreamGrabberRegisterBuffer(PYLON_STREAMGRABBER_HANDLE hStg, void* pBuffer, size_t BufLen, PYLON_STREAMBUFFER_HANDLE *phBuf)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonStreamGrabberDeregisterBuffer(PYLON_STREAMGRABBER_HANDLE hStg, PYLON_STREAMBUFFER_HANDLE hBuf)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE_DOC
PylonStreamGrabberRetrieveResult(PYLON_STREAMGRABBER_HANDLE hStg, PylonGrabResult_t * pGrabResult, PINVOKE_RETVAL_PAR _Bool * pReady)
{
  assert (pGrabResult != NULL);
  assert (pReady != NULL);
  
  memset (pGrabResult, 0, sizeof (*pGrabResult));
  pGrabResult->Status = Grabbed;
  pGrabResult->pBuffer = NULL;

  pGrabResult->Context = (const void *) 1;
  pGrabResult->SizeX = 1024;
  pGrabResult->SizeY = 16;

  *pReady = isGrabberPrepared;

  return S_OK;
}

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE_DOC
PylonStreamGrabberQueueBuffer(PYLON_STREAMGRABBER_HANDLE hStg, PYLON_STREAMBUFFER_HANDLE hBuf, const void * pContext)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonDeviceExecuteCommandFeature(PYLON_DEVICE_HANDLE hDev, STRING_PAR const char *pName)
{ return S_OK; }

PYLONC_API GENAPIC_RESULT PYLONC_CC PINVOKE
PylonWaitObjectWait(PYLON_WAITOBJECT_HANDLE hWobj, uint32_t timeout, PINVOKE_RETVAL_PAR _Bool * pResult)
{
  assert (pResult != NULL);

  Sleep (100);

  *pResult = 1;

  return S_OK;
}

PYLONC_API GENAPIC_RESULT PYLONC_CC
PylonImageWindowDisplayImageGrabResult(size_t winIndex, PylonGrabResult_t *grabResult)
{ return S_OK; }
