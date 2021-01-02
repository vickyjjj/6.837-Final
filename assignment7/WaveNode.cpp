#include "WaveNode.hpp"

#include "gloo/shaders/OceanShader.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"

#include "ParticleState.hpp"

#include "gloo/InputManager.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {
	WaveNode::WaveNode(float amplitude, 
                             float angular_freq,
                             float wavenumber_x,
                             float wavenumber_z,
                             SimplifiedArcBallCameraNode* camera_node)
	: amplitude_(amplitude),
      angular_freq_(angular_freq),
      wavenumber_x_(wavenumber_x),
      wavenumber_z_(wavenumber_z),
      camera_node_(camera_node) {
      	// std::cout << "WaveNode constructor start" << std::endl;
		wave_mesh_ = std::make_shared<VertexObject>();
		glm::vec3 blue_color(0.1f, 0.2f, 0.4f);
		glm::vec3 yellow_color(0.1f, 0.1f, 0.1f);

		// Create particle state
		state_ = std::make_shared<ParticleState>();

		// Make mesh
		ProjectQuads();

		// Create particle system 
		system_ = std::make_shared<WaveSystem>();

		// std::cout << "WaveNode create waves start" << std::endl;		
		// Create waves
		// system_->AddWave(glm::vec2(wavenumber_x_, wavenumber_z_), amplitude_, angular_freq_, 0.0f);
		system_->AddWave(glm::vec2(0.5f, 0.5f), 1.0f, 1.0f, 2.0f); // large fast wave
		system_->AddWave(glm::vec2(0.5f, 0.5f), 0.5f, 0.5f, 0.0f); // medium medium wave
		system_->AddWave(glm::vec2(0.1f, 0.1f), 1.0f, 0.2f, 5.0f); // very slow undulating motion
		system_->AddWave(glm::vec2(0.1f, 5.0f), 0.1f, 1.5f, 0.0f); // very short waves
		system_->AddWave(glm::vec2(0.1f, 5.0f), 0.2f, 1.4f, 0.5f); // very short waves slightly angled
		// system_->AddWave(glm::vec2(0.5f, 0.1f), 1.0f, 1.0f, 2.0f); // large fast wave vertical
		// TODO fix, this should be adjusted with the pixel size 
		// glm::vec2 sigma_squared = UpdateSigmas(); // will be added to the ocean shader

		// Populate indices of grid mesh
		// std::cout << "WaveNode fill indices begin" << std::endl;
		auto indices = make_unique<IndexArray>();
		for (int i = 0; i < GRID_LENGTH; i++) {
	  		for (int j = 0; j < GRID_LENGTH; j++) {
	  			int currentPosition = i * GRID_LENGTH + j;
	  			// std::cout << "WaveNode fill indices for: " << i << ", " << j << ", " << currentPosition << std::endl;
	  			// add upper triangle
				if (i != 0 && j != GRID_LENGTH-1) {
					indices->push_back(currentPosition);
					indices->push_back(currentPosition+1);
					indices->push_back(currentPosition-GRID_LENGTH+1);
					// first point
					if (points_to_triangles_.find(currentPosition) == points_to_triangles_.end()) {
				      std::vector<int> vec_to_add;
				      vec_to_add.push_back(currentPosition+1);
				      vec_to_add.push_back(currentPosition-GRID_LENGTH+1);
				      points_to_triangles_[currentPosition] = vec_to_add;
				    } else {
				      points_to_triangles_[currentPosition].push_back(currentPosition+1);
				      points_to_triangles_[currentPosition].push_back(currentPosition-GRID_LENGTH+1);
				    }
				    // second point
				    if (points_to_triangles_.find(currentPosition+1) == points_to_triangles_.end()) {
				      std::vector<int> vec_to_add;
				      vec_to_add.push_back(currentPosition-GRID_LENGTH+1);
				      vec_to_add.push_back(currentPosition);
				      points_to_triangles_[currentPosition+1] = vec_to_add;
				    } else {
				      points_to_triangles_[currentPosition+1].push_back(currentPosition-GRID_LENGTH+1);
				      points_to_triangles_[currentPosition+1].push_back(currentPosition);
				    }
				    // third point 
				    if (points_to_triangles_.find(currentPosition-GRID_LENGTH+1) == points_to_triangles_.end()) {
				      std::vector<int> vec_to_add;
				      vec_to_add.push_back(currentPosition);
				      vec_to_add.push_back(currentPosition+1);
				      points_to_triangles_[currentPosition-GRID_LENGTH+1] = vec_to_add;
				    } else {
				      points_to_triangles_[currentPosition-GRID_LENGTH+1].push_back(currentPosition);
				      points_to_triangles_[currentPosition-GRID_LENGTH+1].push_back(currentPosition+1);
				    }
				}
				// add lower triangle
				if (i != GRID_LENGTH-1 && j != GRID_LENGTH-1) {
					indices->push_back(currentPosition);
					indices->push_back(currentPosition+GRID_LENGTH);
					indices->push_back(currentPosition+1);
					// first point
					if (points_to_triangles_.find(currentPosition) == points_to_triangles_.end()) {
				      std::vector<int> vec_to_add;
				      vec_to_add.push_back(currentPosition+GRID_LENGTH);
				      vec_to_add.push_back(currentPosition+1);
				      points_to_triangles_[currentPosition] = vec_to_add;
				    } else {
				      points_to_triangles_[currentPosition].push_back(currentPosition+GRID_LENGTH);
				      points_to_triangles_[currentPosition].push_back(currentPosition+1);
				    }
				    // second point
				    if (points_to_triangles_.find(currentPosition+1) == points_to_triangles_.end()) {
				      std::vector<int> vec_to_add;
				      vec_to_add.push_back(currentPosition);
				      vec_to_add.push_back(currentPosition+GRID_LENGTH);
				      points_to_triangles_[currentPosition+1] = vec_to_add;
				    } else {
				      points_to_triangles_[currentPosition+1].push_back(currentPosition);
				      points_to_triangles_[currentPosition+1].push_back(currentPosition+GRID_LENGTH);
				    }
				    // third point 
				    if (points_to_triangles_.find(currentPosition+GRID_LENGTH) == points_to_triangles_.end()) {
				      std::vector<int> vec_to_add;
				      vec_to_add.push_back(currentPosition+1);
				      vec_to_add.push_back(currentPosition);
				      points_to_triangles_[currentPosition+GRID_LENGTH] = vec_to_add;
				    } else {
				      points_to_triangles_[currentPosition+GRID_LENGTH].push_back(currentPosition+1);
				      points_to_triangles_[currentPosition+GRID_LENGTH].push_back(currentPosition);
				    }
				}
	  		}
	  	}
	  	// std::cout << "WaveNode add indices end" << std::endl;

	  	// Update grid mesh 
		wave_mesh_->UpdateIndices(std::move(indices));

		// std::cout << "WaveNode add grid mesh and scene node start" << std::endl;
		// Create grid mesh wave node 
		std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
		// shader->SetSigmas(sigma_squared); // from calculation under waves
		auto wave_node = make_unique<SceneNode>();
		wave_node->CreateComponent<ShadingComponent>(shader);
		wave_node->CreateComponent<RenderingComponent>(wave_mesh_);
		auto& rc = wave_node->CreateComponent<RenderingComponent>(wave_mesh_);
		rc.SetDrawMode(DrawMode::Triangles);
		wave_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
		MaterialComponent* materialComp = wave_node->GetComponentPtr<MaterialComponent>();
        materialComp->GetMaterial().SetAmbientColor(blue_color);
        materialComp->GetMaterial().SetDiffuseColor(blue_color);
        // materialComp->GetMaterial().SetSpecularColor(yellow_color);
        // materialComp->GetMaterial().SetShininess(0.5);
		wave_node_ = wave_node.get();
		this->AddChild(std::move(wave_node));
		// std::cout << "WaveNode add grid mesh and scene node end" << std::endl;

		// set starting time 
		t_ = 0.0f;

		// std::cout << "WaveNode create waves end" << std::endl;		

		// store original camera parameters to compare later 
		prior_position_ = camera_node_->GetTransform().GetPosition();
		prior_angle_ = camera_node_->GetTransform().GetRotation()[3];
		UpdateSigmas();
		// std::cout << "WaveNode constructor end" << std::endl;
	}

	glm::vec2 WaveNode::UpdateSigmas() {
		// TODO fix 
		glm::vec2 result = glm::vec2(0.0f, 0.0f);

		// wave 1
		glm::vec2 k = glm::vec2(0.5f, 0.5f);
		float w = 0.3f;
		float h = 0.5f;
		result += glm::vec2(pow(k[0], 2.0f), pow(k[1], 2.0f)) * ((1.0f - sqrt(1.0f - pow(glm::length(k), 2.0f) * pow(w, 2.0f) * pow(h, 2.0f))) / pow(glm::length(k), 2.0f));

		k = glm::vec2(0.1f, 0.1f);
		w = 0.7f;
		h = 1.0f;
		result += glm::vec2(pow(k[0], 2.0f), pow(k[1], 2.0f)) * ((1.0f - sqrt(1.0f - pow(glm::length(k), 2.0f) * pow(w, 2.0f) * pow(h, 2.0f))) / pow(glm::length(k), 2.0f));

		k = glm::vec2(0.1f, 5.0f);
		w = 0.3f;
		h = 0.1f;
		result += glm::vec2(pow(k[0], 2.0f), pow(k[1], 2.0f)) * ((1.0f - sqrt(1.0f - pow(glm::length(k), 2.0f) * pow(w, 2.0f) * pow(h, 2.0f))) / pow(glm::length(k), 2.0f));

		std::cout << "sigma results: " << glm::to_string(result) << std::endl;
		result = result * 100.0f;
		return result;
	}

	// Tries to use view and projection matrix, then generate ray between near and far points 
	void WaveNode::ProjectQuads() {
		// std::cout << "ProjectQuads start" << std::endl;

		// get camera info
		glm::ivec2 window_size = InputManager::GetInstance().GetWindowSize();
		CameraComponent* camComp = camera_node_->GetComponentPtr<CameraComponent>();
		camComp->SetAspectRatio(static_cast<float>(window_size.x) / static_cast<float>(window_size.y));
		auto projection_matrix = camComp->GetProjectionMatrix();
		auto view_matrix = camComp->GetViewMatrixWithDistance(5.0f);

		auto positions = make_unique<PositionArray>();
		auto normals = make_unique<NormalArray>();
		// auto tangent_xs = make_unique<TangentXArray>();
		// auto tangent_ys = make_unique<TangentYArray>();
		std::vector<glm::vec3> new_state_positions;

		// iterate through screen quads 
		int width_iter = (int) window_size[0] / (GRID_LENGTH-1);
		int height_iter = (int) window_size[1] / (GRID_LENGTH-1);
		// std::cout << "Window stuff: " << glm::to_string(window_size) << ", " << width_iter << ", " << height_iter << std::endl;
		for (int i = 0; i <= window_size[0]; i += width_iter) { // width
			for (int j = 0; j <= window_size[1]; j += height_iter) { // height 
				float x = ((float) i/window_size[0]) * 2.0f - 1.0f;
      			float y = -(((float) j/window_size[1]) * 2.0f - 1.0f);
      			// std::cout << "x: " << x << std::endl;
      			// std::cout << "y: " << y << std::endl;

      			// calculate near point 
      			glm::vec4 near_point = glm::vec4(x, y, -1.0f, 1.0f); 
      			near_point = glm::inverse(projection_matrix) * near_point;
      			glm::vec3 near_position = glm::vec3(near_point[0]/near_point[3], near_point[1]/near_point[3], near_point[2]/near_point[3]);
      			// std::cout << "near_point: " << glm::to_string(near_point) << std::endl; 
      			// std::cout << "near_position: " << glm::to_string(near_position) << std::endl;

      			// calculate near point 
      			glm::vec4 far_point = glm::vec4(x, y, 1.0f, 1.0f); 
      			far_point = glm::inverse(projection_matrix) * far_point;
      			glm::vec3 far_position = glm::vec3(far_point[0]/far_point[3], far_point[1]/far_point[3], far_point[2]/far_point[3]);
      			// std::cout << "far_point: " << glm::to_string(far_point) << std::endl; 
      			// std::cout << "far_position: " << glm::to_string(far_position) << std::endl;

      			// calculate ray using projection  
      			glm::vec3 direction = near_position - far_position;
      			// std::cout << "direction unnormalized: " << glm::to_string(direction) << std::endl;
      			direction = glm::normalize(direction);
      			// std::cout << "direction normalized: " << glm::to_string(direction) << std::endl;
      			Ray ray(near_position, direction);
      			ray.ApplyTransform(glm::inverse(view_matrix));
      			// std::cout << "final ray: " << glm::to_string(ray.GetOrigin()) << ", " << glm::to_string(ray.GetDirection()) << std::endl;

				// calculate intersection of ray with y = 0 plane
				float t = (0.0f - ray.GetOrigin()[1]) / ray.GetDirection()[1];
				glm::vec3 new_position = ray.At(t);
				// std::cout << "t: " << t << std::endl;
				// std::cout << "new_position: " << glm::to_string(new_position) << std::endl;

				// update mesh and state positions
				positions->push_back(new_position);
				new_state_positions.push_back(new_position);
				normals->push_back(glm::vec3(0.0f, 1.0f, 0.0f));
				// tangent_xs->push_back(glm::vec3(1.0f, 0.0f, 0.0f));
				// tangent_ys->push_back(glm::vec3(0.0f, 0.0f, 1.0f));
			}
		}
		// update mesh and state 
		state_->positions = new_state_positions;
		state_->velocities = new_state_positions; // TODO fix: velocities never actually used 
		wave_mesh_->UpdatePositions(std::move(positions));
		wave_mesh_->UpdateNormals(std::move(normals));
		// wave_mesh_->UpdateTangentXs(std::move(tangent_xs));
		// wave_mesh_->UpdateTangentYs(std::move(tangent_ys));
		// std::cout << "ProjectQuads done: " << std::endl;
	}

	void WaveNode::Update(double delta_time) {
		// std::cout << "WaveNode update start" << std::endl;
		// calculate new mesh positions if camera has shifted 
		if (prior_position_ != camera_node_->GetTransform().GetPosition() || prior_angle_ != camera_node_->GetTransform().GetRotation()[3]) {
			ProjectQuads();
			prior_position_ = camera_node_->GetTransform().GetPosition();
			prior_angle_ = camera_node_->GetTransform().GetRotation()[3];
		}
		// calculate new positions
		t_ += delta_time;
		ParticleState new_state = system_->ComputeTimeDerivative(*state_.get(), t_);

		// update mesh positions
		// std::cout << "WaveNode positions change start" << std::endl;
		auto positions = make_unique<PositionArray>();
		for (int i = 0; i < new_state.positions.size(); i++) {
			// TODO faster way to do this?
			positions->push_back(new_state.positions[i]);
		}
		// std::cout << "WaveNode positions change end" << std::endl;
		
		// update mesh normals
		auto normals = make_unique<NormalArray>();
		// auto tangent_xs = make_unique<TangentXArray>();
		// auto tangent_ys = make_unique<TangentYArray>();
		for (int i = 0; i < GRID_LENGTH; i++) {
			for (int k = 0; k < GRID_LENGTH; k++) {
				int currentPosition = i * GRID_LENGTH + k;
				std::vector<glm::vec3> norms;
				// std::vector<glm::vec3> tan_x;
				// std::vector<glm::vec3> tan_z;
			    std::vector<float> areas;
			    float total_area = 0.0f;
			    // iterate through all adjacent triangles 
			    for (int j = 0; j < points_to_triangles_[currentPosition].size()-1; j+=2) {
			      // calculate normal
			      glm::vec3 side_a = positions->at(points_to_triangles_[currentPosition].at(j)) - positions->at(points_to_triangles_[currentPosition].at(j+1));
			      glm::vec3 side_b = positions->at(currentPosition) - positions->at(points_to_triangles_[currentPosition].at(j));
			      norms.push_back(glm::normalize(glm::cross(side_a, side_b)));
			      // calculate tangents
			      // project side onto positive x, z unit vectors
			      // glm::vec3 side_x = glm::normalize(glm::dot(side_a, POS_X_UNIT) / glm::length(POS_X_UNIT));
			      // glm::vec3 side_z = glm::normalize(glm::dot(side_a, POS_Z_UNIT) / glm::length(POS_Z_UNIT));
			      // glm::vec3 side_z_2 = glm::cross(side_x, glm::normalize(glm::cross(side_a, side_b)));
			      // std::cout << "tangents check: " << side_x << "; " << side_z << "; " << side_z_2 << std::endl;
			      // tan_x.push_back(side_x);
			      // tan_z.push_back(side_z);
			      // tan_x.push_back(glm::normalize(side_a));
			      // tan_z.push_back(glm::cross(glm::normalize(side_a), glm::normalize(glm::cross(side_a, side_b)))); // cross between other tangent and normal
			      // tan_z.push_back(glm::normalize(side_b));
			      // calculate area of triangle face 
			      float a = glm::length(side_a);
			      float b = glm::length(side_b);
			      float c = glm::distance(positions->at(currentPosition), positions->at(points_to_triangles_[currentPosition].at(j+1)));
			      float p = (a + b + c) / 2.0f;
			      areas.push_back(sqrt(p * (p-a) * (p-b) * (p-c)));
			      total_area += sqrt(p * (p-a) * (p-b) * (p-c));
			    }
			    glm::vec3 normal_result(0.0f, 0.0f, 0.0f);
			    // glm::vec3 tangent_x_result(0.0f, 0.0f, 0.0f);
			    // glm::vec3 tangent_z_result(0.0f, 0.0f, 0.0f);
			    // final result is sum of proportion of area * normal
			    for (int j = 0; j < norms.size(); j++) {
			      normal_result = normal_result + norms.at(j) * (areas.at(j)/total_area);
			      // tangent_x_result = tangent_x_result + tan_x.at(j) * (areas.at(j)/total_area);
			      // tangent_z_result = tangent_z_result + tan_z.at(j) * (areas.at(j)/total_area);
			    }
			    // std::cout << "Normal: " << glm::to_string(result) << glm::length(result) << std::endl;
			    normals->push_back(glm::normalize(normal_result));
			    // tangent_xs->push_back(glm::normalize(tangent_x_result));
			    // tangent_ys->push_back(glm::normalize(tangent_z_result));
			    // glm::vec3 tangent_x = glm::normalize(glm::cross(glm::normalize(normal_result), WIND_DIR));
			    // tangent_xs->push_back(tangent_x);
			    // tangent_ys->push_back(glm::cross(tangent_x, glm::normalize(normal_result)));


				// tangent_xs->push_back(glm::vec3(1.0f, 0.0f, 0.0f));
				// tangent_ys->push_back(glm::vec3(0.0f, 0.0f, 1.0f));
			}
		}
		// std::cout << "WaveNode normals and tangents change end" << std::endl;
		wave_mesh_->UpdatePositions(std::move(positions));
		wave_mesh_->UpdateNormals(std::move(normals));
		// wave_mesh_->UpdateTangentXs(std::move(tangent_xs));
		// wave_mesh_->UpdateTangentYs(std::move(tangent_ys));

		// Reset rendering component 
		wave_node_->RemoveComponent<RenderingComponent>();
		wave_node_->CreateComponent<RenderingComponent>(wave_mesh_);

		// std::cout << "WaveNode update end" << std::endl;
	}
}