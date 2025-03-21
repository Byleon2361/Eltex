#include <stdio.h>
void printMatrix(int arr[], int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d ", arr[i * N + j]);
        }
        printf("\n");
    }
}
void squareMatrix(int N)
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d ", i * N + j + 1);
        }
        printf("\n");
    }
}
void reverseArr(int arr[], int size)
{
    for (int i = size - 1; i >= 0; i--)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
void triangleMatrix(int N)
{
    int countZero = N - 1;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (countZero != 0)
            {
                printf("%d ", 0);
                countZero--;
            }
            else
            {
                printf("%d ", 1);
            }
        }
        countZero = N - 2 - i;
        printf("\n");
    }
}
void snakeMatrix(int newArr[], int N)
{
    int direction = 0;
    int n = N;
    int k = 0;
    int i = 0;
    int j = 0;
    while (k < N * N)
    {
        switch (direction)
        {
            case 0:
                newArr[i * N + j] = k;
                if (j < n - 1)
                    j++;
                else
                {
                    direction++;
                    i++;
                }
                break;
            case 1:
                newArr[i * N + j] = k;
                if (i < n - 1)
                    i++;
                else
                {
                    direction++;
                    j--;
                }
                break;
            case 2:
                newArr[i * N + j] = k;
                if (j > N - n)
                    j--;
                else
                {
                    direction++;
                    i--;
                }
                break;
            case 3:
                newArr[i * N + j] = k;
                if (i > N - n + 1)
                    i--;
                else
                {
                    direction = 0;
                    n--;
                    j++;
                }
                break;
        }
        k++;
    }
}
int main()
{
    int N = 0;
    printf("Вывод квадратной матрицы размером N\n");
    printf("Введите N\n");
    scanf("%d", &N);
    squareMatrix(N);

    printf("Вывод квадратной матрицы размером N\n");
    printf("Введите N\n");
    scanf("%d", &N);
    int arr[N];
    printf("Введите элементы массива\n");
    for (int i = 0; i < N; i++)
    {
        printf("Элемент под индексом %d: ", i);
        scanf("%d", &arr[i]);
    }
    reverseArr(arr, N);

    printf("Вывод треугольной матрицы размером N\n");
    printf("Введите N\n");
    scanf("%d", &N);
    triangleMatrix(N);

    printf("Вывод матрицы размером N змейкой\n");
    printf("Введите N\n");
    scanf("%d", &N);
    int arr2[N * N];
    snakeMatrix(arr2, N);
    printMatrix(arr2, N);
    return 0;
}