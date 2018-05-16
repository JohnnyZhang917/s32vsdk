/*****************************************************************************
 *
 * Freescale Confidential Proprietary
 *
 * Copyright (c) 2016 Freescale Semiconductor;
 * Copyright (c) 2016-17 NXP;
 * All Rights Reserved
 *
 *****************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
/**
 * \file     maxim_user.cpp
 * \brief    User-space library calls to access maxim functionality.
 * \author   Tomas Babinec
 * \version  0.1
 * \date     16-December-2015
 * \note
 ****************************************************************************/
#include "s32vs234.h"
#include <stdint.h>
#include <cstring>

#ifndef __STANDALONE__
  #include <fcntl.h>      // open 
  #include <unistd.h>     // exit 
  #include <sys/ioctl.h>  // ioctl 
  #include <sys/mman.h>   // mmap
#else
extern "C"
{
  #include "i2c.h"
  #include "sony_func.h"
}

#endif // else from #ifndef __STANDALONE__

#define VDB_LOG_NOTE_OFF
#define MAXIM

#include "isp_hw.h"
#include "isp_csi.h"
#include "isp_cam_generic.h"
#include "cam_ioctl.h"
#include "isp_cam_maxim.h"
#include "isp_cam_sony.h"
#include "sony_config.h"
#include "ov10640_config.h"
#include "ov10635_config_max.h"
#include "vdb_log.h"

//****************************************************************************
// consts
//****************************************************************************

#define DESERIALIZER_COMPATIBLE_STRING "s32v-max9286"
#define SERIALIZER_COMPATIBLE_STRING   "s32v-max9271"
#define SONY_COMPATIBLE_STRING         "s32v-sony-imx224"
#define OV10640_COMPATIBLE_STRING      "s32v-ov10640"
#define OV490_OV10640_COMPATIBLE_STRING      "s32v-ov490-ov10640"
#define OV10635_COMPATIBLE_STRING      "s32v-ov10635"

#define OV10640_CHIP_ID_HIGH_BYTE 0x300A
#define OV10640_CHIP_ID_LOW_BYTE  0x300B

#define INIT_DELAY 2 // delay before i2c config is attempted       

//****************************************************************************
// macros
//****************************************************************************

#ifdef __STANDALONE__
extern "C"
{
  uint64_t get_uptime_microS(void);
}
  #define msleep(_val) \
    {\
      uint64_t lStart = (uint64_t)get_uptime_microS();\
      while((get_uptime_microS() - lStart) < _val*1000U);\
    }  
#else //#ifdef __STANDALONE__ 
  #define msleep(_msec)(usleep(_msec*1000))
#endif // else from #ifdef __STANDALONE__ 



//****************************************************************************
// types
//****************************************************************************

struct MaximQuadCam
{
  MAXIM_Cfg_t    mCfg;
  CamI2cClient_t mDeserializer;
  CamI2cClient_t mpSerializers[5]; // 0=original&broadcast,1-4=slots
  CamI2cClient_t mpCameras[5];     // 0=original&broadcast,1-4=slots
}; // struct MaximQuadCam

static bool spInitialized[2] = {false};

//****************************************************************************
// global variables
//****************************************************************************
#ifndef __STANDALONE__
  extern int32_t gCamFD;   ///< cam driver file descriptor
#endif //ifndef __STANDALONE__

//*****************************************************************************
// local data
//*****************************************************************************

static  MaximQuadCam spClients[2];

//*****************************************************************************
// functions definitions
//*****************************************************************************

CAM_LIB_RESULT MAXIM_Open(const MAXIM_Cfg_t &arConfig)
{
  CAM_LIB_RESULT lRet    = CAM_LIB_SUCCESS;

  if(CAM_Open() == CAM_LIB_SUCCESS)
  {
    CSI_IDX        lCsiIdx = arConfig.mCsiIdx;
    CamI2cCfg_t    lCamCfg;

    // if not initialized allready
    if(!spInitialized[lCsiIdx])
    {
      // *** prepare default I2C clients (deser/ser/cam) ***
      // deserializer 
      lCamCfg.mI2cBusIdx    = (I2C_BUS_IDX) lCsiIdx;
      lCamCfg.mI2cAddr      = (uint8_t)I2C_MAXIM_DES_ADDR;
      lCamCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_8;

      memcpy(lCamCfg.mpCompatible,
              DESERIALIZER_COMPATIBLE_STRING, 
              strlen(DESERIALIZER_COMPATIBLE_STRING));
      
      spClients[lCsiIdx].mDeserializer = CAM_ClientAdd(lCamCfg);
      
      if(spClients[lCsiIdx].mDeserializer != 0)
      {      
        // serializer
        lCamCfg.mI2cAddr      = I2C_MAX_9271_ADDR(MAXIM_CHNL_ORIG_BC);

        memcpy(lCamCfg.mpCompatible,
                SERIALIZER_COMPATIBLE_STRING, 
                strlen(SERIALIZER_COMPATIBLE_STRING));
        
        spClients[lCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC] = 
          CAM_ClientAdd(lCamCfg);
          
        if(spClients[lCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC] != 0)
        {
          // camera
          lCamCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_16; 
          switch(arConfig.mSensorType)
          {
            case CSI_MAX_OV10640:
            {
              lCamCfg.mI2cAddr =
                     (uint8_t)I2C_MAX_OV10640_ADDR((uint8_t)MAXIM_CHNL_ORIG_BC);
              memcpy(lCamCfg.mpCompatible,
                OV10640_COMPATIBLE_STRING, 
                strlen(OV10640_COMPATIBLE_STRING));
            } // case MAX_OV10640
              break;
			case CSI_MAX9286_96705:
            {
              lCamCfg.mI2cAddr =
                     (uint8_t)I2C_MAX_OV490_ADDR((uint8_t)MAXIM_CHNL_ORIG_BC);
              memcpy(lCamCfg.mpCompatible,
                OV490_OV10640_COMPATIBLE_STRING, 
                strlen(OV490_OV10640_COMPATIBLE_STRING));
            } // case MAX9286_96705
              break;
            case CSI_MAX_SONY224:
            {
              lCamCfg.mI2cAddr =
                      (uint8_t)I2C_MAX_IMX224_ADDR((uint8_t)MAXIM_CHNL_ORIG_BC);
              memcpy(lCamCfg.mpCompatible,
                SONY_COMPATIBLE_STRING, 
                strlen(SONY_COMPATIBLE_STRING));
            } // case MAX_SONY224
              break;
            case CSI_MAX_OV10635:
            {
              lCamCfg.mI2cAddr =
                     (uint8_t)I2C_MAX_OV10635_ADDR((uint8_t)MAXIM_CHNL_ORIG_BC);
              memcpy(lCamCfg.mpCompatible,
                OV10635_COMPATIBLE_STRING, 
                strlen(OV10635_COMPATIBLE_STRING));
            } // case MAX_OV10635
              break;
              
            default:
            {
              VDB_LOG_ERROR("Error: unknown sensor type.\n");
              lRet = CAM_LIB_FAILURE;
            } // default
              break;
          } // switch sensor type

          spClients[lCsiIdx].mpCameras[0] = CAM_ClientAdd(lCamCfg);
          if(spClients[lCsiIdx].mpCameras[0] == 0)
          {
            VDB_LOG_ERROR(
              "Failed to add default %s camera client.\n",
              lCamCfg.mpCompatible         
            );
            lRet = CAM_LIB_FAILURE;
          } // if failed to add camera default client
          else
          {
            spInitialized[lCsiIdx] = true;
          }
        } // if default Serializer added successfully
        else
        {
          VDB_LOG_ERROR("Failed to add default serializer client.\n");
          lRet = CAM_LIB_FAILURE;
        } // else from if default Serializer added successfully
      } // if Deserializer added successfully
      else
      {
        VDB_LOG_ERROR("Failed to add deserializer client.\n");
        lRet = CAM_LIB_FAILURE;
      } // else from if Deserializer added successfully
    } // if not created yet
    else
    {
      VDB_LOG_ERROR("Maxim QuadCam setup already active on CSI idx %u.\n",
                    (uint32_t)lCsiIdx);
      lRet = CAM_LIB_FAILURE;
    } // else if not created yet

    // *** initialize the rest of the HW setup ***
    if(lRet == CAM_LIB_SUCCESS)
    {
      // reinit only if camera type has changed
      if( spClients[lCsiIdx].mCfg.mSensorType != arConfig.mSensorType)
      {
        uint32_t i       = (uint32_t)0;
        uint32_t lCamCnt = (uint32_t)0;
        // get number of cameras used
        for(i = (uint32_t)0; i < (uint32_t)MAXIM_CAM_SLOTS; i++)
        {
          if(arConfig.mSensors[i] == MAXIM_CAM_ENABLED)
          {
            lCamCnt++;
          } // if sensor enabled
        } // for all possible MAXIM sensor slots

        spClients[lCsiIdx].mCfg = arConfig;
        
        switch(arConfig.mSensorType)
        {
          case CSI_MAX_OV10640:
          {
            lRet = MAXIM_OV10640_InitComplex(lCsiIdx, lCamCnt);
          } // case MAX_OV10640
            break;
		  case CSI_MAX9286_96705:
          {
            lRet = MAXIM9286_96705_Init(lCsiIdx, lCamCnt);
          } // case MAX9286_96705
            break;
          case CSI_MAX_SONY224:
          {
#ifndef __STANDALONE__
            int8_t lIoctlRet  = 0;
            lIoctlRet = (int8_t)ioctl(gCamFD,
                              IOCTL_SONY_CONNECT,
                              spClients[lCsiIdx].mpCameras[0]);
            if (0L > lIoctlRet)
            {
              lRet = CAM_LIB_FAILURE;
            }
            else
            {
              lRet = CAM_LIB_SUCCESS;
            }
#else
            SONY_DRV_Setup(spClients[lCsiIdx].mpCameras[0]);
#endif // else #ifndef __STANDALONE__

            if(CAM_LIB_SUCCESS == lRet)
            {
              lRet = MAXIM_SONY_InitComplex(arConfig.mCsiIdx, lCamCnt);
            } // else from if failed
          } // case MAX_SONY224
            break;
          case CSI_MAX_OV10635:
          {
            lRet = MAXIM_OV10635_InitComplex(lCsiIdx, lCamCnt);
            VDB_LOG_WARNING("Ov10635 quad cam not implemented.");
          } // case MAX_OV10635
            break;
            
          default:
          {
            VDB_LOG_ERROR("Unknown sensor type.");
            lRet = CAM_LIB_FAILURE;
          } // default
            break;
        } // switch sensor type    
      } // if camera type has changed
    } // if all ok
  } // if generic camera driver ready
  else
  {
    VDB_LOG_ERROR("Failed to open generic camera driver.\n");
    lRet = CAM_LIB_FAILURE;
  } // else from if generic camera driver ready
    
  return lRet;
} // SONY_Open()

