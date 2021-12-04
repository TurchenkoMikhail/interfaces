
#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include "header.h"


/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("My Reader");

TCHAR szMenuName[ ] = _T("Menu");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;               /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);

    wincl.lpszMenuName = szMenuName;                 /*Menu */

    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); /*����� ���� ������*/

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("My Reader"),       /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           lpszArgument                 /* Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*����� ����� ����������� ������� ������ ��� ������ ��� �������
  PARAM [str] - ��� ������ ������, ����������� �������� �������� ������
  RETURN - ����� ���������� ������
*/
uint FindMaxStrLen(char* str) {
    uint i = 0, max = 0, len = 0;
    while(str[i] != '\0') {
        len = 0;
        while(str[i] != '\0' && str[i] != '\n'){
            ++len;
            ++i;
        }
        max= len > max? len:max;
        ++i;
    }
    return max;
}

/*������� ��������� ������ ��� ���������� ��������� ��� �������� �����
  PARAM [text] - ��������� ������ ������
*/
void FreeText(text_t* text) {

    if (text->strStart){
        free(text->strStart);
        text->strStart = NULL;
    }
    if (text->str){
        free(text->str);
        text->str = NULL;
    }

    text->maxStrLen = 0;
    text->strCount = 0;
}

/*������������� ��������� ����������� ����
 PARAM[hwnd] - ��������� ����
 PARAM[filename] - ������ ������������� �����
*/
void FileInit(HWND hwnd, filename_t* filename)
{
    static char szTitleName[_MAX_FNAME + _MAX_EXT]; //����������� ������ ��� �������� �����
    filename->ofn.lStructSize       = sizeof(OPENFILENAME);
    filename->ofn.hwndOwner         = hwnd;
    filename->ofn.lpstrFilter       = "text(*.TXT)\0*.txt\0\0"; //��������� ��� �������� ������ ��������� ������;
    filename->ofn.lpstrDefExt       = "txt";
    filename->ofn.nMaxFile          = _MAX_PATH;
    filename->ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT;
    filename->ofn.nMaxCustFilter    = 0;
    filename->ofn.nFilterIndex      = 0;
    filename->ofn.Flags             = OFN_HIDEREADONLY; //Hides the Read Only check box.
    filename->ofn.nFileOffset       = 0;
    filename->ofn.nFileExtension    = 0;
    filename->ofn.lCustData         = 0L;
    filename->ofn.lpfnHook          = NULL;
    filename->ofn.lpTemplateName    = NULL;
    filename->ofn.lpstrInitialDir   = NULL;
    filename->ofn.lpstrTitle        = NULL;
    filename->ofn.lpstrFileTitle    = szTitleName;//�������� �����
    filename->ofn.lpstrFile         = filename->szFileName; //���� � �����
    filename->ofn.lpstrCustomFilter = NULL;
    filename->ofn.hInstance         = NULL;
}

 /* ������ ������ �� ����� � ��������� ������ ������
  PARAM[text] - ������ ������������� ������, � ������� ��������� �����
  PARAM[filename] - �������� ���������� �����, ������ ������ �����
  PARAM[view] - ������ ������������� ����������� ������

  RETURN: TRUE, ���� ������ ������ ������. ����� FALSE

 */
BOOL ReadText(text_t* text, char* fileName, view_t* view)
{
    FILE* file = fopen(fileName, "r");
    if(!file)
        return FALSE;

    uint strCount=0;
    uint i=0, sizeText=0;

    fseek(file, 0, SEEK_END);
    sizeText = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (sizeText > 0) {
        //��������� ������ ��� ���� ��������� ������
        text->str = (char*)calloc((sizeText + 1), sizeof(char));
        text->strStart = (uint*)calloc(sizeText, sizeof(uint));
    }
    else { //���� ����
        fclose(file);
        return FALSE;
    }

    if (text->str && text->strStart) {
        text->str[sizeText] = '\0';

        fread(text->str, sizeof(char), sizeText, file); //��������� ���� ����
        for (i = 0; i < sizeText; i++) {
            //����� ������
            if (text->str[i] == '\n'){
                strCount++;
                text->strStart[strCount] = i + 1; //����� ������ ������
                sizeText--;
            }
        }
        text->strStart[strCount + 1] = i;
        text->strCount = strCount + 1;

        // ������� ������ ����� ������� ������ ��� ������ ��� �������
        text->maxStrLen = FindMaxStrLen(text->str);

        //���������� ������������ ����� �������� � ������ ��� ������ ��� �������
        view->iMaxWidth = text->maxStrLen * view->sym.xCaps;

    }

    //������ ��������� ������ ��� str ��� strStart
    else {
            FreeText(text);
            fclose(file);
            return FALSE;
        }
    fclose(file);
    return TRUE;
}

 /*������� ��� ���������� ��������� ��� ������ ������
 //PARAM[paint] - ��������� �� ��������� ��������� ��� ������� � ������� ������
 //PARAM[sym] - ��������� ������������� �������
 //PARAM[str] - ����� ��������� ������ ������
 //���������� �� ������� PaintText...
 */
