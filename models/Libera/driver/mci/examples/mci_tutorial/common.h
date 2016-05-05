/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: common.h 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */


#ifndef _COMMON_H
#define _COMMON_H

const std::string cHost              = "10.0.3.106";                                      /* IP of Libera Instrument                                                 */
const int         cCustomPort        = 6001;                                              /* custom port, application is listening at, default port is 5679          */
const std::string cTriggerSourceNode = "boards.evrx2.triggers.t2.source";                 /* name of scalar registry node for setting trigger source                 */
const std::string cStringArrayNode   = "boards.evrx2.signals.event.components_names";     /* name of array registry node, whose data can be read as string variable  */
const std::string cEventStream       = "boards.evrx2.signals.event";                      /* name of event stream signal                                             */

/* These examples require raf3 module to be present in the Libera Brilliance+                                                                                        */

const std::string cSaStream          = "boards.raf3.signals.sa";                          /* name of SA stream signal                                                */
const std::string cFaStream          = "boards.raf3.signals.fa";                          /* name of FA stream signal                                                */
const std::string cAdcDod            = "boards.raf3.signals.adc";                         /* name of adc "data on demand" signal                                     */
const std::string cTbtDod            = "boards.raf3.signals.ddc_synthetic";               /* name of turn-by-turn (DDC) "data on demand"                             */
const std::string cTdpDod            = "boards.raf3.signals.tdp_synthetic";               /* name of turn-by-turn (TDP) "data on demand"                             */
const std::string cAGCNode           = "boards.raf3.conditioning.tuning.agc.enabled";     /* path to the AGC node                                                    */
const std::string cPowerLevelNode    = "boards.raf3.conditioning.tuning.agc.power_level"; /* path to the Power Level node                                            */
#endif
