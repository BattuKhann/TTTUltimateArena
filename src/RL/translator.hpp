#pragma once

#include "../core/types.hpp"
#include "../core/engine.hpp"
#include <atomic>
#include <cstring>

struct Transition{
    BoardState state;
    int8_t action = 0;
    _Float16 value_target = 0.0f16;

    Transition() {}
    Transition(const BoardState& bState, int8_t act, _Float16 value) : state(bState), action(act), value_target(value) {}
};

struct gameTransitionBuffer{
    int8_t count = 0;
    Transition buff[81];

    gameTransitionBuffer() {count = 0;}
};

//Basic PRNG
struct Xoshiro256{
    uint64_t s[4];
    uint64_t splitmix;

    Xoshiro256(uint64_t seed){
        splitmix = seed;
    
        s[0] = splitmix64_next();
        s[1] = splitmix64_next();
        s[2] = splitmix64_next();
        s[3] = splitmix64_next();
    }

    inline uint64_t splitmix64_next() {
        splitmix += 0x9e3779b97f4a7c15ULL;
        uint64_t z = splitmix;
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }

    inline uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    inline uint64_t next() {
        const uint64_t result = rotl(s[0] + s[3], 23) + s[0];

        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result;
    }
};

inline void addTransition(gameTransitionBuffer& gameBuff, const BoardState& state, int8_t action, int8_t reward){
    if(gameBuff.count < 81){
        gameBuff.buff[gameBuff.count] = Transition(state, action, reward);
        gameBuff.count++;
    }
}

inline void applyRewards(gameTransitionBuffer& gameBuff, _Float16 reward, _Float16 gamma){
    gameBuff.buff[gameBuff.count - 1].value_target = reward;
    _Float16 prevReward = reward;
    //Infinite Loop Impossible: Game cannot end in less than 2 moves
    for(int i = gameBuff.count - 2; i >= 0; i--){
        prevReward = (-gamma) * prevReward;
        gameBuff.buff[i].value_target = prevReward;
    }
}

inline void emptyTransitionBuffer(Transition* trajBuff, std::atomic<int>* idx, gameTransitionBuffer* gameBuff){
    int transCount = gameBuff->count;
    gameBuff->count = 0;
    int startidx = (*idx).fetch_add(transCount);
    if (startidx + transCount <= 20'000'000) {
        memcpy(trajBuff + startidx, gameBuff->buff, sizeof(Transition)*transCount);
    }
}

void writeStateVector(const BoardState& state, int8_t* outBuffer);