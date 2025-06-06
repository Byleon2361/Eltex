#include "mc.h"
int main()
{
    initMc();

    MyWindow* left = createTable(0);
    MyWindow* right = createTable(COLS / 2);
    MyWindow* activeWin = left;

    int countFiles = 0;
    int countPrevFiles = 0;
    int countTempFiles = 0;
    struct dirent** namelist = NULL;

    countPrevFiles = wprintDir(right, &namelist, ".", 0);
    countFiles = wprintDir(left, &namelist, ".",0);

    refreshMyWindow(left);
    refreshMyWindow(right);

    char fullPath[PATH_MAX];
    char leftPath[PATH_MAX];
    char rigthPath[PATH_MAX];
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

                strcpy(tempPath,prevPath);
                strcpy(prevPath,currentPath);

              countTempFiles = countPrevFiles;
              countPrevFiles = countFiles;
            if (activeWin == left)
            {
                activeWin = right;
            }
            else
            {
                activeWin = left;
            }

            countFiles = countTempFiles;

            if (y > 2 + countFiles - 1)
            {
              y = (countFiles > 0) ? 2 + countFiles - 2 : 2;
            }
            strcpy(currentPath, tempPath);
        }
        else if (ch == 'w' || ch == 'k' || ch == KEY_UP)  // up
        {
            changeStatus("Up");
            if (y > 2) y--;
            else if(offsetVisibleArea > 0)
            {
              offsetVisibleArea--;
              y = 2;
              wprintDir(activeWin, &namelist, currentPath, offsetVisibleArea);
            }
        }
        else if (ch == 's' || ch == 'j' || ch == KEY_DOWN)  // down
        {
            changeStatus("Down");
            int widthVisibleArea = LINES-3;
            if(y<2+widthVisibleArea - 1 && y < countFiles)
            {
              y++;
            }
            else if (offsetVisibleArea + widthVisibleArea < countFiles)
            {
              offsetVisibleArea++;
              wprintDir(activeWin, &namelist, currentPath, offsetVisibleArea);
            }
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
                countFiles = wprintDir(activeWin, &namelist, fullPath, offsetVisibleArea);
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
