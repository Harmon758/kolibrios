include 'my_skin.inc'

SKIN_PARAMS \
  height          = bmp_base.height,\     ; skin height
  margins         = [19:1:43:1],\         ; margins [left:top:right:bottom]
  colors active   = [binner=0xD0D0D0:\    ; border inner color
                     bouter=0xD0D0D0:\    ; border outer color
                     bframe=0x727272],\   ; border frame color
  colors inactive = [binner=0xDDDDDD:\    ; border inner color
                     bouter=0xDDDDDD:\    ; border outer color
                     bframe=0x919191],\   ; border frame color
  dtp             = 'GREY.DTP'   ; dtp colors

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

BITMAP bmp_left ,'Active/left.bmp'               ; skin bitmaps
BITMAP bmp_oper ,'Active/oper.bmp'
BITMAP bmp_base ,'Active/base.bmp'
BITMAP bmp_left1,'Inactive/left.bmp'
BITMAP bmp_oper1,'Inactive/oper.bmp'
BITMAP bmp_base1,'Inactive/base.bmp'

;================================;
; Created by Rock_maniak_forever ;
;================================;

; A2A2A2

; 727272