
///===========================================
///
/// ���������� ������� ������� ����������
///
///
/// ������� ��� ��� ���� � ����� algolist.manual.ru
/// 
/// ����������� �. ������� aka Albom (albom85@yandex.ru)
///===========================================


///===========================================
/// ���������� ��� ���������� ���� int (4 �����)
///===========================================
void qsi(int *a, int n)
{

int i, j;
int temp, p;

p = *(a+(n>>1));

i = 0; 
j = n;

do 
	{
	while ( *(a+i) < p ) i++;
	while ( *(a+j) > p ) j--;

	if (i <= j) 
		{
		temp = *(a+i);
		*(a+i) = *(a+j);
		*(a+j) = temp;
		i++;
		j--;
		}
	} while ( i<=j );

if ( j > 0 ) 
	qsi(a, j);
if ( n > i ) 
	qsi(a+i, n-i);

}

///===========================================
/// ���������� ��� ���������� ���� short int (2 �����)
///===========================================

void qss(short *a, int n)
{

int i, j;
short temp, p;

p = *(a+(n>>1));

i = 0; 
j = n;

do 
	{
	while ( *(a+i) < p ) i++;
	while ( *(a+j) > p ) j--;

	if (i <= j) 
		{
		temp = *(a+i);
		*(a+i) = *(a+j);
		*(a+j) = temp;
		i++;
		j--;
		}
	} while ( i<=j );

if ( j > 0 ) 
	qss(a, j);
if ( n > i ) 
	qss(a+i, n-i);

}

///===========================================
/// ���������� ��� ���������� ���� char (1 ����)
///===========================================

void qsc(char *a, int n)
{

int i, j;
char temp, p;

p = *(a+(n>>1));

i = 0; 
j = n;

do 
	{
	while ( *(a+i) < p ) i++;
	while ( *(a+j) > p ) j--;

	if (i <= j) 
		{
		temp = *(a+i);
		*(a+i) = *(a+j);
		*(a+j) = temp;
		i++;
		j--;
		}
	} while ( i<=j );

if ( j > 0 ) 
	qsc(a, j);
if ( n > i ) 
	qsc(a+i, n-i);

}

///===========================================
/// ���������� ��� ���������� ���� unsigned int (4 �����)
///===========================================
void qsui(unsigned *a, int n)
{

int i, j;
unsigned temp, p;

p = *(a+(n>>1));

i = 0; 
j = n;

do 
	{
	while ( *(a+i) < p ) i++;
	while ( *(a+j) > p ) j--;

	if (i <= j) 
		{
		temp = *(a+i);
		*(a+i) = *(a+j);
		*(a+j) = temp;
		i++;
		j--;
		}
	} while ( i<=j );

if ( j > 0 ) 
	qsui(a, j);
if ( n > i ) 
	qsui(a+i, n-i);

}

///===========================================
/// ���������� ��� ���������� ���� unsigned short int (2 �����)
///===========================================

void qsus(unsigned short *a, int n)
{

int i, j;
unsigned short temp, p;

p = *(a+(n>>1));

i = 0; 
j = n;

do 
	{
	while ( *(a+i) < p ) i++;
	while ( *(a+j) > p ) j--;

	if (i <= j) 
		{
		temp = *(a+i);
		*(a+i) = *(a+j);
		*(a+j) = temp;
		i++;
		j--;
		}
	} while ( i<=j );

if ( j > 0 ) 
	qsus(a, j);
if ( n > i ) 
	qsus(a+i, n-i);

}

///===========================================
/// ���������� ��� ���������� ���� unsigned char (1 ����)
///===========================================

void qsuc(unsigned char *a, int n)
{

int i, j;
unsigned char temp, p;

p = *(a+(n>>1));

i = 0; 
j = n;

do 
	{
	while ( *(a+i) < p ) i++;
	while ( *(a+j) > p ) j--;

	if (i <= j) 
		{
		temp = *(a+i);
		*(a+i) = *(a+j);
		*(a+j) = temp;
		i++;
		j--;
		}
	} while ( i<=j );

if ( j > 0 ) 
	qsuc(a, j);
if ( n > i ) 
	qsuc(a+i, n-i);

}


///===========================================
/// ���������� ��� ���������� ���� float (4 �����)
///===========================================

void qsf(float *a, int n)
{

int i, j;
float temp, p;

p = *(a+(n>>1));

i = 0; 
j = n;

do 
	{
	while ( *(a+i) < p ) i++;
	while ( *(a+j) > p ) j--;

	if (i <= j) 
		{
		temp = *(a+i);
		*(a+i) = *(a+j);
		*(a+j) = temp;
		i++;
		j--;
		}
	} while ( i<=j );

if ( j > 0 ) 
	qsf(a, j);
if ( n > i ) 
	qsf(a+i, n-i);

}

///===========================================
/// ���������� ��� ���������� ���� double (8 ����)
///===========================================

void qsd(double *a, int n)
{

int i, j;
double temp, p;

p = *(a+(n>>1));

i = 0; 
j = n;

do 
	{
	while ( *(a+i) < p ) i++;
	while ( *(a+j) > p ) j--;

	if (i <= j) 
		{
		temp = *(a+i);
		*(a+i) = *(a+j);
		*(a+j) = temp;
		i++;
		j--;
		}
	} while ( i<=j );

if ( j > 0 ) 
	qsd(a, j);
if ( n > i ) 
	qsd(a+i, n-i);

}

///===========================================


#define NULL ((void*)0)

typedef struct
{
void	*name;
void	*function;
} export_t;

char	szQsi[] = "qsi";
char	szQss[] = "qss";
char	szQsc[] = "qsc";
char	szQsui[] = "qsui";
char	szQsus[] = "qsus";
char	szQsuc[] = "qsuc";
char	szQsf[] = "qsf";
char	szQsd[] = "qsd";

export_t EXPORTS[] =
{
{ szQsi, (void*) qsi },
{ szQss, (void*) qss },
{ szQsc, (void*) qsc },
{ szQsui, (void*) qsui },
{ szQsus, (void*) qsus },
{ szQsuc, (void*) qsuc },
{ szQsf, (void*) qsf },
{ szQsd, (void*) qsd },
{ NULL, NULL },
};
