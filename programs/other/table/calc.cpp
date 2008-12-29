

#include "func.h"
#include "parser.h"
#include "calc.h"
#include "kosSyst.h"
//#include "KosFile.h"

extern DWORD def_col_width, def_row_height;
extern DWORD col_count, row_count;
extern char ***cells;
extern DWORD *col_width, *row_height;
extern char ***values;

extern DWORD *col_left, *row_top;

// ����� ������
extern char ***buffer;
extern DWORD buf_col, buf_row;
extern DWORD buf_old_x, buf_old_y;

extern bool sel_moved;

int cf_x0, cf_x1, cf_y0, cf_y1;


#define sign(x) ((x) < 0 ? -1 : ((x) == 0 ? 0 : 1))


//extern const char er_file_not_found[];
//extern const char er_format[];
extern const char *sFileSign;

struct cell_list
{
	int x,y;
	cell_list *next;
};


// �������� �-���������� ������ � ������� �
int get_x(int x)
{
	int i, r = 0;
	if (x > col_count) 
		x = col_count;
	for (i = 0; i < x; i++)
		r+=col_width[i];
	return r;
}

// ������
int get_y(int y)
{
	int i, r = 0;
	if (y > row_count) 
		y = row_count;
	for (i = 0; i < y; i++)
		r+=row_height[i];
	return r;
}



// ��������� ��������� �������
char *make_col_cap(int i)
{
	char *r = (char*)allocmem(3);
	if (i <= 26)
	{
		r[0] = 'A' + i - 1;
		r[1] = '\0';
		return r;
	}
	else if (i % 26 == 0)	// ���� ���� ������� �� ����� - �� ������� ���� �������� � �����
	{
		r[0] = (i / 26) - 1 + 'A' - 1;
		r[1] = 'Z';
		r[2] = '\0';
		return r;
	}
	r[0] = (i / 26) + 'A' - 1;
	r[1] = (i % 26) + 'A' - 1;
	r[2] = '\0';
	return r;
}

// -"- ������
char *make_row_cap(int i)
{
	char *r = (char*)allocmem(3);
	if (i <= 9)
	{
		r[0] = '0' + i;
		r[1] = '\0';
		return r;
	}
	r[0] = (i / 10) + '0';
	r[1] = (i % 10) + '0';
	r[2] = '\0';
	return r;
}

// ������������� �����
void init()
{
	int i, j;

	//col_count = WND_W / def_col_width;
	//row_count = WND_H / def_row_height;

	col_width = (DWORD*)allocmem(col_count * sizeof(DWORD));
	row_height = (DWORD*)allocmem(row_count * sizeof(DWORD));
	col_left = (DWORD*)allocmem(col_count * sizeof(DWORD));
	row_top = (DWORD*)allocmem(row_count * sizeof(DWORD));
	for (i = 0; i < col_count; i++)
	{
		col_width[i] = def_col_width;
	}

	for (i = 0; i < row_count; i++)
	{
		row_height[i] = def_row_height;
	}

	cells = (char***)allocmem(col_count * sizeof(char**));
	values = (char***)allocmem(col_count * sizeof(char**));
	for (i = 0; i < col_count; i++)
	{
		cells[i] = (char**)allocmem(row_count * sizeof(char*));
		values[i] = (char**)allocmem(row_count * sizeof(char*));
		for (j = 0; j < row_count; j++)
		{
			cells[i][j] = NULL;
			if (i == 0 && j)
			{
				cells[i][j] = make_row_cap(j);
			}
			else if (j == 0 && i)
			{
				cells[i][j] = make_col_cap(i);
			}

		}
	}
}

void reinit()
{
	int i, j;

	for (i = 0; i < col_count; i++)
	{
		col_width[i] = def_col_width;
	}

	for (i = 0; i < row_count; i++)
	{
		row_height[i] = def_row_height;
	}

	for (i = 1; i < col_count; i++)
	{
		for (j = 1; j < row_count; j++)
		{
			if (cells[i][j])
				freemem(cells[i][j]);
			cells[i][j] = NULL;
			if (values[i][j])
				freemem(values[i][j]);
			values[i][j] = NULL;
		}
	}
}

