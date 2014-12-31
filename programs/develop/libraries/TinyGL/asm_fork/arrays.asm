VERTEX_ARRAY   equ 0x0001
COLOR_ARRAY    equ 0x0002
NORMAL_ARRAY   equ 0x0004
TEXCOORD_ARRAY equ 0x0008

;glArrayElement - использование (рисование) одного элемента из массива
;glDrawArrays - рисование всех элементов в массиве
;glDrawElements - рисование тех элементов в массиве, которые заданны в массиве индексов

align 4
proc glopArrayElement uses eax ebx ecx edx, context:dword, param:dword
locals
	p rd 5
endl
	mov eax,[context]
	mov ebx,[param]
	mov ebx,[ebx+4] ;ebx = p[1]
  
	bt dword[eax+offs_cont_client_states],1 ;2^1=COLOR_ARRAY
	jnc @f
		mov ecx,[eax+offs_cont_color_array_size]
		add ecx,[eax+offs_cont_color_array_stride]
		imul ecx,ebx
		shl ecx,2
		add ecx,[eax+offs_cont_color_array] ;ecx = &context.color_array[i]
		mov ebx,ebp
		sub ebx,20 ;=sizeof(dd)*5
		mov edx,[ecx]
		mov [ebx+4],edx
		mov edx,[ecx+4]
		mov [ebx+8],edx
		mov edx,[ecx+8]
		mov [ebx+12],edx
		cmp dword[eax+offs_cont_color_array_size],3
		jg .l0
			mov edx,1.0
			jmp .l1
		.l0:
			mov edx,[ecx+12]
		.l1:
		mov [ebx+16],edx
		stdcall glopColor, eax,ebx
	@@:
	bt dword[eax+offs_cont_client_states],2 ;2^2=NORMAL_ARRAY
	jnc @f
		mov esi,dword[eax+offs_cont_normal_array_stride]
		add esi,3
		imul esi,ebx
		shl esi,2
		add esi,[eax+offs_cont_normal_array] ;esi = &normal_array[ebx * (3 + c->normal_array_stride)]
		mov edi,eax
		add edi,offs_cont_current_normal
		mov ecx,3
		rep movsd
		mov dword[edi],0.0
	@@:
	bt dword[eax+offs_cont_client_states],3 ;2^3=TEXCOORD_ARRAY
	jnc @f
		mov ecx,[eax+offs_cont_texcoord_array_size]
		add ecx,[eax+offs_cont_texcoord_array_stride]
		imul ecx,ebx
		shl ecx,2
		add ecx,[eax+offs_cont_texcoord_array] ;ecx = &context.texcoord_array[i]
		mov edx,[ecx]
		mov [eax+offs_cont_current_tex_coord],edx
		mov edx,[ecx+4]
		mov [eax+offs_cont_current_tex_coord+4],edx

		cmp dword[eax+offs_cont_texcoord_array_size],2
		jg .l2
			mov edx,0.0
			jmp .l3
		.l2:
			mov edx,[ecx+8]
		.l3:
		mov [eax+offs_cont_current_tex_coord+8],edx

		cmp dword[eax+offs_cont_texcoord_array_size],3
		jg .l4
			mov edx,1.0
			jmp .l5
		.l4:
			mov edx,[ecx+12]
		.l5:

		mov [eax+offs_cont_current_tex_coord+12],edx
	@@:
	bt dword[eax+offs_cont_client_states],0 ;2^0=VERTEX_ARRAY
	jnc @f
		mov ecx,[eax+offs_cont_vertex_array_size]
		add ecx,[eax+offs_cont_vertex_array_stride]
		imul ecx,ebx
		shl ecx,2
		add ecx,[eax+offs_cont_vertex_array] ;ecx = &context.vertex_array[i]
		mov ebx,ebp
		sub ebx,20 ;=sizeof(dd)*5
		mov edx,[ecx]
		mov [ebx+4],edx
		mov edx,[ecx+4]
		mov [ebx+8],edx

		cmp dword[eax+offs_cont_vertex_array_size],2
		jg .l6
			mov edx,0.0
			jmp .l7
		.l6:
			mov edx,[ecx+8]
		.l7:
		mov [ebx+12],edx
		cmp dword[eax+offs_cont_vertex_array_size],3
		jg .l8
			mov edx,1.0
			jmp .l9
		.l8:
			mov edx,[ecx+12]
		.l9:

		mov [ebx+16],edx
		stdcall glopVertex, eax,ebx
	@@:
	ret
