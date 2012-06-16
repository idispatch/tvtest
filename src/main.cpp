/*---------------------------------------------------------*/
/*                                                         */
/*   Turbo Vision 1.0                                      */
/*   TVGUID01 Demo Source File                             */
/*   Copyright (c) 1991 by Borland International           */
/*                                                         */
/*---------------------------------------------------------*/

#include <tv.h>
#include <stdlib.h>
#include <time.h>

const int cmMyFileOpen = 200; // assign new command values
const int cmMyNewWin = 201;

const int cmAboutCmd = 100;
const int cmPuzzleCmd = 101;
const int cmCalendarCmd = 102;
const int cmAsciiCmd = 103;
const int cmCalcCmd = 104;
const int cmOpenCmd = 105;
const int cmChDirCmd = 106;
const int cmMouseCmd = 108;
const int cmColorCmd = 109;
const int cmSaveCmd = 110;
const int cmRestoreCmd = 111;
const int cmTestInputBox = 112;

const int cmAsciiTableCmdBase = 910;
const int cmCharFocused = 0;

class TPuzzleView: public TView {

public:

    TPuzzleView(TRect& r);
    virtual TPalette& getPalette() const;
    virtual void handleEvent(TEvent& event);
    virtual void draw();
    void moveKey(int key);
    void moveTile(TPoint point);
    void scramble();
    void winCheck();

private:

    char board[6][6];
    int moves;
    char solved;
};

class TPuzzleWindow: public TWindow {

public:

    TPuzzleWindow();
};

#define cpPuzzlePalette "\x06\x07"

//
// TPuzzleView functions & static variables
//

static const char boardStart[16] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
        'M', 'N', 'O', ' ' };

static char map[15] = { 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1 };

TPuzzleView::TPuzzleView(TRect& r) :
        TView(r) {
    srand (time(NULL));options |= ofSelectable;
    memset(board, ' ', sizeof(board));

    for (int i = 0; i <= 3; i++)
    for (int j = 0; j <= 3; j++)
    board[i][j] = boardStart[i * 4 + j];

    scramble();
}

void TPuzzleView::draw() {
    char tmp[8];
    char color[2], colorBack;
    TDrawBuffer buf;

    color[0] = color[1] = colorBack = getColor(1);
    if (!solved)
        color[1] = getColor(2);

    /* SS: little change */
    short i;
    for (i = 0; i <= 3; i++)
    //for(short i = 0; i <= 3; i++)
            {
        buf.moveChar(0, ' ', colorBack, 18);
        if (i == 1)
            buf.moveStr(13, "Move", colorBack);
        if (i == 2) {
            sprintf(tmp, "%d", moves);
            buf.moveStr(14, tmp, colorBack);
        }
        for (short j = 0; j <= 3; j++) {
            strcpy(tmp, "   ");
            tmp[1] = board[i][j];
            if (board[i][j] == ' ')
                buf.moveStr((short) (j * 3), tmp, color[0]);
            else
                buf.moveStr((short) (j * 3), tmp, color[(int) map[board[i][j] - 'A']]);
        }
        writeLine(0, i, 18, 1, buf);
    }
}

TPalette& TPuzzleView::getPalette() const {
    static TPalette palette(cpPuzzlePalette, sizeof(cpPuzzlePalette) - 1);
    return palette;
}

void TPuzzleView::handleEvent(TEvent& event) {
    TView::handleEvent(event);

    if (solved && (event.what & (evKeyboard | evMouse))) {
        scramble();
        clearEvent(event);
    }

    if (event.what == evMouseDown) {
        moveTile(event.mouse.where);
        clearEvent(event);
        winCheck();
    } else if (event.what == evKeyDown) {
        moveKey(event.keyDown.keyCode);
        clearEvent(event);
        winCheck();
    }
}

