
typedef unsigned int u32_t;
typedef unsignet int handle;
typedef unsigned int color_t;


handle CreateHatchBrush(int hatch, color_t bkcolor, color_t fcolor);

������� ������������ ����� �������� 8�8 ��������

hatch     ��� ���������:

          HS_HORIZONTAL   -------
          HS_VERTICAL     |||||||
          HS_FDIAGONAL    \\\\\\\
          HS_BDIAGONAL    ///////
          HS_CROSS        +++++++
          HS_DIAGCROSS    xxxxxxx

bkcolor   ���� "0"

fcolor    ���� "1"


������������ ��������: ���������� ����� ����� ��� 0




handle CreateMonoBrush(color_t bkcolor, color_t fcolor,
                         u32_t bmp0, u32_t bmp1);

������� ����������� ����� �������� 8�8 ��������

bkcolor     ���� "0"

fcolor      ���� "1"

bmp0 bmp1   ����������� ������ 8�8 ��������


������������ ��������: ���������� ����� ����� ��� 0



void     DestroyBrush(handle brush);

���������� �����.

brush     ���������� ����� �����.


����� ������ ���� ������� ������� CreateHatchBrush ��� CreateMonoBrush




handle CreatePixmap(unsigned width, unsigned height, u32_t format, u32_t flags);

������� ������

width    ������ � ��������. �������� 2048

height   ������ � ��������. �������� 2048

format   ������ ��������. ������ �������������� ������ ARGB32

flags    �������������� �����:

         PX_MEM_SYSTEM =  0 ������ � ��������� ������
         PX_MEM_LOCAL  =  1 ������ � ��������� �����������
         PX_MEM_GART   =  2 ���������������
         ��������� ���� ��������������� � ������ ���� 0


������������ ��������:  ���������� ����� ������� � ������ ������ ��� 0




int DestroyPixmap( handle pixmap)

���������� ������.

pixmap   ���������� ����� �������.

������ ������ ���� ������ CreatePixmap.


������������ ��������: ERR_OK � ������ ������ ��� ERR_PARAM � ������ �������.




int ClearPixmap(handle pixmap, color_t color)

��������� ������ ��������� ������;

pixmap       ���������� ����� �������. SCR_PIXMAP ��� ���������� ������.

color        ���� � ������� ARGB32


������������ ��������: ERR_OK � ������ ������ ��� ERR_PARAM � ������ �������.




int Line(handle pixmap, int x0, int y0, int x1, int y1, color_t color)

���������� �������� ����� ��������� ����� �������� � 1 �������.

pixmap       ���������� ����� ������� � ������� ����� ������������ ���������.
             SCR_PIXMAP ��� ���������� ������

x0,y0 x1,y1  ���������� ��������� � �������� ����� ������

color        ���� � ������� ARGB32


������������ ��������: ERR_OK � ������ ������ ��� ERR_PARAM � ������ �������.




int DrawRect(handle pixmap, int xorg, int yorg,
             int width, int height,
             color_t dst_color, color_t border)

���������� �������� ������������ ��������� ����� c ����������.

pixmap     ���������� ����� ������� � ������� ����� ������������ ���������.
           SCR_PIXMAP ��� ���������� ������

xorg,yorg  ���������� ������ �������� ���� �������������� � ��������

width      ������ �������������� � ��������

height     ������ �������������� � ��������

color      ���� �������������� � ������� ARGB32

border     ���� ��������� � ������� ARGB32. ������� ��������� 1 ������.


������������ ��������: ERR_OK � ������ ������ ��� ERR_PARAM � ������ �������.




int FillRect(handle pixmap, int xorg, int yorg,
             int width, int height,
             brush_t *brush, color_t border)

���������� ������������� ��������� �����

pixmap     ���������� ����� ������� � ������� ����� ������������ ���������.
           SCR_PIXMAP ��� ���������� ������

xorg,yorg  ���������� ������ �������� ���� �������������� � ��������

width      ������ �������������� � ��������

height     ������ �������������� � ��������

brush      ����������� ����� �������� 8�8 ��������

border     ���� ��������� � ������� ARGB32. ������� ��������� 1 ������.
           ��������� �� �������� ���� ����� �������� ����� ����� 0.


����� ������ ���� ������� CreateHatch ��� CreateMonoBrush.


������������ ��������: ERR_OK � ������ ������ ��� ERR_PARAM � ������ �������.




int Blit(handle dst_pixmap, int dst_x, int dst_y,
         handle src_pixmap, int src_x, int src_y,
         int width, int height)

����������� ������������� ������� ��������.

dst_pixmap    ���������� ����� ������� � ������� ����� �����������
              �����������. SCR_PIXMAP ��� ���������� ������.

dst_x, dst_y  ���������� ������ �������� ���� ������� ���������

src_pixmap    ���������� ����� ������� - ��������� ��������.
              SCR_PIXMAP ��� ���������� ������.

src_x,src_y   ���������� ������ �������� ���� ���������� �������

width         ������ ���������� �������

height        ������ ���������� �������

������������ ��������: ERR_OK � ������ ������ ��� ERR_PARAM � ������ �������.




int TransparentBlit(handle dst_pixmap, int dst_x, int dst_y,
         handle src_pixmap, int src_x, int src_y,
         int width, int height, color_t key)

����������� ������������� ������� �������� ��������� ���������� ����.

dst_pixmap    ���������� ����� ������� � ������� ����� �����������
              �����������. SCR_PIXMAP ��� ���������� ������.

dst_x, dst_y  ���������� ������ �������� ���� ������� ���������

src_pixmap    ���������� ����� ������� - ��������� ��������.
              SCR_PIXMAP ��� ���������� ������.

src_x,src_y   ���������� ������ �������� ���� ���������� �������

width         ������ ���������� �������

height        ������ ���������� �������

key           ���������� ���� � ������� ARGB32


������� �� �������� ������� ���� ������� ��������� � key.


������������ ��������: ERR_OK � ������ ������ ��� ERR_PARAM � ������ �������.
