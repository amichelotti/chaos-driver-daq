/* 
 * File:   CmdAcquireDafneAccumulatorBPM.h
 * Author: michelo
 *
 * Created on October 15, 2015, 10:21 AM
 */

#ifndef CMDACQDAFNEACCUMULATORBPM_H
#define	CMDACQDAFNEACCUMULATORBPM_H
#include <driver/misc/CmdSync.h>
#include <driver/misc/ChaosDatasetAttributeSinchronizer.h>
namespace driver {
        
        namespace daq {
            namespace libera{
	      class CmdAcquireDafneAccumulatorBPM : public ::driver::misc::CmdSync {
    
      
      std::vector< ::driver::misc::ChaosDatasetAttribute*> va;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> vb;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> vc;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> vd;
      
      std::vector< ::driver::misc::ChaosDatasetAttribute*> dd;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> mode;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> acquire;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> samples;
      int elem_size;
      ::driver::misc::ChaosDatasetAttributeSinchronizer mode_sync;
public:
    CmdAcquireDafneAccumulatorBPM();
   
    virtual ~CmdAcquireDafneAccumulatorBPM();
    
    uint8_t implementedHandler();
    void acquireHandler();
    void  setHandler(c_data::CDataWrapper *data);


private:

};
        }
        }}
#endif	/* CMDDAFNEACCUMULATORBPM_H */