void fill_cells(int sel_x, int sel_y, int sel_end_x, int sel_end_y, int old_end_x, int old_end_y)
{
	// ����, (sel_x, sel_y) :: (old_end_x, old_end_y) - ��������
	// ��������� �������� ���� � ������ sel_x .. sel_end_x, ���� � ������� sel_y .. sel_end_y
	
	int i, start, end, step, gdir = -1;
	int pdir = -1;
	char *source;

	cf_x0 = cf_y0 = 0;
	cf_x1 = col_count;
	cf_y1 = row_count;

	if (sel_end_x == -1)
		sel_end_x = sel_x;
	if (sel_end_y == -1)
		sel_end_y = sel_y;

	// ���� ����������� ��������� ���������������, �� ������ � ����� ��������� �� ��, ��� ��� 1 ������:

	if (old_end_x == sel_end_x && sel_y == old_end_y)
	{
		gdir = 0;
	}
	else if (old_end_y == sel_end_y && sel_x == old_end_x)
	{
		gdir = 1;
	}

	//sprintf(debuf, "fuck in ass %U %U %U %U %U %U dir %U",sel_x,sel_y,sel_end_x,sel_end_y,old_end_x,old_end_y,gdir);
	//rtlDebugOutString(debuf);
	if (gdir != -1)
	{
		int gstep = gdir ? sign(old_end_y - sel_y) : sign(old_end_x - sel_x);
		if (gstep == 0)
		{
		/*	if (gdir)
			{
				//old_end_y += 1;
			}
			else
			{
				//old_end_x += 1;
			}
		*/
			gstep = 1;
		}

		for (;gdir ? (sel_y != old_end_y + gstep) : (sel_x != old_end_x + gstep); 
			gdir ? (sel_y += gstep) : (sel_x += gstep))
		{
			//sprintf(debuf, "cycle %U %U %U %U %U %U dir %U",sel_x,sel_y,sel_end_x,sel_end_y,old_end_x,old_end_y,gdir);
			//rtlDebugOutString(debuf);
			int dir;
			source = cells[sel_x][sel_y];
			if (gdir == 0)
			{
				start = sel_y;
				end = sel_end_y;
				step = (sel_y < sel_end_y ? 1 : -1);
				dir = 1;
			}
			else
			{
				start = sel_x;
				end = sel_end_x;
				step = (sel_x < sel_end_x ? 1 : -1);
				dir = 0;
			}

			//sprintf(debuf, "cyc %U %U %U %U",start,end,step,dir);
			//rtlDebugOutString(debuf);
			for (i = start + step; i != end + step; i += step)
			{
				//char **p = &cells[dir ? sel_x : i][dir ? i : sel_end_y];
				//sprintf(debuf, "to %U %U dir %U copying '%S'",dir ? sel_x : i,dir ? i : sel_y,dir,source);
				//rtlDebugOutString(debuf);
				if (cells[dir ? sel_x : i][dir ? i : sel_y])
				{
					freemem(cells[dir ? sel_x : i][dir ? i : sel_y]);
				}
				if (source)
				{
					cells[dir ? sel_x : i][dir ? i : sel_y] = change_formula(source, dir ? 0 : (i - start), dir ? (i - start) : 0);
					//cells[dir ? sel_x : i][dir ? i : sel_y] = (char *)allocmem(strlen(source) + 1);
					//strcpy(cells[dir ? sel_x : i][dir ? i : sel_y], source);
				}
				else
					cells[dir ? sel_x : i][dir ? i : sel_y] = NULL;
			}
		}
	}

	// � ��� ���� �����������...
	/*
	
	if (sel_x == sel_end_x && sel_x == old_end_x)
	{
		pdir = 0;
	}
	if (sel_y == sel_end_y && sel_y == old_end_y)
	{
		pdir = 1;
	}
	if (pdir != -1)
	{
		// �������������� ���������� - ���� �����. � ���� ����������� ���������, ���� ���

		sprintf(debuf, "maybe arith dir %U", pdir);
		rtlDebugOutString(debuf);

		int is_arith = 1;
		int gstep = pdir ? sign(old_end_y - sel_y) : sign(old_end_x - sel_x);
		if (gstep == 0)
			gstep = 1;

		for (int i = pdir ? sel_y : sel_x; i != pdir ? (old_end_y + gstep) : (old_end_x + gstep); i++)
		{
			convert_error = 0;
			sprintf(debuf,"cell %U %U", !pdir ? sel_x : i, !pdir ? i : sel_y);
			rtlDebugOutString(debuf);
			if (cells[!pdir ? sel_x : i][!pdir ? i : sel_y])
			{
				double d = atof(cells[!pdir ? sel_x : i][!pdir ? i : sel_y]);
				if (convert_error)
				{
					rtlDebugOutString("failed arith");
					is_arith = 0;
					break;
				}
			}
			else
			{
				is_arith = 0;
				rtlDebugOutString("failed arith in null");
				break;
			}
		}

		double arith_first, arith_step;
		if (is_arith)
		{
			rtlDebugOutString("really arith");
			arith_first = atof(cells[sel_x][sel_y]);
			arith_step = atof(cells[pdir ? sel_x : old_end_x][pdir ? sel_y : old_end_y]) - arith_first;
			arith_first += arith_step * pdir ? abs(sel_end_x - old_end_x) : abs(sel_end_y - old_end_y);
		}
		else
			rtlDebugOutString("none arith");

		// ���������� ����������
		for (i = pdir ? old_end_y : old_end_x; i != pdir ? (sel_end_y + gstep) : (sel_end_x + gstep); i++)
		{
			if (cells[pdir ? sel_x : i][pdir ? i : sel_y])
				freemem(cells[pdir ? sel_x : i][pdir ? i : sel_y]);
			if (is_arith)
			{
				cells[pdir ? sel_x : i][pdir ? i : sel_y] = ftoa(arith_first);
				arith_first += arith_step;
			}
			else
			{
				if (cells[sel_x][sel_y])
				{
					cells[pdir ? sel_x : i][pdir ? i : sel_y] = (char*)allocmem(strlen(cells[sel_x][sel_y]) + 1);
					strcpy(cells[pdir ? sel_x : i][pdir ? i : sel_y], cells[sel_x][sel_y]);
				}
			}
		}
	}
	*/

	calculate_values();
}

