// check the biome at a block position
#include "generator.h"
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>

int mc = MC_1_16_1;

FILE *totalfile;
uint64_t totalFound = 0;

int threadCount = 4;

FILE *logfile;
int lastSeed = 0;

pthread_t logThread;
pthread_t totalThread;

void* saveSeed(void* seed) {
    logfile = fopen("seeds.txt", "a");
    fprintf(logfile, "%" PRId64 "\n", (uint64_t) seed);
    fclose(logfile);
}

void* saveTotal(void* total) {
    totalfile = fopen("total.txt", "w");
    fprintf(totalfile, "%" PRId64, (uint64_t) total);
    fclose(totalfile);
}

void* runThread(void* threadNumber) {
    // terrain is determined by the lower 2^48 bits
    for (uint64_t seed = (uint64_t) threadNumber + lastSeed + 1; seed < 281474976710657; seed = seed+threadCount) {
        int height = getEndSurfaceHeight(mc, seed, 0, 0);

        if (height == 0) {
            totalFound += 1;
            pthread_create(&totalThread, NULL, saveTotal, (void*) totalFound);
            pthread_create(&logThread, NULL, saveSeed, (void*) seed);
            printf("Total Found: %" PRId64 "   Seed: %" PRId64 ", height: %d\n", (uint64_t) totalFound, (uint64_t) seed, height);
        }

        //printf("Total Found: %" PRId64 "   Seed: %" PRId64 ", height: %d\n", (uint64_t) totalFound, (uint64_t) seed, height);
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

    pthread_t thread0, thread1, thread2, thread3;

    pthread_create(&thread0, NULL, runThread, (void*) 0);
    pthread_create(&thread1, NULL, runThread, (void*) 1);
    pthread_create(&thread2, NULL, runThread, (void*) 2);
    pthread_create(&thread3, NULL, runThread, (void*) 3);

    pthread_join(thread0, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    return 0;
}
