#ifndef WAVE_NODE_H_
#define WAVE_NODE_H_

#include "gloo/SceneNode.hpp"

#include <map>

#include "IntegratorBase.hpp"
#include "gloo/VertexObject.hpp"
#include "IntegratorType.hpp"
#include "gloo/cameras/SimplifiedArcBallCameraNode.hpp"
#include "gloo/cameras/Ray.hpp"
#include "WaveSystem.hpp"

namespace GLOO {
	class WaveNode : public SceneNode {
		public:
			WaveNode(float amplitude, 
                     float angular_freq,
                     float wavenumber_x,
                     float wavenumber_z,
                     SimplifiedArcBallCameraNode* camera_node);
			glm::vec2 UpdateSigmas();
			void Update(double delta_time) override;
			void ProjectQuads();
		private: 
			// FOR TESTING
			float amplitude_;
			float angular_freq_;
			float wavenumber_x_;
			float wavenumber_z_;
			// -

			std::shared_ptr<WaveSystem> system_;
			std::shared_ptr<ParticleState> state_;
			float t_;

			// mesh info
			std::shared_ptr<VertexObject> wave_mesh_;
			std::map<int, std::vector<int>> points_to_triangles_;
			SceneNode* wave_node_;

			// camera info
			SimplifiedArcBallCameraNode* camera_node_;
			glm::vec3 prior_position_;
			float prior_angle_;

			const int GRID_LENGTH = 50;
			const float GRID_CUT = 2.0f;
			const float T_MAX = 2.0f;

			const glm::vec3 WIND_DIR = glm::vec3(0.0f, 0.707f, 0.707f); // average wave direction TODO fix? 

			const glm::vec3 POS_X_UNIT = glm::vec3(1.0f, 0.0f, 0.0f);
			const glm::vec3 POS_Z_UNIT = glm::vec3(0.0f, 0.0f, 1.0f);
			
	};
}
#endif