const char *csv_name = ".csv";

int str_is_csv(char *str)
{
	int i, j = 0;

	for (i = 0; i < strlen(str); i++)
	{
		if (str[i] == csv_name[j])
		{
			j++;
			if (j == strlen(csv_name))
				return 1;
		}
		else
		{
			j = 0;
		}
	}
	return 0; 
}

int Kos_FileWrite(kosFileInfo &fileInfo, char *line, int mode = 3) // ���� mode = 2 - ������������ ����
{
	int res = 0;
	fileInfo.dataCount = strlen(line);
	fileInfo.bufferPtr = (Byte*)line;
	fileInfo.rwMode = mode;
	res = kos_FileSystemAccess(&fileInfo);
	if (res != 0)
		return 0;
	fileInfo.OffsetLow += fileInfo.dataCount;
	return 1;
}

int SaveCSV(char *fname)
{
	int i, j;
	int min_col = col_count, min_row = row_count, max_row = -1, max_col = -1;
	int first = 1;

	kosFileInfo fileInfo;
	memset((Byte*)&fileInfo, 0, sizeof(fileInfo));
	strcpy(fileInfo.fileURL,fname);
	fileInfo.OffsetLow = 0;
	fileInfo.OffsetHigh = 0;
	fileInfo.rwMode = 8;	// delete

	rtlDebugOutString("savecsv: old file deleted");

	for (i = 1; i < col_count; i++)
	{
		for (j = 1; j < row_count; j++)
		{
			if (cells[i][j])
			{
				min_col = min(min_col, i);
				min_row = min(min_row, j);
				max_col = max(max_col, i);
				max_row = max(max_row, j);
			}
		}
	}

	sprintf(debuf, "col %U %U row", min_col, max_col, min_row, max_row);
	rtlDebugOutString(debuf);

	for (j = min_row; j <= max_row; j++)
	{
		char buffer[1024]; // �� ���� ��� ������
		int buf_len = 0;

		memset((Byte*)buffer, 0, 1024);

		for (i = min_col; i <= max_col; i++)
		{
			char *cur = values[i][j] ? values[i][j] : cells[i][j];
			if (cur)
			{
				buffer[buf_len++] = '\"';
				for (int k = 0; k < strlen(cur); k++)
				{
					if (cur[k] == '\"')
						buffer[buf_len++] = '\"';	// ������� - �� ���
					buffer[buf_len++] = cur[k];
				}
				buffer[buf_len++] = '\"';
			}
			buffer[buf_len++] = ';';
		}
		rtlDebugOutString(buffer);
		// ��������� ������ ������ � ������
		buffer[buf_len++] = '\n';
		if (!Kos_FileWrite(fileInfo, buffer, first ? (first = 0, 2) : 3))
			return 0;
	}
	return 1;

}

#define BUF_FOR_ALL 5000
int SaveFile(char *fname)
{
	kosFileInfo fileInfo;
	char *buffer = (char*)allocmem(BUF_FOR_ALL);	// ����! �� ���� ��� ����������
	int filePointer = 0;

	int i,j;
	Dword res;

	if (str_is_csv(fname))
		return SaveCSV(fname);


	//rtlDebugOutString(fname);

	memset((Byte*)&fileInfo, 0, sizeof(fileInfo));
	strcpy(fileInfo.fileURL,fname);
	fileInfo.OffsetLow = 0;
	fileInfo.OffsetHigh = 0;
	fileInfo.rwMode = 8;
	res = kos_FileSystemAccess(&fileInfo);	// �������
	fileInfo.dataCount = strlen(sFileSign);
	fileInfo.bufferPtr = (Byte*)sFileSign;
	fileInfo.rwMode = 2;
	res = kos_FileSystemAccess(&fileInfo);
	if (res != 0)
		return 0;
	//sprintf(debuf, "create %U",res);
	//rtlDebugOutString(debuf);
	fileInfo.OffsetLow += fileInfo.dataCount;

	// ������ �������� ���������
	memset((Byte*)buffer,0,BUF_FOR_ALL);
	for (i = 1; i < col_count; i++)
	{
		char smalbuf[32];
		memset((Byte*)smalbuf,0,32);
		sprintf(smalbuf, "%U,", col_width[i]);
		strcpy(buffer+strlen(buffer),smalbuf);
	}
	buffer[strlen(buffer)-1] = '\n';	// �������� ��������� ������� �� ������� ������
	//rtlDebugOutString(buffer);
	fileInfo.dataCount = strlen(buffer);
	fileInfo.bufferPtr = (Byte*)buffer;
	fileInfo.rwMode = 3;
	res = kos_FileSystemAccess(&fileInfo);
	if (res != 0)
		return 0;

	// ���������� ����� � ���� ����
	// �� �� ������ �� �������� �� ���!
	fileInfo.OffsetLow += fileInfo.dataCount;

	// ������ ����� ��������� � ����� ��
	memset((Byte*)buffer,0,BUF_FOR_ALL);
	for (i = 1; i < row_count; i++)
	{
		char smalbuf[32];
		memset((Byte*)smalbuf,0,32);
		sprintf(smalbuf, "%U,", row_height[i]);
		strcpy(buffer+strlen(buffer),smalbuf);
	}
	buffer[strlen(buffer)-1] = '\n';	// �������� ��������� ������� �� ������� ������
	//rtlDebugOutString(buffer);
	fileInfo.dataCount = strlen(buffer);
	fileInfo.bufferPtr = (Byte*)buffer;
	fileInfo.rwMode = 3;
	res = kos_FileSystemAccess(&fileInfo);
	if (res != 0)
		return 0;

	// � ����� ��������� � ��� ����
	
	fileInfo.OffsetLow += fileInfo.dataCount;
	memset((Byte*)buffer,0,BUF_FOR_ALL);

	// ��������� ��������� ����� ��, ��������� ���������� �� ������

	for (i = 1; i < row_count; i++)
	{
		for (j = 1; j < col_count; j++)
			if (cells[j][i])
			{
				memset((Byte*)buffer,0,512);
				sprintf(buffer, "%U %U:%S\n", j, i, cells[j][i]);
				fileInfo.dataCount = strlen(buffer);
				fileInfo.bufferPtr = (Byte*)buffer;
				fileInfo.rwMode = 3;
				res = kos_FileSystemAccess(&fileInfo);
				if (res != 0)
					return 0;
				//sprintf(debuf, "create %U",res);
				//rtlDebugOutString(debuf);
				fileInfo.OffsetLow += fileInfo.dataCount;
			}
	}

	//rtlDebugOutString("saving finished");

	freemem(buffer);
	return 1;
}