void UpdatePaint(metric_t* paint, symSize_t sym, int str)
{
    (*paint).x = sym.xChar;
    (*paint).y = sym.yChar * str;
}

 /*����������� ������ � ������ � �������
 //PARAM[hdc] - ��������� ��������� ���������� (��� ������� TextOut)
 //PARAM[text] - ������ ������������� ������
 //PARAM[view] - ������ ������������� �����������
 //PARAM[scroll] - ������ ������������� �������
 */
void PaintTextWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //��������� ������
	  int textLen = 0; //����� ������ ��� ���������
    metric_t paint;  //������ �������, ��� ������ ���������
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar); //�������
    int yStr = 0;
    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //�������� ���������� ��������� �������

        //����� �������� ��� ������
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]));

        //����� ��������� ������
        textLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - 1;
        if(strIndex < text->strCount)
        {
            //���� ��� ������ ���������� � ����� ������ �� ������
            if (textLen <= view->width)
                    TextOut(hdc, paint.x, paint.y, printNow, textLen);
            else {
                    int k = 0;
                    while(textLen > view->width) {
                        k++;
                        //������� ��� �������
                        TextOut(hdc, paint.x, paint.y, printNow, view->width);
                        strIndex++;
                        UpdatePaint(&paint, view->sym, strIndex);
                        //���, ��� �� ������, ��������� � ����� ������, � �������.
                        printNow = (char*)(text->str + sizeof(char)*(text->strStart[scroll->yPos + yStr]) + sizeof(char) * k * view->width);
                        textLen = text->strStart[scroll->yPos + yStr + 1] - text->strStart[scroll->yPos + yStr] - k * view->width - 1;
                    }
                    if (textLen <= view->width) {
                        TextOut(hdc, paint.x, paint.y, printNow, textLen);
                    }
                }
            yStr++; //����������� ����� �����
        }
    }
}

 /*����������� ������ � ������ ��� ������
 //PARAM[hdc] - ��������� ��������� ���������� (��� ������� TextOut)
 //PARAM[text] - ������ ������������� ������
 //PARAM[view] - ������ ������������� �����������
 //PARAM[scroll] - ������ ������������� �������
 */
