#pragma once
#include <cstdint>
#include <algorithm>
#include <random>

namespace Util
{

    template <typename T>
    concept RandomConcept = std::is_same_v<T, int> || std::is_same_v<T, float>;

    inline uint8_t getHeadingFromTarget(int sx, int sy, int tx, int ty)
    {
        static constexpr uint8_t kTable[3][3] = {
            { 5, 6, 7 },  // dx = -1
            { 4, 0, 0 },  // dx =  0  (centre = no movement)
            { 3, 2, 1 },  // dx = +1
        };
        int dx = std::clamp(tx - sx, -1, 1);
        int dy = std::clamp(ty - sy, -1, 1);
        return kTable[dx + 1][dy + 1];
    }

    template<RandomConcept T>
    inline T getRandomValue(T min, T max) {
        static std::mt19937 gen{ std::random_device{}() };
        if constexpr (std::is_same_v<T, float>) {
            return std::uniform_real_distribution<float>{ min, max }(gen);
        } else {
            return std::uniform_int_distribution<int>{ min, max }(gen);
        }
    }

} // namespace Util