//*****************************************************************************

CAM_LIB_RESULT MAXIM_Close(const enum CSI_IDX aCsiIdx)
{
  CAM_LIB_RESULT lRet = CAM_LIB_SUCCESS; 
  
  // if initialized
  if(spInitialized[aCsiIdx])
  {
    if(CAM_ClientRem(spClients[aCsiIdx].mDeserializer) != CAM_LIB_SUCCESS)
    {
      VDB_LOG_ERROR("Failed to remove the deserializer client.\n");
      lRet = CAM_LIB_FAILURE;
    } // if ClientRem() failed
    else
    {
      spClients[aCsiIdx].mDeserializer = 0;
    } // else from if CLientRem() failed
  
#ifndef __STANDALONE__   
    if (ioctl(gCamFD, 
              IOCTL_SONY_DISCONNECT, 
              spClients[aCsiIdx].mpCameras[0]) < 0) 
#else // #ifndef __STANDALONE__
      //TODO: fixme
    //if(SONY_DRV_Close(spClients[aCsiIdx]) < 0)
#endif // else #ifndef __STANDALONE__
    {
      VDB_LOG_ERROR("Failed to disconnect the camera driver.\n");
      lRet = CAM_LIB_FAILURE;
    } // if failure  
    
    for(uint32_t i = (uint32_t)0; i < (uint32_t)5; i++)
    {
      if(spClients[aCsiIdx].mpSerializers[i] != 0)
      {
        if(CAM_ClientRem(
            spClients[aCsiIdx].mpSerializers[i]) != CAM_LIB_SUCCESS)
        {
          VDB_LOG_ERROR("Failed to remove the seserializer client #%u.\n", i);
          lRet = CAM_LIB_FAILURE;
        } // if ClientRem() failed
        else
        {
          spClients[aCsiIdx].mpSerializers[i] = 0;
        } // else from if CLientRem() failed
      } // if seserializer used
      
      if(spClients[aCsiIdx].mpCameras[i] != 0)
      {
        if(CAM_ClientRem(spClients[aCsiIdx].mpCameras[i]) != CAM_LIB_SUCCESS)
        {
          VDB_LOG_ERROR("Failed to remove the camera client #%u.\n", i);
          lRet = CAM_LIB_FAILURE;
        } // if ClientRem() failed
        else
        {
          spClients[aCsiIdx].mpCameras[i] = 0;
        } // else from if CLientRem() failed
      } // if seserializer used
    } // for all possible quadcam clients
    
    spInitialized[aCsiIdx] = false;
  } // if Deserializer used

  return lRet;
} // MAXIM_Close()

//*****************************************************************************

CAM_LIB_RESULT MAXIM_CsiEnable(const enum CSI_IDX aCsiIdx)
{
  CAM_LIB_RESULT lRet = CAM_LIB_SUCCESS;
  
  switch(spClients[aCsiIdx].mCfg.mSensorType)
  {
    case CSI_MAX_OV10640: // fallthrough
    {
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[0],
                             (uint16_t)0x3012,
                             (uint8_t)0x1);
      // camera streaming ON
      if (CAM_LIB_SUCCESS == lRet)
      {
        lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[0],
                             (uint16_t)0x3012,
                             (uint8_t)0x1);

        msleep((uint32_t)200);
      }
    }  //case CSI_MAX_OV10640
    case CSI_MAX_OV10635: // fallthrough
    {
      uint8_t lVal  = (uint8_t)0;
      uint8_t lVal2 = (uint8_t)0;
      uint16_t lAddr = (uint16_t)((uint16_t)0x30 << 8) + (uint16_t)0x12;
      
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                      lAddr, 
                      (uint8_t)0x1);
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                      lAddr, 
                      (uint8_t)0x1); // camera streaming ON
      msleep((uint32_t)200);
      
/*      printf("start MAXIM OV10640/OV10635\n"); */
      // Enable CSI output , BIT[3] = 1
      lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                      (uint16_t)0x15,  lVal);
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                      (uint16_t)0x15,
                      (uint8_t)(lVal | (uint8_t)0x08));
      msleep((uint32_t)100);
      lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                      (uint16_t)0x15, lVal2);
    } // case MAX_OV10635
      break;
      
    case CSI_MAX_SONY224:
    {
      uint8_t lVal  = (uint8_t)0;

      // Enable CSI output , BIT[3] = 1
      lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                      (uint16_t)0x15, lVal);
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                      (uint16_t)0x15,
                      (uint8_t)(lVal | (uint8_t)0x08));
      msleep((uint32_t)100);
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                      (uint16_t)0x15,
                      (uint8_t)0xF);
    } // case MAX_SONY224
      break;
      
	case CSI_MAX9286_96705:
    {
      printf("start MAXIM9286_96705\n");
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x15, 0x0B);
	  msleep((uint32_t)100);
    } // case MAX9286_96705
      break;
    default:
    {
      VDB_LOG_ERROR("Unknown sensor type.");
    } // default
      break;
  } // switch sensor type    
  
  return lRet;
} // MAXIM_CsiEnable(enum CSI_IDX aCsiIdx)

//*****************************************************************************

CAM_LIB_RESULT MAXIM_CsiDisable(const enum CSI_IDX aCsiIdx)
{
  CAM_LIB_RESULT lRet = CAM_LIB_SUCCESS;
  
  switch(spClients[aCsiIdx].mCfg.mSensorType)
  {
    case CSI_MAX_OV10640:
    {
      uint8_t lVal = (uint8_t)0;
      uint16_t lAddr = ((uint16_t)0x30 << 8) + 0x12;
      
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                             lAddr, 
                             (uint8_t)0x0);
      
      lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                            (uint16_t)0x15, lVal);
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                            (uint16_t)0x15,
                            (uint8_t)(lVal&(uint8_t)0xF7));
    } // case MAX_OV10640
      break;
    case CSI_MAX_SONY224:
    {
      uint8_t lVal = (uint8_t)0;

      // Disable CSI output , BIT[3] = 0
      lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                            (uint16_t)0x15, lVal);
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                            (uint16_t)0x15, lVal&(uint8_t)0xF7); 
    } // case MAX_SONY224
      break;
    case CSI_MAX_OV10635:
    {
      uint8_t lVal = (uint8_t)0;

      // Disable CSI output , BIT[3] = 0
      lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                            (uint16_t)0x15, lVal);
      lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                            (uint16_t)0x15, lVal&(uint8_t)0xF7); 
    } // case MAX_OV10635
      break;
	case CSI_MAX9286_96705:
	  {
		
	   // CAM_ReadSingle(spClients[aCsiIdx].mDeserializer, 0x15, lVal);
	   // CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x15, lVal&0xF7);
	   lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x15, 0x03);
	  } // case MAX9286_MAX96705
		break;
      
    default:
    {
      VDB_LOG_ERROR("Unknown sensor type.");
      lRet = CAM_LIB_FAILURE;
    } // default
      break;
  } // switch sensor type   
    
  msleep((uint32_t)10);
  
  return lRet;
} // MAXIM_CsiDisable(enum CSI_IDX aCsiIdx)

