
align 4
proc glopNormal uses ecx esi edi, context:dword, p:dword
	mov esi,[p]
	add esi,4
	mov edi,[context]
	add edi,offs_cont_current_normal
	mov ecx,3
	rep movsd
	mov dword[edi],0.0 ;context.current_normal.W = 0.0
	ret
endp

align 4
proc glopTexCoord uses ecx esi edi, context:dword, p:dword
	mov esi,[p]
	add esi,4
	mov edi,[context]
	add edi,offs_cont_current_tex_coord
	mov ecx,4
	rep movsd
	ret
endp

align 4
proc glopEdgeFlag uses eax ebx, context:dword, p:dword
	mov eax,[context]
	mov ebx,[p]
	mov ebx,[ebx+4] ;ebx = p[1]
	mov dword[eax+offs_cont_current_edge_flag],ebx
	ret
endp

align 4
proc glopColor uses eax ecx esi edi, context:dword, p:dword
locals
	q rd 7
endl
	;current_color[3] = p[1-4]
	;longcurrent_color[2] = p[5-7]
	mov esi,[p]
	add esi,4
	mov edi,[context]
	add edi,offs_cont_current_color
	mov ecx,7
	rep movsd

	mov eax,[context]
	cmp dword[eax+offs_cont_color_material_enabled],0
	je @f
		mov dword[q],OP_Material
		mov ecx,[eax+offs_cont_current_color_material_mode]
		mov dword[q+4],ecx
		mov ecx,[eax+offs_cont_current_color_material_type]
		mov dword[q+8],ecx
		mov esi,[p]
		add esi,4
		mov edi,ebp
		sub edi,16 ;edi = &q[3]
		mov ecx,4
		rep movsd
		stdcall glopMaterial, eax,[q]
	@@:
	ret
endp

align 4
proc gl_eval_viewport uses eax, context:dword
locals
	zsize dd ? ;float
endl
	mov eax,[context]
	add eax,offs_cont_viewport ;eax = (GLViewport*) v

	mov dword[zsize],(1 shl (ZB_Z_BITS + ZB_POINT_Z_FRAC_BITS))
	fild dword[zsize]
	fstp dword[zsize]

	fld1
	fld1
	fadd st1,st0 ;st1 = 2.0
	fdiv st0,st1 ;st0 = 0.5

	fild dword[eax+offs_vpor_xsize]
	fsub st0,st1
	fdiv st0,st2
	fst dword[eax+offs_vpor_scale+offs_X]
	fiadd dword[eax+offs_vpor_xmin]
	fstp dword[eax+offs_vpor_trans+offs_X]

	fild dword[eax+offs_vpor_ysize]
	fsub st0,st1
	fdiv st0,st2
	fchs
	fst dword[eax+offs_vpor_scale+offs_Y]
	fchs
	fiadd dword[eax+offs_vpor_ymin]
	fstp dword[eax+offs_vpor_trans+offs_Y]

	fld dword[zsize]
	fsub st0,st1
	fdiv st0,st2
	fchs
	fst dword[eax+offs_vpor_scale+offs_Z]
	fchs
	mov dword[zsize],(1 shl ZB_POINT_Z_FRAC_BITS) / 2
	fiadd dword[zsize]
	fstp dword[eax+offs_vpor_trans+offs_Z]
if DEBUG ;gl_eval_viewport
	stdcall dbg_print,f_ev,txt_nl
	add eax,offs_vpor_scale
	stdcall gl_print_matrix,eax,1
	add eax,8
	stdcall gl_print_matrix,eax,1
end if
	ret
endp


align 4
proc glopBegin uses eax ebx ecx edx, context:dword, p:dword
locals
	tmp M4
