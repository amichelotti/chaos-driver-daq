/* 
 * File:   CmdEnvDafneAccumulatorBPM.h
 * Author: michelo
 *
 * Created on October 15, 2015, 10:21 AM
 */

#ifndef CMDENVDAFNEACCUMULATORBPM_H
#define	CMDENVDAFNEACCUMULATORBPM_H
#include <driver/misc/CmdSync.h>
namespace driver {
        
        namespace daq {
            namespace libera{
class CmdEnvDafneAccumulatorBPM : public driver::misc::CmdSync {
    
      
      std::vector<driver::misc::ChaosDatasetAttribute*> va;
      std::vector<driver::misc::ChaosDatasetAttribute*> vb;
      std::vector<driver::misc::ChaosDatasetAttribute*> vc;
      std::vector<driver::misc::ChaosDatasetAttribute*> vd;
      
      std::vector<driver::misc::ChaosDatasetAttribute*> dd;
      std::vector<driver::misc::ChaosDatasetAttribute*> mode;
      std::vector<driver::misc::ChaosDatasetAttribute*> acquire;
      std::vector<driver::misc::ChaosDatasetAttribute*> samples;
      int elem_size;
public:
    CmdEnvDafneAccumulatorBPM();
   
    virtual ~CmdEnvDafneAccumulatorBPM();
    
    uint8_t implementedHandler();
    void acquireHandler();
    void  setHandler(c_data::CDataWrapper *data);


private:

};
        }
        }}
#endif	/* CMDDAFNEACCUMULATORBPM_H */