char *Kos_FileRead(kosFileInfo &fileInfo, int &code)
{
	char buffer[512], *p, *r;
	fileInfo.dataCount = 512;
	fileInfo.rwMode = 0;
	fileInfo.bufferPtr = (Byte *)buffer;
	memset((Byte*)buffer, 0, 512);
	int z = kos_FileSystemAccess(&fileInfo);
	code = z;

	//sprintf(debuf, "kos file read %U", code);
	//rtlDebugOutString(debuf);

	if (z != 0 && z != 6)
		return NULL;

	p = buffer;
	while (*p && *p++ != '\n');

	if (p == buffer)
		return NULL;

	r = (char*)allocmem(p - buffer);
	memset((Byte*)r, 0, p - buffer);
	//strncpy(r, buffer, p - buffer);
	for (int l = 0; l < p - buffer - 1; l++)
		r[l] = buffer[l];
	fileInfo.OffsetLow += p - buffer;
	return r;
}

int LoadCSV(char *fname)
{
	// clear the table
	reinit();

	kosFileInfo fileInfo;
	strcpy(fileInfo.fileURL,fname);
	fileInfo.OffsetLow = 0;
	fileInfo.OffsetHigh = 0;

	char *line;

	int col = 1, row = 1;
	int code = 0;
	do 
	{
		line = Kos_FileRead(fileInfo, code);
		if (!line || *line == '\0' || (code != 0 && code != 6))
		{
			sprintf(debuf, "read end, line not null = %U, code = %U", !line, code);
			rtlDebugOutString(debuf);
			break;
		}
		sprintf(debuf, "read '%S' len %U", line, strlen(line));
		rtlDebugOutString(debuf);

		// ��������� ������
		// �������� ;, ������ ��� "
		int i = 0;
		while (i <= strlen(line))
		{
			int inPar = 0;
			// inPar: 0 - �� �������, 1 - ������ ��� ���� �������, 2 - ������� ����, �� �����
			int start = i;
			while (i <= strlen(line))
			{
				char c = line[i];
				if (!c)
					c = ';'; 
				int yes_semicolon = 0;

				switch (inPar)
				{
					case 0:
						if (c == '\"')
						{
							inPar = 1;
						}
						else
						{
							if (c == ';')
								yes_semicolon = 1;
						}
						break;
					case 1:
						inPar = 2;
						break;
					case 2:
						if (c == '\"')	// ��� ���������
						{
							inPar = 0;
						}
						/*else
						{
							if (c == ';')
								yes_semicolon = 1;

						}*/
						break;
				}
				if (yes_semicolon)
				{
					// ����, line[i] = ';'
					int tmp = line[start] == '"' ? 1 : 0;
					int sz = i - start - tmp * 2;
					if (sz > 0)
					{
						cells[col][row] = (char *)allocmem(sz + 1);
						memset((Byte*)cells[col][row], 0, sz + 1);
						int m = 0;
						for (int l = 0; l < sz; l++)
						{
							if (line[start + tmp + l] == '\"')
							{
								cells[col][row][m++] = '\"';
								l++;	// ���������� ��������� �������
							}
							else
								cells[col][row][m++] = line[start + tmp + l];
						}
						sprintf(debuf, "set %U %U = '%S'", col, row, cells[col][row]);
						rtlDebugOutString(debuf);
					}
					start = i + 1;
					col++;
				}
				i++;
			}
			row++;
			col = 1;
			i++;
		}

	} while(line);

	return 1;
}


