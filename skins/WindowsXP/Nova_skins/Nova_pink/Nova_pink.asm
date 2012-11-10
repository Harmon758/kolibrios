include 'my_skin.inc'

SKIN_PARAMS \
  height          = bmp_base.height,\     ; skin height
  margins         = [19:1:43:1],\         ; margins [left:top:right:bottom]
  colors active   = [binner=0xD79ED2:\    ; border inner color
                     bouter=0xD79ED2:\    ; border outer color
                     bframe=0x7C2C76],\   ; border frame color
  colors inactive = [binner=0xE2B6DE:\    ; border inner color
                     bouter=0xE2B6DE:\    ; border outer color
                     bframe=0x994A95],\   ; border frame color
  dtp             = 'PINK.DTP'   ; dtp colors

SKIN_BUTTONS \
  close    = [-32:0][26:18],\             ; buttons coordinates
  minimize = [-49:0][17:18]               ; [left:top][width:height]

SKIN_BITMAPS \
  left active   = bmp_left,\              ; skin bitmaps pointers
  left inactive = bmp_left1,\
  oper active   = bmp_oper,\
  oper inactive = bmp_oper1,\
  base active   = bmp_base,\
  base inactive = bmp_base1

BITMAP bmp_left ,'active/left.bmp'               ; skin bitmaps
BITMAP bmp_oper ,'active/oper.bmp'
BITMAP bmp_base ,'active/base.bmp'
BITMAP bmp_left1,'inactive/left.bmp'
BITMAP bmp_oper1,'inactive/oper.bmp'
BITMAP bmp_base1,'inactive/base.bmp'

;================================;
; Created by Rock_maniak_forever ;
;================================;

; B03EA7

; 7C2C76