#include "../core/types.hpp"
#include "../core/engine.hpp"
#include "translator.hpp"

struct bufferView{
    BoardState* gameBuffer;
    int8_t* statesBuffer;
    int8_t* actionsBuffer;
    int8_t* rewardsBuffer;

    bufferView() = default;

    void allocate(int size, int8_t* states, int8_t* actions, int8_t* rewards) {
        gameBuffer = new BoardState[size];
        statesBuffer = states;
        actionsBuffer = actions;
        rewardsBuffer = rewards;
    }

    ~bufferView(){
        delete[] gameBuffer;
    }

    bufferView(const bufferView&) = delete;
    bufferView& operator=(const bufferView&) = delete;
};

class Arena {
    private:
        int games;

        bufferView batches[2];

    public:
        Arena(int size, int8_t* s0, int8_t* s1, int8_t* a0, int8_t* a1, int8_t* r0, int8_t* r1);
        ~Arena() {}

        void reset();
        void step(int batch);
        int get_buffer_size() {return games/2;}
};