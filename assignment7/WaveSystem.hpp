#ifndef WAVE_SYSTEM_H_
#define WAVE_SYSTEM_H_

#include <math.h>       /* sin */

#include "ParticleSystemBase.hpp"

namespace GLOO {
class WaveSystem : public ParticleSystemBase {
public:
	// state: initial resting state of particle 
	// time: TODO 
  ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const override {
    // std::cout << "WaveSystem ComputeTimeDerivative start" << std::endl;

  	ParticleState new_state;
  	std::vector<glm::vec3> new_positions;

  	// for each position
  	for (int i = 0; i < state.positions.size(); i++) {
  		glm::vec3 new_position = state.positions[i];
  		// for each wave, add contribution 
  		for (int j = 0; j < wavenumbers.size(); j++) {
  			float inner = angfreqs.at(j) * (time + time_offsets.at(j)) - glm::dot(wavenumbers.at(j), glm::vec2(state.positions[i][0], state.positions[i][2]));
  			new_position[0] += amplitudes.at(j) * wavenumbers.at(j)[0] / glm::length(wavenumbers.at(j)) * sin(inner); 
  			new_position[2] += amplitudes.at(j) * wavenumbers.at(j)[1] / glm::length(wavenumbers.at(j)) * sin(inner); 
  			new_position[1] += amplitudes.at(j) * cos(inner); // altitude 
  		}
  		new_positions.push_back(new_position);
  	}
  	new_state.positions = new_positions;
  	new_state.velocities = state.velocities; // unused

    // std::cout << "WaveSystem ComputeTimeDerivative end" << std::endl;

  	return new_state;
  }

  void AddWave(glm::vec2 wavenumber, float amplitude, float angfreq, float time_offset) {
    // std::cout << "WaveSystem addWave start" << std::endl;
  	wavenumbers.push_back(wavenumber);
  	amplitudes.push_back(amplitude);
  	angfreqs.push_back(angfreq);
    time_offsets.push_back(time_offset);
    // std::cout << "WaveSystem addWave end" << std::endl;
  }

private:
	std::vector<glm::vec2> wavenumbers; // (x, z)
	std::vector<float> amplitudes;
	std::vector<float> angfreqs; 
  std::vector<float> time_offsets; 
};
}  // namespace GLOO

#endif