endp

align 4
proc glArrayElement uses eax, i:dword
locals
	p rd 2
endl
	mov dword[p],OP_ArrayElement
	mov eax,[i]
	mov dword[p+4],eax

	mov eax,ebp
	sub eax,8 ;=sizeof(dd)*2
	stdcall gl_add_op,eax
	ret
endp

align 4
proc glopDrawArrays, context:dword, param:dword
locals
	vi dd ?
	idx dd ?
	states dd ?
	size dd ?
	p rd 8 ;функция glopColor требует 8 параметров, другие функции требуют меньше, берем по максимуму что-бы не портить стек
endl
pushad
	mov edx,[context]
	mov ebx,[param]
	mov ecx,[ebx+12] ;count = param[3].i
	mov eax,[edx+offs_cont_client_states]
	mov [states],eax
	mov eax,[ebx+8]
	mov [idx],eax ;param[2].i
	mov eax,[ebx+4]
	mov [p+4],eax ;p[1].i = param[1].i
	mov eax,ebp
	sub eax,32 ;=sizeof(dd)*8
	stdcall glopBegin, edx,eax

	mov dword[vi],0
align 4
	.cycle_0: ;for (int vi=0; vi<count; vi++)
	cmp dword[vi],ecx
	jge .cycle_0_end
		bt dword[states],1 ;2^1=COLOR_ARRAY
		jnc @f
			mov esi,[edx+offs_cont_color_array_size]
			mov [size],esi
			add esi,[edx+offs_cont_color_array_stride]
			imul esi,[idx] ;esi = i
			shl esi,2
			add esi,[edx+offs_cont_color_array] ;esi = &context.color_array[i]
			mov edi,ebp
			sub edi,28 ;edi = &p[1]
			movsd ;p[1].f = context.color_array[i]
			movsd
			movsd
			cmp dword[size],3
			jle .e1
				movsd
				sub edi,20 ;&p[0]
				jmp .e2
			.e1:
				mov dword[edi],1.0 ;если задано 3 параметра, то 4-й ставим по умолчанию 1.0
				sub edi,16 ;&p[0]
			.e2:
			mov ebx,ebp
			sub ebx,12 ;ebp-12 = &p[5]
			push ebx
			add ebx,4 ;ebp-8 = &p[6]
			push ebx
			add ebx,4 ;ebp-4 = &p[7]
			push ebx
			stdcall RGBFtoRGBI,[edi+12],[edi+8],[edi+4] ;call: r,g,b,&p[7],&p[6],&p[5]
			stdcall glopColor, edx,edi ;(context, p(op,rf,gf,bf,af,ri,gi,bi))
		@@:
		bt dword[states],2 ;2^2=NORMAL_ARRAY
		jnc @f
			mov esi,[edx+offs_cont_normal_array_stride]
			add esi,3
			imul esi,[idx]
			shl esi,2
			add esi,[edx+offs_cont_normal_array] ;esi = &context.normal_array[ idx * (3 + context.normal_array_stride) ]
			mov edi,edx
			add edi,offs_cont_current_normal
			movsd ;context.current_normal.X = context.normal_array[i]
			movsd
			movsd
			mov dword[edi],0.0 ;context.current_normal.W = 0.0f
		@@:
		bt dword[states],3 ;2^3=TEXCOORD_ARRAY
		jnc @f
			mov esi,[edx+offs_cont_texcoord_array_size]
			mov [size],esi
			add esi,[edx+offs_cont_texcoord_array_stride]
			imul esi,[idx] ;esi = i
			shl esi,2
			add esi,[edx+offs_cont_texcoord_array] ;esi = &context.texcoord_array[i]
			mov edi,edx
			add edi,offs_cont_current_tex_coord
			movsd ;context.current_tex_coord.X = ccontext.texcoord_array[i]
			movsd
			cmp dword[size],2
			jle .e3
				movsd
				jmp .e4
			.e3:
				mov dword[edi],0.0 ;если задано 2 параметра, то 3-й ставим по умолчанию 0.0
				add edi,4
			.e4:
			cmp dword[size],3
			jle .e5
				movsd
				jmp @f
			.e5:
				mov dword[edi],1.0 ;если задано 3 параметра, то 4-й ставим по умолчанию 1.0
		@@:
		bt dword[states],0 ;2^0=VERTEX_ARRAY
		jnc @f
			mov esi,[edx+offs_cont_vertex_array_size]
			mov [size],esi
			add esi,[edx+offs_cont_vertex_array_stride]
			imul esi,[idx] ;esi = i
			shl esi,2
			add esi,[edx+offs_cont_vertex_array] ;esi = &context.vertex_array[i]
			mov edi,ebp
			sub edi,28 ;edi = &p[1]
			movsd ;p[1].f = context.vertex_array[i]
			movsd
			cmp dword[size],2
			jle .e6
				movsd
				jmp .e7
			.e6:
				mov dword[edi],0.0 ;если задано 2 параметра, то 3-й ставим по умолчанию 0.0
				add edi,4
			.e7:
			cmp dword[size],3
			jle .e8
				movsd
				sub edi,20 ;edi=&p[0]
				jmp .e9
			.e8:
				mov dword[edi],1.0 ;если задано 3 параметра, то 4-й ставим по умолчанию 1.0
				sub edi,16 ;edi=&p[0]
			.e9:
			stdcall glopVertex, edx,edi
		@@:
		inc dword[idx]
	inc dword[vi]
	jmp .cycle_0
	.cycle_0_end:

	;mov eax,ebp
	;sub eax,32 ;=sizeof(dd)*8
	stdcall glopEnd, edx,eax
