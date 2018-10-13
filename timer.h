#pragma once

#include <chrono>

class timer {
  public:
    void start()
    {
      start_time = std::chrono::high_resolution_clock::now();
    }

    double stop()
    {
      auto stop_time = std::chrono::high_resolution_clock::now();
      return double(std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time).count());
    }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};