endl
;    assert(c->in_begin == 0);

	mov edx,[context]
	mov ebx,[p]
	mov ebx,[ebx+4] ;ebx = p[1]
	mov [edx+offs_cont_begin_type],ebx
	mov dword[edx+offs_cont_in_begin],1
	mov dword[edx+offs_cont_vertex_n],0
	mov dword[edx+offs_cont_vertex_cnt],0

	bt dword[edx+offs_cont_matrix_model_projection_updated],0
	jnc .end_mmpu

	cmp dword[edx+offs_cont_lighting_enabled],0 ;if(context.lighting_enabled)
	je @f
if DEBUG ;context.matrix_stack_ptr[0]
	stdcall gl_print_matrix,dword[edx+offs_cont_matrix_stack_ptr],4
end if
		; precompute inverse modelview
		mov ebx,ebp
		sub ebx,64
		stdcall gl_M4_Inv, ebx,dword[edx+offs_cont_matrix_stack_ptr]
if DEBUG ;tmp
	stdcall dbg_print,txt_sp,txt_nl
	stdcall gl_print_matrix,ebx,4
end if
		push ebx
		mov ebx,edx
		add ebx,offs_cont_matrix_model_view_inv
		stdcall gl_M4_Transpose, ebx
if DEBUG ;context.matrix_model_view_inv
	stdcall dbg_print,txt_sp,txt_nl
	stdcall gl_print_matrix,ebx,4
end if
		jmp .end_if_0
	@@:
		mov ecx,edx
		add ecx,offs_cont_matrix_model_projection
		; precompute projection matrix
		stdcall gl_M4_Mul, ecx,dword[edx+offs_cont_matrix_stack_ptr+4],dword[edx+offs_cont_matrix_stack_ptr]

		; test to accelerate computation
		mov dword[edx+offs_cont_matrix_model_projection_no_w_transform],0
		fldz
		fld dword[ecx+12*4]
		fcomp st1
		fstsw ax
		sahf
		jne .end_if_0
		fld dword[ecx+13*4]
		fcomp st1
		fstsw ax
		sahf
		jne .end_if_0
		fld dword[ecx+14*4]
		fcomp st1
		fstsw ax
		sahf
		jne .end_if_0
			mov dword[edx+offs_cont_matrix_model_projection_no_w_transform],1
	.end_if_0:

		; test if the texture matrix is not Identity
		mov eax,edx
		add eax,offs_cont_matrix_stack_ptr+8
		stdcall gl_M4_IsId,eax
		xor eax,1
		mov dword[edx+offs_cont_apply_texture_matrix],eax

		mov dword[edx+offs_cont_matrix_model_projection_updated],0
	.end_mmpu:

	; viewport
	cmp dword[edx+offs_cont_viewport+offs_vpor_updated],0 ;if (context.viewport.updated)
	je @f
		stdcall gl_eval_viewport,edx
		mov dword[edx+offs_cont_viewport+offs_vpor_updated],0
	@@:
	; triangle drawing functions
	cmp dword[edx+offs_cont_render_mode],GL_SELECT
	jne @f
		mov dword[edx+offs_cont_draw_triangle_front],gl_draw_triangle_select
		mov dword[edx+offs_cont_draw_triangle_back],gl_draw_triangle_select
		jmp .end_if_2
	@@:

	cmp dword[edx+offs_cont_polygon_mode_front],GL_POINT
	jne @f
		mov dword[edx+offs_cont_draw_triangle_front],gl_draw_triangle_point
		jmp .end_if_1
	@@:
	cmp dword[edx+offs_cont_polygon_mode_front],GL_LINE
	jne @f
		mov dword[edx+offs_cont_draw_triangle_front],gl_draw_triangle_line
		jmp .end_if_1
	@@: ;default:
		mov dword[edx+offs_cont_draw_triangle_front],gl_draw_triangle_fill
	.end_if_1:

	cmp dword[edx+offs_cont_polygon_mode_back],GL_POINT
	jne @f
		mov dword[edx+offs_cont_draw_triangle_back],gl_draw_triangle_point
		jmp .end_if_2
	@@:
	cmp dword[edx+offs_cont_polygon_mode_back],GL_LINE
	jne @f
		mov dword[edx+offs_cont_draw_triangle_back],gl_draw_triangle_line
		jmp .end_if_2
	@@: ;default:
	    mov dword[edx+offs_cont_draw_triangle_back],gl_draw_triangle_fill
	.end_if_2:
	ret
