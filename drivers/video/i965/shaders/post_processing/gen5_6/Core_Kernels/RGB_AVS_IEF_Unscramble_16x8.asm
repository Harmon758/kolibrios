/*
 * All Video Processing kernels 
 * Copyright © <2010>, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Eclipse Public License (EPL), version 1.0.  The full text of the EPL is at
 * http://www.opensource.org/licenses/eclipse-1.0.php.
 *
 */

//---------- RGB_AVS_IEF_Unpack_16x8.asm ----------
#include "AVS_IEF.inc"

.declare DEST_B		Base=REG(r,10)	ElementSize=2 SrcRegion=REGION(8,1) DstRegion=<1> Type=uw
.declare DEST_G		Base=REG(r,18)	ElementSize=2 SrcRegion=REGION(8,1) DstRegion=<1> Type=uw
.declare DEST_R		Base=REG(r,26)	ElementSize=2 SrcRegion=REGION(8,1) DstRegion=<1> Type=uw
.declare DEST_A		Base=REG(r,34)	ElementSize=2 SrcRegion=REGION(8,1) DstRegion=<1> Type=uw


#ifdef AVS_OUTPUT_16_BIT
//This portion will need to be changed if unpacking is required for Y416 kernels (in case of blending etc) - vK

//// Move first 8x8 words of B to dest GRF (as packed)
//    mov (4) uwDEST_Y(0,2)<4>       uwAVS_RESPONSE(4,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(1,2)<4>       uwAVS_RESPONSE(4,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(4,2)<4>       uwAVS_RESPONSE(4,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(5,2)<4>       uwAVS_RESPONSE(4,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(8,2)<4>       uwAVS_RESPONSE(5,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(9,2)<4>       uwAVS_RESPONSE(5,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(12,2)<4>      uwAVS_RESPONSE(5,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(13,2)<4>      uwAVS_RESPONSE(5,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(16,2)<4>      uwAVS_RESPONSE(12,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(17,2)<4>      uwAVS_RESPONSE(12,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(20,2)<4>      uwAVS_RESPONSE(12,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(21,2)<4>      uwAVS_RESPONSE(12,12)<4;4,1>                                   
//    mov (4) uwDEST_Y(24,2)<4>      uwAVS_RESPONSE(13,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(25,2)<4>      uwAVS_RESPONSE(13,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(28,2)<4>      uwAVS_RESPONSE(13,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(29,2)<4>      uwAVS_RESPONSE(13,12)<4;4,1>                                   
//
//// Move first 8x8 words of G to dest GRF (as packed)
//    mov (4) uwDEST_Y(0,1)<4>       uwAVS_RESPONSE(2,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(1,1)<4>       uwAVS_RESPONSE(2,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(4,1)<4>       uwAVS_RESPONSE(2,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(5,1)<4>       uwAVS_RESPONSE(2,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(8,1)<4>       uwAVS_RESPONSE(3,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(9,1)<4>       uwAVS_RESPONSE(3,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(12,1)<4>      uwAVS_RESPONSE(3,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(13,1)<4>      uwAVS_RESPONSE(3,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(16,1)<4>      uwAVS_RESPONSE(10,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(17,1)<4>      uwAVS_RESPONSE(10,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(20,1)<4>      uwAVS_RESPONSE(10,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(21,1)<4>      uwAVS_RESPONSE(10,12)<4;4,1>                                   
//    mov (4) uwDEST_Y(24,1)<4>      uwAVS_RESPONSE(11,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(25,1)<4>      uwAVS_RESPONSE(11,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(28,1)<4>      uwAVS_RESPONSE(11,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(29,1)<4>      uwAVS_RESPONSE(11,12)<4;4,1>                                   
//
//// Move first 8x8 words of R to dest GRF (as packed)
//    mov (4) uwDEST_Y(0,0)<4>       uwAVS_RESPONSE(0,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(1,0)<4>       uwAVS_RESPONSE(0,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(4,0)<4>       uwAVS_RESPONSE(0,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(5,0)<4>       uwAVS_RESPONSE(0,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(8,0)<4>       uwAVS_RESPONSE(1,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(9,0)<4>       uwAVS_RESPONSE(1,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(12,0)<4>      uwAVS_RESPONSE(1,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(13,0)<4>      uwAVS_RESPONSE(1,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(16,0)<4>      uwAVS_RESPONSE(8,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(17,0)<4>      uwAVS_RESPONSE(8,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(20,0)<4>      uwAVS_RESPONSE(8,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(21,0)<4>      uwAVS_RESPONSE(8,12)<4;4,1>                                   
//    mov (4) uwDEST_Y(24,0)<4>      uwAVS_RESPONSE(9,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(25,0)<4>      uwAVS_RESPONSE(9,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(28,0)<4>      uwAVS_RESPONSE(9,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(29,0)<4>      uwAVS_RESPONSE(9,12)<4;4,1>                                   
//
//// Move first 8x8 words of A to dest GRF (as packed)
//    mov (4) uwDEST_Y(0,3)<4>       uwAVS_RESPONSE(6,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(1,3)<4>       uwAVS_RESPONSE(6,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(4,3)<4>       uwAVS_RESPONSE(6,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(5,3)<4>       uwAVS_RESPONSE(6,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(8,3)<4>       uwAVS_RESPONSE(7,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(9,3)<4>       uwAVS_RESPONSE(7,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(12,3)<4>      uwAVS_RESPONSE(7,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(13,3)<4>      uwAVS_RESPONSE(7,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(16,3)<4>      uwAVS_RESPONSE(14,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(17,3)<4>      uwAVS_RESPONSE(14,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(20,3)<4>      uwAVS_RESPONSE(14,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(21,3)<4>      uwAVS_RESPONSE(14,12)<4;4,1>                                   
//    mov (4) uwDEST_Y(24,3)<4>      uwAVS_RESPONSE(15,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(25,3)<4>      uwAVS_RESPONSE(15,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(28,3)<4>      uwAVS_RESPONSE(15,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(29,3)<4>      uwAVS_RESPONSE(15,12)<4;4,1>                                   
//
//// Move second 8x8 words of B to dest GRF
//    mov (4) uwDEST_Y(2,2)<4>       uwAVS_RESPONSE_2(4,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(3,2)<4>       uwAVS_RESPONSE_2(4,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(6,2)<4>       uwAVS_RESPONSE_2(4,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(7,2)<4>       uwAVS_RESPONSE_2(4,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(10,2)<4>      uwAVS_RESPONSE_2(5,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(11,2)<4>      uwAVS_RESPONSE_2(5,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(14,2)<4>      uwAVS_RESPONSE_2(5,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(15,2)<4>      uwAVS_RESPONSE_2(5,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(18,2)<4>      uwAVS_RESPONSE_2(12,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(19,2)<4>      uwAVS_RESPONSE_2(12,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(22,2)<4>      uwAVS_RESPONSE_2(12,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(23,2)<4>      uwAVS_RESPONSE_2(12,12)<4;4,1>                                   
//    mov (4) uwDEST_Y(26,2)<4>      uwAVS_RESPONSE_2(13,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(27,2)<4>      uwAVS_RESPONSE_2(13,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(30,2)<4>      uwAVS_RESPONSE_2(13,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(31,2)<4>      uwAVS_RESPONSE_2(13,12)<4;4,1>                                   
//
//// Move second 8x8 words of G to dest GRF
//    mov (4) uwDEST_Y(2,1)<4>       uwAVS_RESPONSE_2(2,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(3,1)<4>       uwAVS_RESPONSE_2(2,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(6,1)<4>       uwAVS_RESPONSE_2(2,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(7,1)<4>       uwAVS_RESPONSE_2(2,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(10,1)<4>      uwAVS_RESPONSE_2(3,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(11,1)<4>      uwAVS_RESPONSE_2(3,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(14,1)<4>      uwAVS_RESPONSE_2(3,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(15,1)<4>      uwAVS_RESPONSE_2(3,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(18,1)<4>      uwAVS_RESPONSE_2(10,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(19,1)<4>      uwAVS_RESPONSE_2(10,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(22,1)<4>      uwAVS_RESPONSE_2(10,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(23,1)<4>      uwAVS_RESPONSE_2(10,12)<4;4,1>                                   
//    mov (4) uwDEST_Y(26,1)<4>      uwAVS_RESPONSE_2(11,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(27,1)<4>      uwAVS_RESPONSE_2(11,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(30,1)<4>      uwAVS_RESPONSE_2(11,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(31,1)<4>      uwAVS_RESPONSE_2(11,12)<4;4,1>                                   
//
//// Move second 8x8 words of R to dest GRF
//    mov (4) uwDEST_Y(2,0)<4>       uwAVS_RESPONSE_2(0,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(3,0)<4>       uwAVS_RESPONSE_2(0,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(6,0)<4>       uwAVS_RESPONSE_2(0,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(7,0)<4>       uwAVS_RESPONSE_2(0,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(10,0)<4>      uwAVS_RESPONSE_2(1,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(11,0)<4>      uwAVS_RESPONSE_2(1,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(14,0)<4>      uwAVS_RESPONSE_2(1,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(15,0)<4>      uwAVS_RESPONSE_2(1,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(18,0)<4>      uwAVS_RESPONSE_2(8,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(19,0)<4>      uwAVS_RESPONSE_2(8,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(22,0)<4>      uwAVS_RESPONSE_2(8,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(23,0)<4>      uwAVS_RESPONSE_2(8,12)<4;4,1>                                   
//    mov (4) uwDEST_Y(26,0)<4>      uwAVS_RESPONSE_2(9,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(27,0)<4>      uwAVS_RESPONSE_2(9,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(30,0)<4>      uwAVS_RESPONSE_2(9,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(31,0)<4>      uwAVS_RESPONSE_2(9,12)<4;4,1>                                   
//
//// Move second 8x8 words of A to dest GRF
//    mov (4) uwDEST_Y(2,3)<4>       uwAVS_RESPONSE_2(6,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(3,3)<4>       uwAVS_RESPONSE_2(6,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(6,3)<4>       uwAVS_RESPONSE_2(6,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(7,3)<4>       uwAVS_RESPONSE_2(6,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(10,3)<4>      uwAVS_RESPONSE_2(7,0)<4;4,1>                                      
//    mov (4) uwDEST_Y(11,3)<4>      uwAVS_RESPONSE_2(7,8)<4;4,1>                                      
//    mov (4) uwDEST_Y(14,3)<4>      uwAVS_RESPONSE_2(7,4)<4;4,1>                                    
//    mov (4) uwDEST_Y(15,3)<4>      uwAVS_RESPONSE_2(7,12)<4;4,1>                                    
//    mov (4) uwDEST_Y(18,3)<4>      uwAVS_RESPONSE_2(14,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(19,3)<4>      uwAVS_RESPONSE_2(14,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(22,3)<4>      uwAVS_RESPONSE_2(14,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(23,3)<4>      uwAVS_RESPONSE_2(14,12)<4;4,1>                                   
//    mov (4) uwDEST_Y(26,3)<4>      uwAVS_RESPONSE_2(15,0)<4;4,1>                                     
//    mov (4) uwDEST_Y(27,3)<4>      uwAVS_RESPONSE_2(15,8)<4;4,1>                                     
//    mov (4) uwDEST_Y(30,3)<4>      uwAVS_RESPONSE_2(15,4)<4;4,1>                                   
//    mov (4) uwDEST_Y(31,3)<4>      uwAVS_RESPONSE_2(15,12)<4;4,1>                                   

