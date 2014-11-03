;
;
; Z buffer: 16 bits Z / 16 bits color
; 
;
;include 'zbuffer.inc'

;output:
; eax - указатель на ZBuffer (0 если не удача)
align 4
proc ZB_open uses ecx edi, xsize:dword, ysize:dword, mode:dword,\
		nb_colors:dword, color_indexes:dword, color_table:dword, frame_buffer:dword

	stdcall gl_malloc, sizeof.ZBuffer
	cmp eax,0
	jne @f
		stdcall dbg_print,f_zb_opn,err_1
		jmp .end_f
	@@:
	mov edi,eax

	mov eax,[ysize]
	mov [edi+offs_zbuf_ysize],eax
	mov eax,[xsize]
	mov [edi+offs_zbuf_xsize],eax
	imul eax,PSZB
	add eax,3
	and eax,not 3
	mov [edi+offs_zbuf_linesize],eax
	mov eax,[mode]
	mov [edi+offs_zbuf_mode],eax

if TGL_FEATURE_8_BITS eq 1
	cmp eax,ZB_MODE_INDEX
	jne @f
;ZB_initDither(edi, nb_colors, color_indexes, color_table);
		jmp .end_s
	@@:
end if
if TGL_FEATURE_32_BITS eq 1
	cmp eax,ZB_MODE_RGBA
	je .correct
end if
if TGL_FEATURE_24_BITS eq 1
	cmp eax,ZB_MODE_RGB24
	je .correct
end if
	cmp eax,ZB_MODE_5R6G5B
	jne @f
	.correct:
		mov dword[edi+offs_zbuf_nb_colors],0
		jmp .end_s
	@@: ;default:
		stdcall dbg_print,f_zb_opn,err_3
		jmp .error
	.end_s:

	mov ecx,[edi+offs_zbuf_xsize]
	imul ecx,[edi+offs_zbuf_ysize]
	shl ecx,1 ;*= sizeof(unsigned short)

	stdcall gl_malloc, ecx
	mov [edi+offs_zbuf_zbuf],eax
	cmp eax,0
	jne @f
		stdcall dbg_print,f_zb_opn,err_2
		jmp .error
	@@:

	mov dword[edi+offs_zbuf_frame_buffer_allocated],0
	mov dword[edi+offs_zbuf_pbuf],0 ;NULL

	mov dword[edi+offs_zbuf_current_texture],0 ;NULL

	mov eax,edi
	jmp .end_f
	.error:
		stdcall gl_free,edi
		xor eax,eax
	.end_f:
	ret
endp

;void ZB_close(ZBuffer * zb)
;{
if TGL_FEATURE_8_BITS eq 1
;    if (zb->mode == ZB_MODE_INDEX)
;	ZB_closeDither(zb);
end if
;
;    if (zb->frame_buffer_allocated)
;	gl_free(zb->pbuf);
;
;    gl_free(zb->zbuf);
;    gl_free(zb);
;}

align 4
proc ZB_resize uses eax ebx ecx edi esi, zb:dword, frame_buffer:dword, xsize:dword, ysize:dword
	mov ebx,[zb]

	; xsize must be a multiple of 4
	mov edi,[xsize]
	and edi,not 3
	mov esi,[ysize]
	
	mov [ebx+offs_zbuf_xsize], edi
	mov [ebx+offs_zbuf_ysize], esi

	mov eax,edi
	imul eax,PSZB
	add eax,3
	and eax,not 3
	mov [ebx+offs_zbuf_linesize],eax ;zb.linesize = (xsize * PSZB + 3) & ~3

	mov ecx,edi
	imul ecx,esi
	shl ecx,1 ;*= sizeof(unsigned short)

	stdcall gl_free,dword[ebx+offs_zbuf_zbuf]
	stdcall gl_malloc,ecx
	mov [ebx+offs_zbuf_zbuf],eax

	cmp dword[ebx+offs_zbuf_frame_buffer_allocated],0
	je @f
		stdcall gl_free,dword[ebx+offs_zbuf_pbuf]
	@@:

	cmp dword[frame_buffer],0
	jne .els_0
		inc esi
		imul esi,dword[ebx+offs_zbuf_linesize]
		stdcall gl_malloc,esi
		mov dword[ebx+offs_zbuf_pbuf],eax
		mov dword[ebx+offs_zbuf_frame_buffer_allocated],1
		jmp @f
	.els_0:
		mov eax,[frame_buffer]
		mov dword[ebx+offs_zbuf_pbuf],eax
		mov dword[ebx+offs_zbuf_frame_buffer_allocated],0
	@@:
	ret
endp

