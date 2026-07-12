#include "../core/types.hpp"
#include "../core/engine.hpp"
#include "translator.hpp"

struct bufferView{
    BoardState* gameBuffer;
    int8_t* statesBuffer;
    int8_t* actionsBuffer;
    gameTransitionBuffer* transitionsBuffer;
    

    bufferView() = default;

    void allocate(int size, int8_t* states, int8_t* actions) {
        gameBuffer = new BoardState[size];
        transitionsBuffer = new gameTransitionBuffer[size];
        statesBuffer = states;
        actionsBuffer = actions;
    }

    ~bufferView(){
        delete[] gameBuffer;
        delete[] transitionsBuffer;
    }

    bufferView(const bufferView&) = delete;
    bufferView& operator=(const bufferView&) = delete;
};

class Arena {
    private:
        int games;

        bufferView batches[2];
        Transition* trajectoryBuffer;
        std::atomic<int> trajectoryIdx {0};

    public:
        Arena(int size, int8_t* s0, int8_t* s1, int8_t* a0, int8_t* a1);
        ~Arena() {delete[] trajectoryBuffer;}

        void reset();
        void step(int batch);
        void flush_trajectory();
        void sample_trajectory(int count, int8_t* states, int8_t* actions, _Float16* values);
        int get_buffer_size() {return games/2;}
};