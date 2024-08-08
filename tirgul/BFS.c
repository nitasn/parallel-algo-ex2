#include "../src/threadpool.h"
#include "../src/graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <math.h> // Include for log2 and ceil
#include <unistd.h>


/********************************************************************* Queue ***************************************************************************************************************/
typedef struct {
    int *items;
    int front;
    int rear;
    int capacity;
    pthread_mutex_t lock; // Mutex for thread safety
} Queue;

Queue* createQueue(int capacity) {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    q->items = (int*)malloc(sizeof(int) * capacity);
    q->capacity = capacity;
    q->front = -1;
    q->rear = -1;
    pthread_mutex_init(&q->lock, NULL); // Initialize the mutex
    return q;
}

int isQueueEmpty(Queue *q) {
    return q->front == -1;
}

void enqueue(Queue *q, int value, int sid) {
    pthread_mutex_lock(&q->lock); // Lock the queue for thread safety
    
    if(q->rear == q->capacity - 1) {
        printf("Queue is full\n");
        pthread_mutex_unlock(&q->lock); // Unlock before returning
        return;
    }
    if(q->front == -1)
        q->front = 0;
    q->rear++;
    q->items[q->rear] = value;
    //printf("at level: %d, enter element: %d\n", sid ,value);
    pthread_mutex_unlock(&q->lock); // Unlock after modifying the queue
}

int dequeue(Queue *q, int sid) {
    pthread_mutex_lock(&q->lock); // Lock the queue for thread safety
    
    if(isQueueEmpty(q)) {
        pthread_mutex_unlock(&q->lock); // Unlock before returning
        return -1;
    }
    int item = q->items[q->front];
    q->front++;
    if(q->front > q->rear) {
        q->front = -1;
        q->rear = -1;
    }
    
   // printf("at level: %d, exit element: %d\n", sid ,item);

    pthread_mutex_unlock(&q->lock); // Unlock after modifying the queue
    return item;
}


// Assuming the Queue structure and related functions are defined here
void freeQueue(Queue *q) {
    if (q != NULL) {
        // First, destroy the mutex
        pthread_mutex_destroy(&q->lock);

        // Free the dynamically allocated array of items
        if (q->items != NULL) {
            free(q->items);
            q->items = NULL;
        }

        // Finally, free the queue structure itself
        free(q);
        q = NULL;
    }
}

Queue** createQueueArray(int numVertices) {
    // Calculate the capacity based on the log of numVertices
    int capacity = numVertices + 1;

    // Allocate memory for an array of Queue pointers
    Queue **queueArray = (Queue**)malloc(numVertices * sizeof(Queue*));
    if (queueArray == NULL) {
        fprintf(stderr, "Memory allocation failed for queue array\n");
        exit(EXIT_FAILURE);
    }
    

    // Initialize each queue in the array with the calculated capacity
    for (int i = 0; i < numVertices; ++i) {
        queueArray[i] = createQueue(capacity);
        // Check for allocation failure in each queue
        if (queueArray[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for queue %d\n", i);
            // Cleanup previously allocated queues before exiting
            for (int j = 0; j < i; ++j) {
                freeQueue(queueArray[j]);
            }
            free(queueArray);
            exit(EXIT_FAILURE);
        }
    }
    
    return queueArray;
}

/********************************************************************* BFS ***************************************************************************************************************/

void print_queue(Queue* q, int id) {
    printf("current queue id: %d\n  ",id);
    for (int i=0; i< q->rear + 1; i++) {
        printf("%d , ", q->items[i]);
    }
    putchar('\n');}

void print_arr(int* arr, int len, int source) {
    printf("distances from %d are: ",source);
    for (int i=0; i< len; i++) {
        printf("%d , ", arr[i]);
    }
    putchar('\n');
}



struct pbfsArgs {
    Graph *graph;
    vertex v;
    int* visited;
    Queue* queue;
    int *distance;
    int level;
};

typedef struct pbfsArgs pbfsArgs;




void parallel_bfs_visit(void *args) {
    //extract the args
    pbfsArgs *data = (pbfsArgs *) args;
    Graph *graph = data->graph;
    vertex v = data->v;
    Queue* q = data -> queue;
    int* visited = data->visited;
    int* distance = data -> distance;
    int level = data->level;
    /* Start enquing */
    node *neighborsPtr = graph->adjacencyLists[v];
    while (neighborsPtr != NULL) {
        vertex neighbor = neighborsPtr->v;

        /* Synchronously increment the number of visits for the neighbor */
        pthread_mutex_lock(&graph->num_visits_mutexes[neighbor]);
        if(visited[neighbor] != 1) {
            visited[neighbor]  = 1;
            distance[neighbor] = distance[v] + 1;
            enqueue(q, neighbor, level);
        }
        pthread_mutex_unlock(&graph->num_visits_mutexes[neighbor]);
        
        neighborsPtr = neighborsPtr->next;
    }
    
   // return NULL;

}


void pbfs(Graph* graph, int startVertex, threadpool thpool, int* d) {
    
  
    int numVertices = graph->numVertices;
    Queue** queue_arr = createQueueArray(numVertices);
    // Initialize visited array for each BFS
    int *visited = (int *)calloc(graph->numVertices, sizeof(int));
    int level = 0;

    Queue* q_l = queue_arr[level];
    enqueue(q_l, startVertex, level);
    pbfsArgs* aargs[numVertices];

    for(;level < graph -> numVertices;  q_l = queue_arr[++level])
    {

        int i = 0;
        while (!isQueueEmpty(q_l)) {
            // create params
            pbfsArgs* params = malloc(sizeof(pbfsArgs));
            int v = dequeue(q_l, level);
            visited[v] = 1; //mark the vertex as visited
            Queue* next_queue = queue_arr[level + 1];
            params->graph = graph;
            params->v = v;
            params->queue = next_queue;
            params-> visited = visited;
            params->distance = d;
            params-> level = level + 1;
            aargs[i++] = params;
        }
        
        for (int j = 0; j<i; j++) {
            thpool_add_work(thpool, parallel_bfs_visit, (void*)aargs[j]);
        }
        
        thpool_wait(thpool);
    }
    print_arr(d, numVertices, startVertex);
}

void bfs(Graph *graph, int **m) {
    
    for (int i = 0; i < graph->numVertices; i++) {
        for (int j = 0; j < graph->numVertices; j++) {
            m[i][j] = (i == j) ? 0 : -1;
        }
    }
    
    threadpool pbfs_tp = thpool_init(2);

    //Should Parallel later
     for (int startVertex = 0; startVertex < graph->numVertices; startVertex++) {
         pbfs(graph, startVertex, pbfs_tp, m[startVertex]);
     }
    
    thpool_destroy(pbfs_tp);
}

