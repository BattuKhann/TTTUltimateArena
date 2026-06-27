#include "arena.hpp"

Arena::Arena(int N){
    games = N;
    currBuffer = 1;
    int bufferSize = games/2;

    BoardState* b1 = new BoardState[bufferSize];
    BoardState* b2 = new BoardState[bufferSize];

    float* o1 = new float[bufferSize * 270];
    float* o2 = new float[bufferSize * 270];

    int* i1 = new int[bufferSize];
    int* i2 = new int[bufferSize];

    batches[0] = bufferView{b1, o1, i1};
    batches[1] = bufferView{b2, o2, i2};
}

Arena::~Arena(){
    delete[] batches[0].gameBuffer;
    delete[] batches[0].outBuffer;
    delete[] batches[0].inBuffer;

    delete[] batches[1].gameBuffer;
    delete[] batches[1].outBuffer;
    delete[] batches[1].inBuffer;
}

void Arena::step(){
    #pragma omp parallel for
    for(int i = 0; i < games/2; i++){
        move(batches[currBuffer].gameBuffer[i], batches[currBuffer].inBuffer[i]);
        writeStateVector(batches[currBuffer].gameBuffer[i], batches[currBuffer].outBuffer + (270*i));
    }
}