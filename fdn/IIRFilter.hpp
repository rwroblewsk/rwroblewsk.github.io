/** @file IIRFilter.hpp
 *  @brief IIR Filter class header file.
 *
 *  This contains the class definition for the IIRFilter class.
 *  This class only supports a fixed-length filter of 5 coefficients.
 *  where the coefficients must be set before filtering.
 *
 *  @author Rebecca Wroblewski
 *  @date 3/4/25
 */

#ifndef IIRFilter_hpp
#define IIRFilter_hpp

#include <stdio.h>

class IIRFilter {
    
    // ToDo: Make filter length scalable
    static const int MAX_LEN = 5;
    
    float a[MAX_LEN];
    float b[MAX_LEN];
    
    float x[MAX_LEN];
    float y[MAX_LEN];
    
    int ptr = 0;
    
public:

    /** @brief Resets pointer, buffers, and filter coefficients.
    *  @param new_a The pointer to filter feedback coefficients of length 5.
    *  @param new_b The pointer to the filter feedforward coefficients of length 5.
    *  @return Void.
    */ 
    void setup(float *new_a, float *new_b);

    /** @brief Filters a new sample through the IIR filter.
    *  @param new_x The new sample to filter.
    *  @return The filtered sample.
    */
    float filter(float new_x);
    
};

#endif /* IIRFilter_hpp */
