/* 
 * File:   LiberaData.h
 * Author: michelo
 *
 * Created on May 11, 2015, 11:26 AM
 */

#ifndef LIBERADATA_H
#define	LIBERADATA_H
#include <stdint.h>
#define LIBERA_IOP_MODE_DD 0x1 // data acquire on demand
#define LIBERA_IOP_MODE_SA 0x2 // streaming data acquire
#define LIBERA_IOP_MODE_ADC 0x4 // ADC data acquire
#define LIBERA_IOP_MODE_PM 0x8 // Post Mortem data acquire
#define LIBERA_IOP_MODE_AVG 0x10 // Average data acquire

#define LIBERA_IOP_MODE_TRIGGERED 0x100
#define LIBERA_IOP_MODE_DECIMATED 0x200
#define LIBERA_IOP_MODE_CONTINUOUS 0x400
#define LIBERA_IOP_MODE_SINGLEPASS 0x800

#define LIBERA_IOP_CMD_ACQUIRE 0x1
#define LIBERA_IOP_CMD_SETENV 0x2 // Setting environment
#define LIBERA_IOP_CMD_GETENV 0x3 // getting environment
#define LIBERA_IOP_CMD_SETTIME 0x4 // Setting Time
#define LIBERA_IOP_CMD_SET_OFFSET 0x5 // set offset in buffer
#define LIBERA_IOP_CMD_SET_SAMPLES 0x6 // set offset in buffer
#define LIBERA_IOP_CMD_STOP 0x7
#define LIBERA_IOP_CMD_GET_TS 0x8 // get time stamps

// ERROR
#define LIBERA_ERROR_READING 0x1
#define LIBERA_ERROR_WRITING 0x2
#define LIBERA_ERROR_STOP_ACQUIRE 0x4
#define LIBERA_ERROR_SWCONFIG 0x8
#define LIBERA_ERROR_ALLOCATE_DATASET 0x10
#define LIBERA_ERROR_SETTING_ENV 0x20
#include <ostream>
#include <vector>
#include <iostream>
#include <sstream>
#define ILK_PARAMCOUNT 8

#define DECLARE_DESC(_T) \
typedef struct _T: public libera_desc {\
static const char* desc[];\
static int size;\
public:\
_T():libera_desc(std::vector<const char*>(desc,desc+size)){}\
} _T ##_t;
    
    
#define DEFINE_DESC(_T,...) \
const char*_T::desc[]=__VA_ARGS__;\
int _T::size=sizeof(desc)/sizeof(const char*);
   
   
#define CHANNEL_DD 0
#define CHANNEL_SA 1
#define CHANNEL_SP 2
#define CHANNEL_AVG 3
#define CHANNEL_ENV 4
    

typedef struct libera_env {
    uint64_t selector;
    int32_t value;
} libera_env_t;

#ifdef CSPI
#include <driver/daq/models/Libera/cspi/cspi.h>

    typedef CSPI_DD_ATOM libera_dd_t;
    typedef CSPI_SA_ATOM libera_sa_t;
    typedef CSPI_ADC_CW_ATOM libera_cw_t;
    typedef CSPI_ADC_SP_ATOM libera_sp_t;
    typedef CSPI_AVERAGE_ATOM libera_avg_t;
    typedef CSPI_AVERAGE_ATOM libera_avg_t;
    typedef CSPI_TIMESTAMP libera_ts_t;
    
    class libera_desc{
    protected:
        std::vector<const char*>p;
    public:
        libera_desc(std::vector< const char* > desc){
            int cnt=0;
            p=desc;
            
            
        }
        std::vector<const char*> getDesc() const {return p;}
        friend std::ostream& operator<<(std::ostream&os,const libera_desc&data);
        
    };
    
    std::ostream& operator<<(std::ostream&os,const libera_desc&data);
    
    
    DECLARE_DESC(libera_dd_desc);
    DECLARE_DESC(libera_sa_desc);
    DECLARE_DESC(libera_cw_desc);
    DECLARE_DESC(libera_sp_desc);
    DECLARE_DESC(libera_avg_desc);

    std::ostream& operator <<(std::ostream&os,const libera_dd_t& data);   
    std::ostream& operator <<(std::ostream&os,const libera_sa_t& data);  
    std::ostream& operator <<(std::ostream&os,const libera_cw_t& data);
    std::ostream& operator <<(std::ostream&os,const libera_sp_t& data); 
    std::ostream& operator <<(std::ostream&os,const libera_avg_t& data);
   
    
   std::stringstream& operator<<(std::stringstream& os, const CSPI_ENVPARAMS& obj);

#else
#error "NO LIBERA PLATFORM SPECIFIED"
#endif
            
    
    
#endif	/* LIBERADATA_H */

