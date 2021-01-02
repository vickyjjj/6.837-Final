#ifndef SIMULATION_APP_H_
#define SIMULATION_APP_H_

#include "gloo/Application.hpp"

#include "IntegratorType.hpp"

namespace GLOO {
class SimulationApp : public Application {
 public:
  SimulationApp(const std::string& app_name,
                glm::ivec2 window_size,
                float amplitude, 
				float angular_freq,
				float wavenumber_x,
				float wavenumber_z);
  void SetupScene() override;

 private:
	float amplitude_;
	float angular_freq_;
	float wavenumber_x_;
	float wavenumber_z_;
};
}  // namespace GLOO

#endif