void TPuzzleView::moveKey(int key) {
    /* SS: little change */
    int i;
    for (i = 0; i <= 15; i++)
//    for(int i = 0; i <= 15; i++)
        if (board[i / 4][i % 4] == ' ')
            break;

    int x = i % 4;
    int y = i / 4;

    switch (key) {
    case kbDown:
        if (y > 0) {
            board[y][x] = board[y - 1][x];
            board[y - 1][x] = ' ';
            if (moves < 1000)
                moves++;
        }
        break;

    case kbUp:
        if (y < 3) {
            board[y][x] = board[y + 1][x];
            board[y + 1][x] = ' ';
            if (moves < 1000)
                moves++;
        }
        break;

    case kbRight:
        if (x > 0) {
            board[y][x] = board[y][x - 1];
            board[y][x - 1] = ' ';
            if (moves < 1000)
                moves++;
        }
        break;

    case kbLeft:
        if (x < 3) {
            board[y][x] = board[y][x + 1];
            board[y][x + 1] = ' ';
            if (moves < 1000)
                moves++;
        }
        break;
    }
    drawView();
}

void TPuzzleView::moveTile(TPoint p) {
    p = makeLocal(p);

    /* SS: little change */
    int i;
    for (i = 0; i <= 15; i++)
//    for(int i = 0; i <= 15; i++)
        if (board[i / 4][i % 4] == ' ')
            break;
    int x = p.x / 3;
    int y = p.y;

    switch ((y * 4 + x - i)) {
    case -4: //  Piece moves down
        moveKey (kbDown);
        break;

    case -1: //  Piece moves right
        moveKey (kbRight);
        break;

    case 1: //  Piece moves left
        moveKey (kbLeft);
        break;

    case 4: //  Piece moves up
        moveKey (kbUp);
        break;

    }
    drawView();
}

void TPuzzleView::scramble() {
    moves = 0;
    solved = 0;
    do {
        switch ((rand() >> 4) % 4) {
        case 0:
            moveKey (kbUp);
            break;

        case 1:
            moveKey (kbDown);
            break;

        case 2:
            moveKey (kbRight);
            break;

        case 3:
            moveKey (kbLeft);
            break;
        }
    } while (moves++ <= 500);

    moves = 0;
    drawView();
}

static const char *solution = "ABCDEFGHIJKLMNO ";

void TPuzzleView::winCheck() {
    /* SS: little change */
    int i;
    for (i = 0; i <= 15; i++)
//    for(int i = 0; i <= 15; i++)
        if (board[i / 4][i % 4] != solution[i])
            break;

    if (i == 16)
        solved = 1;
    drawView();
}

//
// TPuzzleWindow functions
//
TPuzzleWindow::TPuzzleWindow() :
        TWindowInit(&TPuzzleWindow::initFrame), TWindow(TRect(1, 1, 21, 7), "Puzzle", wnNoNumber) {
    flags &= ~(wfZoom | wfGrow);
    growMode = 0;

    TRect r = getExtent();
    r.grow(-1, -1);
    insert(new TPuzzleView(r));
}

class TTable: public TView {

public:

    TTable(TRect& r);
    virtual void draw();
    virtual void handleEvent(TEvent& event);
    void charFocused();

};

class TReport: public TView {

public:

    TReport(TRect& r);
    virtual void draw();
    virtual void handleEvent(TEvent& event);

private:

    long asciiChar;

};

class TAsciiChart: public TWindow {

public:

    TAsciiChart();
    virtual void handleEvent(TEvent &event);
};

TTable::TTable(TRect& r) :
        TView(r) {
    eventMask |= evKeyboard;
}

void TTable::draw() {
    TDrawBuffer buf;
    char color = getColor(6);

    for (ushort y = 0; y <= size.y - 1; y++) {
        buf.moveChar(0, ' ', color, (short) size.x);
        for (ushort x = 0; x <= size.x - 1; x++)
            buf.moveChar(x, (ushort)(32 * y + x), color, (ushort) 1);
        writeLine(0, y, (short) size.x, (ushort) 1, buf);
    }
    showCursor();
}

//
// cmCharFocused is a offset value (basically the ascii code of the
// current selected character) thus should be added, not or'ed, to
// cmAsciiTableCmdBase.
//

