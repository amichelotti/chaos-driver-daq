/* 
 * File:   LiberaData.cpp
 * Author: michelo
 *
 * Created on May 11, 2015, 11:26 AM
 */
#include "models/Libera/LiberaData.h"

DEFINE_DESC(libera_dd_desc,{"VA","VB","VC","VD","X","Y","Q","SUM"});

DEFINE_DESC(libera_sa_desc,{"VA","VB","VC","VD","X","Y","Q","CX","Cy","SUM"});
DEFINE_DESC(libera_sp_desc,{"X","Y","BEFORE","AFTER","THRSH","TRIGGER","SUM"});
DEFINE_DESC(libera_cw_desc,{"QA","QB","QC","QD","X","Y","CHA","CHB","CHC","CHD","SUM"});
DEFINE_DESC(libera_avg_desc,{"AVG"});

 std::ostream& operator<<(std::ostream&os,const libera_desc&data){
  
            for (std::vector<const char*>::iterator i=data.getDesc().begin();i!=data.getDesc().end();i++){
                os<<*i<<",";
            }
            os<<std::endl;
            return os;
   
    }

 
 
          
    
    std::ostream& operator <<(std::ostream&os,const libera_dd_t& data){
        os<<std::dec<<data.Va<<","<<data.Vb<<","<<data.Vc<<","<<data.Vd<<","<<data.X<<","<<data.Y<<","<<data.Q<<","<<data.Sum<<std::endl;
        return os;
        
    }
    
    std::ostream& operator <<(std::ostream&os,const libera_sa_t& data){
        
        return os<<std::dec<<data.Va<<","<<data.Vb<<","<<data.Vc<<","<<data.Vd<<","<<data.X<<","<<data.Y<<","<<data.Q<<","<<data.Cx<<","<<data.Cy<<","<<data.Sum<<std::endl;
    }
    
    std::ostream& operator <<(std::ostream&os,const libera_cw_t& data){
        return os<<std::dec<<data.Qa<<","<<data.Qb<<","<<data.Qc<<","<<data.Qd<<","<<data.X<<","<<data.Y<<","<<data.chA<<","<<data.chB<<","<<data.chC<<","<<data.chD<<","<<data.Sum<<std::endl;
    }
    std::ostream& operator <<(std::ostream&os,const libera_sp_t& data){
        return os<<std::dec<<data.X<<","<<data.Y<<","<<data.n_before<<","<<data.n_after<<","<<data.threshold<<","<<data.trigger<<","<<data.Sum<<std::endl;
    }
    
    std::ostream& operator <<(std::ostream&os,const libera_avg_t& data){
        
        return os<<std::dec<<data.avesum<<std::endl;
    }
