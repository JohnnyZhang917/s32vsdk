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

#ifndef FRAMEINPUTFILE_H
#define FRAMEINPUTFILE_H

#include <frame_input_base.h>
#include <string>

#include <opencv2/opencv.hpp>

namespace io
{
  class FrameInputFile : public FrameInputBase
  {
    public:
      FrameInputFile(std::string filename);
      ~FrameInputFile();
    
      void *           GetFrame();
      IO_STATUS        GetStatus(); 
      
      uint32_t         GetWidth();
      uint32_t         GetHeight();
      IO_DATA_DEPTH    GetDepth();
      IO_DATA_CHANNELS GetChannels();
      
    private:
      IO_STATUS        m_status;
      
      uint32_t         m_width;
      uint32_t         m_height;
      IO_DATA_DEPTH    m_depth;
      IO_DATA_CHANNELS m_channels;
      
      cv::VideoCapture m_capture;
      cv::Mat          m_frame;
  };
}


#endif /* FRAMEINPUTFILE_H */