void TTable::charFocused() {
    message(owner, evBroadcast, cmAsciiTableCmdBase + cmCharFocused,
            (void *) (long) (cursor.x + 32 * cursor.y));
}

void TTable::handleEvent(TEvent& event) {
    TView::handleEvent(event);

    if (event.what == evMouseDown) {
        do {
            if (mouseInView(event.mouse.where)) {
                TPoint spot = makeLocal(event.mouse.where);
                setCursor(spot.x, spot.y);
                charFocused();
            }
        } while (mouseEvent(event, evMouseMove));
        clearEvent(event);
    } else {
        if (event.what == evKeyboard) {
            switch (event.keyDown.keyCode) {
            case kbHome:
                setCursor(0, 0);
                break;
            case kbEnd:
                setCursor(size.x - 1, size.y - 1);
                break;
            case kbUp:
                if (cursor.y > 0)
                    setCursor(cursor.x, cursor.y - 1);
                break;
            case kbDown:
                if (cursor.y < size.y - 1)
                    setCursor(cursor.x, cursor.y + 1);
                break;
            case kbLeft:
                if (cursor.x > 0)
                    setCursor(cursor.x - 1, cursor.y);
                break;
            case kbRight:
                if (cursor.x < size.x - 1)
                    setCursor(cursor.x + 1, cursor.y);
                break;
            default:
                setCursor(event.keyDown.charScan.charCode % 32,
                        event.keyDown.charScan.charCode / 32);
                break;
            }
            charFocused();
            clearEvent(event);
        }
    }
}

//
// TReport functions
//

TReport::TReport(TRect& r) :
        TView(r) {
    asciiChar = 0;
}

void TReport::draw() {
    TDrawBuffer buf;
    char color = getColor(6);
    char str[80];

    sprintf(str,
            "%s%c%s%3d%s%2X  ", //wsz: to print 32 characters
            "  Char: ", (asciiChar == 0) ? (char) 0x20 : (char) asciiChar, " Decimal: ",
            (int) asciiChar, " Hex: ", (int) asciiChar); //wsz: :-)

    buf.moveStr(0, str, color);
    writeLine(0, 0, 32, 1, buf);
}

void TReport::handleEvent(TEvent& event) {
    TView::handleEvent(event);
    if (event.what == evBroadcast) {
        if (event.message.command == cmAsciiTableCmdBase + cmCharFocused) {
            asciiChar = event.message.infoLong;
            drawView();
        }
    }
}

//
// TAsciiChart functions
//

TAsciiChart::TAsciiChart() :
        TWindowInit(&TAsciiChart::initFrame), TWindow(TRect(0, 0, 34, 12), "ASCII Chart",
                wnNoNumber) {
    TView *control;

    flags &= ~(wfGrow | wfZoom);
    palette = wpGrayWindow;

    TRect r = getExtent();
    r.grow(-1, -1);
    r.a.y = r.b.y - 1;
    control = new TReport(r);
    control->options |= ofFramed;
    control->eventMask |= evBroadcast;
    insert(control);

    r = getExtent();
    r.grow(-1, -1);
    r.b.y = r.b.y - 2;
    control = new TTable(r);
    control->options |= ofFramed;
    control->options |= ofSelectable;
    control->blockCursor();
    insert(control);
    control->select();
}

void TAsciiChart::handleEvent(TEvent &event) {
    TWindow::handleEvent(event);
}

class TCalendarView: public TView {

public:

    TCalendarView(TRect & r);
    virtual void handleEvent(TEvent& event);
    virtual void draw();

    // The following are code page dependent, take a look at tvdemo3.cc to
    // see how we handle it
    static char upArrowChar;
    static char oupArrowChar;
    static char downArrowChar;
    static char odownArrowChar;

private:

    unsigned days, month, year;
    unsigned curDay, curMonth, curYear;
};

class TCalendarWindow: public TWindow {

public:
    TCalendarWindow();
};

