/** @file FDN.hpp
 *  @brief Feedback Delay Network class header file.
 *
 *  This contains the class definition for the FDN class.
 *  This class only supports a fixed number of 4 delay paths.
 *
 *  @author Rebecca Wroblewski
 *  @date 3/1/25
 */

#ifndef FDN_hpp
#define FDN_hpp

#include <stdio.h>
#include "FftModule.hpp"
#include "IIRFilter.hpp"

class FDN {
    static const int MAX_DELAY = 8192; // 16 * 512
    static const int MAX_NUM_DELAYS = 4;
    static const int MAX_FILTER_LEN = 5;

    const float sound_speed = 343;
    
    int num_delays = 4;   
    float dmin = 46.0;
    float dmax = 63.0;
    float primes[4] = {2.0, 3.0, 5.0, 7.0};
    
    int delays[MAX_NUM_DELAYS] = {0};
    float v[MAX_NUM_DELAYS] = {0, 0, 0, 0};
    float b[MAX_NUM_DELAYS] = {1, 1, 1, 1};
    float c[MAX_NUM_DELAYS] = {1, 1, 1, 1};
    float g[MAX_NUM_DELAYS] = {0.9322, 0.8648, 0.9140, 0.9131};
    
    // ToDo: Make filterbank perfect reconstruction
    // Butterworth filter coefficients for each delay line path
    float filter_a[MAX_NUM_DELAYS][MAX_FILTER_LEN] =
    {
        {1,-3.813865383597034,5.458723791505589,-3.474942611565109,0.830107707951724},
        {1,-3.879553284437346,5.664339785914956,-3.688851669815090,0.904163040865126},
        {1,-3.722107513923212,5.270608914926116,-3.364528562852842,0.817512403384761},
        {1,-3.256569310958798,4.033768392721253,-2.246043682026213,0.473506452939151}
    };
    float filter_b[MAX_NUM_DELAYS][MAX_FILTER_LEN] = {
        {1.469018448158382e-06,5.876073792633528e-06,8.814110688950292e-06,5.876073792633528e-06,1.469018448158382e-06},
        {0.001207405190260,0,-0.002414810380520,0,0.001207405190260},
        {0.004603998475022,0,-0.009207996950045,0,0.004603998475022},
        {0.688117989915339,-2.752471959661354,4.128707939492031,-2.752471959661354,0.688117989915339}
    };

    IIRFilter filters[MAX_NUM_DELAYS];
    
    float mat_out[MAX_NUM_DELAYS][MAX_FILTER_LEN] = {0.0};
    float filt_out[MAX_NUM_DELAYS][MAX_FILTER_LEN] = {0.0};
    
    // Output mix parameter
    float d = 0.8;
    
    // TODO: Make matrix scalable
    // Mixing matrix for 4 delays
    float Q[16] = {0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5};
    
    float delay_bufs[MAX_NUM_DELAYS][MAX_DELAY+1] = {0};
    int delay_ptrs[MAX_NUM_DELAYS] = {0};
    
    float inputFloat[MAX_DELAY] = {0};
    float outputFloat[MAX_DELAY] = {0};
    
public:

    /** @brief Resets buffers, filters, and delays.
    *  @param min The minimum path to a surface in meters.
    *  @param max The maximum path to a surface in meters.
    *  @param sr The sample rate in Hz.
    *  @return Void.
    */ 
    void setup(float min, float max, int sr);

    /** @brief Computes one frame of FDN output.
    *  @param outputData The pointer to one frame of output data
                         to be populated by the function.
    *  @param inputData The pointer to one frame of input data.
    *  @param inputNumSamples The number of samples in the input 
                              and output buffers.
    *  @param mode A flag used to select the decay in each band.
    *              0: Default decay
    *              1: No filtering, decay after mixing stage
    *              2: Slower decay in low bands
    *              3: Slower decay in high bands
    *              4: Alternate slower decay in all bands
    *              5: Faster decay in all bands
    *  @return Void.
    */ 
    void filter(int16_t *outputData, const int16_t *inputData, int inputNumSamples, int mode);
    
};

#endif /* FDN_hpp */