//****************************************************************************

CAM_LIB_RESULT MAXIM_OV10640_InitComplex(const enum CSI_IDX aCsiIdx, 
                                         uint32_t acCamCnt)
{ 
  uint8_t        lVal  = (uint8_t)0;
  CAM_LIB_RESULT lRet  = CAM_LIB_SUCCESS;
  
#ifdef __STANDALONE__  
  VDB_LOG_NOTE("Initialize the I2C\n"); 
  I2C_Init_Config(aCsiIdx,I2C_MAXIM_DES_ADDR);
#endif
     
  VDB_LOG_NOTE("Maxim OV10640 cameras test\n");
  lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x1e, lVal);
  if(lVal != (uint8_t)0x40)
  {
    // deserializer not recognized
    VDB_LOG_NOTE("Deserializer not recognized 0x%x.\n", lVal);
    lRet = CAM_LIB_FAILURE;
    return lRet;
  }
    
  VDB_LOG_NOTE("Deserializer recognized.\n");
  
  VDB_LOG_NOTE("Initialization of Reverse Control Channel Setup ");
  
  // enable all links control channels (FWD+REV)
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x0A,
                        (uint8_t)0xff); 

  /*********************************************************************/
  /*                    Reverse Control Channel Setup                  */
  /*********************************************************************/

  //Enable Custom Reverse Channel & First Pulse Length
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x3F,
                        (uint8_t)0x4F);
  msleep((uint32_t)2); // Wait 2ms
  //Reverse Channel Amplitude to mid level and transition time
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x3B,
                        (uint8_t)0x1E); 
  msleep((uint32_t)2); // Wait 2ms
  //Enable Configuration Link
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                        (uint16_t)0x04, 
                        (uint8_t)0x43); 
  msleep((uint32_t)5); // Wait 5ms
  //Serializer input gain and thresholds
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                        (uint16_t)0x08, 
                        (uint8_t)0x01); 
  msleep((uint32_t)2); // Wait 2ms
  //Reverse Channel Amplitude level
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x3B,
                        (uint8_t)0x19);  
  msleep((uint32_t)2); // Wait 2ms

  VDB_LOG_NOTE("....Completed\n");

  //change the SER I2C speed to 100kHz
#ifndef __STANDALONE__ // use default I2C speed (400kHz)
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                        (uint16_t)0x0D,
                        (uint8_t)0xAE); 
#endif //#ifndef __STANDALONE__
  msleep((uint32_t)100); // Wait 100ms
  
  
  /*********************************************************************/
  /*                   MAX9286 Pre MAX9271 Setup                       */
  /*********************************************************************/
  VDB_LOG_NOTE("Initialization of MAX9286 Pre MAX9271 Setup ");
   //Disable CSI output
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x15,
                        (uint8_t)0x03);   
  //CSI Lanes, CSI DBL, GMSL DBL, Data Type RAW 11/12
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x12,
                        (uint8_t)0xF7);
  //Internal frames sync uses semi-automatic mode
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x01,
                        (uint8_t)0x01);    
  //Auto Come Back
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x69,
                        (uint8_t)0x30);    
  //Enable GMSL Links, all inputs, VSYNC comes from Camera
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x00,
                        (uint8_t)0x8F);    
  //DBL, Edge Select, HS/VS Encoding
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                        (uint16_t)0x07,
                        (uint8_t)0x84);    //DBL, Edge Select, HS/VS Encoding 
  msleep((uint32_t)2); // Wait 2ms

  VDB_LOG_NOTE("....Completed\n");
  /*********************************************************************/
  /*                          Setup Link 0 - Setup Link CamCnt         */
  /*********************************************************************/
  VDB_LOG_NOTE("Initialization of virtual I2C addresses");

  uint8_t        lVal2 = (uint8_t)0;
  uint8_t        lVal3 = (uint8_t)0;
  
  CamI2cCfg_t    lCamCfg;
  CamI2cCfg_t    lSerCfg;
  lCamCfg.mI2cBusIdx    = (I2C_BUS_IDX) aCsiIdx;
  lCamCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_16;
  lSerCfg.mI2cBusIdx    = (I2C_BUS_IDX) aCsiIdx;
  lSerCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_8;

  memcpy(lCamCfg.mpCompatible,
          OV10640_COMPATIBLE_STRING, 
          strlen(OV10640_COMPATIBLE_STRING));
  memcpy(lSerCfg.mpCompatible,
          SERIALIZER_COMPATIBLE_STRING, 
          strlen(SERIALIZER_COMPATIBLE_STRING));

  uint32_t i = (uint32_t)0;
  if(acCamCnt > MAXIM_CAM_SLOTS)
  {
    acCamCnt = MAXIM_CAM_SLOTS;
  }
  for (i = (uint32_t)1; i < (acCamCnt + (uint32_t)1); i++)
  {
    lVal3 = (uint8_t)(lVal3 << 1) + (uint8_t)0x1;
     // Enable link 0 reverse control channel to serializer (sending)
    CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                    (uint16_t)0xA,
                    (uint8_t)0xF0 + (uint8_t)((uint8_t)1<<(i-(uint32_t)1)));
    msleep((uint32_t)2);
    
    // //Change serializer I2C Slave Address to I2C_MAX_9271_ADDR(i)
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC],  
                    (uint16_t)0x0, 
                    (uint8_t)(I2C_MAX_9271_ADDR(i) << 1));  
    
    lSerCfg.mI2cAddr = I2C_MAX_9271_ADDR(i);
    spClients[aCsiIdx].mpSerializers[i] = CAM_ClientAdd(lSerCfg);
    
    // Create unique image sensor slave address for link[i]
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                    (uint16_t)0x9,
                    (uint8_t)(I2C_MAX_OV10640_ADDR(i) << 1)); 
    
    lCamCfg.mI2cAddr = I2C_MAX_OV10640_ADDR(i);
    spClients[aCsiIdx].mpCameras[i] = CAM_ClientAdd(lCamCfg);
        
    //Remember original Image Sensor Address on link[i]
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[(uint8_t)i], 
                    (uint16_t)0xA, 
                    (uint8_t)(I2C_MAX_OV10640_ADDR(0U) << 1));  

    //Serializer Broadcast Address
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                    (uint16_t)0xB, 
                    (uint8_t)(I2C_MAX_9271_ADDR(0U) << 1));  
  
    //Original Link [i] Serializer Address
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                    (uint16_t)0xC, 
                    (uint8_t)(I2C_MAX_9271_ADDR(i) << 1));  
  }
  VDB_LOG_NOTE("....Completed\n");
    
  /*********************************************************************/
  /*                   MAX9286 Post MAX9271 Setup                      */
  /*********************************************************************/
  VDB_LOG_NOTE("MAX9286 Post MAX9271 Setup \n");
  //Enable all I2C reverse and forward channels
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                    (uint16_t)0x0a,
                    (uint8_t)0xFF);    
  msleep((uint32_t)2); // Wait 2ms

  VDB_LOG_NOTE("Checking %d attached OV10640 cameras", acCamCnt);
  lVal3 = (uint8_t)0;
  for (i = (uint32_t)1; i < (acCamCnt + (uint32_t)1); i++)
  {     
    CAM_ReadSingle(spClients[aCsiIdx].mpCameras[i], 
                    (uint16_t)OV10640_CHIP_ID_HIGH_BYTE, lVal3);
    if (lVal3 != (uint8_t)0xA6) 
    {
      VDB_LOG_NOTE(
        "camera ov10640_mipi was not found (high_byte) id %x\n", 
        lVal3);
      return CAM_LIB_FAILURE;
    }
    
    msleep((uint32_t)5);
    
    CAM_ReadSingle(spClients[aCsiIdx].mpCameras[i], 
                  (uint16_t)OV10640_CHIP_ID_LOW_BYTE,
                   lVal3);
    if (lVal3 != (uint8_t)0x40) {
      VDB_LOG_NOTE("camera ov10640_mipi was not found (low_byte)\n");
      return CAM_LIB_FAILURE;
    }
  } // for all attached cameras
  VDB_LOG_NOTE("....Completed\n");
                                                                    
  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x34, lVal);
  // Disable Local Auto I2C ACK
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x34,
                  (uint8_t)(lVal & (uint8_t)0x7F)); 
  msleep((uint32_t)100);
  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x34, lVal2);
  if(lVal2 != (uint8_t)(lVal & (uint8_t)0x7F))
  {
    VDB_LOG_NOTE("I2c check failed (Disable local auto i2c ack) 0x%x.\n", 
                 lVal2);
  }
  VDB_LOG_NOTE("Reseting cameras");
  // *** Camera reset ***
  CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x3013, 
                  (uint8_t)0x01);
  msleep((uint32_t)200); //wait 200ms      
  // make sure in standby
  CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x3012, 
                  (uint8_t)0x00);

  VDB_LOG_NOTE("....Completed\n");

  // *** Camera config ***
  VDB_LOG_NOTE("Configuring camera registers");
  {
    uint8_t *lpCamConfig = Ov10640_Table3;
    uint32_t lConfigSize = (uint32_t)sizeof(Ov10640_Table3);
    uint32_t j = (uint32_t)0;
    // Configure omnivision cameras
    for(j = (uint32_t)0; j < lConfigSize; j += (uint32_t)3)
    {
      uint16_t lAddr = (uint16_t)((uint16_t)lpCamConfig[j] << 8)
                                      + (uint16_t)lpCamConfig[j + (uint32_t)1];
      CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                      lAddr, 
                      lpCamConfig[j+(uint32_t)2]);
    }   
    msleep((uint32_t)10);
    //   Enable External Frame Syncs 
    for(j=(uint32_t)0; j < (uint32_t)sizeof(Ov10640_FsinTable); j+=(uint32_t)3)  
    {
      uint16_t lAddr = (uint16_t)((uint16_t)Ov10640_FsinTable[j] << 8)
                                 + (uint16_t)Ov10640_FsinTable[j + (uint32_t)1];
      CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                      lAddr, 
                      Ov10640_FsinTable[j+(uint32_t)2]);
      msleep((uint32_t)100);
    } // for all attached cameras
    
    CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                    (uint16_t)0x3012, 
                    (uint8_t)0x1);
  } // Camera config
  VDB_LOG_NOTE("....Completed\n");

  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                    (uint16_t)0x34, lVal);
  // Enable Local Auto I2C ACK
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                    (uint16_t)0x34,
                    (uint8_t)(lVal | (uint8_t)0x80)); 
  msleep((uint32_t)100);
  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                    (uint16_t)0x34, lVal2);  
  if(lVal2 != (uint8_t)(lVal | (uint8_t)0x80))
  {
    VDB_LOG_NOTE("I2c check failed (Enable local auto i2c ack) 0x%x.\n", lVal2);
  }
  VDB_LOG_NOTE("Enabling serializer");
   //Enable all serializer and disable configuration link
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                    (uint16_t)0x04,
                    (uint8_t)0x83); 
  msleep((uint32_t)5); // Wait 5ms    
  VDB_LOG_NOTE("....Completed\n");

  MAXIM_CsiDisable(aCsiIdx);
 
