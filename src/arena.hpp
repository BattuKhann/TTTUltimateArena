#include "core/types.hpp"
#include "core/engine.hpp"
#include "RL/translator.hpp"
#include <omp.h>

struct bufferView{
    BoardState* gameBuffer;
    float* outBuffer;
    int* inBuffer;
};

class Arena {
    private:
        int games;
        int currBuffer;

        bufferView batches[2];

    public:
        Arena(int N);
        ~Arena();

        void reset();
        void step();
};