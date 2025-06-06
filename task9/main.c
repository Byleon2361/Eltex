#include "mc.h"
#include <ncurses.h>

int main()
{
    initMc();

    MyWindow* left = createTable(0);
    MyWindow* right = createTable(COLS / 2);
    MyWindow* activeWin = left;

    int countFiles = 0;
    int countPrevFiles = 0;
    int countTempFiles = 0;

    countPrevFiles = wprintDir(right, ".", 0);
    countFiles = wprintDir(left, ".", 0);

    refreshMyWindow(left);
    refreshMyWindow(right);

    char fullPath[PATH_MAX];
    char currentPath[PATH_MAX];
    char prevPath[PATH_MAX];
    char tempPath[PATH_MAX];
    getcwd(currentPath, sizeof(currentPath));
    getcwd(prevPath, sizeof(prevPath));

    int x = 0;
    int y = 2;
    int offsetVisibleArea = 0;
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

            strcpy(tempPath, prevPath);
            strcpy(prevPath, currentPath);
            strcpy(currentPath, tempPath);

            countTempFiles = countPrevFiles;
            countPrevFiles = countFiles;
            countFiles = countTempFiles;

            if (activeWin == left)
            {
                activeWin = right;
            }
            else
            {
                activeWin = left;
            }

            if (y > 2 + countFiles - 1)
            {
                y = (countFiles > 0) ? 2 + countFiles - 2 : 2;
            }
        }
        else if (ch == 'w' || ch == 'k' || ch == KEY_UP)  // up
        {
            changeStatus("Up");
            if (y > 2)
                y--;
            else if (offsetVisibleArea > 0)
            {
                offsetVisibleArea--;
                y = 2;
                countFiles = wprintDir(activeWin, currentPath, offsetVisibleArea);
            }
        }
        else if (ch == 's' || ch == 'j' || ch == KEY_DOWN)  // down
        {
            changeStatus("Down");
            int widthVisibleArea = LINES - 3;
            if (y < 2 + widthVisibleArea - 1 && y < countFiles)
            {
                y++;
            }
            else if (offsetVisibleArea + widthVisibleArea < countFiles)
            {
                offsetVisibleArea++;
                countFiles = wprintDir(activeWin, currentPath, offsetVisibleArea);
            }
        }
        else if (ch == '\n' || ch == '\r')  // enter
        {

            changeStatus("Enter");

            struct dirent** namelist = NULL;
            int n = scandir(currentPath, &namelist, 0, alphasort);
            if (n < 0)
            {
                changeStatus("Scan dir error");
                continue;
            }

            if (y - 1 >= n)
            {
                freeNamelist(namelist, n);
                changeStatus("Invalid selection");
                continue;
            }

            snprintf(fullPath, sizeof(fullPath), "%s/%s", currentPath, namelist[y - 1]->d_name);

            if (strlen(fullPath) > PATH_MAX)
            {
                freeNamelist(namelist, n);
                changeStatus("Path too long");
                continue;
            }

            if (namelist[y - 1]->d_type == DT_DIR)
            {
                countFiles = wprintDir(activeWin, fullPath, 0);
                strcpy(currentPath, fullPath);
                offsetVisibleArea = 0;
                y = 2;
            }
            else
            {
                changeStatus("It is file");
                if (execl("/usr/bin/vim", "vim", fullPath, (char*)NULL) == -1)
                {
                    perror("vim");
                    return 1;
                }
            }
            freeNamelist(namelist, n);
        }
        else if (ch == 'q' || ch == 27)  // escape
        {
            exit = 1;
        }
        else
        {
            changeStatus("Unknown symbol");
        }
    }

    destroyMyWindow(left);
    destroyMyWindow(right);

    delwin(statusWin);
    slk_restore();
    resetty();
    clear();
    refresh();
    endwin();
    return 0;
}
