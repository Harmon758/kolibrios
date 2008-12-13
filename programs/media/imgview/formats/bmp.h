#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER 
{
  Word   bfType;       //��� ����� (��� �������� ������ - BM)
  Dword  bfSize;       //������ ����� � dword
  Word   bfReserved1;  //�� ������������
  Word   bfReserved2;  //�� ������������
  Dword  bfOffbits;    //�������� ������ �������� ������ �� ��������� � ������
} tagBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  Dword  biSize;          //����� ����, ���������� ���������� BITMAPINFOHEADER
  Dword  biWidth;         //������ �������� ������ � ��������
  Dword  biHeight;        //������ �������� ������ � �������� 
  Word   biPlanes;	  //����� ������� ���������� ����������
  Word   biBitCount;	  //����� ����� �� �������
  Dword  biCompression;	  //��� ������
  Dword  biSizeImage;	  //������ �������� � ������
  Dword  biXPelsPerMeter; //�������������� ���������� ����������, ������/�
  Dword  biYPelPerMeter;  //������������ ���������� ����������, ������/�
  Dword  biClrUsed;       //����� ������������ ������
  Dword  biClrImportant;  //����� "������" ������
} tagBITMAPINFOHEADER;

typedef struct tagRGBQUAD
{
  Byte   rgbBlue;
  Byte   rgbGreen;
  Byte   rgbRed;
  Byte   rgbReserved;  
} tagRGBQUAD;
#pragma pack(pop)

class BMPFile
{
protected:
  tagBITMAPFILEHEADER Bmp_head;
  tagBITMAPINFOHEADER Info_head;
public:
  Dword width;
  Dword height;
  Byte* buffer;
  int  BMPFile::LoadBMPFile(Byte* filebuff, Dword filesize);
};