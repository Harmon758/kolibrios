/*
 * Load reference 16x13 area for luma 8x8 MC
 * Copyright © <2010>, Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file was originally licensed under the following license
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
// Kernel name: LoadRef_Y_16x13.asm
//
// Load reference 16x13 area for luma 8x8 MC


//#if !defined(__LOADREF_Y_16x13__)		// Make sure this is only included once
//#define __LOADREF_Y_16x13__

#if 1

#if 1

	// Check whether MVX is integer MV
	and.z.f0.0 (1) null:w			r[pMV,0]<0;1,0>:w				0x3:w

	// Compute integer and fractional components of MV
    and (2)		gMVX_FRAC<1>:w		r[pMV,0]<2;2,1>:w				0x03:w	//{NoDDClr}
    asr (2)		gMVX_INT<1>:w		r[pMV,0]<2;2,1>:w				0x02:w	//{NoDDChk}
    
    // Check whether MVY is integer
	or.z.f0.1 (8) null:w			gMVY_FRAC<0;1,0>:w				0:w
	
	// If MVX is a multiple of 4 (..., -4, 0, 4, ...) integer MV, do special handling
	(f0.0)	jmpi (1)	INTERLABEL(LOADREF_MVXZERO)	
	
    // Set message descriptor
    (f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					RESP_LEN(4):ud	
	(-f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					RESP_LEN(7):ud
	
	// Compute top-left corner position to be loaded
	// TODO: sel
    (-f0.1) add (2)	gMSGSRC.0<1>:d	gMVX_INT<2;2,1>:w				-0x02:d //{NoDDClr}
    (-f0.1) mov (1)	gMSGSRC.2:ud	0x000c000c:ud							//{NoDDChk}
    (f0.1) add (1)	gMSGSRC.0<1>:d	gMVX_INT<0;1,0>:w				-0x02:d //{NoDDClr}
	(f0.1) mov (1)	gMSGSRC.1<1>:d	gMVY_INT<0;1,0>:w						//{NoDDChk,NoDDClr}
    (f0.1) mov (1)	gMSGSRC.2:ud	0x0007000c:ud							//{NoDDChk}

    // Read 16x13 pixels
    send (8)	gudREF(0)<1>		mMSGHDRY						gMSGSRC<8;8,1>:ud	DAPREAD	pMSGDSC:ud
    
    jmpi INTERLABEL(EXIT_LOADREF_Y_16x13)
    
INTERLABEL(LOADREF_MVXZERO):
    
    // Set message descriptor
#ifdef DEV_ILK
    (f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					0x00200000:ud	
	(-f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					0x00400000:ud
#else
    (f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					0x00020000:ud	
	(-f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					0x00040000:ud
#endif
	
	// Compute top-left corner position to be loaded
	// TODO: sel
	mov (2)	gMSGSRC.0<1>:d	gMVX_INT<2;2,1>:w
    (-f0.1) add (1)	gMSGSRC.1<1>:d	gMVY_INT<0;1,0>:w				-0x02:d
    (-f0.1) mov (1)	gMSGSRC.2:ud	0x000c0007:ud							//{NoDDChk}
    (f0.1) mov (1)	gMSGSRC.2:ud	0x00070007:ud							//{NoDDChk}

    // Read 16x13 pixels
    send (8)	gudREF(0)<1>		mMSGHDRY						gMSGSRC<8;8,1>:ud	DAPREAD	pMSGDSC:ud
    

#else
	// Compute integer and fractional components of MV
    and (2)		gMVX_FRAC<1>:w		r[pMV,0]<2;2,1>:w				0x03:w	//{NoDDClr}
    asr (2)		gMVX_INT<1>:w		r[pMV,0]<2;2,1>:w				0x02:w	//{NoDDChk}
    
    // Check whether MVY is integer
	or.z.f0.1 (8) null:w			gMVY_FRAC<0;1,0>:w				0:w
	
    // Set message descriptor
#ifdef DEV_ILK
    (f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					0x00400000:ud	
	(-f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					0x00700000:ud
#else
    (f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					0x00040000:ud	
	(-f0.1) add (1)	pMSGDSC:ud		gMSGDSC_R:ud					0x00070000:ud
#endif
	
	// Compute top-left corner position to be loaded
	// TODO: sel
    (-f0.1) add (2)	gMSGSRC.0<1>:d	gMVX_INT<2;2,1>:w				-0x02:d //{NoDDClr}
    (-f0.1) mov (1)	gMSGSRC.2:ud	0x000c000c:ud							//{NoDDChk}
    (f0.1) add (1)	gMSGSRC.0<1>:d	gMVX_INT<0;1,0>:w				-0x02:d //{NoDDClr}
	(f0.1) mov (1)	gMSGSRC.1<1>:d	gMVY_INT<0;1,0>:w						//{NoDDChk,NoDDClr}
    (f0.1) mov (1)	gMSGSRC.2:ud	0x0007000c:ud							//{NoDDChk}

    // Read 16x13 pixels
    send (8)	gudREF(0)<1>		mMSGHDRY						gMSGSRC<8;8,1>:ud	DAPREAD	pMSGDSC:ud
#endif
    
#else   

	// Compute integer and fractional components of MV
    and (2)		gMVX_FRAC<1>:w		r[pMV,0]<2;2,1>:w				0x03:w	{NoDDClr} //
    asr (2)		gMVX_INT<1>:w		r[pMV,0]<2;2,1>:w				0x02:w	{NoDDChk} //
    
    // Set message descriptor
#ifdef DEV_ILK
    add (1)		pMSGDSC:ud			gMSGDSC_R:ud					0x00700000:ud
#else
    add (1)		pMSGDSC:ud			gMSGDSC_R:ud					0x00070000:ud
#endif	// DEV_ILK
    
	// Compute top-left corner position to be loaded
    add (2)		gMSGSRC.0<1>:d		gMVX_INT<2;2,1>:w				-0x02:d {NoDDClr} //
    mov (1)		gMSGSRC.2:ud		0x000c000c:ud							{NoDDChk} //

    // Read 16x13 pixels
    send (8)	gudREF(0)<1>	    mMSGHDRY						gMSGSRC<8;8,1>:ud	DAPREAD	pMSGDSC:ud
    
#endif

INTERLABEL(EXIT_LOADREF_Y_16x13):

//#endif	// !defined(__LOADREF_Y_16x13__)
