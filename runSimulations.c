#include "runSimulations.h"

/* Main Function */
int main()
{
  printf("Enter a character at any time to exit.\n");
  printf("Enter the chance for a car to arrive on the left (out of 100): ");
  int leftChance = getInputChance(); /* Get the inputted chance after a prompt */
  printf("Enter the period for the left traffic light: ");
  int leftPeriod = getInputPeriod(); /* Get the inputted period after a prompt */
  printf("Enter the chance for a car to arrive on the right (out of 100): ");
  int rightChance = getInputChance(); /* Repeat the process for the right hand side */
  printf("Enter the period for the right traffic light: ");
  int rightPeriod = getInputPeriod();
  int iteration = 0;
  STATS* leftTotals = createStats(); /* Create stats to store information from the simulations */
  STATS* rightTotals = createStats();
  for(iteration=0; iteration<100; iteration++) /* For 100 iterations */
  {
    srand(iteration); /* Seed the random number based on the current iteration */
    runOneSimulation(leftTotals, rightTotals, leftChance, leftPeriod, rightChance, rightPeriod); /* Run one simulation with the inputted information, and stats */
  }
  /* Display all the information in a series of print statements */
  printf("Parameter values:\n");
  printf("  from left:\n");
  printf("    traffic arrival rate: %d%%\n", leftChance);
  printf("    traffic light period: %d\n", leftPeriod);
  printf("  from right:\n");
  printf("    traffic arrival rate: %d%%\n", rightChance);
  printf("    traffic light period: %d\n", rightPeriod);
  printf("Results (averaged over 100 runs):\n");
  printf("  from left:\n"); /* Values from the totals are divided by 100 to get the average value*/
  printf("    number of vehicles:   %d\n", (leftTotals->carsPassed/100));
  printf("    average waiting time: %d\n", (leftTotals->cumulativeWait/100));
  printf("    maximum waiting time: %d\n", (leftTotals->maxWait/100));
  printf("    clearance time:       %d\n", (leftTotals->clearTime/100));
  printf("  from right:\n");
  printf("    number of vehicles:   %d\n", (rightTotals->carsPassed/100));
  printf("    average waiting time: %d\n", (rightTotals->cumulativeWait/100));
  printf("    maximum waiting time: %d\n", (rightTotals->maxWait/100));
  printf("    clearance time:       %d\n", (rightTotals->clearTime/100));
  freeStats(leftTotals);
  freeStats(rightTotals);
  return 0;
}

int getInputChance() /* Gets an input integer or exits the program if a non integer is recieved */
{
  char correctInput = 1; /* Note that an incorrect input has not yet been entered */
  do
  {
    correctInput = 1;
    int input;
    if(scanf("%d", &input) != 1) /* Read the next input */
    {
      exit; /* Exit if the input is not an integer */
    }
    else /* Otherwise */
    {
      if(input > 100 || input < 0) /* Check that the input is within range */
      {
        printf("Not an integer between 0 and 100\n"); /* Display an error message if it is not */
        correctInput = 0; /* Mark that an incorrect input was entered */
      }
      if(correctInput) /* If the input is still correct */
      {
        return input; /* Return the input */
      }
    }  
  }
  while(correctInput != 1); /* Repeat until a valid input is enterred */
}


int getInputPeriod() /* Gets an input period as an integer or exits the program if a non integer is recieved */
{
  char correctInput = 1; /* Note that an incorrect input has not yet been entered*/
  do
  {
    correctInput = 1;
    int input;
    if(scanf("%d", &input) != 1) /* Read the next input */
    {
      exit; /* Exit if the input is not an integer */
    }
    else /* Otherwise */
    {
      if(input > 500 || input < 1) /* Check that the input is within range*/
      {
        printf("Not an integer between 1 and 500\n"); /* Display an error message if it is not */
        correctInput = 0; /* Mark that an incorrect input was entered */
      }
      if(correctInput) /* If the input is still correct */
      {
        return input; /* Return the input */
      }
    }  
  }
  while(correctInput != 1); /* Repeat until a valid input is enterred */
}

