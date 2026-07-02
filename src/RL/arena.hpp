#include "../core/types.hpp"
#include "../core/engine.hpp"
#include "translator.hpp"
#include <omp.h>

struct bufferView{
    BoardState* gameBuffer;
    uint8_t* inBuffer;
    uint8_t* outBuffer;
    float* rewardBuffer;

    bufferView() = default;

    void allocate(int size) {
        gameBuffer = new BoardState[size];
        inBuffer = new uint8_t[size * 270];
        outBuffer = new uint8_t[size];
        rewardBuffer = new float[size];
    }

    ~bufferView(){
        delete[] gameBuffer;
        delete[] inBuffer;
        delete[] outBuffer;
        delete[] rewardBuffer;
    }

    bufferView(const bufferView&) = delete;
    bufferView& operator=(const bufferView&) = delete;
};

class Arena {
    private:
        int games;

        bufferView batches[2];

    public:
        Arena(int size);
        ~Arena() {}

        void step(int batch);
        int get_buffer_size() {return games/2;}
        uint8_t* get_in_ptr(int batch) {return batches[batch].inBuffer;}
        uint8_t* get_out_ptr(int batch) {return batches[batch].outBuffer;}
        float* get_reward_ptr(int batch) {return batches[batch].rewardBuffer;}
};