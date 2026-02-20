#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

static HANDLE hin, hout;
static DWORD inModeOrig;

static void terminate_on_error(const char *msg) {
    DWORD err = GetLastError(); //"GetLastError()" is an OS thread
    fprintf(stderr, "%s (err=%lu)\n", msg, (unsigned long)err);
    ExitProcess(1);
}



static void enableRawInput(void) {
    hin  = GetStdHandle(STD_INPUT_HANDLE); //hin -> handle to i/o
    hout = GetStdHandle(STD_OUTPUT_HANDLE); //hout -> handle to output screen
    if (hin == INVALID_HANDLE_VALUE || hout == INVALID_HANDLE_VALUE) terminate_on_error("GetStdHandle"); //technically invalid_handle_value represents -1 die throws err

    if (!GetConsoleMode(hin, &inModeOrig)) terminate_on_error("GetConsoleMode"); //inModeOrig stores current console mode into inModeOrig

    DWORD mode = inModeOrig;

    // get keypresses immediately
    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);

    // stop CMD from freezing on mouse select (Quick Edit)
    mode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_MOUSE_INPUT);
    mode |= ENABLE_EXTENDED_FLAGS;

    if (!SetConsoleMode(hin, mode)) terminate_on_error("SetConsoleMode");
}



static void disableRawInput(void) {
    SetConsoleMode(hin, inModeOrig);
}



static int readKey(void) {
    INPUT_RECORD rec;
    DWORD nread;

    while (1) {
        if (!ReadConsoleInputA(hin, &rec, 1, &nread)) terminate_on_error("ReadConsoleInput");
        if (rec.EventType != KEY_EVENT) continue;

        KEY_EVENT_RECORD ke = rec.Event.KeyEvent;
        if (!ke.bKeyDown) continue;

        if (ke.wVirtualKeyCode == VK_BACK) return 8;      // backspace
        if (ke.wVirtualKeyCode == VK_RETURN) return 13;   // enter

        // printable ASCII
        if (ke.uChar.AsciiChar >= 32 && ke.uChar.AsciiChar <= 126)
            return (unsigned char)ke.uChar.AsciiChar;
    }
}



static void writeStr(const char *s) {
    DWORD written;
    if (!WriteConsoleA(hout, s, (DWORD)strlen(s), &written, NULL)) terminate_on_error("WriteConsoleA");
}




int main(void) {
    enableRawInput();
    atexit(disableRawInput);

    const char *prompt = ">>";
    char buf[256] = "edit this";     // prefilled text
    int len = (int)strlen(buf);

    // show prompt + initial text
    writeStr(prompt);
    writeStr(buf);

    while (1) {
        int k = readKey();

        if (k == 13) { // Enter
            writeStr("\r\n");
            break;
        }

        if (k == 8) { // Backspace
            if (len > 0) {
                // remove one char from buffer end
                len--;
                buf[len] = '\0';

                // erase from screen: move left, print space, move left
                writeStr("\b \b");
            }
            continue;
        }

        // add char at end (simple)
        if (len < (int)sizeof(buf) - 1) {
            buf[len++] = (char)k;
            buf[len] = '\0';

            char c = (char)k;
            DWORD written;
            WriteConsoleA(hout, &c, 1, &written, NULL);
        }
    }

    printf("Final text: %s\n", buf);
    return 0;
}
