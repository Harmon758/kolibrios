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
// Module name: load_Cur_Y_Right_Most_4x16.asm
//
// Load luma cur MB right most 4x16 into LEFT_TEMP_B

#if defined(_DEBUG) 
	mov		(1)		EntrySignatureC:w			0xDDD0:w
#endif


#if defined(_PROGRESSIVE) 
	// Read Y 
    add (1)	MSGSRC.0<1>:ud	ORIX_CUR:w		12:w	{ NoDDClr }				// Block origin, move right 12 bytes
    mov (1)	MSGSRC.1<1>:ud	ORIY_CUR:w				{ NoDDClr, NoDDChk }	// Block origin
    mov (1)	MSGSRC.2<1>:ud	0x000F0003:ud			{ NoDDChk }				// Block width and height (4x16)
    send (8) LEFT_TEMP_D(0)<1>	MSGHDRL		MSGSRC<8;8,1>:ud	DAPREAD	RESP_LEN(2)+DWBRMSGDSC_RC+BI_DEST_Y	
#endif


#if defined(_FIELD) || defined(_MBAFF)

    // FieldModeCurrentMbFlag determines how to access left MB
	and.z.f0.0 (1) 	null:w		r[ECM_AddrReg, BitFlags]:ub		FieldModeCurrentMbFlag:w		

    and.nz.f0.1 (1)	NULLREGW 		BitFields:w  	BotFieldFlag:w	// Get bottom field flag

	// Read Y
    add (1)	MSGSRC.0<1>:ud	ORIX_CUR:w		12:w	{ NoDDClr }				// Block origin, move right 12 bytes
    mov (1)	MSGSRC.1<1>:ud	ORIY_CUR:w				{ NoDDClr, NoDDChk }	// Block origin
    mov (1)	MSGSRC.2<1>:ud	0x000F0003:ud			{ NoDDChk }				// Block width and height (4x16)
    
    // Set message descriptor, etc.
    
	(f0.0)	if	(1)		ILDB_LABEL(ELSE_Y_4x16T)

    // Frame picture
    mov (1)	MSGDSC	RESP_LEN(2)+DWBRMSGDSC_RC+BI_DEST_Y:ud			// Read 2 GRFs from DEST_Y
    
	(f0.1) add (1)	MSGSRC.1:d		MSGSRC.1:d		16:w		// Add vertical offset 16 for bot MB in MBAFF mode
    
ILDB_LABEL(ELSE_Y_4x16T): 
	else 	(1)		ILDB_LABEL(ENDIF_Y_4x16T)

	// Field picture
    (f0.1) mov (1)	MSGDSC	RESP_LEN(2)+DWBRMSGDSC_RC_BF+BI_DEST_Y:ud  // Read 2 GRFs from DEST_Y bottom field
    (-f0.1) mov (1)	MSGDSC	RESP_LEN(2)+DWBRMSGDSC_RC_TF+BI_DEST_Y:ud  // Read 2 GRFs from DEST_Y top field

	endif
ILDB_LABEL(ENDIF_Y_4x16T):

//    send (8) BUF_D(0)<1>	MSGHDRY	MSGSRC<8;8,1>:ud	MSGDSC
    send (8) LEFT_TEMP_D(0)<1>	MSGHDRL		MSGSRC<8;8,1>:ud	DAPREAD	MSGDSC
#endif

//	Transpose 4x16 to 16x4

//	Input received from dport:
//	+-----------------------+-----------------------+-----------------------+-----------------------+
//	|73 72 71 70 63 62 61 60 53 52 51 50 43 42 41 40 33 32 31 30 23 22 21 20 13 12 11 10 03 02 01 00|
//	+-----------------------+-----------------------+-----------------------+-----------------------+
//	|f3 f2 f1 f0 e3 e2 e1 e0 d3 d2 d1 d0 c3 c2 c1 c0 b3 b2 b1 b0 a3 a2 a1 a0 93 92 91 90 83 82 81 80|
//	+-----------------------+-----------------------+-----------------------+-----------------------+

//	Output of transpose:		<1>	<= <32;8,4>
//	+-----------------------+-----------------------+-----------------------+-----------------------+
//	|f1 e1 d1 c1 b1 a1 91 81 71 61 51 41 31 21 11 01 f0 e0 d0 c0 b0 a0 90 80 70 60 50 40 30 20 10 00|
//	+-----------------------+-----------------------+-----------------------+-----------------------+
//	|f3 e3 d3 c3 b3 a3 93 83 73 63 53 43 33 23 13 03 f2 e2 d2 c2 b2 a2 92 82 72 62 52 42 32 22 12 02|
//	+-----------------------+-----------------------+-----------------------+-----------------------+
/*
	// Transpose the data, also occupy 2 GRFs
	mov (16)	PREV_MB_YB(0)<1>			BUF_B(0, 0)<32;8,4>		{ NoDDClr }
	mov (16)	PREV_MB_YB(0, 16)<1>		BUF_B(0, 1)<32;8,4>		{ NoDDChk }
	mov (16)	PREV_MB_YB(1)<1>			BUF_B(0, 2)<32;8,4>		{ NoDDClr }
	mov (16)	PREV_MB_YB(1, 16)<1>		BUF_B(0, 3)<32;8,4>		{ NoDDChk }
*/
// End of load_Y_4x16T

