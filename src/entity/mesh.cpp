#pragma mark - Mesh
#pragma region Mesh {

#include <rush/entity/mesh.hpp>


entity::Mesh::Mesh(const std::vector<float> &vertices, const std::vector<uint32_t> &indices) {
    this -> vertices = vertices;
    this -> indices = indices;
}

void entity::Mesh::set_material(const entity::Material &material) {
    this -> material = material;
}

void entity::Mesh::set_vao(const uint32_t vao) {
    this -> vao = vao;
}

void entity::Mesh::set_vbo(const uint32_t vbo) {
    this -> vbo = vbo;
}

void entity::Mesh::set_ebo(const uint32_t ebo) {
    this -> ebo = ebo;
}

// void entity::Mesh::bind() {
//     std::vector<entity::Vertex> uninted_vertices;
//     for (int i = 0; i < this -> vertices.size(); i++) {
//         Vertex vertex = {this -> vertices[i], this -> uvs[i], this -> normals[i]};
//         uninted_vertices.push_back(vertex);
//     }

//     glGenVertexArrays(1, &this -> vao);
//     glGenBuffers(1, &this -> vbo);
//     // glGenBuffers(1, &this -> ebo);

//     glBindVertexArray(this -> vao);
//     glBindBuffer(GL_ARRAY_BUFFER, this -> vbo);
//     glBufferData(GL_ARRAY_BUFFER, uninted_vertices.size() * sizeof(Vertex), uninted_vertices.data(), GL_STATIC_DRAW);

//     // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this -> ebo);
//     // glBufferData(GL_ELEMENT_ARRAY_BUFFER, this -> indices.size() * sizeof(unsigned int), &this -> indices[0], GL_STATIC_DRAW);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
//     glEnableVertexAttribArray(0);
//     // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (2 * sizeof(float)));
//     // glEnableVertexAttribArray(1);
//     // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (5 * sizeof(float)));
//     // glEnableVertexAttribArray(2);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (3 * sizeof(float)));
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (6 * sizeof(float)));
//     glEnableVertexAttribArray(2);

//     glBindVertexArray(0);
// }

// void entity::Mesh::draw() {
//     glUseProgram(this -> shader_program);

//     glm::mat4 model = glm::mat4(1.0f);
//     model = glm::translate(model, positions[0]);
//     glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);

//     glBindVertexArray(this -> vao);
//     // glDrawElements(GL_TRIANGLES, static_cast<int>(this -> indices.size()), GL_UNSIGNED_INT, 0);
//     glDrawArrays(GL_TRIANGLES, 0, (this -> vertices.size() * 2 + this -> uvs.size()));
//     glBindVertexArray(0);
// }

// void entity::Mesh::set_shader_program(const unsigned int program) {
//     this -> shader_program = program;
// }

// void entity::Mesh::set_texture(const unsigned int texture) {
//     this -> texture = texture;
// }

#pragma endregion Mesh }