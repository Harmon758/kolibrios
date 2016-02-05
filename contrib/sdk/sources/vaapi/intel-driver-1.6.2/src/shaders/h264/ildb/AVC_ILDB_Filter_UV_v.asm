/*
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
////////// AVC LDB filter vertical UV ///////////////////////////////////////////////////////
//
//	This filter code prepares the src data and control data for ILDB filtering on all vertical edges of UV.
//
//	It sssumes the data for vertical de-blocking is already transposed.  
//
//		Chroma:
//
//		+-------+-------+
//		|		|		|
//		|		|		|
//		|		|		|
//		+-------+-------+
//		|		|		|
//		|		|		|
//		|		|		|
//		+-------+-------+
//
//		V0		V1		
//		Edge	Edge	
//
/////////////////////////////////////////////////////////////////////////////

#if defined(_DEBUG) 
	mov		(1)		EntrySignatureC:w			0xBBBC:w
#endif	

//=============== Chroma deblocking ================

	and.z.f0.0  (1) null:w		r[ECM_AddrReg, BitFlags]:ub		FilterLeftMbEdgeFlag:w		// Check for FilterLeftMbEdgeFlag 
//    (f0.0)	jmpi	BYPASS_EXT_LEFT_EDGE_UV	
 
	// Get vertical border edge control data.  
	
	// Get Luma maskA and maskB	
	shr (16)	TempRow0(0)<1>		r[ECM_AddrReg, wEdgeCntlMapA_ExtLeftVert0]<0;1,0>:uw		RRampW(0)
	shr (16)	TempRow1(0)<1>		r[ECM_AddrReg, wEdgeCntlMapB_ExtLeftVert0]<0;1,0>:uw		RRampW(0)
	
    (f0.0)	jmpi	ILDB_LABEL(BYPASS_EXT_LEFT_EDGE_UV)

	// Extract UV MaskA and MaskB from every other bit of Y masks
	and.nz.f0.0 (8) null:w			TempRow0(0)<16;8,2>		1:w
	and.nz.f0.1 (8) null:w			TempRow1(0)<16;8,2>		1:w

//---------- Deblock U external edge ----------
	//	p1 = Prev MB U row 0
	//	p0 = Prev MB U row 1
	// 	q0 = Cur MB U row 0
	//	q1 = Cur MB U row 1
	mov (1)	P_AddrReg:w		PREV_MB_U_BASE:w									{ NoDDClr }
	mov (1)	Q_AddrReg:w		SRC_MB_U_BASE:w										{ NoDDChk }

	// alpha = bAlphaLeft0_Cb, beta = bBetaLeft0_Cb
	mov	(2)	alpha<1>:w	r[ECM_AddrReg, bAlphaLeft0_Cb]<2;2,1>:ub				{ NoDDClr }
	// tc0 has bTc0_v30_0_Cb + bTc0_v20_0_Cb + bTc0_v10_0_Cb + bTc0_v00_0_Cb
	mov (4)	tc0<1>:ub	r[ECM_AddrReg, bTc0_v00_0_Cb]<4;4,1>:ub					{ NoDDChk }
	
	// UV MaskA and MaskB
	mov (2)		MaskA<1>:uw			f0.0<2;2,1>:uw

	CALL(FILTER_UV, 1)	

//---------- Deblock V external edge ----------
	//	p1 = Prev MB V row 0
	//	p0 = Prev MB V row 1
	// 	q0 = Cur MB V row 0
	//	q1 = Cur MB V row 1
	mov (1)	P_AddrReg:w		PREV_MB_V_BASE:w									{ NoDDClr }		
	mov (1)	Q_AddrReg:w		SRC_MB_V_BASE:w										{ NoDDChk }

	// for vert edge: alpha = bAlphaLeft0_Cr, beta = bBetaLeft0_Cr
	mov	(2)	alpha<1>:w	r[ECM_AddrReg, bAlphaLeft0_Cr]<2;2,1>:ub				{ NoDDClr }
	
	// tc0 has bTc0_v30_0_Cr + bTc0_v20_0_Cr + bTc0_v10_0_Cr + bTc0_v00_0_Cr
	mov (4)	tc0<1>:ub	r[ECM_AddrReg, bTc0_v00_0_Cr]<4;4,1>:ub					{ NoDDChk }

	// UV MaskA and MaskB
	mov (2)		f0.0<1>:uw		MaskA<2;2,1>:uw

	CALL(FILTER_UV, 1)	


ILDB_LABEL(BYPASS_EXT_LEFT_EDGE_UV):
	// Set EdgeCntlMap2 = 0, so it always uses bS < 4 algorithm.
	// Same alpha and beta for all internal vert and horiz edges 


	//***** Need to take every other bit to form U or V maskA
	// Get Luma maskA and maskB	
	shr (16)	TempRow0(0)<1>		r[ECM_AddrReg, wEdgeCntlMap_IntMidVert]<0;1,0>:uw		RRampW(0)

//---------- Deblock U internal edge ----------
	//	p1 = Cur MB U row 2
	//	p0 = Cur MB U row 3
	// 	q0 = Cur MB U row 4
	//	q1 = Cur MB U row 5
	mov (1)	P_AddrReg:w		4*UV_ROW_WIDTH+SRC_MB_U_BASE:w					{ NoDDClr }
	mov (1)	Q_AddrReg:w		8*UV_ROW_WIDTH+SRC_MB_U_BASE:w					{ NoDDChk }

	// alpha = bAlphaInternal_Cb, beta = bBetaInternal_Cb
	mov	(2)	alpha<1>:w	r[ECM_AddrReg, bAlphaInternal_Cb]<2;2,1>:ub 		{ NoDDClr }

	// tc0 has bTc0_v32_Cb + bTc0_v22_Cb + bTc0_v12_Cb + bTc0_v02_Cb	
	mov (4)	tc0<1>:ub	r[ECM_AddrReg, bTc0_v02_Cb]<4;4,1>:ub				{ NoDDChk }

	// Extract UV MaskA and MaskB from every other bit of Y masks
	and.nz.f0.0 (8) null:w			TempRow0(0)<16;8,2>		1:w

	// UV MaskA and MaskB
	mov (1) f0.1:uw		0:w
	mov (1)	MaskB:uw	0:w													{ NoDDClr }
	mov (1)	MaskA:uw	f0.0:uw												{ NoDDChk }
	
	CALL(FILTER_UV, 1)	


//---------- Deblock V internal edge ----------
	//	P1 = Cur MB V row 2
	//	P0 = Cur MB V row 3
	// 	Q0 = Cur MB V row 4
	//	Q1 = Cur MB V row 5
	mov (1)	P_AddrReg:w		4*UV_ROW_WIDTH+SRC_MB_V_BASE:w					{ NoDDClr }
	mov (1)	Q_AddrReg:w		8*UV_ROW_WIDTH+SRC_MB_V_BASE:w					{ NoDDChk }

	// alpha = bAlphaInternal_Cr, beta = bBetaInternal_Cr
	mov	(2)	alpha<1>:w	r[ECM_AddrReg, bAlphaInternal_Cr]<2;2,1>:ub 		{ NoDDClr }	

	// tc0 has bTc0_v32_Cr + bTc0_v22_Cr + bTc0_v12_Cr + bTc0_v02_Cr	
	mov (4)	tc0<1>:ub	r[ECM_AddrReg, bTc0_v02_Cr]<4;4,1>:ub				{ NoDDChk }

	// UV MaskA and MaskB
	mov (2)		f0.0<1>:uw		MaskA<2;2,1>:uw

	CALL(FILTER_UV, 1)	


//BYPASS_4x4_DEBLOCK_V:
