/* 
 * File:   CmdAcquireDafneAccumulatorBPM.h
 * Author: michelo
 *
 * Created on October 15, 2015, 10:21 AM
 */

#ifndef CMDACQDAFNEACCUMULATORBPM_H
#define	CMDACQDAFNEACCUMULATORBPM_H
#include <driver/misc/core/CmdSync.h>
#include <driver/misc/core/ChaosDatasetAttributeSinchronizer.h>
namespace driver {
#define MAX_BPM_CONTAINER 32

        namespace daq {
            namespace libera{
                DEFINE_BATCH_COMMAND_CLASS(CmdAcquireDafneAccumulatorBPM,::driver::misc::CmdSync){
    
      
      std::vector< ::driver::misc::ChaosDatasetAttribute*> va;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> vb;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> vc;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> vd;
      
      std::vector< ::driver::misc::ChaosDatasetAttribute*> va_acq;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> vb_acq;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> vc_acq;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> vd_acq;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> sum_acq;

      std::vector< ::driver::misc::ChaosDatasetAttribute*> poly_type;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> x;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> y;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> x_acq;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> y_acq;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> mode;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> acquire;
      std::vector< ::driver::misc::ChaosDatasetAttribute*> samples;
      int elem_size;
      ::driver::misc::ChaosDatasetAttribute* dafne_status,*linac_mode;
      ::driver::misc::ChaosDatasetAttributeSinchronizer mode_sync;
      std::vector< ::driver::misc::ChaosDatasetAttribute* > rattrs;
      double coeff_u[MAX_BPM_CONTAINER][6],coeff_v[MAX_BPM_CONTAINER][6];
      c_data::CDataWrapper* last_command;
      int tomode,recover;
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

