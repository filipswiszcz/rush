#pragma mark Renderer
#pragma region Renderer {

#include <rush/core/renderer.hpp>


core::Renderer::Renderer() : repository() {}

void core::Renderer::bind() {
    // decide which scene
    for (const auto &scene : this -> scenes) {
        // scene -> bind(this -> repository);
    }
}

void core::Renderer::draw() {
    for (const auto &scene : this -> scenes) {
        // scene -> draw(this -> repository);
    }
}

void core::Renderer::release() {
    this -> scenes.clear();
}

void core::Renderer::add_scene(const std::shared_ptr<core::Scene> &scene) {
    this -> scenes.push_back(scene);
}

void core::Renderer::remove_scene(const std::shared_ptr<core::Scene> &scene) {
    this -> scenes.erase(std::remove(this -> scenes.begin(), this -> scenes.end(), scene), this -> scenes.end());
}

#pragma endregion Renderer }