endp

; coords, tranformation , clip code and projection
; TODO : handle all cases
align 4
proc gl_vertex_transform, context:dword, v:dword
pushad
	mov eax,[context]
	mov edx,[v]
	cmp dword[eax+offs_cont_lighting_enabled],0 ;if (context.lighting_enabled)
	je .els_0
		; eye coordinates needed for lighting
		mov ebx,dword[eax+offs_cont_matrix_stack_ptr]
		finit
		fld dword[edx+offs_vert_coord+offs_X]
		fld dword[edx+offs_vert_coord+offs_Y]
		fld dword[edx+offs_vert_coord+offs_Z]

		mov ecx,4
		.cycle_0:
			fld dword[ebx]     ;st0 = m[0]
			fmul st0,st3       ;st0 *= v.coord.X
			fld dword[ebx+4]   ;st0 = m[1]
			fmul st0,st3       ;st0 *= v.coord.Y
			fld dword[ebx+8]   ;st0 = m[2]
			fmul st0,st3       ;st0 *= v.coord.Z
			fadd dword[ebx+12] ;st0 += m[3]
			faddp              ;st0 += v.coord.Z * m[2] 
			faddp              ;st0 += v.coord.Y * m[1] 
			fstp dword[edx+offs_vert_ec] ;v.ec.X = v.coord.X * m[0] + v.coord.Y * m[1] + v.coord.Z * m[2] + m[3]
			add ebx,16 ;следущая строка матрицы
			add edx,4  ;следущая координата вектора
		loop .cycle_0
		ffree st0
		fincstp
		ffree st0
		fincstp
		ffree st0
		fincstp

		; projection coordinates
		mov ebx,dword[eax+offs_cont_matrix_stack_ptr+4]
		mov edx,[v]

		fld dword[edx+offs_vert_ec+offs_X]
		fld dword[edx+offs_vert_ec+offs_Y]
		fld dword[edx+offs_vert_ec+offs_Z]

		mov ecx,4
		.cycle_1:
			fld dword[ebx]     ;st0 = m[0]
			fmul st0,st3       ;st0 *= v.ec.X
			fld dword[ebx+4]   ;st0 = m[1]
			fmul st0,st3       ;st0 *= v.ec.Y
			fld dword[ebx+8]   ;st0 = m[2]
			fmul st0,st3       ;st0 *= v.ec.Z
			fadd dword[ebx+12] ;st0 += m[3]
			faddp              ;st0 += v.ec.Z * m[2]
			faddp              ;st0 += v.ec.Y * m[1]
			fstp dword[edx+offs_vert_pc] ;v.pc.X = v.ec.X * m[0] + v.ec.Y * m[1] + v.ec.Z * m[2] + m[3]
			add ebx,16 ;следущая строка матрицы
			add edx,4  ;следущая координата вектора
		loop .cycle_1
		ffree st0
		fincstp
		ffree st0
		fincstp
		ffree st0
		fincstp

		mov ebx,eax
		add ebx,offs_cont_matrix_model_view_inv
		mov edi,eax
		add edi,offs_cont_current_normal
		mov edx,[v]

		fld dword[edi] ;edi = &n
		fld dword[edi+offs_Y]
		fld dword[edi+offs_Z]

		mov ecx,3
		.cycle_2:
			fld dword[ebx]   ;st0 = m[0]
			fmul st0,st3     ;st0 *= n.X
			fld dword[ebx+4] ;st0 = m[1]
			fmul st0,st3     ;st0 *= n.Y
			fld dword[ebx+8] ;st0 = m[2]
			fmul st0,st3     ;st0 *= n.Z
			faddp            ;st0 += n.Z * m[2]
			faddp            ;st0 += n.Y * m[1]
			fstp dword[edx+offs_vert_normal] ;v.normal.X = n.X * m[0] + n.Y * m[1] + n.Z * m[2]
			add ebx,16 ;следущая строка матрицы
			add edx,4  ;следущая координата вектора
		loop .cycle_2

		cmp dword[eax+offs_cont_normalize_enabled],0
		je .end_els
			mov edx,[v]
			add edx,offs_vert_normal
			stdcall gl_V3_Norm,edx
		jmp .end_els
	.els_0:
		; no eye coordinates needed, no normal
		; NOTE: W = 1 is assumed
		mov ebx,eax
		add ebx,offs_cont_matrix_model_projection

		finit
		fld dword[edx+offs_vert_coord+offs_X]
		fld dword[edx+offs_vert_coord+offs_Y]
		fld dword[edx+offs_vert_coord+offs_Z]

		mov esi,edx
		add esi,offs_vert_pc

		fld dword[ebx]     ;st0 = m[0]
		fmul st0,st3       ;st0 *= v.coord.X
		fld dword[ebx+4]   ;st0 = m[1]
		fmul st0,st3       ;st0 *= v.coord.Y
		fld dword[ebx+8]   ;st0 = m[2]
		fmul st0,st3       ;st0 *= v.coord.Z
		fadd dword[ebx+12] ;st0 += m[3]
		faddp              ;st0 += v.coord.Z * m[2]
		faddp              ;st0 += v.coord.Y * m[1]
		fstp dword[esi]    ;v.pc.X = v.coord.X * m[0] + v.coord.Y * m[1] + v.coord.Z * m[2] + m[3]

		fld dword[ebx+16]  ;st0 = m[4]
		fmul st0,st3       ;st0 *= v.coord.X
		fld dword[ebx+20]  ;st0 = m[5]
		fmul st0,st3       ;st0 *= v.coord.Y
		fld dword[ebx+24]  ;st0 = m[6]
		fmul st0,st3       ;st0 *= v.coord.Z
		fadd dword[ebx+28] ;st0 += m[7]
		faddp              ;st0 += v.coord.Z * m[6]
		faddp              ;st0 += v.coord.Y * m[5]
		fstp dword[esi+4]  ;v.pc.X = v.coord.X * m[4] + v.coord.Y * m[5] + v.coord.Z * m[6] + m[7]

		fld dword[ebx+32]  ;st0 = m[8]
		fmul st0,st3       ;st0 *= v.coord.X
		fld dword[ebx+36]  ;st0 = m[9]
		fmul st0,st3       ;st0 *= v.coord.Y
		fld dword[ebx+40]  ;st0 = m[10]
		fmul st0,st3       ;st0 *= v.coord.Z
		fadd dword[ebx+44] ;st0 += m[11]
		faddp              ;st0 += v.coord.Z * m[10]
		faddp              ;st0 += v.coord.Y * m[9]
		fstp dword[esi+8]  ;v.pc.X = v.coord.X * m[8] + v.coord.Y * m[9] + v.coord.Z * m[10] + m[11]

		cmp dword[eax+offs_cont_matrix_model_projection_no_w_transform],0
		je .els_1
			;if (context.matrix_model_projection_no_w_transform)
			mov ebx,dword[ebx+60] ;ebx = m[15]
			mov dword[esi+12],ebx ;v.pc.W = m[15]
			jmp .end_els
		.els_1:
			fld dword[ebx+48]  ;st0 = m[12]
			fmul st0,st3       ;st0 *= v.coord.X
			fld dword[ebx+52]  ;st0 = m[13]
			fmul st0,st3       ;st0 *= v.coord.Y
			fld dword[ebx+56]  ;st0 = m[14]
			fmul st0,st3       ;st0 *= v.coord.Z
			fadd dword[ebx+60] ;st0 += m[15]
			faddp              ;st0 += v.coord.Z * m[14] 
			faddp              ;st0 += v.coord.Y * m[13] 
			fstp dword[esi+12] ;v.pc.W = v.coord.X * m[12] + v.coord.Y * m[13] + v.coord.Z * m[14] + m[15]
	.end_els:
	ffree st0
	fincstp
	ffree st0
	fincstp
	ffree st0
	fincstp

