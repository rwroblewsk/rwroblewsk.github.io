/** @file IIRFilter.cpp
 *  @brief IIR Filter class implementation file.
 *
 *  This contains the class implentation for the IIRFilter class.
 *  This class only supports a fixed-length filter of 5 coefficients.
 *  where the coefficients must be set before filtering.
 *
 *  @author Rebecca Wroblewski
 *  @date 3/4/25
 */

#include "IIRFilter.hpp"

void IIRFilter::setup(float *new_a, float *new_b){
    
    // Reset pointer, buffers, and filter coefficients
    for (int i = 0; i < MAX_LEN; i++){
        x[i] = 0.0;
        y[i] = 0.0;
        a[i] = new_a[i];
        b[i] = new_b[i];
        ptr = 0;
    }
}

float IIRFilter::filter(float new_x){
    
    // Calculate components from input and first delay (current pointer position)
    float sum = new_x * b[0];
    int point = ptr;
    sum = sum + x[point] * b[1];
    sum = sum - a[1] * y[point];
    
    // Iterate through the rest of the delayed samples
    for (int i = 2; i < MAX_LEN; i++){
        
        // Update circular buffer pointer for delayed samples
        if(point-1<0){
            point = MAX_LEN-1;
        }else{
            point--;
        }
        
        // Calculate feedback and feedforward components
        sum = sum + x[point] * b[i];
        sum = sum - a[i] * y[point];
    }
    
    // Update circular buffer pointer for new samples
    if (ptr+1 >= MAX_LEN){
        ptr = 0;
    }
    else{
        ptr++;
    }
    
    // Store current input and output
    x[ptr] = new_x;
    y[ptr] = sum;
    
    return sum;
}