;static void ZB_copyBuffer(ZBuffer * zb,
;                          void *buf,
;                          int linesize)
;{
;    unsigned char *p1;
;    PIXEL *q;
;    int y, n;
;
;    q = zb->pbuf;
;    p1 = buf;
;    n = zb->xsize * PSZB;
;    for (y = 0; y < zb->ysize; y++) {
;	memcpy(p1, q, n);
;	p1 += linesize;
;	q = (PIXEL *) ((char *) q + zb->linesize);
;    }
;}
;
;#if TGL_FEATURE_RENDER_BITS == 16

;/* 32 bpp copy */

;#ifdef TGL_FEATURE_32_BITS

;#define RGB16_TO_RGB32(p0,p1,v)\
;{\
;    unsigned int g,b,gb;\
;    g = (v & 0x07E007E0) << 5;\
;    b = (v & 0x001F001F) << 3;\
;    gb = g | b;\
;    p0 = (gb & 0x0000FFFF) | ((v & 0x0000F800) << 8);\
;    p1 = (gb >> 16) | ((v & 0xF8000000) >> 8);\
;}

;static void ZB_copyFrameBufferRGB32(ZBuffer * zb,
;                                    void *buf,
;                                    int linesize)
;{
;    unsigned short *q;
;    unsigned int *p, *p1, v, w0, w1;
;    int y, n;
;
;    q = zb->pbuf;
;    p1 = (unsigned int *) buf;
;
;    for (y = 0; y < zb->ysize; y++) {
;	p = p1;
;	n = zb->xsize >> 2;
;	do {
;	    v = *(unsigned int *) q;
;#if BYTE_ORDER == BIG_ENDIAN
;	    RGB16_TO_RGB32(w1, w0, v);
;#else
;	    RGB16_TO_RGB32(w0, w1, v);
;#endif
;	    p[0] = w0;
;	    p[1] = w1;
;
;	    v = *(unsigned int *) (q + 2);
;#if BYTE_ORDER == BIG_ENDIAN
;	    RGB16_TO_RGB32(w1, w0, v);
;#else
;	    RGB16_TO_RGB32(w0, w1, v);
;#endif
;	    p[2] = w0;
;	    p[3] = w1;
;
;	    q += 4;
;	    p += 4;
;	} while (--n > 0);
;
;	p1 += linesize;
;    }
;}

;#endif

;/* 24 bit packed pixel handling */

;#ifdef TGL_FEATURE_24_BITS

;/* order: RGBR GBRG BRGB */

;/* XXX: packed pixel 24 bit support not tested */
;/* XXX: big endian case not optimised */

;#if BYTE_ORDER == BIG_ENDIAN

;#define RGB16_TO_RGB24(p0,p1,p2,v1,v2)\
;{\
;    unsigned int r1,g1,b1,gb1,g2,b2,gb2;\
;    v1 = (v1 << 16) | (v1 >> 16);\
;    v2 = (v2 << 16) | (v2 >> 16);\
;    r1 = (v1 & 0xF800F800);\
;    g1 = (v1 & 0x07E007E0) << 5;\
;    b1 = (v1 & 0x001F001F) << 3;\
;    gb1 = g1 | b1;\
;    p0 = ((gb1 & 0x0000FFFF) << 8) | (r1 << 16) | (r1 >> 24);\
;    g2 = (v2 & 0x07E007E0) << 5;\
;    b2 = (v2 & 0x001F001F) << 3;\
;    gb2 = g2 | b2;\
;    p1 = (gb1 & 0xFFFF0000) | (v2 & 0xF800) | ((gb2 >> 8) & 0xff);\
;    p2 = (gb2 << 24) | ((v2 & 0xF8000000) >> 8) | (gb2 >> 16);\
;}

;#else

;#define RGB16_TO_RGB24(p0,p1,p2,v1,v2)\
;{\
;    unsigned int r1,g1,b1,gb1,g2,b2,gb2;\
;    r1 = (v1 & 0xF800F800);\
;    g1 = (v1 & 0x07E007E0) << 5;\
;    b1 = (v1 & 0x001F001F) << 3;\
;    gb1 = g1 | b1;\
;    p0 = ((gb1 & 0x0000FFFF) << 8) | (r1 << 16) | (r1 >> 24);\
;    g2 = (v2 & 0x07E007E0) << 5;\
;    b2 = (v2 & 0x001F001F) << 3;\
;    gb2 = g2 | b2;\
;    p1 = (gb1 & 0xFFFF0000) | (v2 & 0xF800) | ((gb2 >> 8) & 0xff);\
;    p2 = (gb2 << 24) | ((v2 & 0xF8000000) >> 8) | (gb2 >> 16);\
;}

;#endif