if DEBUG ;gl_vertex_transform
	stdcall dbg_print,f_vt,txt_nl
	mov edx,[v]
	add edx,offs_vert_pc
	stdcall gl_print_matrix,edx,1
end if
	mov edx,[v]
	stdcall gl_clipcode, dword[edx+offs_vert_pc+offs_X], dword[edx+offs_vert_pc+offs_Y],\
		dword[edx+offs_vert_pc+offs_Z], dword[edx+offs_vert_pc+offs_W]
	mov dword[edx+offs_vert_clip_code],eax
popad
	ret
endp

align 4
proc glopVertex, context:dword, p:dword
locals
	;ebx = GLVertex * v
	n dd ? ;ebp-4
endl
pushad
	mov edx,[context]

;    assert(c->in_begin != 0);

	mov ecx,[edx+offs_cont_vertex_n]
	mov [n],ecx
	inc dword[edx+offs_cont_vertex_cnt]

	; quick fix to avoid crashes on large polygons
	mov ecx,[edx+offs_cont_vertex_max]
	cmp dword[n],ecx
	jl @f
		shl dword[edx+offs_cont_vertex_max],1 ; just double size
		imul ecx,2*sizeof.GLVertex
		stdcall gl_malloc,ecx
		cmp eax,0
		jne .no_err
