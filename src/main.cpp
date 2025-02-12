#include <iostream>
#include <vector>

#include <rush/core/renderer.hpp>
#include <rush/core/scene.hpp>
#include <rush/core/shader.hpp>
#include <rush/entity/camera.hpp>
#include <rush/entity/mesh.hpp>
extern "C" {
    #include <rush/util/log.h>
}
#include <rush/util/resource_loader.hpp>
#include <rush/util/shader_loader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const size_t WIDTH = 1280;
const size_t HEIGHT = 960;
const char* WINDOW_NAME = "rush (BUILD v0.1.7)";

float TIME_OF_LAST_FRAME = 0.0f;
float TIME_BETWEEN_FRAMES = 0.0f;
float FRAMES_PER_SECOND = 0.0f;
int TEMPORARY_FRAMES_HOLDER = 0;

float CAMERA_TIME_BETWEEN_FRAMES = 0.0f;
float CAMERA_TIME_OF_LAST_FRAME = 0.0f;

bool IS_FIRST_PLAY = true;
bool IS_FULLSCREEN = false;

void mouse_input(GLFWwindow *window, entity::Camera &camera) {
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    if (IS_FIRST_PLAY) {
        camera.set_mouse_x(mouse_x);
        camera.set_mouse_y(mouse_y);
        IS_FIRST_PLAY = false;
    }

    float offset_x = mouse_x - camera.get_mouse_x();
    float offset_y = camera.get_mouse_y() - mouse_y;

    camera.set_mouse_x(mouse_x);
    camera.set_mouse_y(mouse_y);

    offset_x *= camera.get_sensitivity();
    offset_y *= camera.get_sensitivity();

    camera.set_yaw(camera.get_yaw() + offset_x);
    camera.set_pitch(camera.get_pitch() + offset_y);

    if (camera.get_pitch() > 89.0f) camera.set_pitch(89.0f);
    if (camera.get_pitch() < -89.0f) camera.set_pitch(-89.0f);

    glm::vec3 t;
    t.x = glm::cos(glm::radians(camera.get_yaw())) * glm::cos(glm::radians(camera.get_pitch()));
    t.y = glm::sin(glm::radians(camera.get_pitch()));
    t.z = glm::sin(glm::radians(camera.get_yaw())) * glm::cos(glm::radians(camera.get_pitch()));

    camera.set_target_position(glm::normalize(t));
}