char TCalendarView::upArrowChar = '\036';
char TCalendarView::oupArrowChar = '\036';
char TCalendarView::downArrowChar = '\037';
char TCalendarView::odownArrowChar = '\037';

static const char *monthNames[] = { "", "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December" };

static unsigned char daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//
// TCalendarView functions
//

TCalendarView::TCalendarView(TRect& r) :
        TView(r) {
    /* SS: little change */

//    struct date d;
    options |= ofSelectable;
    eventMask |= evMouseAuto;

    time_t now = time(NULL);
    tm *broken = localtime(&now);

    year = curYear = broken->tm_year + 1900;
    month = curMonth = broken->tm_mon + 1;
    curDay = broken->tm_mday;

    drawView();
}

int dayOfWeek(int day, int month, int year) {
    int century, yr, dw;

    if (month < 3) {
        month += 10;
        --year;
    } else
        month -= 2;

    century = year / 100;
    yr = year % 100;
    dw = (((26 * (int) month - 2) / 10) + (int) day + yr + (yr / 4) + (century / 4) - (2 * century))
            % 7;

    if (dw < 0)
        dw += 7;

    return ((int) dw);
}

void TCalendarView::draw() {
    AllocLocalStr(str, size.x + 1);

    unsigned current = 1 - dayOfWeek(1, month, year);
    unsigned days = daysInMonth[month] + ((year % 4 == 0 && month == 2) ? 1 : 0);
    char color, boldColor;
    int i, j;
    TDrawBuffer buf;

    color = getColor(6);
    boldColor = getColor(7);

    buf.moveChar(0, ' ', color, size.x);

    sprintf(str, "%c%12s %4d %c", upArrowChar, monthNames[month], year, downArrowChar); //wsz: to see full year and down arrow
    /*ostrstream( str, sizeof str)
     << setw(9) << monthNames[month] << " " << setw(4) << year
     << " " << (char) 30 << "  " << (char) 31 << " " << ends;*/

    buf.moveStr(0, str, color);
    writeLine(0, 0, size.x, 1, buf);

    buf.moveChar(0, ' ', color, size.x);
    buf.moveStr(0, "Su Mo Tu We Th Fr Sa", color);
    writeLine(0, 1, size.x, 1, buf);

    for (i = 1; i <= 6; i++) {
        buf.moveChar(0, ' ', color, size.x);
        for (j = 0; j <= 6; j++) {
            if (current < 1 || current > days)
                buf.moveStr(j * 3, "   ", color);
            else {
                sprintf(str, "%2d", (int) current);
                if (year == curYear && month == curMonth && current == curDay)
                    buf.moveStr(j * 3, str, boldColor);
                else
                    buf.moveStr(j * 3, str, color);
            }
            current++;
        }
        writeLine(0, (short) (i + 1), size.x, 1, buf);
    }
}

void TCalendarView::handleEvent(TEvent& event) {
    TPoint point;

    TView::handleEvent(event);
    if (state && sfSelected)
    {
        if ( (event.what & evMouse) && (evMouseDown || evMouseAuto) )
        {
            point = makeLocal(event.mouse.where);
            if (point.x == 15 && point.y == 0)
            {
                ++month;
                if (month > 12)
                {
                    ++year;
                    month = 1;
                }
                drawView();
            }
            else if (point.x == 18 && point.y == 0)
            {
                --month;
                if (month < 1)
                {
                    --year;
                    month = 12;
                }
                drawView();
            }
        }
        else if (event.what == evKeyboard)
        {
            if ( (loByte(event.keyDown.keyCode) == '+') ||
            event.keyDown.keyCode == kbDown)
            {
                ++month;
                if (month > 12)
                {
                    ++year;
                    month = 1;
                }
            }
            else if ( (loByte(event.keyDown.keyCode) == '-') ||
            event.keyDown.keyCode == kbUp)
            {
                --month;
                if (month < 1)
                {
                    --year;
                    month = 12;
                }
            }
            drawView();
        }
    }
}

//
// TCalendarWindow functions
//