#else   /* OUTPUT_8_BIT */

// Move first 8x8 words of B to dest GRF
    mov (8) DEST_B(0)<1>				ubAVS_RESPONSE(4,1)<16;4,2>                                      
    mov (8) DEST_B(1)<1>				ubAVS_RESPONSE(4,8+1)<16;4,2>                                    
    mov (8) DEST_B(2)<1>				ubAVS_RESPONSE(5,1)<16;4,2>                                      
    mov (8) DEST_B(3)<1>				ubAVS_RESPONSE(5,8+1)<16;4,2>                                    
    mov (8) DEST_B(4)<1>				ubAVS_RESPONSE(12,1)<16;4,2>                                     
    mov (8) DEST_B(5)<1>				ubAVS_RESPONSE(12,8+1)<16;4,2>                                   
    mov (8) DEST_B(6)<1>				ubAVS_RESPONSE(13,1)<16;4,2>                                     
    mov (8) DEST_B(7)<1>				ubAVS_RESPONSE(13,8+1)<16;4,2>                                   

// Move first 8x8 words of G to dest GRF
    mov (8) DEST_G(0)<1>				ubAVS_RESPONSE(2,1)<16;4,2>              
    mov (8) DEST_G(1)<1>				ubAVS_RESPONSE(2,8+1)<16;4,2>            
    mov (8) DEST_G(2)<1>				ubAVS_RESPONSE(3,1)<16;4,2>              
    mov (8) DEST_G(3)<1>				ubAVS_RESPONSE(3,8+1)<16;4,2>            
    mov (8) DEST_G(4)<1>				ubAVS_RESPONSE(10,1)<16;4,2>             
    mov (8) DEST_G(5)<1>				ubAVS_RESPONSE(10,8+1)<16;4,2>           
    mov (8) DEST_G(6)<1>				ubAVS_RESPONSE(11,1)<16;4,2>             
    mov (8) DEST_G(7)<1>				ubAVS_RESPONSE(11,8+1)<16;4,2>           