/* Queue Functions */
QUEUE *newCar (int wait) /* Adds a new car with the inputted wait time */
{
  QUEUE *c = (QUEUE *)malloc(sizeof(QUEUE)); /* Allocate memory for a queue node */
  if (c==NULL) /* If memory fails to allocate */
  {
    fprintf(stderr, "Error when allocating to memory, error code: %d, %s\n", errno, strerror(errno)); /* Display an error and exit */
    exit(-1);
  }
  c->wait = wait; /* Otherwise, set its wait value to the inputted wait, and its next to null */
  c->next = NULL;
  return c; /* Return the pointer to the queue node */
}

void enqueue(QUEUE **queue, int wait) /* Adds a car to the end of the queue */
{
  if(*queue == NULL) /* If the location pointed to is null */
  {
     QUEUE *newNode = newCar(wait); /* Create a new node */
     *queue = newNode; /* Set the current pointer to point to the new node */
  }
  else
  {
    QUEUE **nextPointer = &(**queue).next; /* Get the pointer of the current node */
    enqueue(nextPointer, wait); /* Enqueue using the next pointer of the current node */
  }
}


int dequeue(QUEUE **queue) /* Remove the first member of the queue and return its value */
{
  if(*queue != NULL)
  {
    QUEUE *queueStart = *queue; /* Create a pointer to the head of the queue */
    int value = queueStart->wait; /* Store the value of the head of the queue */
    *queue = queueStart->next; /* Set the pointer of the start of the queue to point to the next member of the queue */
    free(queueStart); /* Free the memory belonging to the old head of the queue */ 
    return value;
  }
}

void addToWaits(QUEUE *queue) /* Add one to the values of the weights in a queue */
{
  if(queue == NULL) /* If the current queue node is empty */
  {
    return; /* Return */
  }
  else /* Otherwise */
  {
    queue->wait = (queue->wait + 1); /* Add 1 to the wait value of the current node */
    addToWaits(queue->next); /* Attempt to add to the waits of the next queue node */
  }
}

int countCars(QUEUE *queue, int count) /* Counts the number of cars in a queue */
{
  if(queue == NULL) /* If the current queue node is null */
  {
    return count; /* Return the current count */
  }
  else
  {
    countCars(queue->next, (count + 1)); /* Otherwise, continue to the next node with the current count + 1 */
  }
}

/* Statistic functions */
STATS *createStats() /* Create a set of stats */
{
  STATS *newStat = (STATS *)malloc(sizeof(STATS)); /* Allocate memory for some statistics */
  if (newStat==NULL) /* If memory fails to allocate */
  {
    fprintf(stderr, "Error when allocating to memory, error code: %d, %s\n", errno, strerror(errno)); /* Display an error and exit */
    exit(-1);
  }
  newStat->maxWait = 0; /* Otherwise, set the values of the stats to 0 */
  newStat->cumulativeWait = 0;
  newStat->carsPassed = 0;
  newStat->clearTime = 0;
  return newStat; /* Return the pointer to the  node */
}

void freeStats(STATS *stat) /* Frees the memory allocated to a stat */
{
  free(stat);
}

void addToStats(STATS *stats, int carValue) /* Adds the information from a given car to a set of stats */
{
  stats->cumulativeWait += carValue; /* Add the cars wait time to the cumulative wait time in the stats */
  stats->carsPassed += 1; /* Add that a car has passed */
  if(stats->maxWait < carValue) /* If the car waited longer than the current maximum wait time */
  {
    stats->maxWait = carValue; /* Set a new maximum wait time */
  };
  return;
}

void addToTotals(STATS *stats, STATS *totals) /* Adds the information from a simulation to some totals */
{ 
  if(stats->carsPassed > 0) /* If at least 1 car passed */
  {
    totals->cumulativeWait+=(stats->cumulativeWait/stats->carsPassed); /* Add the average car wait time to the cumulative wait of the totals */
  }
  totals->carsPassed+=stats->carsPassed; /* Add the number of cars, the maximum wait, and time to clear to the totals respectively*/
  totals->maxWait+=stats->maxWait;
  totals->clearTime+=stats->clearTime;
  return;
}

