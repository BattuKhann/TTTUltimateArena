#include "arena.hpp"

Arena::Arena(int size){
    games = size;

    if (games % 2 == 1) games++;
    
    batches[0].allocate(games/2);
    batches[1].allocate(games/2);
}

void Arena::step(int batch){
    #pragma omp parallel for schedule(static) num_threads(6)
    for(int i = 0; i < games/2; i++){
        float reward = move(batches[batch].gameBuffer[i], batches[batch].outBuffer[i]);;
        batches[batch].rewardBuffer[i] = reward;
        if (reward == 1.0f){
            batches[batch].gameBuffer[i] = BoardState();
        }
        writeStateVector(batches[batch].gameBuffer[i], batches[batch].inBuffer + (270*i));
    }
}