// Move first 8x8 words of R to dest GRF
    mov (8) DEST_R(0)<1>				ubAVS_RESPONSE(0,1)<16;4,2>                                      
    mov (8) DEST_R(1)<1>				ubAVS_RESPONSE(0,8+1)<16;4,2>                                    
    mov (8) DEST_R(2)<1>				ubAVS_RESPONSE(1,1)<16;4,2>                                      
    mov (8) DEST_R(3)<1>				ubAVS_RESPONSE(1,8+1)<16;4,2>                                    
    mov (8) DEST_R(4)<1>				ubAVS_RESPONSE(8,1)<16;4,2>                                      
    mov (8) DEST_R(5)<1>				ubAVS_RESPONSE(8,8+1)<16;4,2>                                    
    mov (8) DEST_R(6)<1>				ubAVS_RESPONSE(9,1)<16;4,2>                                      
    mov (8) DEST_R(7)<1>				ubAVS_RESPONSE(9,8+1)<16;4,2>                                    

// Move first 8x8 words of A to dest GRF
    mov (8) DEST_A(0)<1>				ubAVS_RESPONSE(6,1)<16;4,2>                                      
    mov (8) DEST_A(1)<1>				ubAVS_RESPONSE(6,8+1)<16;4,2>                                    
    mov (8) DEST_A(2)<1>				ubAVS_RESPONSE(7,1)<16;4,2>                                      
    mov (8) DEST_A(3)<1>				ubAVS_RESPONSE(7,8+1)<16;4,2>                                    
    mov (8) DEST_A(4)<1>				ubAVS_RESPONSE(14,1)<16;4,2>                                     
    mov (8) DEST_A(5)<1>				ubAVS_RESPONSE(14,8+1)<16;4,2>                                   
    mov (8) DEST_A(6)<1>				ubAVS_RESPONSE(15,1)<16;4,2>                                     
    mov (8) DEST_A(7)<1>				ubAVS_RESPONSE(15,8+1)<16;4,2>                                   

