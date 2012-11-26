char *unicode_tags[]={
"nbsp",  " ",
"#38",   " ",
"#160",  " ",

"copy",  "(c)",
"#169",  "(c)",

"trade", "[TM]",

"reg",   "(r)",
"#174",  "(r)",

"bdquo", ",,",

"amp",   "&",
"#38",   "&",

"lt",    "<",
"#60",   "<",

"gt",    ">",
"#62",   ">",

"minus", "-",
"ndash", "-",
"mdash", "-", //--
"#8722", "-",
"#8211", "-",
"#151",  "-",
"#149",  "-",

"rsquo", "'",
"#39",   "'",
"#96",   "'",
"#8217", "'",

"quot",  "\"",
"#34",   "\"",
"ldquo", "\"",
"rdquo", "\"",
"#8222", "\"",
"#8221", "\"",

"laquo", "<<",
"#171",  "<<",
"raquo", ">>",
"#187",  ">>",

"uarr",  "\24",
"darr",  "\25",
"rarr",  "\26",
"larr",  "\27", 

"#1028", "\242",
"#1030", "I",
"#1031", "\244",

"#8470", "N",
"bull",  "-", //������ ����� �����
"percnt","%",

0}; 


byte unicode_chars[] = "������������������������������������������������������������������h�\243i\105\244\0";

unsigned char GetUnicodeSymbol()
{
	int j;
	
	for (j=0; unicode_tags[j]!=0; j+=2;) 
	{
		if (!strcmp(#tag, unicode_tags[j]))
		{
			strcat(#line, unicode_tags[j+1]);
			return NULL;
		}
	}
	
	rez = atoi(#tag + 1) - 1040;
	if (tag[1] == '1') && (rez>=0) && (rez<=72) && (strlen(#tag) == 5)
	{
		return unicode_chars[rez];
	}
	
	strcat(#line,#tag); //������� �� ����� �������������� ���, ��� �������� �����-�� ������
	return NULL;
}