/* Checking functions */
char lightCheck(int lightIteration, char leftLightGreen, int leftLimit, int rightLimit) /* Checks whether the lights will switch based on inputted periods and iterations */
{
  switch(leftLightGreen)
  {
    case 0: if(rightLimit <= lightIteration) {return 1;} else {return 0;} /* If the left light is red, return 1 if the right light has reached its period, or 0 if it has not */
    default: if(leftLimit <= lightIteration) {return 1;} else {return 0;} /* If the left light is green, return 1 if it has reached its period, or 0 if it has not*/
  }
}

char randomCheck(int randomChance) /* Returns 0 or 1 based which a chance of returning 1 based on the inputted number */
{
  if(randomChance <= (rand()%100)) /* Check if a random number from 0 to 100 is greater than the number */
  {
    return 0; /* Return 0 if it is greater */
  }
  else
  {
    return 1; /* Return 1 if it isn't */
  }
}

/* Simulation functions */
void runOneSimulation(STATS *leftTotals, STATS *rightTotals, int leftChance, int leftLimit, int rightChance, int rightLimit)
{
  char leftLightGreen = 0; /* The traffic lights are represented as a boolean. */
  QUEUE* leftQueue = NULL; /* Create two empty queues */
  QUEUE* rightQueue = NULL;
  STATS* leftStats = createStats(); /* Create two stat sets for each queue*/
  STATS* rightStats = createStats();
  int iteration = 0; /* Set the iteration and current light period to 0 */
  int lightPeriod = 0;
  while((iteration < 500) || !(leftQueue == NULL && rightQueue == NULL)) /* Whilst under 500 iterations have been done, or both queues are not empty */
  {
    if(lightCheck(lightPeriod, leftLightGreen, leftLimit, rightLimit)) /* Check if the lights are changing */
    {
      lightPeriod = 0;
      switch(leftLightGreen) /* If they are */
      {
         case 0: leftLightGreen = 1; break; /* Switch the left light to red if is green */
         default:  leftLightGreen = 0; /* Switch the left light to green if it is red */
      }
    }
    else
    {
      if(randomCheck(leftChance) && iteration < 500) /* Check if a car is arriving on the left */
      {
        enqueue(&leftQueue, 0);
      }
      if(randomCheck(rightChance) && iteration < 500) /*Check if a car is arriving on the right */
      {
        enqueue(&rightQueue, 0);
      }
      switch(leftLightGreen) /* Depending on whether the left light is green*/
      {
        case 0: if(countCars(rightQueue, 0) != 0) /* If it is not green, and the right queue is not empty */
                {
                  addToStats(rightStats, dequeue(&rightQueue)); /* Dequeue the car at the front of the right queue, and add its information to the right statistics */
                }
                break;
        default: if(countCars(leftQueue, 0) != 0) /* Otherwise, do the same with the left queue and stats */
                 {
                   addToStats(leftStats, dequeue(&leftQueue));
                 }
      }
      lightPeriod++;
    }
    if(iteration >= 500) /* If cars are no longer arriving */
    { /* Check if the queues are not empty */
      if(countCars(leftQueue, 0) != 0) /* Add to the clear times if the queues are not empty */
      {
        leftStats->clearTime++; /* Add one to the value of the clear times of the stats */
      }
      if(countCars(rightQueue, 0) != 0) /* Repeat for the right hand side */
      {
        rightStats->clearTime++;
      }
    }
    addToWaits(leftQueue);  /* Add weights to both sides */
    addToWaits(rightQueue);
    iteration++;
  }
  addToTotals(leftStats, leftTotals); /* Add the statistics from this simulation to the total values of all simulations */
  addToTotals(rightStats, rightTotals);
  freeStats(leftStats); /* Free the memory allocated to the stats */
  freeStats(rightStats);
  return;
}