int LoadFile(char *fname)
{
	kosFileInfo fileInfo;
	kosBDVK bdvk;
	int filePointer = 0, i, j;
	Dword res, filesize;
	char buffer[512 + 1];
	char *d, *s, *k;
	int step = 0, items;

	if (str_is_csv(fname))
		return LoadCSV(fname);


	//rtlDebugOutString(fname);

	strcpy(fileInfo.fileURL,fname);
	fileInfo.OffsetLow = 0;
	fileInfo.OffsetHigh = 0;

	fileInfo.rwMode = 5;
	fileInfo.bufferPtr = (Byte *)&bdvk;
	Dword rr = kos_FileSystemAccess(&fileInfo); // � CKosFile ��� ����������� �������
	//sprintf(debuf, "getsize: %U\n", rr);
	//rtlDebugOutString(debuf);
	if (rr != 0)
	{
		return -1;
	}

	// clear the table
	reinit();

	//rtlDebugOutString("clear done");

	filesize = bdvk.size_low;

	fileInfo.rwMode = 0;
	fileInfo.dataCount = strlen(sFileSign);
	fileInfo.bufferPtr = (Byte*)buffer;
	kos_FileSystemAccess(&fileInfo);
	s = (char*)sFileSign;
	d = buffer;
	while (*s && *d && *s++==*d++);		// ���������� ����
	if (*s != '\0' || *d != '\0')
	{
		return -2;
	}
	fileInfo.OffsetLow += fileInfo.dataCount;
	items = 1;
	while (fileInfo.OffsetLow < filesize)
	{
		// ��� ��������� �� �� ������ ���� ��������, � ����� ���� ����� ��������� �� ��?
		fileInfo.dataCount = 512;
		memset((Byte*)buffer, 0, 512);
		kos_FileSystemAccess(&fileInfo);
		//sprintf(debuf, "%U", fileInfo.OffsetLow);
		//rtlDebugOutString(debuf);
		//sprintf(debuf, "buffer: %S", buffer);
		//rtlDebugOutString(debuf);
		// ��� � ����� �� ����� �������
		// �� ��� ������� � ��������� ����
		// ����� ����� ����� ������ ������
		// ������� ��������������� �����

		switch (step)
		{
		case 0:			// �������
			d = buffer;
			while (*d && *d != ',' && *d != '\n') d++;	
			//d--;
			if (!*d)
			{	
				return -2;
			}
			*d = '\0';
			i = atoi(buffer);
			col_width[items++] = i;
			if (items == col_count)
			{
				step++;
				items = 1;	//	������ ������ ����� ������ �� ����� �����
							//  ���� �� ������������ � ���� ��� ������
				//sprintf(debuf, "cols read done last buf %S file pos %U",buffer,fileInfo.OffsetLow);
				//rtlDebugOutString(debuf);
			}
			d+=2;
			break;

		case 1:			// ������, ���� ������ ��������
			d = buffer;
			while (*d && *d != ',' && *d != '\n') d++;	
			//d--;
			if (!*d)
			{	
				//sprintf(debuf,"oh shit, error at %U",items);
				//rtlDebugOutString(debuf);
				return -2;
			}
			*d = '\0';
			i = atoi(buffer);
			row_height[items++] = i;
			/*if (items > 5)
			{
				sprintf(debuf, "set row from %S hei %U %U",buffer,items-1,i);
				rtlDebugOutString(debuf);
			}*/

			if (items == row_count)
			{
				step++;		// � ����� ����� ������ � �����
							// �������� ��� � ������ �������
							// ��� ����� ��� ��� � ������ �����
							// �� �� ����� �����, ������� � ���
							// ����� �� � ����� �� ������

							// �������� ��� ��� ��� �������� ������
							// �� ����� ������� - ���� ������� ������
							// ���������� - � �� ��� ��� ��������.
							// ��� ����� - � ��� ����� �� �������.
				//sprintf(debuf, "before read cells offset %U %X",fileInfo.OffsetLow,fileInfo.OffsetLow);
				//rtlDebugOutString(debuf);
			}
			d+=2;
			break;

			// �, ����� � ����� ������. � ����.
			// ����� ���, �� ������ ���� �������
			// ���� ���, ��� break �� ������ ����� casa
			// ������� ������, ��� �� �� �����
			// ����� �� � ����� ��������?

		case 2:			// ������, ��� �� ���������� ��������� ����� �� ������������
			d = buffer;
			while (*d && *d++ != ' ');	// ������. ����� ������ � ����� ���� ����....
			d--;
			if (!*d)
			{	
				return -2;
			}
			*d = '\0';
			i = atoi(buffer);
			d++;
			s=d;
			while (*d && *d++ != ':');	// �����-�� � ���������, ��� ���� ����� ����� ����� ������... �����
			d--;
			if (!*d)
			{	
				return -2;
			}
			*d = '\0';
			j = atoi(s);
			//rtlDebugOutString(s);
			d++;
			k = d;
			while (*d && *d++ != '\n');
			d--;
			*d = '\0';
			d+=2;
			//sprintf(debuf, "i:%U j:%U d:%S\n",i,j,k);
			//rtlDebugOutString(debuf);
			cells[i][j] = (char*)allocmem(strlen(k) + 1);
			//memset(cells[i][j], 0, strlen(k) + 1);
			strcpy(cells[i][j], k);
			//sprintf(debuf, "offset: %U", fileInfo.OffsetLow);
			//rtlDebugOutString(debuf);
		}
		fileInfo.OffsetLow += d - (char*)buffer - 1;
	}
	//rtlDebugOutString("loading finished");
	return 1;
}