/*  
  uint8_t lpMaxDump[256] = {0};
  MAXIM_DeserDump(lpMaxDump, 256);
  
  for(uint32_t i = 0; i < 256; i ++)
  {
    if(lpMaxDump[i] > 0xf)
    {
     printf("0x%x ", lpMaxDump[i]);
    }
    else
    {
      printf("0x0%x ", lpMaxDump[i]);
    }
    if(((i + 1) % 16) == 0)
    {
      printf("\n");
    }
  } // for all  
*/
  return lRet;
} // MAXIM_OV10640_InitComplex()

//****************************************************************************

CAM_LIB_RESULT MAXIM_OV10640_InitSimple(const enum CSI_IDX aCsiIdx,
                                                 uint32_t acCamCnt)
{ 
  uint8_t        lVal  = (uint8_t)0;
  CAM_LIB_RESULT lRet  = CAM_LIB_SUCCESS;

  uint32_t lCnt = (uint32_t)INIT_DELAY;
  // introduce i2c initialization delay if desired

  for(;lCnt > (uint32_t)0; lCnt--)
  {
    VDB_LOG_NOTE("%u \n", lCnt);
    msleep((uint32_t)500);
  }

#ifdef __STANDALONE__  
  VDB_LOG_NOTE("Initialize the I2C\n"); 
  I2C_Init_Config(aCsiIdx,I2C_MAXIM_DES_ADDR);
#endif

  // *** A ***
  VDB_LOG_NOTE("=== Section A ===\n");
  msleep((uint32_t)20);
  lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x1e, lVal);
  if(lVal != (uint8_t)0x40)
  {
    // deserializer not recognized
    VDB_LOG_NOTE("Deserializer not recognized 0x%x.\n", lVal);
    lRet = CAM_LIB_FAILURE;
    return lRet;
  }

  VDB_LOG_NOTE("Deserializer recognized.\n");
/*added
  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer, 0x34, lVal);
  VDB_LOG_NOTE("DesReg addr 0x34 val = %x \n", lVal);

   enable only the first link control channel (FWD+REV)
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,0x0A, 0xF1); 
   Jason ADD - begin
*/
  // enable all links control channels (FWD+REV)
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                       (uint16_t)0x0A,
                       (uint8_t)0xff); 
  //Updated to current procedure 1-14-16 Blaine Ashley
  msleep((uint32_t)1000);
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                       (uint16_t)0x3F,
                       (uint8_t)0x4F); //BA 1-14-16
  msleep((uint32_t)1000); //BA 1-14-16
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                       (uint16_t)0x3B,
                       (uint8_t)0x1E); //BA 1-14-16
  msleep((uint32_t)1000); //BA 1-14-16
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                       (uint16_t)0x04, 
                       (uint8_t)0x43); //BA 1-14-16
  msleep((uint32_t)1000); //BA 1-14-16
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                       (uint16_t)0x00,
                       (uint8_t)0x8F); //TB
  msleep((uint32_t)1000); //BA 1-14-16
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                       (uint16_t)0x08,
                       (uint8_t)0x01); //BA 1-14-16
  msleep((uint32_t)1000); //BA 1-14-16
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                       (uint16_t)0x3B,
                       (uint8_t)0x19); //BA 1-14-16
  msleep((uint32_t)1000); //BA 1-14-16


  VDB_LOG_NOTE("Please run initializatio  n\n"); 
  lCnt = (uint32_t)INIT_DELAY;  
  for(;lCnt > (uint32_t)0; lCnt--)
  {
    VDB_LOG_NOTE("%u \n", lCnt);
    msleep((uint32_t)500);
  }
  VDB_LOG_NOTE("Cnt down finished\n");

  //change the SER I2C speed
#ifndef __STANDALONE__ // use default I2C speed (400kHz)
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x0D,
                  (uint8_t)0xAE); 
#endif  // #ifndef __STANDALONE__
  msleep((uint32_t)2000); //YW 1-20-16

  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x15, lVal);
  // Disable CSI output Bits[3]=0b0
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x15,
                  (uint8_t)(lVal & (uint8_t)0xF7)); 


  uint32_t i = (uint32_t)0;
  if(acCamCnt > MAXIM_CAM_SLOTS)
  {
    acCamCnt = MAXIM_CAM_SLOTS;
  }
  VDB_LOG_NOTE("%u cameras attached.\n", acCamCnt);
  uint8_t lVal2 = (uint8_t)0;

  for(i = (uint32_t)0; i < acCamCnt; i++)
  {
    lVal2 = (uint8_t)(lVal2 << 1) + (uint8_t)1;
  } // for all cameras
  lVal2 += (uint8_t)0x80;
  // Auto detect csi clock source, disable VSYNC gen (will come from camera),
  // enable input links from used cameras
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x0,
                  (uint8_t)((uint8_t)(lVal & (uint8_t)0x00) | lVal2)); 
  // set MIPI-CSI-2 data type
  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x12, lVal);
  // RAW12. single input mode, csidbl = 0
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x12,
                  (uint8_t)((uint8_t)(lVal & (uint8_t)0xC0) | (uint8_t)0x07));  
  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x01, lVal);
  // enable Frame Sync
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x01,
                  (uint8_t)((uint8_t)(lVal & (uint8_t)0x01) | (uint8_t)0x01));

  // *** B ***
  VDB_LOG_NOTE("=== Section B ===\n");

  CAM_ReadSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x7, lVal);

  VDB_LOG_NOTE("Register 0x07 of Serial[%u] contains 0x%x\n", 0, lVal);
  // Enable single mode, PCLK latched on Rising edge, Enable HS/VS encoding
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC],  
                  (uint16_t)0x7,
                  (uint8_t)((uint8_t)(lVal & (uint8_t)0x6F) | (uint8_t)0x04));  

  msleep((uint32_t)1000); // Wait 3 ms min

  // *** F ***
  VDB_LOG_NOTE("=== Section F ===\n");

  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x34, lVal);
  // Disable Local Auto I2C ACK
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x34,
                  (uint8_t)(lVal & (uint8_t)0x7F)); 
  msleep((uint32_t)1000);

  // reset the camera
  CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x3013, 
                  (uint8_t)0x01);
  msleep((uint32_t)100);

  // make sure in standby
  CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x3012, 
                  (uint8_t)0x00);
  msleep((uint32_t)1000);

  // *** Camera config ***
  {
    uint8_t *lpCamConfig = Ov10640_Table3;
    uint32_t lConfigSize = (uint32_t)sizeof(Ov10640_Table3);
    uint32_t j = (uint32_t)0;
    
    // Configure omnivision cameras
    for(j = (uint32_t)0; j < lConfigSize; j += (uint32_t)3) 
    {
        uint16_t lAddr = (uint16_t)((uint16_t)lpCamConfig[j] << 8)
                                       + (uint16_t)lpCamConfig[j + (uint32_t)1];
        //for (i = 1; i < (acCamCnt + 1); i++)//BA 1-14-16
        //{        //BA 1-14-16
        CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                        lAddr, 
                        lpCamConfig[j+(uint32_t)2]);
        msleep((uint32_t)20);
        //} // for all attached cameras//BA 1-14-16
    } // for all configurations
    VDB_LOG_NOTE("Configured camera registers.\n");

    //   Enable External Frame Syncs 
    for(j = (uint32_t)0; j < sizeof(Ov10640_FsinTable); j += (uint32_t)3)  
    {
        uint16_t lAddr = (uint16_t)((uint16_t)Ov10640_FsinTable[j] << 8)
                          + (uint16_t)Ov10640_FsinTable[j + (uint32_t)1];
        //for (i = 1; i < (acCamCnt + 1); i++)//BA 1-14-16
        //{        //BA 1-14-16
        CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                        lAddr, 
                        Ov10640_FsinTable[j+(uint32_t)2]);
        msleep((uint32_t)200);
        //} // for all configurations//BA 1-14-16
    } // for all attached cameras

    // put cam to standby
    CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                        (uint16_t)0x3012, 
                        (uint8_t)0x1);
    VDB_LOG_NOTE("FSYNC on. Cameras in standby\n");
  } // Camera config

  msleep((uint32_t)1000);  
  VDB_LOG_NOTE("Enabled external frame syncs.\n");

  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x34, lVal);
  // Enable Local Auto I2C ACK
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x34,
                        (uint8_t)(lVal | (uint8_t)0x80)); 
  msleep((uint32_t)1000);

  VDB_LOG_NOTE("=== Section H ===\n");
  lVal = (uint8_t)0;
  msleep((uint32_t)100);

  CAM_ReadSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC],
                  (uint16_t)0x04, lVal);
  // Try to enable serial links
  CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x04,
                  (uint8_t)((uint8_t)(lVal & (uint8_t)0xBF) | (uint8_t)0x80));
  msleep((uint32_t)2000);
  VDB_LOG_NOTE("Ser%u done.\n", i);

  msleep((uint32_t)1000);  // Wait 10 ms min
  // enable all links control channels (FWD+REV)
  msleep((uint32_t)1000);
  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x12, lVal);

  // Enable CSI data lanes D0 - D3 , BIT[7:6]=01 // DBL OFF, CSI_DBL OFF
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x12,
                  (uint8_t)((uint8_t)(lVal & (uint8_t)0x0F) | (uint8_t)0xC0)); 

  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x31, lVal);
  VDB_LOG_NOTE("Des 0x31: 0x%x\n", lVal);

  msleep((uint32_t)1000);

  MAXIM_CsiDisable(aCsiIdx);
  msleep((uint32_t)3000);

  return lRet;
} // MAXIM_OV10640_InitSimple()