;static void ZB_copyFrameBufferRGB24(ZBuffer * zb,
;                                    void *buf, int linesize)
;{
;    unsigned short *q;
;    unsigned int *p, *p1, w0, w1, w2, v0, v1;
;    int y, n;
;
;    q = zb->pbuf;
;    p1 = (unsigned int *) buf;
;    linesize = linesize * 3;
;
;    for (y = 0; y < zb->ysize; y++) {
;	p = p1;
;	n = zb->xsize >> 2;
;	do {
;	    v0 = *(unsigned int *) q;
;	    v1 = *(unsigned int *) (q + 2);
;	    RGB16_TO_RGB24(w0, w1, w2, v0, v1);
;	    p[0] = w0;
;	    p[1] = w1;
;	    p[2] = w2;

;	    q += 4;
;	    p += 3;
;	} while (--n > 0);
;
;	(char *) p1 += linesize;
;    }
;}

;#endif

;void ZB_copyFrameBuffer(ZBuffer * zb, void *buf,
;			int linesize)
;{
;    switch (zb->mode) {
;#ifdef TGL_FEATURE_8_BITS
;    case ZB_MODE_INDEX:
;	ZB_ditherFrameBuffer(zb, buf, linesize >> 1);
;	break;
;#endif
;#ifdef TGL_FEATURE_16_BITS
;    case ZB_MODE_5R6G5B:
;	ZB_copyBuffer(zb, buf, linesize);
;	break;
;#endif
;#ifdef TGL_FEATURE_32_BITS
;    case ZB_MODE_RGBA:
;	ZB_copyFrameBufferRGB32(zb, buf, linesize >> 1);
;	break;
;#endif
;#ifdef TGL_FEATURE_24_BITS
;    case ZB_MODE_RGB24:
;	ZB_copyFrameBufferRGB24(zb, buf, linesize >> 1);
;	break;
;#endif
;    default:
;	assert(0);
;    }
;}

;#endif /* TGL_FEATURE_RENDER_BITS == 16 */

;#if TGL_FEATURE_RENDER_BITS == 24

;#define RGB24_TO_RGB16(r, g, b) \
;  ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

;/* XXX: not optimized */
;static void ZB_copyFrameBuffer5R6G5B(ZBuffer * zb, 
;                                     void *buf, int linesize) 
;{
;    PIXEL *q;
;    unsigned short *p, *p1;
;    int y, n;
;
;    q = zb->pbuf;
;    p1 = (unsigned short *) buf;
;
;    for (y = 0; y < zb->ysize; y++) {
;	p = p1;
;	n = zb->xsize >> 2;
;	do {
;            p[0] = RGB24_TO_RGB16(q[0], q[1], q[2]);
;            p[1] = RGB24_TO_RGB16(q[3], q[4], q[5]);
;            p[2] = RGB24_TO_RGB16(q[6], q[7], q[8]);
;            p[3] = RGB24_TO_RGB16(q[9], q[10], q[11]);
;	    q = (PIXEL *)((char *)q + 4 * PSZB);
;	    p += 4;
;	} while (--n > 0);
;	p1 = (unsigned short *)((char *)p1 + linesize);
;    }
;}

;void ZB_copyFrameBuffer(ZBuffer * zb, void *buf,
;			int linesize)
;{
;    switch (zb->mode) {
;#ifdef TGL_FEATURE_16_BITS
;    case ZB_MODE_5R6G5B:
;	ZB_copyFrameBuffer5R6G5B(zb, buf, linesize);
;	break;
;#endif
;#ifdef TGL_FEATURE_24_BITS
;    case ZB_MODE_RGB24:
;	ZB_copyBuffer(zb, buf, linesize);
;	break;
;#endif
;    default:
;	assert(0);
;    }
;}

;#endif /* TGL_FEATURE_RENDER_BITS == 24 */

;#if TGL_FEATURE_RENDER_BITS == 32

;#define RGB32_TO_RGB16(v) \
;  (((v >> 8) & 0xf800) | (((v) >> 5) & 0x07e0) | (((v) & 0xff) >> 3))

