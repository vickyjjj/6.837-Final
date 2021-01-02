#include "SimulationApp.hpp"

#include "glm/gtx/string_cast.hpp"

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/lights/OceanLight.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/cameras/SimplifiedArcBallCameraNode.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/debug/AxisNode.hpp"

#include "WaveNode.hpp"

#include "glm/gtx/string_cast.hpp"

namespace GLOO {
SimulationApp::SimulationApp(const std::string& app_name,
                             glm::ivec2 window_size,
                             float amplitude, 
                             float angular_freq,
                             float wavenumber_x,
                             float wavenumber_z)
    : Application(app_name, window_size),
      amplitude_(amplitude),
      angular_freq_(angular_freq),
      wavenumber_x_(wavenumber_x),
      wavenumber_z_(wavenumber_z) {
}

void SimulationApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  auto camera_node = make_unique<SimplifiedArcBallCameraNode>();
  camera_node->GetTransform().SetPosition(glm::vec3(0.0f, -20.0f, -20.0f));
  camera_node->GetTransform().SetRotation(glm::vec3(1.0f, 0.0f, 0.0f), kPi / 4);
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());

  root.AddChild(make_unique<AxisNode>('A'));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.0f, 0.0f, 0.5f));
  root.CreateComponent<LightComponent>(ambient_light);

  // auto point_light = std::make_shared<PointLight>();
  // point_light->SetDiffuseColor(glm::vec3(0.8f, 0.8f, 0.2f));
  // point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 0.5f));
  // point_light->SetAttenuation(glm::vec3(0.01f, 0.01f, 0.005f));
  // auto point_light_node = make_unique<SceneNode>();
  // point_light_node->CreateComponent<LightComponent>(point_light);
  // point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 2.0f, 4.f));
  // root.AddChild(std::move(point_light_node));

  auto directional_light = std::make_shared<DirectionalLight>();
  directional_light->SetDiffuseColor(glm::vec3(1.0f, 1.0f, 1.0f));
  directional_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  glm::vec3 light_dir(0.0f, -1.0f, -1.0f);
  directional_light->SetDirection(glm::normalize(light_dir));
  root.CreateComponent<LightComponent>(directional_light);

  // auto ocean_light = std::make_shared<OceanLight>();
  // ocean_light->SetSkyColor(glm::vec3(0.0f, 0.0f, 0.0f));// TODO fix, unused
  // ocean_light->SetSunColor(glm::vec3(1.0f, 1.0f, 0.8f));
  // ocean_light->SetSeaColor(glm::vec3(0.0f, 0.0f, 0.5f));
  // auto ocean_light_node = make_unique<SceneNode>();
  // ocean_light_node->CreateComponent<LightComponent>(ocean_light);
  // ocean_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.f));
  // root.AddChild(std::move(ocean_light_node));
  // root.CreateComponent<LightComponent>(ocean_light);

  // Add SceneNode for wavemesh 
  auto wave_node = make_unique<WaveNode>(amplitude_,
                                          angular_freq_,
                                          wavenumber_x_,
                                          wavenumber_z_,
                                          camera_node.get());
  root.AddChild(std::move(camera_node));
  root.AddChild(std::move(wave_node));
}
}  // namespace GLOO