void PaintTextNoWrap(HDC hdc, text_t* text, view_t* view, scroll_t* scroll)
{
    char* printNow = NULL;   //�������, ������� ����� ����������
	int textLen = 0; //����� ������ ��� ���������
    metric_t paint;  //������ �������, ��� ��������
    int strIndex, paintBegin=0;
    int paintEnd = text->strCount < (view->clientScreen.y / view->sym.yChar) ? text->strCount : (view->clientScreen.y / view->sym.yChar);

    for(strIndex = paintBegin; strIndex < paintEnd; strIndex++)
    {
        UpdatePaint(&paint, view->sym, strIndex); //�������� ���������� ��������� �������

        //������� ��� ������
        printNow = (char*)(text->str + sizeof(char)*(text->strStart[strIndex + scroll->yPos]) + sizeof(char) * scroll->xPos);
        //����� ���� ������
        textLen = text->strStart[strIndex + 1 + scroll->yPos] - text->strStart[strIndex + scroll->yPos] - scroll->xPos - 1;

        if(strIndex < text->strCount)
            TextOut(hdc, paint.x, paint.y, printNow, textLen);

    }
}
/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    TEXTMETRIC tm; //�������� ���������� � ���������� ������
    RECT rect; //���������� ����

    static HMENU hMenu;

    static filename_t filename;
    static text_t text; //������ ������ � �������
    static view_t view;
    static scroll_t scroll;

    switch (message)                  /* handle the messages */
    {
        case WM_CREATE: {

        //������������� ����������� ����������
        view.newStrCount = 0;
        view.yOneList = 0;
        view.xOneList = 0;
        scroll.sizeCoef = 1.0;
        view.selectedMode = IDM_WRAP_OFF;

        hdc = GetDC(hwnd);
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        GetTextMetrics(hdc, &tm);
        view.sym.xChar = tm.tmAveCharWidth; //������� ������ ��������
        view.sym.xCaps =(tm.tmPitchAndFamily & 1 ? 3 : 2) * view.sym.xChar / 2;
        view.sym.yChar = tm.tmHeight + tm.tmExternalLeading; //������ + ����������� ������������
        ReleaseDC(hwnd, hdc); //����������� �������� ���������� ��� ������������� ������� ������������

        CREATESTRUCT* p = (CREATESTRUCT*) lParam;
        FileInit(hwnd, &filename);

        //���� ���� ��� ������� ����� �������� ��������� ������
        char* file = (char*)p->lpCreateParams;
         if (ReadText(&text, file, &view) == TRUE) {
            hMenu = GetMenu(hwnd);
            CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED); //������������� ������� "�������" ��������� ������ ���� � ��������� ���������
        }
        return 0;
    }
        case WM_PAINT: {
            hdc = BeginPaint(hwnd, &ps);

            //����� ����������, ����� ������� ���
            if(text.str != NULL) {
                if (view.selectedMode == IDM_WRAP_ON)
                    PaintTextWrap(hdc, &text, &view, &scroll);
                else if(view.selectedMode == IDM_WRAP_OFF)
                    PaintTextNoWrap(hdc, &text, &view, &scroll);
            }
	    	EndPaint(hwnd, &ps);
	    	return 0;
        }

        //��������� �� ��������� ����
        case WM_SIZE : {
            view.clientScreen.x = LOWORD(lParam);
            view.clientScreen.y = HIWORD(lParam);

            GetClientRect(hwnd, &rect);
            GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

            //���������� ������������� �������
		    if (view.selectedMode == IDM_WRAP_ON)
               VertScrollWrap(&text, &view, rect, &scroll);
            else if(view.selectedMode == IDM_WRAP_OFF)
                VertScrollNoWrap(&text, &view,  rect, &scroll);
            SetScrollRange(hwnd, SB_VERT, 0, scroll.scroll.nMax, FALSE);
            SetScrollPos(hwnd, SB_VERT, scroll.scroll.nPos, TRUE);

            //���������� ��������������� �������
            if (view.selectedMode == IDM_WRAP_ON)
                HertScrollWrap(&scroll);
            else if(view.selectedMode == IDM_WRAP_OFF)
                HertScrollNoWrap(&text, &view, &scroll);
            SetScrollRange(hwnd, SB_HORZ, 0, scroll.scroll.nMax, FALSE);
            SetScrollPos(hwnd, SB_HORZ, scroll.scroll.nPos, TRUE);

		    InvalidateRect(hwnd, NULL, TRUE); //��������� ������������� ��� ����������� ����
		    UpdateWindow(hwnd); //���������� ����

            return 0;
        }

        //������ �� �������������� ������
        case WM_HSCROLL:
	    {
		    scroll.scroll.cbSize = sizeof(scroll.scroll);
		    scroll.scroll.fMask = SIF_ALL; //SIF_PAGE, SIF_POS, SIF_RANGE, SIF_TRACKPOS

            //�������� ��������� ��� �������������� ������ ��������� � ����
		    GetScrollInfo(hwnd, SB_HORZ, &(scroll.scroll));
		    scroll.xPos = scroll.scroll.nPos; //������� ��������

		    switch (LOWORD(wParam))
		    {
		        case SB_LINERIGHT:
		        	scroll.scroll.nPos++;
		        	break;
		        case SB_LINELEFT:
		        	scroll.scroll.nPos--;
		        	break;
                case SB_PAGERIGHT: //���� ������ �� ������ ����
                    scroll.scroll.nPos += view.xOneList;
                    break;
	    	    case SB_PAGELEFT: //���� ����� �� ������ ����
		     	    scroll.scroll.nPos -= view.xOneList;
		     	    break;
	    	    case SB_THUMBTRACK: //������������ ������� ���������
	    	    	scroll.scroll.nPos = scroll.scroll.nTrackPos;
	     	    	break;
	    	    }
	    	    scroll.scroll.fMask = SIF_POS;

	    	    if(scroll.scroll.nPos <= 0)
                    scroll.scroll.nPos = 0;
                else if(scroll.scroll.nPos >= scroll.scroll.nMax*scroll.sizeCoef)
                    scroll.scroll.nPos = scroll.scroll.nMax * scroll.sizeCoef;

		        //������������� ��������� �������������� ������ ���������
		        SetScrollInfo(hwnd, SB_HORZ, &(scroll.scroll), TRUE);

		        //�������� ����������
	    	    if (scroll.scroll.nPos != scroll.xPos){
		        	scroll.xPos = scroll.scroll.nPos;
		        	InvalidateRect(hwnd, NULL, TRUE);
		        	UpdateWindow(hwnd);
	    	    }
		    return 0;
	    }
    //������ �� ������������ ������
	case WM_VSCROLL:
	    {

		    scroll.scroll.cbSize = sizeof(scroll.scroll);
		    scroll.scroll.fMask = SIF_ALL;

		    GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));
            scroll.prevPos = scroll.yPos;

		    switch (LOWORD(wParam))
            {
		        case SB_TOP:
			        scroll.yPos = 0;
			        scroll.scroll.nPos = scroll.scroll.nMin;
			        break;
		        case SB_BOTTOM:
		    	    scroll.scroll.nPos = scroll.scroll.nMax;
		    	    scroll.yPos = scroll.scroll.nMax * scroll.sizeCoef; //����� ������� � ������ ���������
                    break;
		        case SB_LINEUP:
			        scroll.yPos--;
			        scroll.scroll.nPos = (int)(scroll.yPos / scroll.sizeCoef);
			        break;
	    	    case SB_LINEDOWN:
		    	    scroll.yPos++;
		    	    scroll.scroll.nPos = (int)(scroll.yPos / scroll.sizeCoef);
		    	    break;
		        case SB_PAGEUP: //������ ����� �� ������ ����
			        scroll.yPos -= view.yOneList;
			        scroll.scroll.nPos = (int)(scroll.yPos / scroll.sizeCoef);
			        break;
		        case SB_PAGEDOWN: //������ ���� �� ������ ����
		        	scroll.yPos += view.yOneList;
		        	scroll.scroll.nPos = (int)(scroll.yPos / scroll.sizeCoef);
	    	    	break;
	    	    case SB_THUMBTRACK: //������������ ������� ���������
		    	    scroll.scroll.nPos = (scroll.scroll.nTrackPos);
		    	    scroll.yPos = scroll.scroll.nPos * scroll.sizeCoef;
		    	    break;
            }

            if(scroll.yPos <= 0)
                scroll.yPos = 0;
            else if(scroll.yPos >= scroll.scroll.nMax*scroll.sizeCoef)
                scroll.yPos = scroll.scroll.nMax * scroll.sizeCoef;

            scroll.scroll.fMask = SIF_POS;
		    SetScrollInfo(hwnd, SB_VERT, &(scroll.scroll), TRUE);

		    if (scroll.yPos != scroll.prevPos){
			    InvalidateRect(hwnd, NULL, TRUE);
			    UpdateWindow(hwnd);
            }
		    return 0;
	    }

        //������������ ������ ������ ��������� �����
        //���������� ��������� �� ��������������� ������
	    case WM_KEYDOWN:
	    {
	        switch (wParam)
	        {
		        case VK_UP:
			        SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    break;
		        case VK_DOWN:
		        	SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			        break;
		        case VK_LEFT:
		        	SendMessage(hwnd, WM_HSCROLL, SB_LINEUP, 0);
		        	break;
		        case VK_RIGHT:
			        SendMessage(hwnd, WM_HSCROLL, SB_LINEDOWN, 0);
			        break;
                case VK_PRIOR: //PAGE UP key
			        SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
			        break;
		        case VK_NEXT: //PAGE DOWN key
		        	SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			        break;
                case VK_HOME: //HOME key
                    SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
                    break;
                case VK_END: //END key
                    SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
                    break;
	    	}
		    return 0;
	    }

        //������� � ���� ���������
	    case WM_COMMAND:
        {
		    hMenu = GetMenu(hwnd);
		    switch (LOWORD(wParam))
            {
		    case IDM_OPEN:

		        //�������� ���������� �����
                if(GetOpenFileName(&(filename.ofn)))
                {
                    //������ ����������� ������, ���� �� ���
			        FreeText(&text);

			        //������������� ������� "�������" ��������� ������ ���� � ��������� ���������
			        CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED);

                    //������ ������
                    if (ReadText(&text, filename.szFileName, &view) == FALSE)
                        break;

			        scroll.xPos = 0;
			        scroll.yPos = 0;
			        view.iMaxWidth = text.maxStrLen * view.sym.xCaps; //������������ �����
			        GetClientRect(hwnd, &rect);
                    GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

                    //��������� ������������� �������
                    if (view.selectedMode == IDM_WRAP_ON)
                        VertScrollWrap(&text, &view, rect, &scroll);
                    else if(view.selectedMode == IDM_WRAP_OFF)
                        VertScrollNoWrap(&text, &view,  rect, &scroll);
                    SetScrollRange(hwnd, SB_VERT, 0, scroll.scroll.nMax, FALSE);
                    SetScrollPos(hwnd, SB_VERT, scroll.scroll.nPos, TRUE);

                    //��������� ��������������� �������
                    if (view.selectedMode == IDM_WRAP_ON)
                        HertScrollWrap(&scroll);
                    else if(view.selectedMode == IDM_WRAP_OFF)
                        HertScrollNoWrap(&text, &view,  &scroll);
                    SetScrollRange(hwnd, SB_HORZ, 0, scroll.scroll.nMax, FALSE);
                    SetScrollPos(hwnd, SB_HORZ, scroll.scroll.nPos, TRUE);
                }

			    InvalidateRect(hwnd, NULL, TRUE);
			    UpdateWindow(hwnd);
			    break;

            //�������� ���������� �����, �� ���������, � �������� �������
            case IDM_CLOSE:
                FreeText(&text);

                //������������� ������� "�������" ��������� ������ ���� � ��������� ���������
                CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED);

                scroll.xPos = 0;
                scroll.yPos = 0;
                view.iMaxWidth = 0; //������������ �����
                view.newStrCount = 0;
                GetClientRect(hwnd, &rect);
                GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

                //������� ������������� �������
                if (view.selectedMode == IDM_WRAP_ON)
                   VertScrollWrap(&text, &view, rect, &scroll);
                else if(view.selectedMode == IDM_WRAP_OFF)
                    VertScrollNoWrap(&text, &view,  rect, &scroll);
                SetScrollRange(hwnd, SB_VERT, 0, 0, FALSE);
                SetScrollPos(hwnd, SB_VERT, 0, TRUE);

                //������� ��������������� �������
                if (view.selectedMode == IDM_WRAP_ON)
                    HertScrollWrap(&scroll);
                else if(view.selectedMode == IDM_WRAP_OFF)
                    HertScrollNoWrap(&text, &view,  &scroll);
                SetScrollRange(hwnd, SB_HORZ, 0, 0, FALSE);
                SetScrollPos(hwnd, SB_HORZ, 0, TRUE);
			    InvalidateRect(hwnd, NULL, TRUE);
			    UpdateWindow(hwnd);
                break;

            //������� ��� ���������
            case IDM_EXIT:
                SendMessage(hwnd, WM_CLOSE,0,0);
                break;

		    case IDM_WRAP_OFF:
                //������ ������� � ���������� ������
		        CheckMenuItem(hMenu, view.selectedMode, MF_UNCHECKED);
			    view.selectedMode = LOWORD(wParam);

			    //��������� ������� �� ����� �����
			    CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED);

			    GetClientRect(hwnd, &rect);
                GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

                //��������� ������������� �������
                VertScrollNoWrap(&text, &view,  rect, &scroll);
                SetScrollRange(hwnd, SB_VERT, 0, scroll.scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_VERT, scroll.scroll.nPos, TRUE);

                //��������� ��������������� �������
                HertScrollNoWrap(&text, &view,  &scroll);
                SetScrollRange(hwnd, SB_HORZ, 0, scroll.scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_HORZ, scroll.scroll.nPos, TRUE);

                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);

		    	return 0;

            case IDM_WRAP_ON:

		        //������ ������� � ���������� ������
			    CheckMenuItem(hMenu, view.selectedMode, MF_UNCHECKED);
			    view.selectedMode = LOWORD(wParam);

			    //��������� ������� �� ����� �����
			    CheckMenuItem(hMenu, view.selectedMode, MF_CHECKED);

                GetClientRect(hwnd, &rect);
                GetScrollInfo(hwnd, SB_VERT, &(scroll.scroll));

                /*������������ ������*/
                VertScrollWrap(&text, &view, rect, &scroll);
                SetScrollRange(hwnd, SB_VERT, 0, scroll.scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_VERT, scroll.scroll.nPos, TRUE);

                //�������������� ������: ��������� �� ���� � ������ �������
                HertScrollWrap(&scroll);
                scroll.xPos = 0;
                SetScrollRange(hwnd, SB_HORZ, 0, scroll.scroll.nMax, FALSE);
                SetScrollPos(hwnd, SB_HORZ, scroll.scroll.nPos, TRUE);

		    	InvalidateRect(hwnd, NULL, TRUE);
		    	UpdateWindow(hwnd);
		    	return 0;
	    	}
	      	return 0;
        }

        case WM_DESTROY: {
            FreeText(&text);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        }
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