// �������� ����� ������
void freeBuffer()
{
	int i, j;

	if (!buffer)
		return;
	for (i = 0; i < buf_col; i++)
	{
		for (j = 0; j < buf_row; j++)
			if (buffer[i][j])
				freemem(buffer[i][j]);
		freemem(buffer[i]);
	}
	freemem(buffer);
	buffer = NULL;
	buf_row = buf_col = 0;

}


// ����� - ���������� �� ��������

int abort_calc = 0;
cell_list *last_dep;

// ���, ��� �� ���, � ��� ���� ����
//#define allocmem2(x) allocmem(x+1000)

double calc_callback(char *str)
{
	int i,j,x,y;

	if (abort_calc == 1)
		return 0.0;

	//rtlDebugOutString(str);
	if (*str == '$') str++;
	for (i = 0; i < strlen(str); i++)
		if (str[i] >= '0' && str[i] <= '9')
			break;
	if (str[i-1] == '$')
		i--;
	if (i == strlen(str))
	{
		abort_calc = 1;
		serror(ERR_BADVARIABLE);
		return 0.0;
	}
	x = -1;
	for (j = 0; j < col_count; j++)
//		if (strnicmp(str,cells[j][0],i-1)==0)
		if (str[0] == cells[j][0][0] && ((i == 1) || (str[1] == cells[j][0][1])))
		{
			x = j;
			break;
		}
	if (str[i] == '$')
		i++;
	y = -1;
	for (j = 0; j < row_count; j++)
		if (strcmp(str+i,cells[0][j])==0)
		{
			y = j;
			break;
		}
	if (x == -1 || y == -1)
	{
		abort_calc = 1;
		serror(ERR_BADVARIABLE);
		return 0.0;
	}

	double hold;
	if (values[x][y])
		if (values[x][y][0] == '#')
		{
			serror(ERR_BADVARIABLE);
			abort_calc = 1;
		}
		else
		{
			hold = atof(values[x][y]);
			//if (convert_error)				// ���������� ������...
			//{
			//	serror(ERR_BADVARIABLE);
			//	abort_calc = 1;
			//}
		}
	else
	{
		if (cells[x][y])
		{
			hold = atof(cells[x][y]);
			if (convert_error == ERROR || convert_error == ERROR_END)
			{
				serror(ERR_BADVARIABLE);
				abort_calc = 1;
			}
		}
		else
		{
			sprintf(debuf, "bad var %S", str);
			rtlDebugOutString(debuf);
			serror(ERR_BADVARIABLE);
			abort_calc = 1;
		}
	}
	return hold;
}

double depend_callback(char *str)
{
	cell_list *cur;
	// ���� ������� �� ��47 �������� � � �.
	int i,j,x,y;

	if (abort_calc == 1)
		return 0.0;

	if (*str == '$') str++;
	for (i = 0; i < strlen(str); i++)
		if (str[i] >= '0' && str[i] <= '9')
			break;
	if (str[i-1] == '$')
		i--;
	if (i == strlen(str))
	{
		abort_calc = 1;
		serror(ERR_BADVARIABLE);
		return 0.0;
	}
	x = -1;
	for (j = 1; j < col_count; j++)
		//if (strncmp(str,cells[j][0],i)==0)
		if (str[0] == cells[j][0][0] && ((i == 1) || (str[1] == cells[j][0][1])))
		{
			x = j;
			break;
		}
	if (str[i] == '$')
		i++;

	y = -1;
	for (j = 1; j < row_count; j++)
		if (strcmp(str+i,cells[0][j])==0)
		{
			y = j;
			break;
		}
	if (x == -1 || y == -1)
	{
		abort_calc = 1;
		serror(ERR_BADVARIABLE);
		return 0.0;
	}
	cur = (cell_list*)allocmem(sizeof(cell_list));
	cur->x = x;
	cur->y = y;
	cur->next = last_dep;
	last_dep = cur;

	return 0.0;
}

cell_list *find_depend(char *str)
{
	double hold;
	last_dep = NULL;
	find_var = &depend_callback;
	set_exp(str);
	get_exp(&hold);

	return last_dep;
}

bool is_in_list(cell_list *c1, cell_list *c2)
{
	cell_list *p = c2;
	while (p)
	{
		if (c1->x == p->x && c1->y == p->y)
			return 1;
		p = p->next;
	}
	return 0;
}

