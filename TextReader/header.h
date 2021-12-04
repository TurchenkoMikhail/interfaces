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

//структура модели текста
typedef struct text_t {
	char* str; //весь текст
	uint* strStart; //массив смещений (начала строк)
	uint maxStrLen; //длина самой длинной строки
	uint strCount; //количество строк
}text_t;

//метрика дл€ размера окна
typedef struct metric_t {
  uint x;
  uint y;
}metric_t;

//»нициализаци€ структуры происходит с помощью TEXTMETRIC и GetTextMetrics()
typedef struct symSize_t {
    uint xChar; //средн€€ ширина
    uint xCaps; //средн€€ ширина капса
    uint yChar; //средн€€ высота
} symSize_t;

//структура модели представлени€
typedef struct view_t{
    int selectedMode; //режим с версткой и без верстки
    uint newStrCount; //количество строк дл€ режима с вЄрсткой
    uint yOneList, xOneList; //размер одного листа дл€ в символах (высота и ширина)
    uint iMaxWidth;   //максимальное количество символов (режим без верстки)
    metric_t clientScreen; //размеры рабочего окна (клиентской области)
    symSize_t sym; //размеры символа
    int width; //ширина окна в символах
}view_t;

typedef struct scroll_t{
    SCROLLINFO scroll; //параметры скролла, устанавливаемые функцией SetScrollInfo
    int xPos, yPos, prevPos; //сдвиг по х ползунка прокрутки, сдвиг по у, предыдущий двиг по у ползунка прокрутки
    double sizeCoef;   //коэффициент изменени€ скролла
} scroll_t;

typedef struct fileName_t {
    OPENFILENAME ofn; //информаци€ дл€ инициализации диалогового окна
    char szFileName[_MAX_PATH]; //максимально доступное название файла
}filename_t;

void VertScrollWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll);
void VertScrollNoWrap(text_t* text, view_t* view, RECT rect, scroll_t* scroll);

void HertScrollWrap(scroll_t* scroll);
void HertScrollNoWrap(text_t* text, view_t* view, scroll_t* scroll);


#endif // HEADER_H_INCLUDED