void keyboard_input(GLFWwindow *window, entity::Camera &camera) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.set_position(camera.get_position() + (camera.get_target_position() * (camera.get_speed() * CAMERA_TIME_BETWEEN_FRAMES)));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.set_position(camera.get_position() - (camera.get_target_position() * (camera.get_speed() * CAMERA_TIME_BETWEEN_FRAMES)));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.set_position(camera.get_position() - glm::normalize(glm::cross(camera.get_target_position(), camera.get_up_position())) * (camera.get_speed() * CAMERA_TIME_BETWEEN_FRAMES));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.set_position(camera.get_position() + glm::normalize(glm::cross(camera.get_target_position(), camera.get_up_position())) * (camera.get_speed() * CAMERA_TIME_BETWEEN_FRAMES));
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        // it works but really weird and needs fixing
        int k;
        GLFWmonitor **monitors = glfwGetMonitors(&k);
        GLFWmonitor *monitor = monitors[1];
        // GLFWmonitor *monitor = monitors[0];
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (IS_FULLSCREEN) {glfwSetWindowMonitor(window, nullptr, 0, 0, WIDTH, HEIGHT, mode -> refreshRate); IS_FULLSCREEN = false;}
        else {glfwSetWindowMonitor(window, monitor, 0, 0, mode -> width, mode -> height, mode -> refreshRate); IS_FULLSCREEN = true;}
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.set_speed(5.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.set_speed(2.0f);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void window_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void frames_update(GLFWwindow *window) {
    float current_time = static_cast<float>(glfwGetTime());
    TIME_BETWEEN_FRAMES = current_time - TIME_OF_LAST_FRAME;
    TEMPORARY_FRAMES_HOLDER++;
    if (TIME_BETWEEN_FRAMES > 1.0) {
        FRAMES_PER_SECOND = TEMPORARY_FRAMES_HOLDER / TIME_BETWEEN_FRAMES;
        TEMPORARY_FRAMES_HOLDER = 0;
        TIME_OF_LAST_FRAME = current_time;
        std::ostringstream modif_window_name;
        modif_window_name << WINDOW_NAME << " [" << static_cast<int>(FRAMES_PER_SECOND) << " FPS]";
        glfwSetWindowTitle(window, (modif_window_name.str()).c_str());
    }
}

int main() {

    GLFWwindow *window;

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, NULL, NULL);
    if (!window) {
        log_fatal("failed to create window"); glfwTerminate(); return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, window_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    // shader code
    core::Shader shader("shaders/basic.vs", "shaders/basic.fs");


    // std::string vertex_shader_code = util::read_shader_file("shaders/basic.vs");
    // std::string frag_shader_code = util::read_shader_file("shaders/basic.fs");
    // GLuint vertex_shader = util::compile_shader(vertex_shader_code, GL_VERTEX_SHADER);
    // GLuint frag_shader = util::compile_shader(frag_shader_code, GL_FRAGMENT_SHADER);

    // GLuint program = util::link_shaders(vertex_shader, frag_shader);
    // end of shader code

    // mtl code
    // float shininess, density, transparency;
    // glm::vec3 ambient, diffuse, specular, emissivity;
    // int illumination;

    // util::load_mesh_mtl("resources/models/cube.mtl", shininess, ambient, diffuse, specular, emissivity, density, transparency, illumination);
    // end of mesh code

    // light code
    // std::string light_ver_shader_code = util::read_shader_file("shaders/light.vs");
    // std::string light_frag_shader_code = util::read_shader_file("shaders/light.fs");
    // GLuint light_ver_shader = util::compile_shader(light_ver_shader_code, GL_VERTEX_SHADER);
    // GLuint light_frag_shader = util::compile_shader(light_frag_shader_code, GL_FRAGMENT_SHADER);

    // GLuint light_shader_program = util::link_shaders(light_ver_shader, light_frag_shader);

    glm::vec3 positions[256];
    unsigned int index = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            positions[index] = glm::vec3(i * 2.0f, 0.0f, j * 2.0f); index++;
        }
    }

    // unsigned int vbo, vao;
    // glGenVertexArrays(1, &vao);
    // glGenBuffers(1, &vbo);

    // glBindVertexArray(vao);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    // glEnableVertexAttribArray(0);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
    
    // end of light code

    // texture
    // unsigned int temp_texture = util::temp_load_texture("resources/textures/witcher_wallpaper.jpg");
    // unsigned int texture = util::temp_load_texture("resources/textures/fantasy_world_map.png");
    unsigned int texture = util::temp_load_texture("resources/textures/red_metal.jpg");

    // glUseProgram(program);
    glUniform1i(glGetUniformLocation(shader.get_id(), "texture_t"), 0); // here
    // end of texture

    // grid code
    std::vector<float> grid_vertices;
    glm::vec3 grid_color(1.0f, 0.0f, 0.235f);

    for (int i = 0; i < 250; i++) {
        grid_vertices.push_back(-i);
        grid_vertices.push_back(0.0f);
        grid_vertices.push_back(-1000.0f);
        grid_vertices.push_back(grid_color.r);
        grid_vertices.push_back(grid_color.g);
        grid_vertices.push_back(grid_color.b);

        grid_vertices.push_back(-i);
        grid_vertices.push_back(0.0f);
        grid_vertices.push_back(1000.0f);
        grid_vertices.push_back(grid_color.r);
        grid_vertices.push_back(grid_color.g);
        grid_vertices.push_back(grid_color.b);
    }
    for (int i = 0; i < 250; i++) {
        grid_vertices.push_back(i);
        grid_vertices.push_back(0.0f);
        grid_vertices.push_back(-1000.0f);
        grid_vertices.push_back(grid_color.r);
        grid_vertices.push_back(grid_color.g);
        grid_vertices.push_back(grid_color.b);

        grid_vertices.push_back(i);
        grid_vertices.push_back(0.0f);
        grid_vertices.push_back(1000.0f);
        grid_vertices.push_back(grid_color.r);
        grid_vertices.push_back(grid_color.g);
        grid_vertices.push_back(grid_color.b);
    }
    for (int i = 0; i < 250; i++) {
        grid_vertices.push_back(-1000.0f);
        grid_vertices.push_back(0.0f);
        grid_vertices.push_back(-i);
        grid_vertices.push_back(grid_color.r);
        grid_vertices.push_back(grid_color.g);
        grid_vertices.push_back(grid_color.b);

        grid_vertices.push_back(1000.0f);
        grid_vertices.push_back(0.0f);
        grid_vertices.push_back(-i);
        grid_vertices.push_back(grid_color.r);
        grid_vertices.push_back(grid_color.g);
        grid_vertices.push_back(grid_color.b);
    }
    for (int i = 0; i < 250; i++) {
        grid_vertices.push_back(-1000.0f);
        grid_vertices.push_back(0.0f);
        grid_vertices.push_back(i);
        grid_vertices.push_back(grid_color.r);
        grid_vertices.push_back(grid_color.g);
        grid_vertices.push_back(grid_color.b);

        grid_vertices.push_back(1000.0f);
        grid_vertices.push_back(0.0f);
        grid_vertices.push_back(i);
        grid_vertices.push_back(grid_color.r);
        grid_vertices.push_back(grid_color.g);
        grid_vertices.push_back(grid_color.b);
    }

    unsigned int grid_vao, grid_vbo;
    glGenVertexArrays(1, &grid_vao);
    glGenBuffers(1, &grid_vbo);

    glBindVertexArray(grid_vao);
    glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, grid_vertices.size() * sizeof(float), grid_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // end of grid code

    // renderer code
    core::Renderer renderer;
    // end of renderer code

    // another mesh code
    std::vector<glm::vec3> mesh_vertices, mesh_normals;
    std::vector<glm::vec2> mesh_uvs;
    util::load_mesh("resources/models/sphere.obj", mesh_vertices, mesh_uvs, mesh_normals);

    entity::Mesh mesh(mesh_vertices, mesh_uvs, mesh_normals);
    mesh.set_shader_program(shader.get_id()); // here
    mesh.set_texture(texture);
    std::shared_ptr<entity::Mesh> shared_mesh = std::make_shared<entity::Mesh>(mesh);

    entity::Object sphere;
    sphere.set_position(positions[0]);
    sphere.set_mesh(shared_mesh);

    core::Scene scene;
    scene.add_object(std::make_shared<entity::Object>(sphere));
    scene.bind();

    renderer.add_scene(std::make_shared<core::Scene>(scene));
    // end of antoher code

    // infinite grid code

    // end of infinite grid code

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.get_id(), "projection"), 1, GL_FALSE, &projection[0][0]); // here

    entity::Camera camera;
    camera.set_position(glm::vec3(0.0f, 1.0f, 3.0f));
    camera.set_target_position(glm::vec3(0.0f, 0.0f, -1.0f));
    camera.set_up_position(glm::vec3(0.0f, 1.0f, 0.0f));

    while (!glfwWindowShouldClose(window)) {
        // change time
        float current_frame_time = static_cast<float>(glfwGetTime());
        CAMERA_TIME_BETWEEN_FRAMES = current_frame_time - CAMERA_TIME_OF_LAST_FRAME;
        CAMERA_TIME_OF_LAST_FRAME = current_frame_time;

        // frames
        frames_update(window);

        // inputs
        keyboard_input(window, camera);
        mouse_input(window, camera);

        // render commands
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glUniform1i(glGetUniformLocation(program, "is_coloring"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(shader.get_id()); // here

        glm::mat4 view = camera.get_look_at();
        glUniformMatrix4fv(glGetUniformLocation(shader.get_id(), "view"), 1, GL_FALSE, &view[0][0]); // here
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, positions[0]);
        // glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
        
        // glBindVertexArray(vao);
        // for (unsigned int i = 0; i < 256; i++) {
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, positions[i]);
            // float angle = 20.0f * i;
            // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            // glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
            // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            // glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);
        // }

        // grid code
        glUniform1i(glGetUniformLocation(shader.get_id(), "is_coloring"), 1); // here
        glBindVertexArray(grid_vao);
        glDrawArrays(GL_LINES, 0, grid_vertices.size() / 2);
        // end of grid code

        // another mesh code
        glUniform1i(glGetUniformLocation(shader.get_id(), "is_coloring"), 0); // here
        renderer.draw();
        // end of another mesh code

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &grid_vao);
    glDeleteBuffers(1, &grid_vbo);
    glfwTerminate();

    return 0;
}