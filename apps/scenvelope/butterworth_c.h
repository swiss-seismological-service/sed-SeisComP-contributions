/***************************************************************************
 *   Copyright (C) by ETHZ/SED                                             *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU Affero General Public License as published*
 * by the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Affero General Public License for more details.                     *
 *                                                                         *
 *   Developed by gempa GmbH                                               *
 ***************************************************************************/

#ifndef BUTT_C_H
#define BUTT_C_H

#ifdef __cplusplus
extern "C" {
#endif

#define BUTTER_MAX_ORDER 12

typedef struct cmplx
    {
        double real ;
        double imag ;
    } complex ;

void highpass (double fc, double dt, int n, complex *p, double *b);

void lowpass    (double fc, double dt, int n, complex *p,double *b);

void filt (double a1, double a2, double b1, double b2, int npts, double *fi, double *fo, double *d1, double *d2);

complex mul_c (complex u, complex v);

complex add_c (complex u, complex v);

complex     cmul_c (double a, complex u);

void wdat (char *FF, int npts, double *ar);

#ifdef __cplusplus
}
#endif

#endif
