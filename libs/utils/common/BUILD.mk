##############################################################################
#
# Freescale Confidential Proprietary
#
# Copyright (c) 2014 Freescale Semiconductor;
# All Rights Reserved
#
##############################################################################
#
# THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
#
##############################################################################

SDK_ROOT :=  ../../../..

ARM_LIB =    libcommon.a
VPATH += ../src
##############################################################################
# libcommon.a
##############################################################################
ARM_LIB_SRCS +=                                                              \
    time_measure.cpp                                                         \
    base_process_init.cpp                                                    \
    swt_arm_report.cpp                                                       
 

ARM_INCS +=                                                                  \
    -I../include                                                             \
    -I$(OPENCV_ROOT)/include                                                 \
    -I$(SDK_ROOT)/libs/utils/common/include                                  \
    -I$(SDK_ROOT)/libs/io/semihost/include                                   \
    -I$(SDK_ROOT)/include                                                    \
    -I$(SDK_ROOT)                                                            \



ARM_CXOPTS += -w
