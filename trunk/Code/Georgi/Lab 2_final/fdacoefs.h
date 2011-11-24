/*
 * Filter Coefficients (C Source) generated by the Filter Design and Analysis Tool
 *
 * Generated by MATLAB(R) 7.10 and the Signal Processing Toolbox 6.13.
 *
 * Generated on: 10-Nov-2011 13:08:59
 *
 */

/*
 * Discrete-Time FIR Filter (real)
 * -------------------------------
 * Filter Structure  : Direct-Form FIR
 * Filter Length     : 11
 * Stable            : Yes
 * Linear Phase      : Yes (Type 1)
 */

/* General type conversion for MATLAB generated C-code  */

/* 
 * Expected path to tmwtypes.h 
 * C:\Program Files\MATLAB\R2010a\extern\include\tmwtypes.h 
 */
/*
 * Warning - Filter coefficients were truncated to fit specified data type.  
 *   The resulting response may not match generated theoretical response.
 *   Use the Filter Design & Analysis Tool to design accurate
 *   single-precision filter coefficients.
 */
const u8 BL = 11;
const float B[11] = {
   -0.07324777544, -0.02121166326,  0.04746883363,   0.1530413926,    0.249984324,
     0.2893228531,    0.249984324,   0.1530413926,  0.04746883363, -0.02121166326,
   -0.07324777544
};
