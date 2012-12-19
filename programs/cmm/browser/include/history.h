struct UrlsHistory {
	dword CurrentUrl();
	void AddUrl();
	byte GoBack();
	byte GoForward();
};

UrlsHistory BrowserHistory;

struct path_string {
char Item[4096];
};

#define MAX_HISTORY_NUM 40
path_string history_list[MAX_HISTORY_NUM];
int history_num;
int history_current;

dword UrlsHistory::CurrentUrl()
{
	return #history_list[history_current].Item;
}

void UrlsHistory::AddUrl() //��� ����� �������� ������� - ��� ���������������
{
	int i;
	if (history_num>0) && (!strcmp(#URL,#history_list[history_current].Item)) return;

	if (history_current>=MAX_HISTORY_NUM-1)
	{
		history_current/=2;
		for (i=0; i<history_current; i++;)
		{
			strlcpy(#history_list[i].Item, #history_list[MAX_HISTORY_NUM-i].Item, sizeof(history_list[0].Item));
		}	
	}
	history_current++;
	strlcpy(#history_list[history_current].Item, #URL, sizeof(history_list[0].Item));
	history_num=history_current;
}


byte UrlsHistory::GoBack()
{
	if (history_current<=1) return 0;
	
	history_current--;
	strlcpy(#URL, #history_list[history_current].Item, sizeof(URL));
	return 1;
}


byte UrlsHistory::GoForward()
{
	if (history_current==history_num) return 0;
	history_current++;
	strlcpy(#URL, #history_list[history_current].Item, sizeof(URL));
	return 1;
}