popad
	ret
endp

align 4
proc glDrawArrays uses eax, mode:dword, first:dword, count:dword
locals
	p rd 4
endl
	mov dword[p],OP_DrawArrays
	mov eax,[mode]
	mov dword[p+4],eax
	mov eax,[first]
	mov dword[p+8],eax
	mov eax,[count]
	mov dword[p+12],eax

	mov eax,ebp
	sub eax,16 ;=sizeof(dd)*4
	stdcall gl_add_op,eax
	ret
endp

align 4
proc glopDrawElements uses eax ebx edx, context:dword, param:dword
locals
	p rd 5
endl

	mov edx,[context]
	mov ebx,[param]
  ;int i;
  ;int idx;
  ;int states = c->client_states;
  ;int count = param[2].i;
  ;int type = param[3].i;
  ;unsigned int *indices_u32 = (unsigned int*)param[4].p;
  ;unsigned short *indices_u16 = (unsigned short*)indices_u32;

	mov eax,[ebx+4]
	mov [p+4],eax ;p[1].i = param[1].i
	mov eax,ebp
	sub eax,20 ;=sizeof(dd)*5
	stdcall glopBegin, edx,eax

;  for (int ii=0; ii<count; ii++) {
;    idx = (type == GL_UNSIGNED_INT) ? indices_u32[ii] : indices_u16[ii];
;    if (states & COLOR_ARRAY) {
;      GLParam p[5];
;      int size = c->color_array_size; 
;      i = idx * (size + c->color_array_stride);
;      p[1].f = c->color_array[i];
;      p[2].f = c->color_array[i+1];
;      p[3].f = c->color_array[i+2];
;      p[4].f = size > 3 ? c->color_array[i+3] : 1.0f;
;      glopColor(c, p);  
;    }
;    if (states & NORMAL_ARRAY) {
;      i = idx * (3 + c->normal_array_stride);
;      c->current_normal.X = c->normal_array[i];
;      c->current_normal.Y = c->normal_array[i+1];
;      c->current_normal.Z = c->normal_array[i+2];
;      c->current_normal.W = 0.0f;
;    }
;    if (states & TEXCOORD_ARRAY) {
;      int size = c->texcoord_array_size;
;      i = idx * (size + c->texcoord_array_stride);
;      c->current_tex_coord.X = c->texcoord_array[i];
;      c->current_tex_coord.Y = c->texcoord_array[i+1];
;      c->current_tex_coord.Z = size > 2 ? c->texcoord_array[i+2] : 0.0f;
;      c->current_tex_coord.W = size > 3 ? c->texcoord_array[i+3] : 1.0f;
;    }
;    if (states & VERTEX_ARRAY) {
;      GLParam p[5];
;      int size = c->vertex_array_size;
;      i = idx * (size + c->vertex_array_stride);
;      p[1].f = c->vertex_array[i];
;      p[2].f = c->vertex_array[i+1];
;      p[3].f = size > 2 ? c->vertex_array[i+2] : 0.0f;
;      p[4].f = size > 3 ? c->vertex_array[i+3] : 1.0f;
;      glopVertex(c, p);
;    }
;  }
	mov eax,ebp
	sub eax,20 ;=sizeof(dd)*5
	stdcall glopEnd, edx,eax
	ret
