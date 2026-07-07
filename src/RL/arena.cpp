#include <cstring>
#include <omp.h>
#include "arena.hpp"

Arena::Arena(int size, int8_t* s0, int8_t* s1, int8_t* a0, int8_t* a1, int8_t* r0, int8_t* r1){
    games = size;

    if (games % 2 == 1) games++;
    
    batches[0].allocate(games/2, s0, a0, r0);
    batches[1].allocate(games/2, s1, a1, r1);

    reset();
}

void Arena::reset(){
    int batchsize = games / 2;

    for (int b = 0; b < 2; b++){
        memset(batches[b].actionsBuffer, 0, sizeof(uint8_t) * batchsize);
        memset(batches[b].rewardsBuffer, 0, sizeof(uint8_t) * batchsize);

        //#pragma omp parallel for schedule(static) num_threads(12)
        for (int i = 0; i < batchsize; i++){
            batches[b].gameBuffer[i] = BoardState();
            writeStateVector(batches[b].gameBuffer[i], batches[b].statesBuffer + (270*i));
        }
        
    }
}

void Arena::step(int batch){
    #pragma omp parallel for schedule(static) num_threads(12)
    for(int i = 0; i < games/2; i++){
        int8_t reward = move(batches[batch].gameBuffer[i], batches[batch].actionsBuffer[i]);;
        batches[batch].rewardsBuffer[i] = reward;
        if (reward == 10){
            batches[batch].gameBuffer[i] = BoardState();
        }
        writeStateVector(batches[batch].gameBuffer[i], batches[batch].statesBuffer + (270*i));
    }
}