;gl_fatal_error("unable to allocate GLVertex array.\n");
		.no_err:
		mov edi,eax
		mov ebx,eax
		mov esi,[edx+offs_cont_vertex]
		mov ecx,[n]
		imul ecx,(sizeof.GLVertex)/4 ;((...)/4) что-бы использовать movsd вместо movsb
		rep movsd
		stdcall gl_free,dword[edx+offs_cont_vertex]
		mov dword[edx+offs_cont_vertex],ebx
	@@:
	; new vertex entry
	mov ebx,[n]
	imul ebx,sizeof.GLVertex
	add ebx,[edx+offs_cont_vertex]
	inc dword[n]

	mov esi,[p]
	add esi,4
	mov edi,ebx
	add edi,offs_vert_coord ;edi = &v.coord
	mov ecx,4
	rep movsd

	stdcall gl_vertex_transform, edx, ebx

	; color
	cmp dword[edx+offs_cont_lighting_enabled],0
	je .els_0
		stdcall gl_shade_vertex, edx,ebx
		jmp @f
	.els_0:
		mov eax,[edx+offs_cont_current_color]
		mov [ebx+offs_vert_color],eax
		mov eax,[edx+offs_cont_current_color+4]
		mov [ebx+offs_vert_color+4],eax
		mov eax,[edx+offs_cont_current_color+8]
		mov [ebx+offs_vert_color+8],eax
	@@:

	; tex coords
	cmp dword[edx+offs_cont_texture_2d_enabled],0
	je @f
		cmp dword[edx+offs_cont_apply_texture_matrix],0
		je .els_1
