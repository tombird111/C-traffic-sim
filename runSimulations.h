/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

/* Structures */
struct queueStruct
{
  int wait;
  struct queueStruct *next;
};
 typedef struct queueStruct QUEUE;

struct statStruct
{
  int carsPassed;
  int cumulativeWait;
  int maxWait;
  int clearTime;
};
 typedef struct statStruct STATS;


/* Function prototypes */
QUEUE *newCar(int wait);
void enqueue(QUEUE **queue, int wait);
int dequeue(QUEUE **queue);
void addToWaits(QUEUE *queue);
int countCars(QUEUE *queue, int count);
STATS *createStats();
void freeStats(STATS *stats);
void runOneSimulation(STATS *leftTotals, STATS *rightTotals, int leftChance, int leftLimit, int rightChance, int rightLimit);
void addToStats(STATS *stats, int carValue);
void addToTotals(STATS *stats, STATS *totals);
char lightCheck(int lightIteration, char leftLightGreen, int leftLimit, int rightLimit);
char randomCheck(int randomChance);
