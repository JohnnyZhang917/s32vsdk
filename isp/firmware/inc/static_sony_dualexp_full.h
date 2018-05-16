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
/**
 * \file    static_sony_dualexp.h
 * \brief   ISP (sequencer) static configuration
 * \author
 * \author
 * \version 0.1
 * \date    27-01-2016
 * \note    initial version
 ****************************************************************************/

#ifndef STATICSONYDUALEXP_H
#define STATICSONYDUALEXP_H
 
#define	IPUS0_START	47
#define	IPUS2_START	(IPUS0_START+2)
#define	IPUS1_START	(IPUS2_START+4)
#define	IPUS5_START	(IPUS1_START+4)
#define	IPUV1_START	(IPUS5_START+1)
#define	IPUV2_START	(IPUV1_START+6)
#define	IPUS4_START	(IPUV2_START+4)
#define	IPUS3_START	(IPUS4_START+4)
#define	FDMA0_START	(IPUS3_START+2)

/****************************************************************************/

#define	STATIC_SEQ						\
  initIPUnode(3, IPUS0_START, 0x1);	/* Expose */		\
  initIPUnode(7, IPUS2_START, 0x1);	/* Vignetting */	\
  initIPUnode(4, IPUS1_START, 0x1);	/* Debayer_G */		\
  initIPUnode(5, IPUS5_START, 0x1);	/* Debayer_BR */	\
  initIPUnode(6, IPUV1_START, 0x1);	/* RGB2YUV */		\
  initIPUnode(8, IPUV2_START, 0x1);	/* DenoiseFilter */	\
  initIPUnode(9, IPUS4_START, 0x1);	/* DenoiseSelect */	\
  initIPUnode(2, IPUS3_START, 0x1);	/* ConvertOut */	\
								\
  initFDMAnode(0, FDMA0_START, 0x1);				\

#endif /* STATICSONYDUALEXP_H */
  
/* EOF */