#if 1   //max96705+max9286  8bit  mark semi-auto new camera
CAM_LIB_RESULT MAXIM9286_96705_Init(enum CSI_IDX aCsiIdx,
                                         uint32_t acCamCnt)
{
  uint8_t        lVal  = 0;
  uint8_t        lVal2 = 0;
  uint8_t        lVal3 = 0;
  CAM_LIB_RESULT lRet  = CAM_LIB_SUCCESS;
  uint32_t       i     = 0;
  uint8_t reg, sensor_addr = 0;
  unsigned int g_sensor_num = 0;
  unsigned char g_sensor_is_there = 0;

  if(acCamCnt > MAXIM_CAM_SLOTS)
  {
    acCamCnt = MAXIM_CAM_SLOTS;
  }

#ifdef __STANDALONE__
  VDB_LOG_NOTE("Initialize the I2C\n");
  I2C_Init_Config(aCsiIdx,I2C_MAXIM_DES_ADDR);
#endif

  printf("Maxim9286_96705 cameras init\n");
/*  CAM_ReadSingle(spClients[aCsiIdx].mDeserializer, 0x1e, lVal);
  if(lVal != 0x40)
  {
    // deserializer not recognized
    printf("Deserializer not recognized 0x%x.\n", lVal);
    lRet = CAM_LIB_FAILURE;
    return lRet;
  }

  printf("Deserializer recognized.\n");

  printf("Initialization of Reverse Control Channel Setup ");
*/
	CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x15, 0x03);
	  msleep(10);

	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x0A, 0x00);
	  msleep(10);

    CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x1C, 0xF5); //
	msleep(50);
	//Disable CSI Output,CSI-2 outputs on virtual channel 0,Set virtual channel according to the link number,disable CSI-2 output

	  //Enable PRBS test
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x0E, 0x5F);
	  msleep(10);

	  //Enable Custom Reverse Channel & First Pulse Length
	  //CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x3F, 0x4F);
	  //msleep(10);

	  //Reverse Channel Amplitude to mid level and transition time
	  //CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x3B, 0x1E);
	  //msleep(10);

	  //Enable MAX9271 Configuration Link
	 // CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[0], 0x04, 0x43);
	 // msleep(30);

	 // CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[0], 0x04, 0x43);
	 // msleep(30);

	  //Increase serializer reverse channel input thresholds

	  //CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[0], 0x08, 0x01);

	 // msleep(10);

	  //Reverse Channel Amplitude level
	 // CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x3B, 0x19);
	 // msleep(5);

	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x12, 0xF3);//0xF3 //F4	  //YUV422,8bit,Double Data Rate, 4 data lane
	  msleep(5);

	  //Frame Sync

	  //Automatic Mode
	  //DesRegWrite(0x01, 0xC2);  //��·ģʽ��ʹ����ͬ��
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x06, 0x00);
	  msleep(10);
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x07, 0xf0);//9f
	  msleep(10);
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x08, 0x25);//24
	  msleep(10);
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x01, 0x01);	  //��·ģʽ����ͬ����0x01[7:6]=0b00,
	  msleep(10);
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x02, 0x00);	  //�����Ž�������
	  msleep(10);
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x63, 0x00);
	  msleep(10);

	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x64, 0x00);
	  msleep(10);
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x69, 0x30);
	  msleep(10);

	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x05, 0x19);	  //�����Ž�������

	  msleep(100);

	  // Detect link
	  g_sensor_num = 0;
	  //CAM_ReadSingle(spClients[aCsiIdx].mDeserializer, 0x49,reg);  //���link���,Ӧ�ü�⵽link2,
	  reg = 0xFF;
	  g_sensor_is_there = ((reg >> 4) & 0xF) | (reg & 0xF);
	  if (g_sensor_is_there & (0x1 << 0))
		  g_sensor_num += 1;
	  if (g_sensor_is_there & (0x1 << 1))
		  g_sensor_num += 1;
	  if (g_sensor_is_there & (0x1 << 2))
		  g_sensor_num += 1;
	  if (g_sensor_is_there & (0x1 << 3))
		  g_sensor_num += 1;
	  printf("max9286_mipi: sensor number = %d.\n", g_sensor_num); //�õ���Ч��·������ȥ������ͷ������

	  if (g_sensor_num == 0) {
		  printf("%s: no camera connected.\n", __func__);
		  return CAM_LIB_FAILURE;
	  }

	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x0E, 0x50);	  //Disable PRBS test
	  msleep(2);
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x0D, 0x03);	  //�Ž���

	  // Set link order in MIPI CSI-2 output
	  reg = 0xE4;  //Default setting
	  if (g_sensor_num == 1) {			  //ֻ����1������ͷ
		  switch (g_sensor_is_there) {
			  case 0x8: 	  //link3
				  reg = 0x27;
				  break;
			  case 0x4: 	  //link2
				  reg = 0xC6;
				  break;
			  case 0x2: 	  //link1
				  reg = 0xE1;
				  break;
			  case 0x1: 	  //link0
			  default:
				  reg = 0xE4;
				  break;
		  }
	  } else if (g_sensor_num == 2) {	  //������2������ͷ
		  switch (g_sensor_is_there) {
			  case 0xC:
				  reg = 0x4E;
				  break;
			  case 0xA:
				  reg = 0x72;
				  break;
			  case 0x9:
				  reg = 0x78;
				  break;
			  case 0x6:
				  reg = 0xD2;
				  break;
			  case 0x5:
				  reg = 0xD8;
				  break;
			  case 0x3:
			  default:
				  reg = 0xE4;
				  break;
		  }
	  } else if (g_sensor_num == 3) {	  //������3������ͷ
		  switch (g_sensor_is_there) {
			  case 0xE:
				  reg = 0x93;
				  break;
			  case 0xD:
				  reg = 0x9C;
				  break;
			  case 0xB:
				  reg = 0xB4;
				  break;
			  case 0x7:
			  default:
				  reg = 0xE4;
				  break;
		  }
	  }
	  //DesRegWrite(0x0B, reg);   //0x0B�Ĵ���Ϊlink out order


	  reg = 0xE0 | g_sensor_is_there;
	  //DesRegWrite(0x00, reg);   //enable all links,auto select
	  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x00, 0xEF);

  /*********************************************************************/
  /*                          Setup Link 0 - Setup Link CamCnt         */
  /*********************************************************************/
  printf("Initialization of virtual I2C addresses");
  lVal3 = 0;

  CamI2cCfg_t    lCamCfg;
  CamI2cCfg_t    lSerCfg;
  lCamCfg.mI2cBusIdx    = (I2C_BUS_IDX) aCsiIdx;
  lCamCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_16;
  lSerCfg.mI2cBusIdx    = (I2C_BUS_IDX) aCsiIdx;
  lSerCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_8;

  memcpy(lCamCfg.mpCompatible,
          OV490_OV10640_COMPATIBLE_STRING,
          strlen(OV490_OV10640_COMPATIBLE_STRING));
  memcpy(lSerCfg.mpCompatible,
          SERIALIZER_COMPATIBLE_STRING,
          strlen(SERIALIZER_COMPATIBLE_STRING));

  for (i = 1; i < (acCamCnt + 1); i++)
  {
    lVal3 = (lVal3 << 1) + 0x1;
    if (((0x1 << (i-1)) & g_sensor_is_there) == 0)
		{
			continue;
		}

		//Enable Link control channel
		reg = (0x11 << (i-1));
		CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x0A, reg);

		//CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[0], 0x04, 0x43);
	  	msleep(5);
    // //Change serializer I2C Slave Address to I2C_MAX_96705_ADDR(i)
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[0],
                    0x0,
                    I2C_MAX_96705_ADDR(i) << 1);

    lSerCfg.mI2cAddr = I2C_MAX_96705_ADDR(i);
    spClients[aCsiIdx].mpSerializers[i] = CAM_ClientAdd(lSerCfg);

	CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 0x01, I2C_MAXIM_DES_ADDR << 1);
	msleep(5);

    // Unique Link 0 image sensor slave address
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i],
                    0x9,
                    I2C_MAX_OV490_ADDR(i) << 1);

    lCamCfg.mI2cAddr = I2C_MAX_OV490_ADDR(i);
    spClients[aCsiIdx].mpCameras[i] = CAM_ClientAdd(lCamCfg);

    //Link [i] Image Sensor Address
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i],
                    0xA,
                    I2C_MAX_OV490_ADDR(0) << 1);

    //Serializer Broadcast Address
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i],
                    0xB,
                    I2C_MAX_96705_ADDR(5) << 1);
    msleep(10);
    //Link [i] Serializer Address
    CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i],
                    0xC,
                    I2C_MAX_96705_ADDR(i) << 1);

	CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 0x07, 0x84);
	msleep(5);
  }

  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x1C, 0xF4); //
	msleep(10);

  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x1B, 0x0F);   //enable equalizer
  msleep(10);
  
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x32, 0x99);   //
  msleep(10);
  
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x33, 0x99);   //
  msleep(10);

  printf("....Completed\n");

  /*********************************************************************/
  /*                   MAX9286 Post MAX9271 Setup                      */
  /*********************************************************************/
  printf("MAX9286 Post MAX96705 Setup \n");
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x0a,0xFF);    //Enable all I2C reverse and forward channels
  msleep(2); // Wait 2ms
  CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x0a,0xFF);    //Enable all I2C reverse and forward channels
  msleep(2); // Wait 2ms


   //printf("Checking %d attached OV490 cameras\n", acCamCnt);
   //lVal3 = 0;

   // CAM_WriteSingle(spClients[aCsiIdx].mpCameras[3], 0xfffd, 0x80);
	 //CAM_WriteSingle(spClients[aCsiIdx].mpCameras[3], 0xfffe, 0x80);
   //  CAM_ReadSingle(spClients[aCsiIdx].mpCameras[3], 0x0120, lVal3);
   // printf("camera2 Ov490 Reg0x80800120:	0x%x\n",lVal3);


  //CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,  0x34, 0x36); // Disable Local Auto I2C ACK
  //msleep(10);

  //CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, 0x1A, 0x60);
  //msleep(10);

  //CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,  0x34, 0xB6); // Enable Local Auto I2C ACK
  //msleep(10);

  //VDB_LOG_NOTE("Enabling serializer");
  //CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[1], 0x04,0x83);    //Enable all serializer and disable configuration link
  //msleep(5); // Wait 5ms
  //CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[2], 0x04,0x83);    //Enable all serializer and disable configuration link
  //msleep(5); // Wait 5ms
 // CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[3], 0x04,0x83);    //Enable all serializer and disable configuration link
 // msleep(5); // Wait 5ms
 // CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[4], 0x04,0x83);    //Enable all serializer and disable configuration link

  //CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[0], 0x04,0x83);	  //Enable all serializer and disable configuration link

  msleep(100); // Wait 5ms
  printf("....Completed\n");

  MAXIM_CsiDisable(aCsiIdx);