;/* XXX: not optimized */
;static void ZB_copyFrameBuffer5R6G5B(ZBuffer * zb, 
;                                     void *buf, int linesize) 
;{
;    PIXEL *q;
;    unsigned short *p, *p1;
;    int y, n;
;
;    q = zb->pbuf;
;    p1 = (unsigned short *) buf;
;
;    for (y = 0; y < zb->ysize; y++) {
;	p = p1;
;	n = zb->xsize >> 2;
;	do {
;            p[0] = RGB32_TO_RGB16(q[0]);
;            p[1] = RGB32_TO_RGB16(q[1]);
;            p[2] = RGB32_TO_RGB16(q[2]);
;            p[3] = RGB32_TO_RGB16(q[3]);
;	    q += 4;
;	    p += 4;
;	} while (--n > 0);
;	p1 = (unsigned short *)((char *)p1 + linesize);
;    }
;}
;
;void ZB_copyFrameBuffer(ZBuffer * zb, void *buf,
;			int linesize)
;{
;    switch (zb->mode) {
;#ifdef TGL_FEATURE_16_BITS
;    case ZB_MODE_5R6G5B:
;	ZB_copyFrameBuffer5R6G5B(zb, buf, linesize);
;	break;
;#endif
;#ifdef TGL_FEATURE_32_BITS
;    case ZB_MODE_RGBA:
;	ZB_copyBuffer(zb, buf, linesize);
;	break;
;#endif
;    default:
;	assert(0);
;    }
;}
;
;#endif /* TGL_FEATURE_RENDER_BITS == 32 */


;
; adr must be aligned on an 'int'
;
align 4
proc memset_s uses eax ecx edi, adr:dword, val:dword, count:dword
	mov eax,[val]
	mov di,ax
	ror eax,16
	mov ax,di
	mov ecx,[count]
	shr ecx,1
	mov edi,[adr]
	rep stosd

	bt dword[count],0
	jnc @f
		stosw
	@@:
	ret
endp

align 4
proc memset_l uses eax ecx edi, adr:dword, val:dword, count:dword
	mov eax,[val]
	mov ecx,[count]
	mov edi,[adr]
	rep stosd
	ret
endp

; count must be a multiple of 4 and >= 4
align 4
proc memset_RGB24 uses eax ecx edi esi, adr:dword, r:dword, g:dword, b:dword, count:dword
	mov esi,[adr]
	mov eax,[r] ;копируем в буфер первые 12 байт (минимальное число кратное 3 и 4)
	mov byte[esi],al
	mov byte[esi+3],al
	mov byte[esi+6],al
	mov byte[esi+9],al
	mov eax,[g]
	mov byte[esi+1],al
	mov byte[esi+4],al
	mov byte[esi+7],al
	mov byte[esi+10],al
	mov eax,[b]
	mov byte[esi+2],al
	mov byte[esi+5],al
	mov byte[esi+8],al
	mov byte[esi+11],al

	mov ecx,[count]
	shr ecx,2
	cmp ecx,1
	jle .end_f ;если ширина буфера меньше 12 байт, то выходим
	dec ecx
	mov edi,esi
	add edi,12

	mov eax,[esi]
	cmp eax,[esi+4]
	jne @f
		;если r=g и g=b и b=r
		mov esi,ecx
		shl ecx,2
		sub ecx,esi ;ecx*=3
		rep stosd
		jmp .end_f
	@@:

	;если r!=g или g!=b или b!=r
	@@:
		movsd
		movsd
		movsd
		sub esi,12
	loop @b
	.end_f:
	ret
endp

align 4
proc ZB_clear uses eax ebx ecx, zb:dword, clear_z:dword, z:dword, clear_color:dword,\
	r:dword, g:dword, b:dword
;if TGL_FEATURE_RENDER_BITS != 24
;	color dd ?
;end if

	mov eax,[zb]
	cmp dword[clear_z],0
	je @f
		mov ebx,[eax+offs_zbuf_xsize]
		imul ebx,[eax+offs_zbuf_ysize]
		stdcall memset_s, [eax+offs_zbuf_zbuf],[z],ebx
	@@:
	cmp dword[clear_color],0
	je @f
if TGL_FEATURE_RENDER_BITS eq 24
		mov ebx,[eax+offs_zbuf_xsize]
		push ebx
		mov ebx,[b]
		shr ebx,8
		push ebx
		mov ebx,[g]
		shr ebx,8
		push ebx
		mov ebx,[r]
		shr ebx,8
		push ebx
		add esp,16
end if
		mov ebx,[eax+offs_zbuf_pbuf]
		mov ecx,[eax+offs_zbuf_ysize]
		.cycle_0:
if (TGL_FEATURE_RENDER_BITS eq 15) ;or (TGL_FEATURE_RENDER_BITS eq 16)
			;color = RGB_TO_PIXEL(r, g, b);
			;memset_s(ebx, color, zb->xsize);
end if
if TGL_FEATURE_RENDER_BITS eq 32
			;color = RGB_TO_PIXEL(r, g, b);
			;memset_l(ebx, color, zb->xsize);
end if
if TGL_FEATURE_RENDER_BITS eq 24
			sub esp,16
			stdcall memset_RGB24,ebx
end if
			add ebx,[eax+offs_zbuf_linesize]
		loop .cycle_0
	@@:
	ret
endp
