// gfxdef.h

#define MAIN_FACE_NDX		0
#define GAME_FACE_NDX		1
#define BUTTONS_NDX			2
#define NUMBERS_NDX			3

#define BONUS_FREE_BLOCK	11
#define BONUS_DIAG_BLOCK	12

#define WNDLEFT			64
#define WNDTOP			64
#define WNDHEADCOLOUR	0xD4C233
#define WNDTITLECOLOUR	0x47151C

//
#define blockSize		41
#define blocksNum		15

extern int maxGameLevel;
extern int startGameLevel;

#define START_LEVEL				1

////
//struct CPoint
//{
//	int x, y;
//	//
//	CPoint()
//	{
//		this->x = 0;
//		this->y = 0;
//	};
//	//
//	CPoint( int iX, int iY )
//	{
//		this->x = iX;
//		this->y = iY;
//	};
//	//
//	CPoint( CPoint &pt )
//	{
//		this->x = pt.x;
//		this->y = pt.y;
//	};
//};


// �������� ������� ������� � �����
struct SCompBmpHeader
{
	short int sizeX;
	short int sizeY;
	int compressedSize;
	int physicalOffset;
	int uncompressedSize;
};


// ����� ������� ��� ������������� � ���������
class CKosBitmap
{
protected:
	int bmpID;
	RGB *buffer;
	Word sizeX;
	Word sizeY;
public:
	CKosBitmap();
	~CKosBitmap();
	// �������� �� ������� �����
	bool LoadFromArch( SCompBmpHeader *bmpArchDesc, CKosFile *fromFile, int ID );
	// ������� � ���� ��������
	void Draw( Word x, Word y );
	// �������� ��������� �� ������� ������
	RGB *GetBits();
	// �������� ������ ��������
	void GetSize( Word &cx, Word &cy );
	// ������� �������� �� �������� �������� �������
	void Scale(Word size, RGB* mainBits);
};


// ����� ����� �������� ����
class CFishka
{
protected:
	//
	RGB *bits;
	//
	RGB transColour;
	//
	RGB *highLighted;
public:
	CFishka( CKosBitmap *fromBmp, int yOffset, RGB insColour );
	virtual ~CFishka();
	virtual RGB * GetBits(void);
	virtual RGB * GetHighlightedBits(void);
};

