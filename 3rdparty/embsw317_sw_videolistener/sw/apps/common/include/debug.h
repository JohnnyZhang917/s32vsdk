/**
 @file             debug.h
 @version          0.0.0.0

 @brief            Wrapper for the debug printing API
 @details          This file is used to connect debugging macros to the print function.

 Project           M4_ETH_PROJECT_NAME
 Platform          M4_ETH_PLATFORM

 SWVersion         M4_ETH_RELEASE_VERSION

 
 Copyright (c) 2016 NXP Semiconductors
 All Rights Reserved
*/
/*==================================================================================================
==================================================================================================*/

#ifndef DEBUG_H
    #define DEBUG_H

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/  
/* It is not a good idea to include header files in header file but here we do
   that because this is just a API wrapper */
#include "fsl_printf.h"
/*==================================================================================================
                               SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define ERROR(fmt, args...)   fsl_printf("[sm_app] ERROR: " fmt, ##args)
#define WARNING(fmt, args...) fsl_printf("[sm_app] WARNING: " fmt, ##args)
#define NOTICE(fmt, args...)  fsl_printf("[sm_app] NOTICE: " fmt, ##args)
#define INFO(fmt, args...)    fsl_printf("[sm_app] INFO: " fmt, ##args)

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
    
/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

#endif /* DEBUG_H */
