#include "header.h"

/*���������� ������������� ������� � ��� ������� � ������ � ��������
 PARAM[text] - ������ ������������� ������
 PARAM[view] - ������ ������������� �����������
 PARAM[rect] - ������������� ����������� ����
 PARAM[scroll] - ������ ������������� �������
*/
void VertScrollWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll){
    view->width = (rect.right - rect.left) / view->sym.xCaps; //������ ���� � ��������
    view->yOneList = (rect.bottom - rect.top) / view->sym.yChar; //������ ���� � ��������
    scroll->scroll.cbSize = sizeof((scroll->scroll));
    scroll->scroll.fMask = SIF_ALL;
    scroll->scroll.nMin = 0; //������ ������� �������
    scroll->scroll.nPage = (rect.bottom - rect.top) / view->sym.yChar; //������ ��������

    int strIndex = 0, paintBegin=0;
    int paintEnd = text->strCount; //������� ����� ���������� � ������� �������
    int yStr = 0;
    int extraCountStr = 0; //��� �������� ����, ��� �� ������
    long int textLen = 0; //����� ����� ������

	if (text->str != NULL) {
		for (strIndex = paintBegin; strIndex < paintEnd; strIndex++)
		{
			textLen = text->strStart[yStr+1] - text->strStart[yStr]; //����� ������ ������ � ��������
			if (strIndex < text->strCount + extraCountStr)
			{
				int k = 0;
				if (textLen > view->width) {
					while (textLen > view->width) {
						k++;
						strIndex++;
						//��������� ����� ������, ����� ��� ������. ��������� ��������� � ��������� ������
						textLen = text->strStart[yStr + 1] - text->strStart[yStr] - k * view->width;
						paintEnd++;
						extraCountStr++;
					}
				}
				yStr++; //����������� ����� �����
			}
		}
		view->newStrCount = paintEnd;
	}

	//���� ���� ����� ������ � ���� ����, �� ������� �� ����
	if (view->yOneList >= view->newStrCount) {
		scroll->scroll.cbSize = sizeof(scroll->scroll);
		scroll->scroll.fMask = SIF_ALL;
		scroll->scroll.nMin = 0;
		scroll->scroll.nMax = 0;
		scroll->scroll.nPage = 0;
		scroll->scroll.nPos = 0;
	}

	//����� ������ �����
	else {
		if (view->newStrCount <= LONG_MAX)
        {
			scroll->scroll.nMax = text->strCount - 1;
			scroll->scroll.nPos = scroll->yPos;
			scroll->sizeCoef = 1.0;
		}
		else
		{
			scroll->scroll.nMax = LONG_MAX;
			scroll->sizeCoef = (double)(fabs((double)(text->strCount - 1)) / (double)LONG_MAX);
			scroll->scroll.nPos = (int)(scroll->yPos / scroll->sizeCoef);
			scroll->scroll.nPage = (int)((rect.bottom - rect.top) / view->sym.yChar) / scroll->sizeCoef;
		}
	}
}

/*���������� ������������� ������� � ��� ������� � ������ ��� �������
 PARAM[text] - ������ ������������� ������
 PARAM[view] - ������ ������������� �����������
 PARAM[rect] - ������������� ����������� ����
 PARAM[scroll] - ������ ������������� �������
*/
void VertScrollNoWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll)
{
     view->yOneList = (rect.bottom - rect.top) /  view->sym.yChar; //������ ����� ���� � ��������
     view->xOneList = (rect.right - rect.left) /  view->sym.xChar; //������ ����� ���� � ��������

    //���� ��� �������, �� ������� �� �����
    if ( view->yOneList >= text->strCount) {
        scroll->scroll.cbSize = sizeof(scroll);
        scroll->scroll.fMask = SIF_ALL;
        scroll->scroll.nMin = 0;
        scroll->scroll.nMax = 0;
        scroll->scroll.nPage = 0;
        scroll->scroll.nPos = 0;
    }

    //����� ������������� ������
    else {
        scroll->scroll.cbSize = sizeof(scroll->scroll);
        scroll->scroll.fMask = SIF_ALL;
        scroll->scroll.nMin = 0;
        scroll->scroll.nPage = (rect.bottom - rect.top) /  view->sym.yChar;

        if (text->strCount <= LONG_MAX)
        {
            scroll->scroll.nMax = text->strCount -  view->clientScreen.y /  view->sym.yChar;
            scroll->scroll.nPos = scroll->yPos;
            scroll->sizeCoef = 1.0;
        }
        else
        {
            scroll->scroll.nMax = LONG_MAX;
            scroll->sizeCoef = (double)((double)(text->strCount - 1) / (double)LONG_MAX);
            scroll->scroll.nPos = (int)(scroll->yPos/scroll->sizeCoef);
            scroll->scroll.nPage = (int)((rect.bottom - rect.top) /  view->sym.yChar) / scroll->sizeCoef;
        }
    }
}

/*���������� ��������������� ������� � ��� ������� � ������ � ��������
 PARAM[scroll] - ������ ������������� �������
*/
void HertScrollWrap(scroll_t* scroll)
{
    scroll-> scroll.cbSize = sizeof(scroll->scroll);
    scroll-> scroll.fMask = SIF_ALL;
    scroll-> scroll.nMin = 0;
    scroll-> scroll.nMax = 0;
    scroll-> scroll.nPage = 0;
    scroll-> scroll.nPos = 0;
}


/*���������� ��������������� ������� � ��� ������� � ������ ��� �������
 PARAM[text] - ������ ������������� ������
 PARAM[view] - ������ ������������� �����������
 PARAM[scroll] - ������ ������������� �������
*/
void HertScrollNoWrap(text_t* text, view_t* view, scroll_t* scroll)
{
    //���� ��� �������, �� ������ �� �����
    if (view->xOneList >= text->maxStrLen) {
        scroll->scroll.cbSize = sizeof(scroll);
        scroll->scroll.fMask = SIF_ALL;
        scroll->scroll.nMin = 0;
        scroll->scroll.nMax = 0;
        scroll->scroll.nPage = 0;
        scroll->scroll.nPos = 0;
    }
    else {

        //������������� �������������� ������, ���� ����� ����� ������� ������
        scroll->scroll.nMax = 2 + (view->iMaxWidth - view->clientScreen.x) / view->sym.xChar;
        if (scroll->scroll.nMax < 0) {
            scroll->scroll.nMax = 0;
        }
        scroll->scroll.nPage = view->clientScreen.x / view->sym.xChar;
        scroll->scroll.nPos = scroll->xPos;
    }
}
