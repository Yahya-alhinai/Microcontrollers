#include <p24Fxxxx.h>
#include "xc.h"
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void user_volatge(void){
    int i=0;
    if (ON_OFF == 0) {
        power ^= 1;
    }
    if (power == 1) {
        if(outputX==0){
            //output_x =(output_x+1)& 4;
            if (output_x<4){
            output_x++;
            }else{
            output_x=0;
            }
        }
        if(incV==0){
            switch (output_x) {
                case 0:
                if(v_set[0]<limit_HV){
                    v_set[0]=v_set[0]+0.1;
                    V_SET[0]=(v_set[0]/16.94)*3.33;
                }   
                break;
                case 1:
                    if(v_set[1]<limit_HV){
                        v_set[1]=v_set[1]+0.1;
                        V_SET[1]=(v_set[1]/16.94)*3.33;
                    }
                break;
                case 2:
                    if(v_set[2]<limit_HV){
                        v_set[2]=v_set[2]+0.1;
                        V_SET[2]=(v_set[2]/16.94)*3.33;
                    }
                break;
                case 3:
                    if(v_set[3]<limit_HV){
                        v_set[3]=v_set[3]+0.1;
                        V_SET[3]=(v_set[3]/16.94)*3.33;
                    }
                break;
                case 4:
                    if(v_set[4]<limit_HV){
                        v_set[4]=v_set[4]+0.1;
                        V_SET[4]=(v_set[4]/16.94)*3.33;
                    
                    }
                break;
                default:
                    for (i=0;i<5;i++){
                        V_SET[i]=0;
                    }             
            }        
        }
        if(decV==0){
            switch (output_x) {
                case 0:
                if(v_set[0]>limit_LV){
                    v_set[0]=v_set[0]-0.1;
                    V_SET[0]=(v_set[0]/16.94)*3.33;
                }
                break;
                case 1:
                if(v_set[1]>limit_LV){    
                    v_set[1]=v_set[1]-0.1;
                    V_SET[1]=(v_set[1]/16.94)*3.33;
                }
                break;
                case 2:
                if(v_set[2]>limit_LV){
                    v_set[2]=v_set[2]-0.1;
                    V_SET[2]=(v_set[2]/16.94)*3.33;
                }
                break;
                case 3:
                if(v_set[3]>limit_LV){
                    v_set[3]=v_set[3]-0.1;
                    V_SET[3]=(v_set[3]/16.94)*3.33;
                }
                break;
                case 4:
                if(v_set[4]>limit_LV){
                    v_set[4]=v_set[4]-0.1;
                    V_SET[4]=(v_set[4]/16.94)*3.33;
                }
                break;
                default:
                    for (i=0;i<5;i++){
                        V_SET[i]=0;
                    }     
            }        
        }
    }
}