;           gl_M4_MulV4(&v->tex_coord, c->matrix_stack_ptr[2], &c->current_tex_coord);
			jmp @f
		.els_1:
			mov eax,[edx+offs_cont_current_tex_coord]
			mov [ebx+offs_vert_tex_coord],eax
	@@:

	; precompute the mapping to the viewport
	cmp dword[ebx+offs_vert_clip_code],0
	jne @f
		stdcall gl_transform_to_viewport, edx,ebx
	@@:

	; edge flag
	mov eax,[edx+offs_cont_current_edge_flag]
	mov dword[ebx+offs_vert_edge_flag],eax ;v.edge_flag = context.current_edge_flag

	cmp dword[edx+offs_cont_begin_type],GL_POINTS
	jne @f
		stdcall gl_draw_point, edx, dword[edx+offs_cont_vertex] ;dword[edx+...] = &context.vertex[0]
		mov dword[n],0
		jmp .end_f
	@@:
	cmp dword[edx+offs_cont_begin_type],GL_LINES
	jne @f
		cmp dword[n],2
		jne .end_f
			mov eax,[edx+offs_cont_vertex]
			push eax
			add eax,sizeof.GLVertex
			stdcall gl_draw_line, edx, eax
			xor eax,eax
			mov dword[n],eax
		jmp .end_f
	@@:
	cmp dword[edx+offs_cont_begin_type],GL_LINE_STRIP
	je .li_loop
	cmp dword[edx+offs_cont_begin_type],GL_LINE_LOOP
	jne @f
		.li_loop:
		cmp dword[n],1
		jne .els_2
			mov esi,[edx+offs_cont_vertex]
			mov edi,esi
			add edi,2*sizeof.GLVertex
			mov ecx,(sizeof.GLVertex)/4 ;((...)/4) что-бы использовать movsd вместо movsb
			rep movsd ;context.vertex[2] = context.vertex[0]
			jmp .end_f
		.els_2:
		cmp dword[n],2
		jne .end_f ;else if (n == 2)
			mov eax,[edx+offs_cont_vertex]
			push eax
			mov edi,eax
			add eax,sizeof.GLVertex
			mov esi,eax
			stdcall gl_draw_line, edx, eax
			mov ecx,(sizeof.GLVertex)/4 ;((...)/4) что-бы использовать movsd вместо movsb
			rep movsd ;context.vertex[0] = context.vertex[1]
			mov dword[n],1
		jmp .end_f
	@@:
	cmp dword[edx+offs_cont_begin_type],GL_TRIANGLES
	jne @f
		cmp dword[n],3
		jne .end_f
			mov eax,[edx+offs_cont_vertex]
			mov [esp-4],eax
			add eax,sizeof.GLVertex
			mov [esp-8],eax
			add eax,sizeof.GLVertex
			mov [esp-12],eax
			sub esp,12
			stdcall gl_draw_triangle, edx
			xor eax,eax
			mov dword[n],eax
		jmp .end_f
	@@:
	cmp dword[edx+offs_cont_begin_type],GL_TRIANGLE_STRIP
	jne @f
