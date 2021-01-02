#include <iostream>
#include <chrono>
#include <string>
#include <cstdio>
#include <stdexcept>

#include "SimulationApp.hpp"
#include "IntegratorType.hpp"

using namespace GLOO;

int main(int argc, char** argv) {
  if (argc != 5) {
    printf("Usage: %s <amplitude> <angular frequency> <wavenumber x> <wavenumber z>\n", argv[0]);
    return -1;
  }

  float amplitude = std::stof(argv[1]);
  float angfreq = std::stof(argv[2]);
  float wavenumber_x = std::stof(argv[3]);
  float wavenumber_z = std::stof(argv[4]);

  std::unique_ptr<SimulationApp> app = make_unique<SimulationApp>(
      "Assignment3", glm::ivec2(1440, 900), amplitude, angfreq, wavenumber_x, wavenumber_z);

  app->SetupScene();

  using Clock = std::chrono::high_resolution_clock;
  using TimePoint =
      std::chrono::time_point<Clock, std::chrono::duration<double>>;
  TimePoint last_tick_time = Clock::now();
  TimePoint start_tick_time = last_tick_time;
  while (!app->IsFinished()) {
    TimePoint current_tick_time = Clock::now();
    double delta_time = (current_tick_time - last_tick_time).count();
    double total_elapsed_time = (current_tick_time - start_tick_time).count();
    app->Tick(delta_time, total_elapsed_time);
    last_tick_time = current_tick_time;
  }
  return 0;
}
