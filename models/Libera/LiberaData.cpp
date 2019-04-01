/* 
 * File:   LiberaData.cpp
 * Author: michelo
 *
 * Created on May 11, 2015, 11:26 AM
 */
#include "models/Libera/LiberaData.h"
#include <iomanip>
#include <iterator>
DEFINE_DESC(libera_dd_desc,{"VA","VB","VC","VD","X","Y","Q","SUM"});

DEFINE_DESC(libera_sa_desc,{"VA","VB","VC","VD","X","Y","Q","CX","Cy","SUM"});
DEFINE_DESC(libera_sp_desc,{"X","Y","BEFORE","AFTER","THRSH","TRIGGER","SUM"});
DEFINE_DESC(libera_cw_desc,{"QA","QB","QC","QD","X","Y","CHA","CHB","CHC","CHD","SUM"});
DEFINE_DESC(libera_avg_desc,{"AVG"});


std::stringstream& operator<<(std::stringstream& os, const CSPI_ENVPARAMS& obj){
	const size_t tab = 17;
	const char* labels[] = {
		"Kx [nm]", "Ky [nm]",
		"Xoffset [nm]", "Yoffset [nm]", "Qoffset [nm]",
		"Switches",
		"Level [dBm]",
		"AGC",
		"DSC",
		//"Interlock",
		0
	};

	const int *p = reinterpret_cast<const int*>(&obj);
	
	// Health
	os << std::setw(tab) << "Temp [C]" << ": ";
	os << *p++ << std::endl;

	os << std::setw(tab) << "Fans [rpm]" << ": ";
	os << *p++ ;
	os << " " << *p++ << std::endl;

	os << std::setw(tab) << "Voltages [mV]" << ": ";

	for (size_t i=0; i<8; ++i) {
		os << *p++ << " ";
	}
	os << std::endl;

	os << std::setw(tab) << "SC PLL" << ": ";
	os << (*p++ ? "locked" : "unlocked") << std::endl;

	os << std::setw(tab) << "MC PLL" << ": ";
	os << (*p++ ? "locked" : "unlocked") << std::endl;

	// TRIGmode
	os << std::setw(tab) << "TRIGmode" << ": ";
	os << *p++ << std::endl;

	// Feature registers
	os << std::setw(tab) << "Feature" << ": 0x";
	os << std::hex << std::setfill('0');
	os << std::setw(8) << *p++ << " 0x";
	unsigned long feature = *p++;
	os << std::setw(8) << feature;
	os << std::dec << std::setfill(' ') ;
	if ( LIBERA_IS_BRILLIANCE(feature) )
		os << ", Brilliance";
	if ( LIBERA_IS_GBETHERNET(feature) ) {
		os << ", GbE";
		if ( LIBERA_IS_GBE_DEMO(feature) )
			os << "_demo";
	}
	if ( LIBERA_IS_DESY_MOLEX(feature) )
		os << ", Desy Molex";
	if ( LIBERA_IS_GROUPING(feature) )
		os << ", Grouping (RIO)";
	if ( LIBERA_IS_MAF(feature) )
		os << ", MAF";
	if ( LIBERA_IS_DCC(feature) )
		os << ", Communication Controller";
	os << std::endl;
	// From Kx/y on
	for (size_t i=0; labels[i]; ++i) {
		os << std::setw(tab) << labels[i] << ": " << *p++ << std::endl;
	}

	os << std::setw(tab) << "Interlock" << ": ";
	const int *q = p + ILK_PARAMCOUNT - 1;

	std::copy(p, q, std::ostream_iterator<int>(std::cout," "));
	os << *q << std::endl;
	p += ILK_PARAMCOUNT;

	os << std::setw(tab) << "InterlockStatus" << ": ";
	unsigned long ilkStatus = *p++;
	os << ilkStatus;
	if ( INTERLOCK_XPOSITION(ilkStatus) )
			os << ", X pos";
	if ( INTERLOCK_YPOSITION(ilkStatus) )
			os << ", Y pos";
	if ( INTERLOCK_ATT_LIMIT(ilkStatus) )
			os << ", attenuator limit";
	if ( INTERLOCK_ADC_OVERFLOW(ilkStatus) )
			os << ", ADC overflow";
	if ( INTERLOCK_ADC_NOT_FILTERED(ilkStatus) )
			os << ", ADC not filtered";
	os << std::endl;
	os << std::setw(tab) << "PostMortem" << ": ";
	os << obj.pm.mode << " ";
	os << obj.pm.min_x_limit << " ";
	os << obj.pm.max_x_limit << " ";
	os << obj.pm.min_y_limit << " ";
	os << obj.pm.max_y_limit << " ";
	os << obj.pm.overflow_limit << " ";
	os << obj.pm.overflow_dur << " ";
	os << std::endl;
	os << std::setw(tab) << "PMoffset, PMdec" << ": ";
	os << *p++ << ", ";
	os << *p++ << std::endl;
	os << std::setw(tab) << "TrigDelay" << ": ";
	os << *p++ << std::endl;

	os << std::setw(tab) << "ExternalSwitching" << ": ";
	os << (*p++ ? "external" : "internal") << std::endl;
	os << std::setw(tab) << "SwitchingDelay" << ": ";
	os << *p++ << std::endl;

	if ( LIBERA_IS_MAF(feature) ) {
		os << std::setw(tab) << "MAF Length, Delay" << ": ";
		os << obj.ddc_maflength << ", " << obj.ddc_mafdelay << std::endl;
	}

	const mt_status_t& mt_stat = obj.pll_status.mt_stat;
	const st_status_t& st_stat = obj.pll_status.st_stat;

	os << std::setw(tab) << "MT controller" << ": ";
	os << "RtmSts= " << mt_stat.status << " "
	<< "OffsTune= " << mt_stat.vcxo_offset << " "
	<< "CompTune= " << (mt_stat.nco_shift ? "yes":"no") << " "
	<< std::endl;
	os << std::setw(tab) << "ST controller" << ": ";
	os << "RtmSts= " << st_stat.status << " "
	<< std::endl;

	os << std::setw(tab) << "SpikeRemoval" << ": ";
	os << obj.sr.enable << " ";
	os << obj.sr.cspi_enable << " ";
	os << obj.sr.averaging_stop << " ";
	os << obj.sr.average_window << " ";
	os << obj.sr.start << " ";
	os << obj.sr.window << " ";
	os << std::endl;

	os << std::setw(tab) << "SinglePass" << ": ";
	os << obj.sp.threshold << " ";
	os << obj.sp.n_before << " ";
	os << obj.sp.n_after << " ";
	os << std::endl;

	os << std::setw(tab) << "MaxADC" << ": ";
	os << obj.max_adc << " "	<< std::endl;

	return os ;
}

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

 /*       
if(isnan(g)) g=0; \
else if(isinf(g)){g=(g>0)?std::numeric_limits<float>::max():std::numeric_limits<float>::min();}
   */     
    
