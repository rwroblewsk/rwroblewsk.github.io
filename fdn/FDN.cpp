/** @file FDN.cpp
 *  @brief Feedback Delay Network class implementation file.
 *
 *  This contains the class implementation for the FDN class.
 *  This class only supports a fixed number of 4 delay paths.
 *
 *  @author Rebecca Wroblewski
 *  @date 3/1/25
 */

#include "FDN.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>

void FDN::setup(float min, float max, int sr){
    
    // ToDo: Allow num_delays <= MAX_NUM_DELAYS
    num_delays = 4;
    
    dmin = float(min);
    dmax = float(max);
    
    dmin = float(sr) * dmin / sound_speed;
    dmax = float(sr) * dmax / sound_speed;
    
    // Calculate delays to be mutually prime
    for (int i = 0; i < num_delays; i ++){
        float delay_s = dmin * pow((dmax/dmin), (float(i)/float((num_delays-1))));
        delays[i] = pow(primes[i], std::floor( std::log10(delay_s) / std::log10(primes[i]) ));
        if (delays[i] > MAX_DELAY){
            delays[i] = MAX_DELAY;
        }
    }
    
    // Reset buffers and filters
    for (int i = 0; i < num_delays; i ++){
        for (int j = 0; j < MAX_DELAY+1; j ++){
            delay_bufs[i][j] = 0.0;
        }
        for (int j = 0; j < MAX_FILTER_LEN; j ++){
            mat_out[i][j] = 0.0;
            filt_out[i][j] = 0.0;
        }
        
        filters[i].setup(filter_a[i], filter_b[i]);
    }
}

void FDN::filter(int16_t *outputData, const int16_t *inputData, int inputNumSamples, int mode){
    
    // Use mode to change decay in each band
    if (mode == 2){
        float g_temp[MAX_NUM_DELAYS] = {0.98, 0.95, 0.1, 0.1};
        memcpy(g, g_temp, sizeof(g_temp));
    }
    else if (mode == 3){
        float g_temp[MAX_NUM_DELAYS] = {0.1, 0.1, 0.7, 0.9};
        memcpy(g, g_temp, sizeof(g_temp));
    }
    else if (mode == 4){
        float g_temp[MAX_NUM_DELAYS] = {0.95, 0.92, 0.8, 0.9};
        memcpy(g, g_temp, sizeof(g_temp));
    }
    else if (mode == 5){
        float g_temp[MAX_NUM_DELAYS] = {0.8, 0.6, 0.8, 0.8};
        memcpy(g, g_temp, sizeof(g_temp));
    }
    
    
    // Cast input to float
    vDSP_vflt16(inputData, 1, inputFloat, 1, inputNumSamples);
    
    for (int n = 0; n < inputNumSamples; n ++){
        
        // Delay Stage
        for (int i = 0; i < num_delays; i ++){
            int ind = delay_ptrs[i]-delays[i];
            if(ind < 0){
                ind = MAX_DELAY + ind;
            }
            v[i] = delay_bufs[i][ind];
        }
            
        // Filter Stage
        if (mode != 1){
            for (int i = 0; i < num_delays; i ++){
                v[i] = g[i] * filters[i].filter(v[i]);
            }
        }
        
        // Mixing Stage
        float y_temp[num_delays];
        vDSP_mmul(Q, 1, v, 1, y_temp, 1, 4, 1, 4);
        
        // Alternate decay after mixing stage
        if (mode == 1){
            for (int i = 0; i < num_delays; i ++){
                y_temp[i] = g[i] * y_temp[i];
            }
        }  
            
        // Gain and Sum Stage
        int ind = 0;
        float sum = 0.0;
        for (int i = 0; i < num_delays; i ++){
            ind = delay_ptrs[i];
            
            // Change delay pointer
            if (ind+1 >= MAX_DELAY){
                ind = 0;
            }
            else{
                ind++;
            }
            
            delay_bufs[i][ind] = b[i] * inputFloat[n] + y_temp[i];
            delay_ptrs[i] = ind;
            
            // Output
            sum = sum + c[i] * v[i];
        }
            
        // Output Sum
        outputFloat[n] = sum + (d * inputFloat[n]);
        
    }
        
    // Cast output to int
    vDSP_vfix16(outputFloat, 1, outputData, 1, inputNumSamples);
}
