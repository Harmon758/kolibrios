;
;   Pong Gaem 3Ver Mini Sample by Pavlushin Evgeni for ASCL
;   www.waptap@mail.ru
;
;   Not use bmpfile!

;******************************************************************************
format binary as ""

    use32
    org    0x0
    db     'MENUET01'              ; 8 byte id
    dd     0x01                    ; header version
    dd     START                   ; start of code
    dd     IM_END                  ; size of image
    dd     0x300000                ; memory for app
    dd     0x300000                ; esp
    dd     temp_area, 0x0          ; I_Param , I_Path

;******************************************************************************

include '..\..\..\macros.inc'
include 'ascl.inc'
include 'ascgl.inc'
include 'ascgml.inc'


START:                          ; start of execution
        call    draw_window

        convbmp pongfile, tsoi
        bmptoimg pongfile, tsoi,pong
        getimg pong, 0, 0, 80, 4, img
        getimg pong, 0, 4, 80, 4, img2
        getimg pong, 5, 38, 15, 15, img3
        getimg pong, 0, 8, 80, 20, img5
        fullimg img4, 80 ,20 ,0x00000000   ;black for rocket
        fullimg img6, 15 ,15 ,0x00000000   ;black for ball

still:
        scevent red,key,button

        ;mcall 48, 4 ;get skin width
        ;sub [skin_h], 16
        ;div eax, 2
        ;mov [skin_h], eax
        
        outcount [scoreb], 300, 4, 0x10000000 + cl_Blue, 5 shl 16
        outcount [scorea], 350, 4, 0x10000000 + cl_Red, 5 shl 16

del_images:
        setimg [ply1x], [ply1y], img4
        setimg [ply2x], [ply2y], img4
        setimg [ballx], [bally], img6
        setimg [gravx], [gravy], img4

move_ply1:
        correct [ply1x], [ply1rx], 4
        correct [ply1y], [ply1ry], 2

; automove ball
        mov     eax, [ballxv]
        add     [ballx], eax
        mov     eax, [ballyv]
        add     [bally], eax

;autoslow ball
        cmp     [ballxv], 0
        jl       ballb
balla:
        cmp     [ballxv], 2
        jng     balln
        dec     [ballxv]
        jmp     balln
ballb:
        cmp     [ballxv], -2
        jnl     balln
        inc     [ballxv]
        jmp     balln
balln:
        cmp     [ballyv], 2
        jng     by_n
        dec     [ballyv]
by_n:

;ball collusion of screen
        cmp     [ballx], 640-32
        jna     xa_ok
        neg     [ballxv]
xa_ok:
        cmp     [ballx], 6
        jnb     xb_ok
        neg     [ballxv]
xb_ok:

;if ball far out of screen come back
; is not work already
        cmp     [bally], 466
        jng     yax_ok
        call    draw_window
        inc     [scoreb]
        mov     [bally], 240
        mov     [ballx], 310
        mov     [ballyv], 2
        random 5, [ballxv]
        sub     [ballxv], 2
yax_ok:
        cmp     [bally], 30
        jnl     yax_ok2
        call    draw_window
        inc     [scorea]
        mov     [bally], 240
        mov     [ballx], 310
        mov     [ballyv], 2
        random 5, [ballxv]
        sub     [ballxv], 2
yax_ok2:

xorx:
        cmp     [ballxv], 0
        jne     norx
        random 5, [ballxv]
        sub     [ballxv], 2
        cmp     [ballxv], 0
        je      xorx
norx:

;test on collusion ply1 of ball
collusion_test:
        collimg img, [ply1x], [ply1y], img3, [ballx], [bally], eax
        cmp     eax, 1
        jne     not_coll
        neg     [ballyv]
        add     [bally], 4
;        neg     [ballxv]
not_coll:

;test on collusion com of ball
collusion_com:
        collimg img, [ply2x], [ply2y], img3, [ballx], [bally], eax
        cmp     eax, 1
        jne     not_collcom
        neg     [ballyv]
        sub     [bally], 4
;        neg dword [ballxv]
not_collcom:

;test on collusion gravity of ball
collusion_grav:
        collimg img, [gravx], [gravy], img3, [ballx], [bally], eax
        cmp     eax, 1
        jne     not_collg
        neg     [ballyv]