void calculate_values()
{
	cell_list ***depend = NULL;
	cell_list *first = NULL;
	cell_list *sorted = NULL, *sorted_last = NULL;
	cell_list *p = NULL;
	int i,j;

	//rtlDebugOutString("calc");

	abort_calc = 0;
	depend = (cell_list***)allocmem(col_count * sizeof(void*));
	for (i = 0; i < col_count; i++)
	{
		depend[i] = (cell_list**)allocmem(row_count * sizeof(void*));
		for (j = 0; j < row_count; j++)
		{
			if (values[i][j])
				freemem(values[i][j]);
			values[i][j] = NULL;

			if (cells[i][j] && cells[i][j][0] == '=')
			{
				depend[i][j] = find_depend(cells[i][j] + 1);		// ����� =
				if (abort_calc)
				{
					values[i][j] = (char*)allocmem(2);
					values[i][j][0] = '#';
					values[i][j][1] = '\0';
					abort_calc = 0;
					continue;
				}
				cell_list *cur;
				cur = (cell_list*)allocmem(sizeof(cell_list));
				cur->x = i;
				cur->y = j;
				cur->next = first;	// �������� ���. ������ � ������ ������ ����� � ���������
				first = cur;
			}
		}
	}

	//rtlDebugOutString("depend end");
	// �������������� ����������
	if (!first)
		goto free_memory;

	if (abort_calc)
		goto free_memory;

	while (first)
	{
		// ����� ���������� �������. ���� ��� ��� - ������, �.�. ����������� �����������
		cell_list *prev = NULL,*min = first;

		bool is_min;
		while (min)
		{
			cell_list *p = first;
			is_min = 1;
			while (p && is_min)
			{
				if (is_in_list(p,depend[min->x][min->y]))
					is_min = 0;
				p = p->next;
			}
			if (is_min)
				break;
			prev = min;
			min = min->next;
		}
		if (!is_min)
		{
			abort_calc = 1;
			goto free_memory;		// ��� �����. ������. � ������, �� ���� goto
		}
		// ���� ������ ������� �� ������ ������
		if (prev == NULL)
		{
			first = first->next;
		}
		else
		{
			prev->next = min->next;
		}
		/*
		min->next = sorted;
		sorted = min;
		*/
		if (sorted == NULL)
		{
			sorted = min;
			sorted_last = min;
		}
		else
		{
			sorted_last->next = min;
			sorted_last = min;
			min->next = NULL;
		}
	}

	// ���������� ��������
	//rtlDebugOutString("sort end");

	p = sorted;
	while (p)
	{
		double d;
		abort_calc = 0;
		set_exp(cells[p->x][p->y]+1);	// ��� ��� ����� "="
		find_var = &calc_callback;
		if (get_exp(&d))
		{
			char *new_val = ftoa(d);
			if (values[p->x][p->y] && strcmp(values[p->x][p->y],new_val) == 0)
			{
				freemem(new_val);
			}
			else
			{
				if (values[p->x][p->y]) 
					freemem(values[p->x][p->y]);
				values[p->x][p->y] = new_val;
				sel_moved = 0;
			}
			//sprintf(debuf,"calc %U %U formula %S result %f",p->x,p->y,cells[p->x][p->y]+1,d);
			//rtlDebugOutString(debuf);
		}
		else
		{
			values[p->x][p->y] = (char*)allocmem(2);
			values[p->x][p->y][0] = '#';
			values[p->x][p->y][1] = '\0';
			//sprintf(debuf,"calc %U %U formula %S result #",p->x,p->y,cells[p->x][p->y]+1);
			//rtlDebugOutString(debuf);
		}
		p = p->next;
	}

	if (abort_calc)
		goto free_memory;

	//rtlDebugOutString("calc end");


	// ������������ ������

free_memory:
	
	p = sorted;
	while (p)
	{
		cell_list *tmp = p->next;
		cell_list *pp = depend[p->x][p->y];
		while (pp)
		{
			cell_list *tmp = pp->next;
			freemem(pp);
			pp = tmp;
		}
		freemem(p);
		p = tmp;
	}

	for (i = 0; i < col_count; i++)
		freemem(depend[i]);
	freemem(depend);

	//rtlDebugOutString("freemem end");

	
}

int parse_cell_name(char *str, int *px, int *py, int *xd, int *yd)
{
	// ���� ������� �� ��47 �������� � � �.
	int i,j,x,y,dx = 0,dy = 0;

	if (*str == '$') 
	{
		str++;
		dx = 1;
	}
	for (i = 0; i < strlen(str); i++)
		if (str[i] >= '0' && str[i] <= '9')
			break;
	if (str[i-1] == '$')
	{
		i--;
		dy = 1;
	}
	if (i == strlen(str))
	{
		return 0;
	}
	x = -1;
	for (j = 1; j < col_count; j++)
		if (strncmp(str,cells[j][0],i)==0)
	{
		/*int p = 0, z = 1;
		for (p = 0; p < i; p++)
			if (!str[p] || str[p] != cells[j][0][p])
			{
				z = 0;
				break;
			}
		if (z)
		*/
		{
			x = j;
			break;
		}
	}
	if (str[i] == '$')
		i++;
	y = -1;
	for (j = 1; j < row_count; j++)
		if (strcmp(str+i,cells[0][j])==0)
	{
			/*
		int p = 0, z = 1;
		for (p = 0;; p++)
		{
			if (str[i + p] != cells[0][j][p])
			{
				z = 0;
				break;
			}
			if (cells[0][j][p] == '\0')
				break;
		}
		if (z)
		*/
		{
			y = j;
			break;
		}
	}
	if (x == -1 || y == -1)
	{
		return 0;
	}
	*px = x;
	*py = y;
	if (xd)
		*xd = dx;
	if (yd)
		*yd = dy;
	return 1;
}

