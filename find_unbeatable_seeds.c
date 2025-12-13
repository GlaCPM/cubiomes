// check the biome at a block position
#include "generator.h"
#include <inttypes.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>

int mc = MC_1_16_1;

FILE *totalfile;
uint64_t totalFound = 0;

int threadCount = 12;

FILE *logfile;
int lastSeed = 0;

pthread_t logThread;
pthread_t totalThread;

int saving = 2;

void* saveSeed(void* seed) {
    logfile = fopen("seeds.txt", "a");
    fprintf(logfile, "%" PRId64 "\n", (uint64_t) seed);
    fclose(logfile);
    saving++;
}

void* saveTotal(void* total) {
    totalfile = fopen("total.txt", "w");
    fprintf(totalfile, "%" PRId64, (uint64_t) total);
    fclose(totalfile);
    saving++;
}

void* runThread(void* threadNumber) {
    // terrain is determined by the lower 2^48 bits
    for (uint64_t seed = (uint64_t) threadNumber + lastSeed + 1; seed < 281474976710657; seed = seed+threadCount) {
        int height = getEndSurfaceHeight(mc, seed);

        if (height == 0) {
	    // prevent race conditions
	    while (saving != 2) {
	        continue;
	    }
	    saving = 0;
            totalFound += 1;
            pthread_create(&totalThread, NULL, saveTotal, (void*) totalFound);
            pthread_create(&logThread, NULL, saveSeed, (void*) seed);
            float_t percent = ((float_t) seed)/2814749767106.56;
            printf("Total Found: %" PRId64 " | Seed: %" PRId64 ", height: %d | %f%%\n", (uint64_t) totalFound, (uint64_t) seed, height, percent);
        }
    }
}

int main() {
    //get the last seed in the list
    char lastSeedString[32];
    logfile = fopen("seeds.txt", "r");
    if (logfile != NULL) {
        while (fgets(lastSeedString, 32, logfile)) {
            continue;
        }
        lastSeed = atoi(lastSeedString);
        fclose(logfile);
    }

    char totalString[8];
    totalfile = fopen("total.txt", "r");
    if (totalfile != NULL) {
        fgets(totalString, 8, totalfile);
        totalFound = atoi(totalString);
        fclose(totalfile);
    }

    pthread_t thread0, thread1, thread2, thread3, thread4, thread5, thread6, thread7, thread8, thread9, thread10, thread11;

    pthread_create(&thread0, NULL, runThread, (void*) 0);
    pthread_create(&thread1, NULL, runThread, (void*) 1);
    pthread_create(&thread2, NULL, runThread, (void*) 2);
    pthread_create(&thread3, NULL, runThread, (void*) 3);
    pthread_create(&thread4, NULL, runThread, (void*) 4);
    pthread_create(&thread5, NULL, runThread, (void*) 5);
    pthread_create(&thread6, NULL, runThread, (void*) 6);
    pthread_create(&thread7, NULL, runThread, (void*) 7);
    pthread_create(&thread8, NULL, runThread, (void*) 8);
    pthread_create(&thread9, NULL, runThread, (void*) 9);
    pthread_create(&thread10, NULL, runThread, (void*) 10);
    pthread_create(&thread11, NULL, runThread, (void*) 11);

    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);
    pthread_join(thread6, NULL);
    pthread_join(thread7, NULL);
    pthread_join(thread8, NULL);
    pthread_join(thread9, NULL);
    pthread_join(thread10, NULL);
    pthread_join(thread11, NULL);

    pthread_join(totalThread, NULL);
    pthread_join(logThread, NULL);

    return 0;
}
