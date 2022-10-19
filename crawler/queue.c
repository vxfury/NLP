#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

// �洢�ڵ�Ķ���
Queue *que_init()
{
    Queue *q;
    if ((q = malloc(sizeof(Queue))) == NULL) return NULL;
    memset(q, 0, sizeof(Queue));
    q->head = 0;
    q->tail = 0;
    return q;
}
// �����п�
int queue_empty(Queue *q)
{
    if (q->head == q->tail) return 1;
    return 0;
}

// ��ӣ��ɹ�����1�����򷵻�0
int enqueue(Queue *q, char *node)
{
    if ((q->head - q->tail + maxq) % maxq == 1)
        return 0;
    else {
        q->myqueue[q->tail] = node;
        q->tail = (q->tail + 1) % maxq;
        return 1;
    }
}
// ���ӣ����س��ӽڵ��NULL���ӿգ�
char *dequeue(Queue *q)
{
    if (q->head == q->tail)
        return NULL;
    else {
        int head = q->head;
        q->head = (head + 1) % maxq;
        return q->myqueue[head];
    }
}

// ���ض��г���
int queue_length(const Queue *const q)
{
    if (q == NULL) // ���������Ϊָ�룬�����п�
        exit(-1);  // �����쳣����

    unsigned int length = q->tail - q->head;

    return length;
}