char *make_cell_name(int x, int y, int xd, int yd)
{
	char *col_cap = make_col_cap(x);
	char *row_cap = make_row_cap(y);

	if (x <= 0 || x > col_count || y <= 0 || y > row_count)
		return NULL;

	char *res = (char*)allocmem(strlen(col_cap) + strlen(row_cap) + xd ? 1 : 0 + yd ? 1 : 0 + 1);
	int i = 0;
	if (xd)
	{
		res[i] = '$';
		i++;
	}
	strcpy(res + i, col_cap);
	i += strlen(col_cap);
	if (yd)
	{
		res[i] = '$';
		i++;
	}
	strcpy(res + i, row_cap);
	i += strlen(row_cap);
	res[i] = '\0';
	freemem(col_cap);
	freemem(row_cap);
	return res;
}

// ������ ������ �� ���� ������
char *change_cell_ref(char *name, int sx, int sy)
{
	int x0, y0, xd, yd;

	parse_cell_name(name, &x0, &y0, &xd, &yd);

	//sprintf(debuf, "parsed cell name %S to %U %U", name, x0, y0);
	//rtlDebugOutString(debuf);

	// � ��� ���� �0 � �0.
	//sprintf(debuf, "%U in %U %U, %U in %U %U",x0, cf_x0, cf_x1, y0, cf_y0, cf_y1);
	//rtlDebugOutString(debuf);
	if (x0 >= cf_x0 && x0 <= cf_x1 && y0 >= cf_y0 && y0 <= cf_y1)
	{
		if (!xd)
		{
			x0 += sx;
			if (x0 <= 0 || x0 > col_count)
				x0 -= sx;
		}
		if (!yd)
		{
			y0 += sy;
			if (y0 <= 0 || y0 > row_count)
				y0 -= sy;
		}
	}

	return make_cell_name(x0, y0, xd, yd);
}

// ������ ���� ������ �� ������
char *change_formula(char *name, int sx, int sy)
{
	int i = 0;
	int in_name = 0;	// 1 - ������ ��������� �����. 2 - ������ ��������. 0 - ������ ����������� � �.�.
	int alp_len = 0, dig_len = 0;
	int buf_i = 0;

	char buffer[256]; // ����� �����
	memset((Byte*)buffer, 0, 256);

	//sprintf(debuf, "change formula %S by %U %U", name, sx, sy);
	//rtlDebugOutString(debuf);

	while (i < strlen(name) + 1)
	{
		char c;
		if (i == strlen(name))
			c = ' ';
		else
			c = name[i];
		buffer[buf_i++] = c;

		switch (in_name)
		{
			case 0:
			{
				if (isalpha2(c) || c == '$')
				{
					in_name = 1;
					alp_len = 1;
					dig_len = 0;
				}
			}
			break;
			case 1:
			{
				if (isalpha2(c))
				{
					alp_len++;
				}
				else if (c == '$' || isdigit(c))
				{
					in_name = 2;
					dig_len++;
				}
				else
				{
					// ������������� ��� ������ - �� ���
					in_name = 0;
					alp_len = dig_len = 0;
				}
			}
			break;
			case 2:
			{
				if (isdigit(c))
				{
					dig_len++;
				}
				else
				{
					if (alp_len > 0 && dig_len > 0)
					{
						// ��� ���������� ������
						int idx = i - alp_len - dig_len;
						int len = alp_len + dig_len;
						char *cell = (char*)allocmem(len + 1);
						//strncpy(cell, name + i, alp_len + dig_len);
						for (int l = 0; l < len; l++)
							cell[l] = name[idx + l];
						cell[len] = '\0';

						//sprintf(debuf, "found cell name '%S' alp %U dig %U", cell, alp_len, dig_len);
						//rtlDebugOutString(debuf);
						char *cell_new = change_cell_ref(cell, sx, sy);
						//sprintf(debuf, "rename to '%S'", cell_new);
						//rtlDebugOutString(debuf);
						if (cell_new)
						{
							char cc = buffer[buf_i - 1];
							strcpy(buffer + buf_i - len - 1, cell_new);
							buf_i += strlen(cell_new) - len;
							buffer[buf_i - 1] = cc;
						}
						//freemem(cell);
						//freemem(cell_new);
						alp_len = dig_len = 0;
						in_name = 0;
					}
				}
			}
		}
		i++;
	}
	//sprintf(debuf, "change formula done");
	//rtlDebugOutString(debuf);
	char *res = (char*)allocmem(strlen(buffer) + 1);
	strcpy(res, buffer);
	return res;
}




