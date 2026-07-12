#include <cstring>
#include <omp.h>
#include <iostream>
#include "arena.hpp"

Arena::Arena(int size, int8_t* s0, int8_t* s1, int8_t* a0, int8_t* a1){
    games = size;

    if (games % 2 == 1) games++;
    
    batches[0].allocate(games/2, s0, a0);
    batches[1].allocate(games/2, s1, a1);
    trajectoryBuffer = new Transition[20'000'000];

    reset();
}

void Arena::reset(){
    int batchsize = games / 2;

    for (int b = 0; b < 2; b++){
        memset(batches[b].actionsBuffer, 0, sizeof(uint8_t) * batchsize);
        memset(batches[b].transitionsBuffer, 0, sizeof(gameTransitionBuffer) * batchsize);

        #pragma omp parallel for schedule(static) num_threads(12)
        for (int i = 0; i < batchsize; i++){
            batches[b].gameBuffer[i] = BoardState();
            writeStateVector(batches[b].gameBuffer[i], batches[b].statesBuffer + (270*i));
        }
        
    }
}

void Arena::step(int batch){
    bufferView &currbatch = batches[batch];

    #pragma omp parallel for schedule(static) num_threads(12)
    for(int i = 0; i < games/2; i++){
        addTransition(currbatch.transitionsBuffer[i], currbatch.gameBuffer[i], currbatch.actionsBuffer[i], 0);
        uint8_t reward = move(currbatch.gameBuffer[i], currbatch.actionsBuffer[i]);
        if(reward){ 
            applyRewards(currbatch.transitionsBuffer[i], (_Float16)(reward & 1), 0.99f16);
            emptyTransitionBuffer(trajectoryBuffer, &trajectoryIdx, &currbatch.transitionsBuffer[i]);
            currbatch.gameBuffer[i] = BoardState();
        }
        writeStateVector(currbatch.gameBuffer[i], currbatch.statesBuffer + (270*i));
    }
}

void Arena::flush_trajectory(){
    trajectoryIdx.store(0, std::memory_order_relaxed);
}

void Arena::sample_trajectory(int count, int8_t* states, int8_t* actions, _Float16* values){
    int max_valid = trajectoryIdx.load(std::memory_order_relaxed);
    if (max_valid == 0) return;

    #pragma omp parallel num_threads(12)
    {
        static thread_local Xoshiro256 rng((uint64_t)trajectoryBuffer + omp_get_thread_num());
        
        //Potential Optimization: Generate then sort random indicies before writing
        #pragma omp for schedule(static)
        for(int i = 0; i < count; i++){
            int idx = rng.next() % max_valid;
            writeStateVector(trajectoryBuffer[idx].state, states + (270*i));
            actions[i] = trajectoryBuffer[idx].action;
            values[i] = trajectoryBuffer[idx].value_target;
        }
    }
}