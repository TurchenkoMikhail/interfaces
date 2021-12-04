#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <math.h>
#include <limits.h>

#include "mymenu.h"

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif

#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif

typedef unsigned int uint;

//��������� ������ ������
typedef struct text_t {
	char* str; //���� �����
	uint* strStart; //������ �������� (������ �����)
	uint maxStrLen; //����� ����� ������� ������
	uint strCount; //���������� �����
}text_t;

//������� ��� ������� ����
typedef struct metric_t {
  uint x;
  uint y;
}metric_t;

//������������� ��������� ���������� � ������� TEXTMETRIC � GetTextMetrics()
typedef struct symSize_t {
    uint xChar; //������� ������
    uint xCaps; //������� ������ �����
    uint yChar; //������� ������
} symSize_t;

//��������� ������ �������������
typedef struct view_t{
    int selectedMode; //����� � �������� � ��� �������
    uint newStrCount; //���������� ����� ��� ������ � �������
    uint yOneList, xOneList; //������ ������ ����� ��� � �������� (������ � ������)
    uint iMaxWidth;   //������������ ���������� �������� (����� ��� �������)
    metric_t clientScreen; //������� �������� ���� (���������� �������)
    symSize_t sym; //������� �������
    int width; //������ ���� � ��������
}view_t;

typedef struct scroll_t{
    SCROLLINFO scroll; //��������� �������, ��������������� �������� SetScrollInfo
    int xPos, yPos, prevPos; //����� �� � �������� ���������, ����� �� �, ���������� ���� �� � �������� ���������
    double sizeCoef;   //����������� ��������� �������
} scroll_t;

typedef struct fileName_t {
    OPENFILENAME ofn; //���������� ��� ������������� ����������� ����
    char szFileName[_MAX_PATH]; //����������� ��������� �������� �����
}filename_t;

void VertScrollWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll);
void VertScrollNoWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll);

void HertScrollWrap(scroll_t* scroll);
void HertScrollNoWrap(text_t* text, view_t* view, scroll_t* scroll);


#endif // HEADER_H_INCLUDED