#if 0
  uint8_t lpMaxDump[256] = {0};
  MAXIM_DeserDump(lpMaxDump, 256);

  for(uint32_t i = 0; i < 256; i ++)
  {
    (lpMaxDump[i] > 0xf)?printf("0x%x ", lpMaxDump[i]):printf("0x0%x ", lpMaxDump[i]);
    if(((i + 1) % 16) == 0)
    {
      printf("\n");
    }
  } // for all
#endif // 0
  return lRet;
} // MAXIM_OV10640_InitComplex()
#endif


//*****************************************************************************

CAM_LIB_RESULT MAXIM_CAM_RegWrite(
  const enum CSI_IDX  aCsiIdx, 
  const uint16_t aRegAddr,
  const uint8_t  aVal,
  const enum MAXIM_CHNL aChnlIdx
)
{
  return CAM_WriteSingle(spClients[aCsiIdx].mpCameras[aChnlIdx], 
                         aRegAddr, 
                         aVal);
} // MAXIM_CAM_RegWrite

//*****************************************************************************

CAM_LIB_RESULT MAXIM_CAM_RegRead(  
  const enum CSI_IDX aCsiIdx, 
  const uint16_t aRegAddr,
  uint8_t  &arVal,
  const enum MAXIM_CHNL aChnlIdx
)
{
  return CAM_ReadSingle(spClients[aCsiIdx].mpCameras[aChnlIdx], 
                        aRegAddr, 
                        arVal);
} // MAXIM_CAM_RegRead

//*****************************************************************************

CAM_LIB_RESULT MAXIM_SER_RegWrite(
  const enum CSI_IDX  aCsiIdx, 
  const uint16_t aRegAddr,
  const uint8_t  aVal,
  const enum MAXIM_CHNL aChnlIdx
)
{
  return CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[aChnlIdx], 
                         aRegAddr, 
                         aVal);
} // MAXIM_SER_RegWrite

//*****************************************************************************

CAM_LIB_RESULT MAXIM_SER_RegRead(
  const enum CSI_IDX  aCsiIdx, 
  const uint16_t aRegAddr,
  uint8_t  &arVal,
  const enum MAXIM_CHNL aChnlIdx
)
{
  return CAM_ReadSingle(spClients[aCsiIdx].mpSerializers[aChnlIdx], 
                        aRegAddr, 
                        arVal);
} // MAXIM_SER_RegRead

//*****************************************************************************

CAM_LIB_RESULT MAXIM_DES_RegWrite(
  const enum CSI_IDX  aCsiIdx, 
  const uint16_t aRegAddr,
  const uint8_t  aVal
)
{
  return CAM_WriteSingle(spClients[aCsiIdx].mDeserializer, aRegAddr, aVal);
} // MAXIM_DES_RegWrite

//*****************************************************************************

CAM_LIB_RESULT MAXIM_DES_RegRead(
  const enum CSI_IDX  aCsiIdx, 
  const uint16_t aRegAddr,
  uint8_t &arVal
)
{
  return CAM_ReadSingle(spClients[aCsiIdx].mDeserializer, aRegAddr, arVal);
} // MAXIM_DES_RegRead

//*****************************************************************************

CAM_LIB_RESULT MAXIM_OV10640_ExposureSet(OV10640_Exposure_t &arExp)
{
  arExp.mClient = spClients[arExp.mCsiIdx].mpCameras[0];
  return OV10640_ExposureSet(arExp);
} // MAXIM_OV10640_ExposureSet

//****************************************************************************