// Move second 8x8 words of B to dest GRF
    mov (8) DEST_B(0,8)<1>			ubAVS_RESPONSE_2(4,1)<16;4,2>                                      
    mov (8) DEST_B(1,8)<1>			ubAVS_RESPONSE_2(4,8+1)<16;4,2>                                    
    mov (8) DEST_B(2,8)<1>			ubAVS_RESPONSE_2(5,1)<16;4,2>                                      
    mov (8) DEST_B(3,8)<1>			ubAVS_RESPONSE_2(5,8+1)<16;4,2>                                    
    mov (8) DEST_B(4,8)<1>			ubAVS_RESPONSE_2(12,1)<16;4,2>                                     
    mov (8) DEST_B(5,8)<1>			ubAVS_RESPONSE_2(12,8+1)<16;4,2>                                   
    mov (8) DEST_B(6,8)<1>			ubAVS_RESPONSE_2(13,1)<16;4,2>                                     
    mov (8) DEST_B(7,8)<1>			ubAVS_RESPONSE_2(13,8+1)<16;4,2>                                   

// Move second 8x8 words of G to dest GRF
    mov (8) DEST_G(0,8)<1>			ubAVS_RESPONSE_2(2,1)<16;4,2>              
    mov (8) DEST_G(1,8)<1>			ubAVS_RESPONSE_2(2,8+1)<16;4,2>            
    mov (8) DEST_G(2,8)<1>			ubAVS_RESPONSE_2(3,1)<16;4,2>              
    mov (8) DEST_G(3,8)<1>			ubAVS_RESPONSE_2(3,8+1)<16;4,2>            
    mov (8) DEST_G(4,8)<1>			ubAVS_RESPONSE_2(10,1)<16;4,2>             
    mov (8) DEST_G(5,8)<1>			ubAVS_RESPONSE_2(10,8+1)<16;4,2>           
    mov (8) DEST_G(6,8)<1>			ubAVS_RESPONSE_2(11,1)<16;4,2>             
    mov (8) DEST_G(7,8)<1>			ubAVS_RESPONSE_2(11,8+1)<16;4,2>           

