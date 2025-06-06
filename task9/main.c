#include "mc.h"
int main()
{
    initMc();

    MyWindow* left = createTable(0);
    MyWindow* right = createTable(COLS / 2);
    MyWindow* activeWin = left;

    int countFiles = 0;
    struct dirent** namelist = NULL;

    countFiles = wprintDir(right, &namelist, ".");
    countFiles = wprintDir(left, &namelist, ".");

    refreshMyWindow(left);
    refreshMyWindow(right);

    char fullPath[PATH_MAX];
    char currentPath[PATH_MAX];
    getcwd(currentPath, sizeof(currentPath));

    int x = 0;
    int y = 2;
    int ch;
    bool exit = 0;
    while (!exit)
    {
        highlightFile(activeWin, y, x);
        refresh();
        ch = getch();
        if (ch == '\t')  // tab
        {
            changeStatus("Tab");
            dehighlightFile(activeWin);
            if (activeWin == left)
            {
                activeWin = right;
            }
            else
            {
                activeWin = left;
            }
            countFiles = wprintDir(activeWin, &namelist, ".");
        }
        else if (ch == 'w' || ch == 'k' || ch == KEY_UP)  // up
        {
            changeStatus("Up");
            if (y > 2) y--;
        }
        else if (ch == 's' || ch == 'j' || ch == KEY_DOWN)  // down
        {
            changeStatus("Down");
            if (y < countFiles) y++;
        }
        else if (ch == '\n' || ch == '\r')  // enter
        {
            changeStatus("Enter");
            snprintf(fullPath, sizeof(fullPath), "%s/%s", currentPath, namelist[y - 1]->d_name);
            if (strlen(fullPath) > PATH_MAX)
            {
                changeStatus("Path too long");
                continue;
            }

            if (namelist[y - 1]->d_type == DT_DIR)
            {
                freeNamelist(namelist, countFiles);
                countFiles = wprintDir(activeWin, &namelist, fullPath);
                strcpy(currentPath, fullPath);
            }
            else
            {
                changeStatus("It is file");
            }
        }
        else if (ch == 'q' || ch == 27)  // escape
        {
            exit = 1;
        }
        else
        {
            changeStatus("Unknow symbol");
        }
    }

    endwin();
    return 0;
}