CAM_LIB_RESULT MAXIM_OV10635_InitComplex(const enum CSI_IDX aCsiIdx, 
                                         uint32_t acCamCnt)
{ 
  uint8_t        lVal  = (uint8_t)0;
  CAM_LIB_RESULT lRet  = CAM_LIB_SUCCESS;
 
#ifdef __STANDALONE__  
  VDB_LOG_NOTE("Initialize the I2C\n"); 
  I2C_Init_Config(aCsiIdx, I2C_MAXIM_DES_ADDR);
#endif

  VDB_LOG_NOTE("Deserializer connection test\n");
  lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x1e, lVal);
  if(lVal != (uint8_t)0x40)
  {
    // deserializer not recognized
    VDB_LOG_NOTE("Deserializer not recognized 0x%x.\n", lVal);
    lRet = CAM_LIB_FAILURE;
    return lRet;
  }
  VDB_LOG_NOTE("Deserializer recognized.\n");

  // enable all links control channels (FWD+REV)
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x0A,
                  (uint8_t)0xff); 

  /*********************************************************************/
  /*                    Reverse Control Channel Setup                  */
  /*********************************************************************/

  //Enable Custom Reverse Channel & First Pulse Length
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x3F,
                  (uint8_t)0x4F); 
  msleep((uint32_t)2); // Wait 2ms
  //Reverse Channel Amplitude to mid level and transition time
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x3B,
                  (uint8_t)0x1E); 
  msleep((uint32_t)2); // Wait 2ms
  //Enable Configuration Link
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x04, 
                  (uint8_t)0x43); 
  msleep((uint32_t)5); // Wait 5ms
  //Serializer input gain and thresholds
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x08, 
                  (uint8_t)0x01); 
  msleep((uint32_t)2); // Wait 2ms
  //Reverse Channel Amplitude level
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x3B,
                  (uint8_t)0x19);
  msleep((uint32_t)2); // Wait 2ms

  //change the SER I2C speed to 100kHz
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x0D,
                  (uint8_t)0xAE); 
  msleep((uint32_t)100); // Wait 100ms

  /*********************************************************************/
  /*                   MAX9286 Pre MAX9271 Setup                       */
  /*********************************************************************/
  //Disable CSI output
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x15,
                  (uint8_t)0x03);    
  //CSI Lanes, CSI DBL, GMSL DBL, Data YUV422 10bit Type dbl check datatype
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x12,
                  (uint8_t)0xF4);
  //Frame Sync
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x01,
                  (uint8_t)0x02);    
  //Auto Come Back
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x69,
                  (uint8_t)0x30);    
  //Enable GMSL Links
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x00,
                  (uint8_t)0xFF);    
  //DBL, Edge Select, HS/VS Encoding
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x07,
                  (uint8_t)0x84);    
  msleep((uint32_t)2); // Wait 2ms

  /*********************************************************************/
  /*                          Setup Link 0 - Setup Link CamCnt         */
  /*********************************************************************/

  CamI2cCfg_t    lCamCfg;
  CamI2cCfg_t    lSerCfg;
  lCamCfg.mI2cBusIdx    = (I2C_BUS_IDX) aCsiIdx;
  lCamCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_16;
  lSerCfg.mI2cBusIdx    = (I2C_BUS_IDX) aCsiIdx;
  lSerCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_8;

  memcpy(lCamCfg.mpCompatible,
          OV10635_COMPATIBLE_STRING, 
          strlen(OV10635_COMPATIBLE_STRING));
  memcpy(lSerCfg.mpCompatible,
          SERIALIZER_COMPATIBLE_STRING, 
          strlen(SERIALIZER_COMPATIBLE_STRING));
  
  uint32_t i = (uint32_t)0;
  lVal = (uint8_t)0;
  if(acCamCnt > MAXIM_CAM_SLOTS)
  {
    acCamCnt = MAXIM_CAM_SLOTS;
  }
  for (i = (uint32_t)1; i < (acCamCnt + (uint32_t)1); i++)
  {
    lVal = (uint8_t)(lVal << 1) + 0x1;
    // Enable link 0 reverse control channel to serializer (sending)
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
               (uint16_t)0xA,
               (uint8_t)((uint8_t)0xF0+(uint8_t)((uint8_t)1<<(i-(uint32_t)1))));
    
    // //Change serializer I2C Slave Address to I2C_MAX_9271_ADDR(i)
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC],  
                    (uint16_t)0x0, 
                    (uint8_t)((uint8_t)I2C_MAX_9271_ADDR(i) << 1));  
    
    lSerCfg.mI2cAddr = (uint8_t)I2C_MAX_9271_ADDR(i);
    spClients[aCsiIdx].mpSerializers[i] = CAM_ClientAdd(lSerCfg);

    // Unique Link 0 image sensor slave address  
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                   (uint16_t)0x9, 
                   (uint8_t)((uint8_t)I2C_MAX_OV10635_ADDR(i) << 1)); 
   
    lCamCfg.mI2cAddr = (uint8_t)I2C_MAX_OV10635_ADDR(i);
    spClients[aCsiIdx].mpCameras[i] = CAM_ClientAdd(lCamCfg);
    
    //Link [i] Image Sensor Address
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                   (uint16_t)0xA, 
                   (uint8_t)(I2C_MAX_OV10635_ADDR((uint8_t)0) << 1));  

    //Serializer Broadcast Address
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                   (uint16_t)0xB, 
                   (uint8_t)(I2C_MAX_9271_ADDR(0U) << 1));  
  
    //Link [i] Serializer Address
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                   (uint16_t)0xC, 
                   (uint8_t)((uint8_t)I2C_MAX_9271_ADDR(i) << 1));  
  }

  /*********************************************************************/
  /*                   MAX9286 Post MAX9271 Setup                      */
  /*********************************************************************/
  //Enable all I2C reverse and forward channels
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x0a,
                         (uint8_t)0xFF);    
  msleep((uint32_t)2); // Wait 2ms

  // *** F ***
  VDB_LOG_NOTE("=== Section F ===\n");
  lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x34, lVal);
  // Disable Local Auto I2C ACK
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                        (uint16_t)0x34,
                        (uint8_t)(lVal&0x7F)); 

  msleep((uint32_t)2); // Wait 2ms
  // *** Camera config ***
  {
    uint8_t *lpCamConfig = OV10635_Table;
    uint32_t lConfigSize = (uint32_t)sizeof(OV10635_Table);
    uint32_t j = (uint32_t)0;
    
    // Configure omnivision cameras
    for(j = (uint32_t)0; j < lConfigSize; j += (uint32_t)3) 
    {
        uint16_t lAddr = (uint16_t)((uint16_t)lpCamConfig[j] << 8)
                                                 + lpCamConfig[j + (uint32_t)1];
        lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                        lAddr, 
                        lpCamConfig[j+(uint32_t)2]);

    } // for all configurations
    VDB_LOG_NOTE("Configured camera registers.\n");
    
    msleep((uint32_t)10);

    //   Enable External Frame Syncs 
    for(j=(uint32_t)0; j < (uint32_t)sizeof(Ov10635_FsinTable); j+=(uint32_t)3)  
    {
        uint16_t lAddr = ((uint16_t)((uint16_t)Ov10635_FsinTable[j] << 8)) + \
                         Ov10635_FsinTable[j + (uint32_t)1];
        lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                        lAddr, 
                        Ov10635_FsinTable[j + (uint32_t)2]);

    } // for all attached cameras

    VDB_LOG_NOTE("FSYNC on. Cameras in standby\n");
  } // Camera config

  lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x34, lVal);
  // Enable Local Auto I2C ACK
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x34,
                  (uint8_t)(lVal | (uint8_t)0x80)); 


  /*********************************************************************/
  /*               MAX9286 Post Image Sensor Setup                     */
  /*********************************************************************/

  //Enable all serializer and disable configuration link
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x04,
                  (uint8_t)0x83);    
  msleep((uint32_t)5); // Wait 5ms

  MAXIM_CsiDisable(aCsiIdx);

  return lRet;
} // MAXIM_OV10635_InitComplex()

//****************************************************************************

