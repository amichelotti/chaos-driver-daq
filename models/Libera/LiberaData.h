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
#define LIBERA_IOP_MODE_PERMLOOP 0x1000

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
    
   #include <math.h>
 
#define DEFINE_DESC(_T,...) \
const char*_T::desc[]=__VA_ARGS__;\
int _T::size=sizeof(desc)/sizeof(const char*);
   
   
#define CHANNEL_DD 0
#define CHANNEL_SA 1
#define CHANNEL_SP 2
#define CHANNEL_AVG 3
#define CHANNEL_ENV 4
#include <cmath>    
#define FIX_NUM(g) \
if(std::isnormal(g)==false)g=0;

typedef struct libera_env {
    uint64_t selector;
    int32_t value;
} libera_env_t;


#include <driver/daq/models/Libera/driver/cspi/cspi.h>

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
struct bpmpos {
    float x;
    float y;
};


  
   inline bpmpos bpm_voltage_to_mm(const double *u[6],const double *v[6],int32_t va,int32_t vb,int32_t vc,int32_t vd){
    bpmpos pos;
    float x=0,y=0;
    if((va +vb +vc+vd)==0)return pos;
    float U= ((double)(vb +vd -va -vc))/(va +vb +vc+vd);
    float V= ((double)(va +vb -vc -vd))/(va +vb +vc+vd);
 
    for(int cnt=0;cnt<7;cnt++){
        x = *u[0] * U + *u[1] * y*y*U +  (*u[2])*y*y*y*y*U + (*u[3]) *x*x*U +(*u[4])*x*x*y*y*U+(*u[5])*x*x*x*x*U;
        y = *v[0] * V + (*v[1]) * y*y*V +  (*v[2])*y*y*y*y*V + (*v[3]) *x*x*V +(*v[4])*x*x*y*y*V+(*v[5])*x*x*x*x*V;
    }
    
    /*MATLAB*/
    /*Xs=0;
Ys=0;

for i=1:7

x=a(1)*U+a(2)*Ys^2*U+a(3)*Ys^4*U+a(4)*Xs^2*U+a(5)*Xs^2*Ys^2*U+a(6)*Xs^4*U;
y=b(1)*V+b(2)*Ys^2*V+b(3)*Ys^4*V+b(4)*Xs^2*V+b(5)*Xs^2*Ys^2*V+b(6)*Xs^4*V;

Xs=x;
Ys=y;

end*/
    FIX_NUM(x);
    FIX_NUM(y);
    pos.x=x;
    pos.y=y;
    return pos;
}

   inline bpmpos bpm_voltage_to_mm(int type,int32_t va,int32_t vb,int32_t vc,int32_t vd){
       
    const double a[2][6]={{28.5574,-0.046125,5.43125e-5,0.0172085,-1.15991e-5,1.94837e-7},{9.8435 ,-0.022408,0.00014638 ,0.034859 ,-1.4584e-6  ,-9.9279e-6}};
    const double b[2][6]={{28.5574,0.0172085,1.94837e-7,-0.046125,-1.15991e-5,5.43125e-5},{32.0137,0.0432143,0.000222447,-0.339764,-0.000318269,0.00167884}};
    const double *u[6],*v[6];
    for(int cnt=0;cnt<6;cnt++){
        u[cnt]=&a[type][cnt];
        v[cnt]=&b[type][cnt];
    }   
    return bpm_voltage_to_mm(u,v,va,vb,vc,vd);
   } 

    
    
#endif	/* LIBERADATA_H */

