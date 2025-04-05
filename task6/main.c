#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct abonent
{
    char name[10];
    char second_name[10];
    char tel[10];
} Abonent;
typedef struct node
{
    Abonent* abonent;
    struct node* next;
    struct node* prev;
} Node;

typedef struct linkedList
{
    Node* head;
    Node* tail;
    int count;
} LinkedList;

LinkedList* initLinkedList()
{
    LinkedList* linkedList = (LinkedList*)malloc(sizeof(LinkedList));

    linkedList->head = NULL;
    linkedList->tail = NULL;
    linkedList->count = 0;

    return linkedList;
}
void addAbonent(LinkedList* list, Abonent* abonent)
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->abonent = abonent;

    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
        node->prev = NULL;
        node->next = NULL;
        return;
    }
    else
    {
        node->prev = list->tail;
        node->next = NULL;

        list->tail->next = node;
        list->tail = node;
    }
}
void printAbonent(Abonent* abonent)
{
    printf("Имя: %s\n", abonent->name);
    printf("Фамилия: %s\n", abonent->second_name);
    printf("Номер: %s\n", abonent->tel);
    printf("--------------------------\n");
}
void printAllAbonents(LinkedList* list)
{
    Node* node = list->head;
    while (node != NULL)
    {
        if (node->abonent != NULL)
        {
            printAbonent(node->abonent);
        }
        node = node->next;
    }
}

Node* searchNode(LinkedList* list, char* name)
{
    Node* node = list->head;
    while (node != NULL)
    {
        if (strcmp(node->abonent->name, name) == 0)
        {
            return node;
        }
        node = node->next;
    }
    return NULL;
}
void removeAbonent(LinkedList* list, char* name)
{
    if (list == NULL) return;

    Node* node = searchNode(list, name);
    if (node == NULL) return;

    Node* prevN = node->prev;
    Node* nextN = node->next;

    if (prevN != NULL)
    {
        prevN->next = nextN;
    }
    else
    {
        list->head = nextN;
    }
    if (nextN != NULL)
    {
        nextN->prev = prevN;
    }
    else
    {
        list->tail = prevN;
    }
    free(node->abonent);
    free(node);
}
void freeList(LinkedList* list)
{
    Node* node = list->head;
    while (node != NULL)
    {
        Node* nextNode = node->next;
        free(node->abonent);
        free(node);
        node = nextNode;
    }
    free(list);
}
int main()
{
    LinkedList* list = initLinkedList();
    char name[10];
    int number = 0;
    while (number != 5)
    {

        printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по имени\n4) Вывод всех записей\n5) Выход\n");
        scanf("%d", &number);
        switch (number)
        {
            case 1:
                Abonent* abonent = (Abonent*)malloc(sizeof(Abonent));
                printf("Введите имя\n");
                scanf("%9s", abonent->name);
                printf("Введите фамилию\n");
                scanf("%9s", abonent->second_name);
                printf("Введите номер\n");
                scanf("%9s", abonent->tel);
                addAbonent(list, abonent);
                break;
            case 2:
                printf("Введите имя абонента для удаления\n");
                scanf("%9s", name);
                removeAbonent(list, name);
                break;
            case 3:
                printf("Введите имя для поиска\n");
                scanf("%9s", name);
                if (searchNode(list, name) != NULL)
                    printAbonent(searchNode(list, name)->abonent);
                else
                    printf("Абонент не найден\n");
                break;
            case 4: printAllAbonents(list); break;
            case 5:
                freeList(list);
                return 0;
                break;
            default: printf("Неверный номер"); break;
        }
    }
    freeList(list);
    return 0;
}