TCalendarWindow::TCalendarWindow() :
        TWindowInit(&TCalendarWindow::initFrame), TWindow(TRect(1, 1, 23, 11), "Calendar",
                wnNoNumber) {
    TRect r(getExtent());

    flags &= ~(wfZoom | wfGrow);
    growMode = 0;

    palette = wpCyanWindow;

    r.grow(-1, -1);
    insert(new TCalendarView(r));
}

class TMyApp: public TApplication {

public:
    TMyApp();
    static TStatusLine *initStatusLine(TRect r);
    static TMenuBar *initMenuBar(TRect r);
    virtual void handleEvent(TEvent& event);
protected:
    void myNewWindow();
    void aboutDlgBox();
    void puzzle();
    void calendar();
    void asciiTable();
    void calculator();
    void tile();
    void cascade();
public:
    static ushort executeDialog(TDialog* pD, void* data = 0);
private:
    static uchar systemMenuIcon[];
};

uchar TMyApp::systemMenuIcon[] = "~\360~";

ushort TMyApp::executeDialog(TDialog* pD, void* data) {
    ushort c = cmCancel;

    if (TProgram::application->validView(pD)) {
        if (data)
            pD->setData(data);
        c = TProgram::deskTop->execView(pD);
        if ((c != cmCancel) && (data))
            pD->getData(data);
        CLY_destroy(pD);
    }

    return c;
}

void TMyApp::aboutDlgBox() {
    TDialog *aboutBox = new TDialog(TRect(0, 0, 39, 13), "About");

    aboutBox->insert(new TStaticText(TRect(9, 2, 30, 9), "\003Turbo Vision Demo\n\n" // These strings will be
                    "\003C++ Version\n\n"// concatenated by the compiler.
                    "\003Copyright (c) 1994\n\n"// The \003 centers the line.
                    "\003Borland International"));

    aboutBox->insert(new TButton(TRect(14, 10, 26, 12), " OK", cmOK, bfDefault));

    aboutBox->options |= ofCentered;

    executeDialog(aboutBox);
}

void TMyApp::asciiTable() {
    TAsciiChart *chart = (TAsciiChart *) validView(new TAsciiChart);

    if (chart != 0) {
        deskTop->insert(chart);
    }
}

void TMyApp::calendar() {
    TCalendarWindow *cal = (TCalendarWindow *) validView(new TCalendarWindow);

    if (cal != 0) {
        deskTop->insert(cal);
    }
}

void TMyApp::calculator() {
    TCalculator *calc = (TCalculator *) validView(new TCalculator);

    if (calc != 0) {
        deskTop->insert(calc);
    }
}

void TMyApp::puzzle() {
    TPuzzleWindow *puzz = (TPuzzleWindow *) validView(new TPuzzleWindow);

    if (puzz != 0) {
        deskTop->insert(puzz);
    }
}

void TMyApp::tile() {
    deskTop->tile(deskTop->getExtent());
}

void TMyApp::cascade() {
    deskTop->cascade(deskTop->getExtent());
}

static short winNumber = 0; // initialize window number

class TDemoWindow: public TWindow // define a new window class
{

public:

    TDemoWindow(const TRect& r, const char *aTitle, short aNumber);

};

class TInterior: public TView {

public:

    TInterior(const TRect& bounds); // constructor
    virtual void draw(); // override TView::draw
};

TInterior::TInterior(const TRect& bounds) :
        TView(bounds) {
    growMode = gfGrowHiX | gfGrowHiY; //make size follow that of the window
    options = options | ofFramed;
}

void TInterior::draw() {
    const char *hstr = "Hello World!";
    ushort color = getColor(0x0301);
    TView::draw();
    TDrawBuffer b;
    b.moveStr(0, hstr, color);
    writeLine(4, 2, 12, 1, b);
}

TMyApp::TMyApp() :
        TProgInit(&TMyApp::initStatusLine, &TMyApp::initMenuBar, &TMyApp::initDeskTop) {
}