// Move second 8x8 words of R to dest GRF
    mov (8) DEST_R(0,8)<1>			ubAVS_RESPONSE_2(0,1)<16;4,2>                                      
    mov (8) DEST_R(1,8)<1>			ubAVS_RESPONSE_2(0,8+1)<16;4,2>                                    
    mov (8) DEST_R(2,8)<1>			ubAVS_RESPONSE_2(1,1)<16;4,2>                                      
    mov (8) DEST_R(3,8)<1>			ubAVS_RESPONSE_2(1,8+1)<16;4,2>                                    
    mov (8) DEST_R(4,8)<1>			ubAVS_RESPONSE_2(8,1)<16;4,2>                                      
    mov (8) DEST_R(5,8)<1>			ubAVS_RESPONSE_2(8,8+1)<16;4,2>                                    
    mov (8) DEST_R(6,8)<1>			ubAVS_RESPONSE_2(9,1)<16;4,2>                                      
    mov (8) DEST_R(7,8)<1>			ubAVS_RESPONSE_2(9,8+1)<16;4,2>                                    

// Move second 8x8 words of A to dest GRF
    mov (8) DEST_A(0,8)<1>			ubAVS_RESPONSE_2(6,1)<16;4,2>                                      
    mov (8) DEST_A(1,8)<1>			ubAVS_RESPONSE_2(6,8+1)<16;4,2>                                    
    mov (8) DEST_A(2,8)<1>			ubAVS_RESPONSE_2(7,1)<16;4,2>                                      
    mov (8) DEST_A(3,8)<1>			ubAVS_RESPONSE_2(7,8+1)<16;4,2>                                    
    mov (8) DEST_A(4,8)<1>			ubAVS_RESPONSE_2(14,1)<16;4,2>                                     
    mov (8) DEST_A(5,8)<1>			ubAVS_RESPONSE_2(14,8+1)<16;4,2>                                   
    mov (8) DEST_A(6,8)<1>			ubAVS_RESPONSE_2(15,1)<16;4,2>                                     
    mov (8) DEST_A(7,8)<1>			ubAVS_RESPONSE_2(15,8+1)<16;4,2>                                   
#endif
//------------------------------------------------------------------------------

    // Set to write bottom region to memory
    #define SRC_REGION                              REGION_1
 
    // Re-define new # of lines
    #undef nUV_NUM_OF_ROWS
    #undef nY_NUM_OF_ROWS
       
    #define nY_NUM_OF_ROWS      8
    #define nUV_NUM_OF_ROWS     8
        