CAM_LIB_RESULT MAXIM_SONY_InitComplex(const enum CSI_IDX aCsiIdx,
                                                 uint32_t acCamCnt)
{ 
  uint8_t        lVal  = (uint8_t)0;
  CAM_LIB_RESULT lRet  = CAM_LIB_SUCCESS;

#ifdef __STANDALONE__  
  VDB_LOG_NOTE("Initialize the I2C\n"); 
  I2C_Init_Config(aCsiIdx,I2C_MAXIM_DES_ADDR);
#endif

  VDB_LOG_NOTE("Deserializer connection test\n");
  lRet = CAM_ReadSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x1e, lVal);
  if(lVal != (uint8_t)0x40)
  {
    // deserializer not recognized
    VDB_LOG_NOTE("Deserializer not recognized 0x%x.\n", lVal);
    lRet = CAM_LIB_FAILURE;
    return lRet;
  }
  VDB_LOG_NOTE("Deserializer recognized.\n");

  /*********************************************************************/
  /*                    Reverse Control Channel Setup                  */
  /*********************************************************************/
  // Enable Custom Reverse Channel & First Pulse lengh
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x3F,
                   (uint8_t)0x4F);
  msleep((uint32_t)2); // Wait 2ms
  // First pulse length rise time changed from 300ns to 200ns
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x3B,
                   (uint8_t)0x1E);    
  msleep((uint32_t)2); // Wait 2ms
  // Enable configuration link, use I2C link as local control channel
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC],  
                  (uint16_t)0x04,
                   (uint8_t)0x43);  
  msleep((uint32_t)5); // Wait 5ms
  // Enable high threshold for reverse channel input buffer. 
  // This increases immunity to power supply noise when the coaxial link is used 
  // for power as well as signal.
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC],  
                  (uint16_t)0x08,
                   (uint8_t)0x81);  
  msleep((uint32_t)2); // Wait 2ms
  // Increase reverse amplitude from 100mV to 170mV. 
  // This compensates for the higher threshold of step 5.
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x3B, 
                          (uint8_t)0x19);    
  msleep((uint32_t)2); // Wait 2ms
  
  //Auto Come Back
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x69, 
                          (uint8_t)0x30);    

  // change the SER I2C speed to 100kHz fro Linux
  // on SA use default I2C speed (400kHz)
  #ifndef __STANDALONE__ 
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                         (uint16_t)0x0D, 
                          (uint8_t)0xAE); 
  #endif //#ifndef __STANDALONE__
  msleep((uint32_t)100); // Wait 100ms

  /*********************************************************************/
  /*                   MAX9286 Pre MAX9271 Setup                       */
  /*********************************************************************/
  // Disable CSI output
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x15, 
                          (uint8_t)0x03);     
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC], 
                         (uint16_t)0x07, 
                          (uint8_t)0xA5);
  msleep((uint32_t)2); // Wait 2ms
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x0C, 
                          (uint8_t)0xB9);
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint8_t)0x12, 
                         (uint8_t)0xF7);
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint8_t)0x1C, 
                         (uint8_t)0x05);
  msleep((uint32_t)1000); // Wait 100ms
  // Internally generate frame sync, FSYNC/GPI outputs frame sync, 
  // Internal frame sync uses automatic mode
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x01, 
                          (uint8_t)0x00);     
  msleep((uint32_t)1000); // Wait 100ms
  //FPS calculation , FPS defined in sony_config.h
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x08, 
                         (uint8_t)((37500000/FPS) >> 16));    //MSB byte 
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x07, 
                         (uint8_t)((37500000/FPS) >> 8));      //Mid byte 
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x06, 
                         (uint8_t)(37500000/FPS));            //Low byte 

  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x63,
                          (uint8_t)0x00);     // 
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                         (uint16_t)0x64,
                          (uint8_t)0x00);     // 

  uint32_t i = (uint32_t)0;
  lVal = (uint8_t)0;
  if(acCamCnt > (uint32_t)MAXIM_CAM_SLOTS)
  {
    acCamCnt = (uint32_t)MAXIM_CAM_SLOTS;
  }

  for(i = (uint32_t)0; i < acCamCnt; i++)
  {
    lVal = (uint8_t)(lVal << 1) + 1;
  } // for all cameras
  // Auto detect csi clock source, disable VSYNC gen (will come from camera), 
  // enable input links from used cameras
  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,(uint16_t)0x0,
                         (uint8_t)((uint8_t)0xE0 + lVal)); 

  msleep((uint32_t)10); // Wait 10ms

  /*********************************************************************/
  /*                          Setup Link 0 - Setup Link CamCnt         */
  /*********************************************************************/

  CamI2cCfg_t    lCamCfg;
  CamI2cCfg_t    lSerCfg;
  lCamCfg.mI2cBusIdx    = (I2C_BUS_IDX) aCsiIdx;
  lCamCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_16;
  lSerCfg.mI2cBusIdx    = (I2C_BUS_IDX) aCsiIdx;
  lSerCfg.mRegAddrWidth = I2C_REG_ADDR_WIDTH_8;

  memcpy(lCamCfg.mpCompatible,
          SONY_COMPATIBLE_STRING, 
          strlen(SONY_COMPATIBLE_STRING));
  memcpy(lSerCfg.mpCompatible,
          SERIALIZER_COMPATIBLE_STRING, 
          strlen(SERIALIZER_COMPATIBLE_STRING));
  
  // 4. GMSL Link Setup
  lVal = (uint8_t)0;
  for (i = (uint32_t)1; i < acCamCnt + (uint32_t)1; i++)
  {
    lVal = (uint8_t)(lVal << 1) + 0x1;
    // Enable link 0 reverse control channel to serializer (sending)
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
              (uint16_t)0x0A, 
               (uint8_t)0xF0+(uint8_t)((uint8_t)1<<((uint8_t)(i-(uint32_t)1)))); 
    msleep((uint32_t)10); // Wait 10ms
    
    // Change I2C slave ID of MAX9271 to 0x82
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC],  
                    (uint16_t)0x00, 
                    (uint8_t)((uint8_t)I2C_MAX_9271_ADDR(i) << 1));
    
    lSerCfg.mI2cAddr = (uint8_t)I2C_MAX_9271_ADDR(i);
    spClients[aCsiIdx].mpSerializers[i] = CAM_ClientAdd(lSerCfg);
    
    msleep((uint32_t)10); // Wait 10ms
    // Unique Link 0 image sensor slave address
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                    (uint16_t)0x09, 
                    (uint8_t)((uint8_t)I2C_MAX_IMX224_ADDR(i) << 1));     
    
    lCamCfg.mI2cAddr = (uint8_t)I2C_MAX_IMX224_ADDR(i);
    spClients[aCsiIdx].mpCameras[i] = CAM_ClientAdd(lCamCfg);
    
    msleep((uint32_t)10); // Wait 10ms
    // Link image sensor slave address
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                    (uint16_t)0x0A, 
                    (uint8_t)(I2C_MAX_IMX224_ADDR((uint8_t)0) << 1));
    msleep((uint32_t)10); // Wait 10ms
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                    (uint16_t)0x0B, 
                    (uint8_t)(I2C_MAX_9271_ADDR((uint8_t)0) << 1));  
    msleep((uint32_t)10); // Wait 10ms
    // Program original address of MAX9271 = 0x82
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[i], 
                    (uint16_t)0x0C, 
                    (uint8_t)((uint8_t)(I2C_MAX_9271_ADDR(i) << 1)));  
    msleep((uint32_t)10); // Wait 10ms
  }

  lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                    (uint16_t)0x0A,
                     (uint8_t)0xFF);
  msleep((uint32_t)2); // Wait 2ms
  // 5. Image Sensor Initialization

  uint8_t lReg;
  // *** Camera config ***
  {
    uint8_t *lpCamConfig = IMX224_CMOS_SDR_HD720p_12bit_30fps;
    uint32_t lConfigSize = (uint32_t)sizeof(IMX224_CMOS_SDR_HD720p_12bit_30fps);
    uint32_t j = (uint32_t)0;
   
    // Configure sony cameras
    for(j = (uint32_t)3; j < lConfigSize; j += (uint32_t)3)
    {
        uint16_t lAddr =(uint16_t)((uint16_t)lpCamConfig[j] << 8)
                                       + (uint16_t)lpCamConfig[j + (uint32_t)1];     
        lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_0], 
                        lAddr, 
                        lpCamConfig[j+(uint32_t)2]);
        lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_1], 
                        lAddr, 
                        lpCamConfig[j+(uint32_t)2]);
        lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_2], 
                        lAddr, 
                        lpCamConfig[j+(uint32_t)2]);
        lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_3], 
                        lAddr, 
                        lpCamConfig[j+(uint32_t)2]);
        msleep((uint32_t)20);   
    } // for all configurations
    VDB_LOG_NOTE("Configured camera registers.\n");
  } // Camera config

  if (CAM_LIB_SUCCESS == lRet)
  {
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpCameras[MAXIM_CHNL_ORIG_BC], 
                  (uint16_t)0x3000, 
                   (uint8_t)0x00);    //getting camera to normal mode

    /*********************************************************************/
    /*                   MAX9286 Post MAX9271 Setup                      */
    /*********************************************************************/

    lRet = CAM_WriteSingle(spClients[aCsiIdx].mpSerializers[MAXIM_CHNL_ORIG_BC],  
                  (uint16_t)0x04, 
                   (uint8_t)0x83);
    msleep((uint32_t)5); // Wait 5ms
    // Enable CSI data lanes D0, D1, D2, D3 
    lRet = CAM_WriteSingle(spClients[aCsiIdx].mDeserializer,
                  (uint16_t)0x12,
                   (uint8_t)0xF7); 
    msleep((uint32_t)5); // Wait 5ms    
    MAXIM_CsiDisable(aCsiIdx);
  }

  return lRet;
} // MAXIM_SONY_InitComplex()

//***************************************************************************

void MAXIM_DeserDump(uint8_t* const apDump, uint32_t aSize)
{
  uint32_t i = (uint32_t)0;

  aSize = (aSize > (uint32_t)64)?(uint32_t)64:aSize;

  for( i = (uint32_t)0x0; i < aSize; i++)
  {
    uint8_t lVal = (uint8_t)0;

    CAM_ReadSingle(spClients[0].mDeserializer, (uint16_t)i, lVal);
    apDump[i] = lVal;
    msleep((uint32_t)30);
  }
} // MAXIM_DeserDump()

//***************************************************************************

/*EOF*/