TStatusLine *TMyApp::initStatusLine(TRect r) {
    r.a.y = r.b.y - 1; // move top to 1 line above bottom
    return new TStatusLine(r, *new TStatusDef(0, 0xFFFF) +
    // set range of help contexts
            *new TStatusItem(0, kbF10, cmMenu) +
            // define an item
            *new TStatusItem("~Alt-X~ Exit", kbAltX, cmQuit) +
            // define an item
            *new TStatusItem("~Alt-F3~ Close", kbAltF3, cmClose)
            // and another one
                    );
}

TMenuBar *TMyApp::initMenuBar(TRect r) {
    r.b.y = r.a.y + 1; // set bottom line 1 line below top line
    TSubMenu& sub1 = *new TSubMenu((char *) systemMenuIcon, 0, hcNoContext)
            + *new TMenuItem("~A~bout...", cmAboutCmd, kbNoKey, hcNoContext) + newLine()
            + *new TMenuItem("~P~uzzle", cmPuzzleCmd, kbNoKey, hcNoContext)
            + *new TMenuItem("Ca~l~endar", cmCalendarCmd, kbNoKey, hcNoContext)
            + *new TMenuItem("Ascii ~T~able", cmAsciiCmd, kbNoKey, hcNoContext)
            + *new TMenuItem("~C~alculator", cmCalcCmd, kbNoKey, hcNoContext);
    TSubMenu& sub2 = *new TSubMenu("~F~ile", kbAltF)
            + *new TMenuItem("~O~pen", cmMyFileOpen, kbF3, hcNoContext, "F3")
            + *new TMenuItem("~N~ew", cmMyNewWin, kbF4, hcNoContext, "F4")
            + *new TMenuItem("~A~bout", cmAboutCmd, kbF1, hcNoContext, "F1") + newLine()
            + *new TMenuItem("E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X");
    TSubMenu& sub3 = *new TSubMenu("~W~indow", kbAltW)
            + *new TMenuItem("~R~esize/move", cmResize, kbCtrlF5, hcNoContext, "Ctrl-F5")
            + *new TMenuItem("~C~lose", cmClose, kbAltF3, hcNoContext, "Alt-F3")
            + *new TMenuItem("~T~ile", cmTile, kbNoKey, hcNoContext)
            + *new TMenuItem("C~a~scade", cmCascade, kbNoKey, hcNoContext)
            + *new TMenuItem("~N~ext", cmNext, kbF6, hcNoContext, "F6")
            + *new TMenuItem("~Z~oom", cmZoom, kbF5, hcNoContext, "F5");

    return new TMenuBar(r, sub1 + sub2 + sub3);
}

void TMyApp::handleEvent(TEvent& event) {
    TApplication::handleEvent(event); // act like base!
    if (event.what == evCommand) {
        switch (event.message.command) {
        case cmMyNewWin:
            myNewWindow();
            break;
        case cmAboutCmd:
            aboutDlgBox();
            break;
        case cmCalendarCmd:
            calendar();
            break;
        case cmAsciiCmd:
            asciiTable();
            break;
        case cmCalcCmd:
            calculator();
            break;
        case cmPuzzleCmd:
            puzzle();
            break;
        case cmTile:
            tile();
            break;
        case cmCascade:
            cascade();
            break;
        default:
            return;
        }
        clearEvent(event); // clear event after handling
    }
}

void TMyApp::myNewWindow() {
    TRect r(0, 0, 26, 7);
    r.move(rand() % 53, rand() % 16);
    TDemoWindow *window = new TDemoWindow(r, "Demo Window", ++winNumber);
    deskTop->insert(window);
}

TDemoWindow::TDemoWindow(const TRect& bounds, const char *aTitle, short aNumber) :
        TWindowInit(&TDemoWindow::initFrame), TWindow(bounds, aTitle, aNumber) {
    TRect r = getClipRect();
    r.grow(-1, -1);
    options |= ofTileable;
    insert(new TInterior(r));
}

int main() {
    TMyApp myApp;
    myApp.run();
    return 0;
}