endp

align 4
proc glDrawElements uses eax, mode:dword, count:dword, type:dword, indices:dword
locals
	p rd 5
endl
;assert(type == GL_UNSIGNED_SHORT || type == GL_UNSIGNED_INT/* nonstandard */);
	mov dword[p],OP_DrawElements
	mov eax,[mode]
	mov dword[p+4],eax
	mov eax,[count]
	mov dword[p+8],eax
	mov eax,[type]
	mov dword[p+12],eax
	mov eax,[indices]
	mov dword[p+16],eax

	mov eax,ebp
	sub eax,20 ;=sizeof(dd)*5
	stdcall gl_add_op,eax
	ret
endp

align 4
proc glopEnableClientState uses eax ebx, context:dword, p:dword
	mov eax,[context]
	mov ebx,[p]
	mov ebx,[ebx+4] ;ebx = p[1]
	or dword[eax+offs_cont_client_states],ebx
	ret
endp

align 4
proc glEnableClientState uses eax, array:dword
locals
	p rd 2
endl
	mov dword[p],OP_EnableClientState

	cmp dword[array],GL_VERTEX_ARRAY
	jne @f
		mov dword[p+4],VERTEX_ARRAY
		jmp .end_f
	@@:
	cmp dword[array],GL_NORMAL_ARRAY
	jne @f
		mov dword[p+4],NORMAL_ARRAY
		jmp .end_f
	@@:
	cmp dword[array],GL_COLOR_ARRAY
	jne @f
		mov dword[p+4],COLOR_ARRAY
		jmp .end_f
	@@:
	cmp dword[array],GL_TEXTURE_COORD_ARRAY
	jne @f
		mov dword[p+4],TEXCOORD_ARRAY
		jmp .end_f
	@@:
		;assert(0);
	.end_f:

	mov eax,ebp
	sub eax,8 ;=sizeof(dd)*2
	stdcall gl_add_op,eax
	ret
endp

align 4
proc glopDisableClientState uses eax ebx, context:dword, p:dword
	mov eax,[context]
	mov ebx,[p]
	mov ebx,[ebx+4] ;ebx = p[1]
	and dword[eax+offs_cont_client_states],ebx
	ret
endp

align 4
proc glDisableClientState uses eax, array:dword
locals
	p rd 2
endl
	mov dword[p],OP_DisableClientState

	cmp dword[array],GL_VERTEX_ARRAY
	jne @f
		mov dword[p+4], not VERTEX_ARRAY
		jmp .end_f
	@@:
	cmp dword[array],GL_NORMAL_ARRAY
	jne @f
		mov dword[p+4], not NORMAL_ARRAY
		jmp .end_f
	@@:
	cmp dword[array],GL_COLOR_ARRAY
	jne @f
		mov dword[p+4], not COLOR_ARRAY
		jmp .end_f
	@@:
	cmp dword[array],GL_TEXTURE_COORD_ARRAY
	jne @f
		mov dword[p+4], not TEXCOORD_ARRAY
		jmp .end_f
	@@:
		;assert(0);
	.end_f:

	mov eax,ebp
	sub eax,8 ;=sizeof(dd)*2
	stdcall gl_add_op,eax
	ret
endp

