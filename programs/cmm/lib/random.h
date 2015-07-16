/*******************************************************************************

    MenuetOS MineSweeper
    Copyright (C) 2003  Ivan Poddubny

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*******************************************************************************/
#define MASK_RAND 123456789
#define IQ_RAND 12773
#define IA_RAND 16807
#define IR_RAND 2836
#define IM_RAND 2147483647
#define AM_RAND (1./2147483647)

inline fastcall int random( ECX)
// get pseudo-random number - �������� ��������������� �����
{
  $push ebx
 
  $rdtsc        // eax & edx
  $xor eax,edx
  $not eax

  EBX = __generator;
  $ror ebx,3
  $xor ebx,0xdeadbeef
  EBX += EAX;
  __generator = EBX;
  
  EAX += EBX;
  EAX = EAX % ECX;
  
  $pop ebx  
}

:inline long unirand0(void)
{
	 long k,ans,tmp,save;
	 save = __generator;
	 __generator^=MASK_RAND;   /* avoid __generator==0 */
	 k=__generator/IQ_RAND;
	 tmp=__generator-k*IQ_RAND;
	 __generator*=IA_RAND*tmp;
	 __generator-=IR_RAND*k;
	 if(__generator<0) __generator+=IM_RAND;
	 if(save == __generator) return unirand0();
	 ans=__generator;
	 __generator^=MASK_RAND;   /* restore unmasked dummy */
	 return ans;
}

:long RAND_A,RAND_C,RAND_TMP;
:inline long rand(signed long x1,x2)
{
	long tmp,xx;
	RAND_A = __generator;
	__generator = RAND_A*__generator+RAND_C;
	RAND_C = __generator^RAND_A;
	RAND_C>>=1;
	RAND_A<<=1;
	__generator^=RAND_A;
	xx=x2;
	if(x1<0)xx+=-x1;
	tmp = __generator%xx;
	if(tmp<0)tmp=-tmp;
	tmp+=x1;
	return tmp;
}

inline fastcall randomize()
// initialize random number __generator - ���������������� ��������� ��������� �����
{
  asm
  {
    mov eax,3
    int 0x40
    ror eax,16
  }
  //if(EAX == __generator)return randomize();
  __generator = EAX;
}

