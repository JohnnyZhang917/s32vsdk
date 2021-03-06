/*****************************************************************************
* 
* NXP Confidential Proprietary
*
* Copyright (c) 2014-2016 Freescale Semiconductor
* Copyright 2017 NXP 
* All Rights Reserved
*
******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************/

/*!****************************************************************************
*  \file apexcv_pro_pyramid_multi.h
*  \brief ApexCV Host wrapper for pyramid creation
*******************************************************************************/

#ifndef APEXCVPROPYRAMIDMULTI_H
#define APEXCVPROPYRAMIDMULTI_H

#include <umat.hpp>
#include "acf_process_apu.h"
#include <apexcv_error_codes.hpp>

#define MAX_PYRAMID_MULTI_LEVELS 4

namespace apexcv {

/*!****************************************************************************
*  \brief Pyramid_multi creation class
*
*  This class is an interface for using the pyramid_multi creation algorithm.
******************************************************************************/  
class PyramidMultiCreation
{
public:

/*!****************************************************************************
*  \brief Default constructor.
******************************************************************************/
   PyramidMultiCreation();

/*!****************************************************************************
*  \brief Default destructor.
******************************************************************************/
   ~PyramidMultiCreation();

/*!****************************************************************************
*  \brief Initializes the class.
*  
*  Connects the buffers to the process.
******************************************************************************/
   APEXCV_LIB_RESULT Initialize(      vsdk::UMat  src   /*!< Source memory buffer. Datatype is 08U. */,
                        vsdk::UMat *  dst   /*!< Destination memory buffer. Datatype is 08U. */
                  );

/*!****************************************************************************
*  \brief Connects the input/outputs to the process
*
*  Use this to reconnect the Input and Output Buffers. This only needs to
*  be done if the connected Input/Outputs are changed. If only the data within (no size,
*  or type changes), then this does not need to be called.
******************************************************************************/
   APEXCV_LIB_RESULT ReconnectIO(     vsdk::UMat  src   /*!< Source memory buffer. Datatype is 08U. */,
                        vsdk::UMat *  dst   /*!< Destination memory buffer. */
                  );

/*!****************************************************************************
*  \brief Run the pyramid_multi creation process.
*
*  Upscale or downscale _src_ buffer and stores the result in _dst_ buffer.
*  The process will upscale _src_ buffer if _isPyramidUp_ = true.
*  The process will downscale _src_ buffer if _isPyramidUp_ = false.
*  Default is _isPyramidUp_ = true.
*
*  Supported datatypes are:
*    - unsigned 8 bit to unsigned 8 bit
*
*  \return Error code (zero on success).
***********************************************************************************/
   APEXCV_LIB_RESULT Process(bool isPyramidUp   /*!< Set true if execute pyramid up, or false if execute pyramid down. */
               );


   APEXCV_LIB_RESULT SelectApuConfiguration(ACF_APU_CFG lApuConfig,
                              int32_t lApexId);

/*!*********************************************************************************
 *  \brief SelectApexCore
 *  \return Error code for the ACF execution (zero on success).
 *
 *  Select which APEX core (0 or 1) to be selected to run the processing.
 *  This function has to be called after initialize() and can be executed per frame base.
 ***********************************************************************************/
   APEXCV_LIB_RESULT SelectApexCore(int32_t aApexId /*!< The ID of the desired APEX (e.g if there are 2 APEXs, valid values for lApexId would be 0 and 1).*/
               );

private:
   int mInitState;      /*!< Flag indicating the process has been initialized. */
   int mWidth;         /*!< Input width.  */
   int mHeight;        /*!< Input height. */
   int mLevels;

   void *mProcessDownMulti0;
   void *mProcessDownMulti1;

   ACF_APU_CFG mApuConfig;
   int32_t     mApexId;
   bool        mWeSelectedApuConfiguration;
}; /* class PyramidCreation */

} /* namespace apex */

#endif /* APEXCVPROPYRAMIDMULTI_H */