;       if (c->vertex_cnt >= 3) {
;           if (n == 3)
;               n = 0;
;            /* needed to respect triangle orientation */
;            switch(c->vertex_cnt & 1) {
;            case 0:
;               gl_draw_triangle(c,&c->vertex[2],&c->vertex[1],&c->vertex[0]);
;               break;
;            default:
;            case 1:
;               gl_draw_triangle(c,&c->vertex[0],&c->vertex[1],&c->vertex[2]);
;               break;
;            }
;       }
		jmp .end_f
	@@:
	cmp dword[edx+offs_cont_begin_type],GL_TRIANGLE_FAN
	jne @f
		cmp dword[n],3
		jne .end_f
			mov eax,[edx+offs_cont_vertex]
			mov [esp-4],eax
			add eax,sizeof.GLVertex
			mov [esp-8],eax
			mov edi,eax
			add eax,sizeof.GLVertex
			mov [esp-12],eax
			mov esi,eax
			sub esp,12
			stdcall gl_draw_triangle, edx
			mov ecx,(sizeof.GLVertex)/4 ;((...)/4) что-бы использовать movsd вместо movsb
			rep movsd ;context.vertex[1] = context.vertex[2]
			mov dword[n],2
		jmp .end_f
	@@:
	cmp dword[edx+offs_cont_begin_type],GL_QUADS
	jne @f
		cmp dword[n],4
		jne .end_f
			mov eax,[edx+offs_cont_vertex]
			add eax,2*sizeof.GLVertex
			mov dword[eax+offs_vert_edge_flag],0 ;context.vertex[2].edge_flag = 0
			push eax
			sub eax,sizeof.GLVertex
			push eax
			sub eax,sizeof.GLVertex
			stdcall gl_draw_triangle, edx,eax ;v0,v1,v2
			mov dword[eax+offs_vert_edge_flag],0 ;context.vertex[0].edge_flag = 0
			add eax,2*sizeof.GLVertex
			mov dword[eax+offs_vert_edge_flag],1 ;context.vertex[2].edge_flag = 1
			add eax,sizeof.GLVertex
			push eax
			sub eax,sizeof.GLVertex
			push eax
			sub eax,2*sizeof.GLVertex
			stdcall gl_draw_triangle, edx,eax ;v0,v2,v3
			xor eax,eax
			mov dword[n],eax
		jmp .end_f
	@@:
	cmp dword[edx+offs_cont_begin_type],GL_QUAD_STRIP
	jne @f
		cmp dword[n],4
		jne .end_f
			mov eax,[edx+offs_cont_vertex]
			mov [esp-4],eax
			mov edi,eax
			add eax,sizeof.GLVertex
			mov [esp-8],eax
			add eax,sizeof.GLVertex
			mov [esp-12],eax
			mov esi,eax
			sub esp,12
			stdcall gl_draw_triangle, edx ;v0,v1,v2
			mov [esp-12],eax
			add eax,sizeof.GLVertex
			mov [esp-8],eax
			sub eax,2*sizeof.GLVertex
			mov [esp-4],eax
			sub esp,12
			stdcall gl_draw_triangle, edx ;v1,v3,v2
			mov ecx,(sizeof.GLVertex)/2 ;((...)/2) копируем 2 вершины
			rep movsd ;context.vertex[0] = context.vertex[2], context.vertex[1] = context.vertex[3]
			mov dword[n],2
		jmp .end_f
	@@:
	cmp dword[edx+offs_cont_begin_type],GL_POLYGON
	je .end_f
;    default:
;       gl_fatal_error("glBegin: type %x not handled\n", c->begin_type);
;    }
	.end_f:

	mov ecx,[n]
	mov [edx+offs_cont_vertex_n],ecx
popad
	ret
endp

align 4
proc glopEnd uses eax ebx ecx, context:dword, p:dword
	mov eax,[context]
;    assert(c->in_begin == 1);

	cmp dword[eax+offs_cont_begin_type],GL_LINE_LOOP
	jne .else_i
		cmp dword[eax+offs_cont_vertex_cnt],3
		jl .end_i
			mov ebx,[eax+offs_cont_vertex]
			push ebx
			add ebx,2*sizeof.GLVertex
			push ebx
			stdcall gl_draw_line, eax
		jmp .end_i
	.else_i:
	cmp dword[eax+offs_cont_begin_type],GL_POLYGON
	jne .end_i
		mov ebx,dword[eax+offs_cont_vertex_cnt]
		@@: ;while (ebx >= 3)
		cmp ebx,3
		jl .end_i
			dec ebx
			mov ecx,ebx
			imul ecx,sizeof.GLVertex
			add ecx,[eax+offs_cont_vertex]
			push ecx ;ecx = &context.vertex[i]
			sub ecx,sizeof.GLVertex
			push ecx ;ecx = &context.vertex[i-1]
			stdcall gl_draw_triangle, eax,[eax+offs_cont_vertex]
		jmp @b
	.end_i:
	mov dword[eax+offs_cont_in_begin],0
	ret
endp