align 4
proc glopVertexPointer uses eax ebx ecx, context:dword, p:dword
	mov eax,[context]
	mov ebx,[p]
	mov ecx,[ebx+4] ;ecx = p[1]
	mov dword[eax+offs_cont_vertex_array_size],ecx
	mov ecx,[ebx+8] ;ecx = p[2]
	mov dword[eax+offs_cont_vertex_array_stride],ecx
	mov ecx,[ebx+12] ;ecx = p[3]
	mov dword[eax+offs_cont_vertex_array],ecx
	ret
endp

align 4
proc glVertexPointer uses eax, size:dword, type:dword, stride:dword, pointer:dword
locals
	p rd 4
endl
;  assert(type == GL_FLOAT);

	mov dword[p],OP_VertexPointer
	mov eax,[size]
	mov dword[p+4],eax
	mov eax,[stride]
	mov dword[p+8],eax
	mov eax,[pointer]
	mov dword[p+12],eax

	mov eax,ebp
	sub eax,16 ;=sizeof(dd)*4
	stdcall gl_add_op,eax
	ret
endp

align 4
proc glopColorPointer uses eax ebx ecx, context:dword, p:dword
	mov eax,[context]
	mov ebx,[p]
	mov ecx,[ebx+4] ;ecx = p[1]
	mov dword[eax+offs_cont_color_array_size],ecx
	mov ecx,[ebx+8] ;ecx = p[2]
	mov dword[eax+offs_cont_color_array_stride],ecx
	mov ecx,[ebx+12] ;ecx = p[3]
	mov dword[eax+offs_cont_color_array],ecx
	ret
endp

align 4
proc glColorPointer uses eax, size:dword, type:dword, stride:dword, pointer:dword
locals
	p rd 4
endl
;  assert(type == GL_FLOAT);

	mov dword[p],OP_ColorPointer
	mov eax,[size]
	mov dword[p+4],eax
	mov eax,[stride]
	mov dword[p+8],eax
	mov eax,[pointer]
	mov dword[p+12],eax

	mov eax,ebp
	sub eax,16 ;=sizeof(dd)*4
	stdcall gl_add_op,eax
	ret
endp

align 4
proc glopNormalPointer uses eax ebx ecx, context:dword, p:dword
	mov eax,[context]
	mov ebx,[p]
	mov ecx,[ebx+4] ;ecx = p[1]
	mov dword[eax+offs_cont_normal_array_stride],ecx
	mov ecx,[ebx+8] ;ecx = p[2]
	mov dword[eax+offs_cont_normal_array],ecx
	ret
endp

align 4
proc glNormalPointer uses eax, type:dword, stride:dword, pointer:dword
locals
	p rd 3
endl
;  assert(type == GL_FLOAT);

	mov dword[p],OP_NormalPointer
	mov eax,[stride]
	mov dword[p+4],eax
	mov eax,[pointer]
	mov dword[p+8],eax

	mov eax,ebp
	sub eax,12 ;=sizeof(dd)*3
	stdcall gl_add_op,eax
	ret
endp

align 4
proc glopTexCoordPointer uses eax ebx ecx, context:dword, p:dword
	mov eax,[context]
	mov ebx,[p]
	mov ecx,[ebx+4] ;ecx = p[1]
	mov dword[eax+offs_cont_texcoord_array_size],ecx
	mov ecx,[ebx+8] ;ecx = p[2]
	mov dword[eax+offs_cont_texcoord_array_stride],ecx
	mov ecx,[ebx+12] ;ecx = p[3]
	mov dword[eax+offs_cont_texcoord_array],ecx
	ret
endp

align 4
proc glTexCoordPointer uses eax, size:dword, type:dword, stride:dword, pointer:dword
locals
	p rd 4
endl
;  assert(type == GL_FLOAT);

	mov dword[p],OP_TexCoordPointer
	mov eax,[size]
	mov dword[p+4],eax
	mov eax,[stride]
	mov dword[p+8],eax
	mov eax,[pointer]
	mov dword[p+12],eax

	mov eax,ebp
	sub eax,16 ;=sizeof(dd)*4
	stdcall gl_add_op,eax
	ret
endp