;        mov     [ballxv], -20

        cmp     [ballyv], 0
        jl      ab
        jg      bf
        jmp     not_collgx
ab:
        sub     [ballyv], 25
        jmp     not_collgx
bf:
        add     [ballyv], 25
not_collgx:
        cmp     [ballxv], 0
        jl      abx
        jg      bfx
        jmp     not_collg
abx:
        sub     [ballxv], 8 ;15
        jmp     not_collg
bfx:
        add     [ballxv], 8


;        mov     [ballyv], 20
not_collg:

;ply contorl
        cmp     [ply1rx], 560
        jna     plyok
        cmp     [ply1rx], 12000
        jna     paa
        mov     [ply1rx], 4
        jmp     plyok
paa:
        mov     [ply1rx], 560
plyok:


;com contorl
        cmp     [ply2x], 560
        jna     cplyok
        cmp     [ply2x], 12000
        jna     cpaa
        mov     [ply2x], 4
        jmp     cplyok
cpaa:
        mov     [ply2x], 560
cplyok:


;com move
cx_move:
        cmp     [bally], 160
        jna     cno_m
        mov     eax, [ballx]
        sub     eax, 30
        cmp     [ply2x], eax
        je      cno_m
        cmp     [ply2x], eax
        ja      cm_m
cm_p:
        add     [ply2x], 3
        jmp     cno_m
cm_m:
        sub     [ply2x], 3
cno_m:

;garvity
gravity:
        cmp     [gravtime], 0
        je      no_dg
        dec     [gravtime]
no_dg:

draw_gravity:
        cmp     [gravtime], 0
        je      nograv
        mov     eax, [ply1x]
        mov     ebx, [ply1y]
        add     ebx, 10
        mov     [gravx], eax
        mov     [gravy], ebx
        jmp     endgrav
nograv:
        mov     [gravx], 1000
        mov     [gravy], 1000
endgrav:

redraw_images:
        setimg [ply1x], [ply1y], img
        setimg [ply2x], [ply2y], img2
        setimg [ballx], [bally], img3
        setimg [gravx], [gravy], img5

        delay 1             ;don't generate delay for fast speed programm

        jmp     still

  red:
        call    draw_window
        jmp     still

  key:                          ; key
        mov     eax, 2
        mcall
        cmp     ah, key_Left
        jne     no_l
        sub     [ply1rx], 32 ;16
no_l:
        cmp     ah, key_Right
        jne     no_r
        add     [ply1rx], 32 ;16
no_r:
        cmp     ah, key_Up
        jne     no_u
        sub     [ply1ry], 16
no_u:
        cmp     ah, key_Down
        jne     no_d
        add     [ply1ry], 16
no_d:
        cmp     ah, key_Space
        jne     no_sp
        mov     [gravtime], 100
no_sp:

        jmp     still

  button:                       ; button
        mov     eax, 17                 ; get id
        mcall
        cmp     ah, 1                   ; button id=1 ?
        jne     noclose
        mov     eax, -1                 ; close this program
        mcall
  noclose:
        jmp     still


draw_window:
        mcall 12,1 ;start window redraw
        mcall 0, <10, 640+8>, <10, 480+24>, 0x14000000,, wtitle 
        mcall 12,2 ;end window redraw
        ret


; DATA AREA
wtitle db 'PONG: USE ARROW KEYS           SCORE:',0
;skin_h dd 25

xt              dd 100
yt              dd 100

gravtime        dd 10
gravx           dd 1000
gravy           dd 1000

ply1rx          dd 200
ply1ry          dd 50

ply1x           dd 200
ply1y           dd 50

ply2x           dd 200
ply2y           dd 400

ballx           dd 200
bally           dd 200

ballyv          dd 3
ballxv          dd 3

scorea          dd 0
scoreb          dd 0

counter         dd 0
tsoi            dd 0

pongfile        file 'pong.bmp'

IM_END:

temp            rb 20000

pong            rb 80*60*3+8
img             rb 32*32*3+8
img2            rb 32*32*3+8
img3            rb 32*32*3+8
img4            rb 80*20*3+8
img5            rb 32*32*3+8
img6            rb 15*15*3+8

temp_area       rb 0x15000

I_END:
