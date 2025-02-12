#include <rush/util/resource_loader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#pragma mark Resource_Loader
#pragma region Resource_Loader {

unsigned int util::temp_load_texture(const std::string &filename) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels_k;
    // stbi_set_flip_vertically_on_load(true);
    unsigned char *img = stbi_load(filename.c_str(), &width, &height, &channels_k, 0);
    if (!img) {
        log_warn("failed to load img texture.");
    } else {
        int frmt, in_frmt;
        switch (channels_k) {
            case 1:
                frmt = GL_RED;
                in_frmt = GL_RED;
                break;
            case 3:
                frmt = GL_RGB;
                in_frmt = GL_RGB;
                break;
            case 4:
                frmt = GL_RGBA;
                in_frmt = GL_RGBA;
                break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, in_frmt, width, height, 0, frmt, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(img);

    return texture;
}

entity::Mesh util::load_mesh(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        log_warn("mesh file: %s not found", filename.c_str()); return entity::Mesh({}, {});
    }

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    entity::Material material;

    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("v ")) {
            float x, y, z;
            sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        } else if (line.starts_with("f")) {
            std::regex pattern("([a-z]) (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+)");
            std::smatch match;
            if (!std::regex_match(line, match, pattern)) continue;
            else {
                indices.push_back(std::stoi(match[2].str()));
                indices.push_back(std::stoi(match[5].str()));
                indices.push_back(std::stoi(match[8].str()));
            }
        } else if ("mtllib") {
            // std::string mtl_filename;
            char mtl_filename[256];
            sscanf(line.c_str(), "mtllib %s", mtl_filename);
            material = load_mesh_material("resources/objects/geometric_shapes/" + std::string(mtl_filename));
        } else continue;
    }

    entity::Mesh mesh(vertices, indices);
    mesh.set_material(material);

    return mesh;
}

void util::load_mesh_p(const std::string &filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals) {
    std::vector<glm::vec3> t_vertices, t_normals;
    std::vector<glm::vec2> t_uvs;

    std::ifstream file(filename);
    if (!file) {
        log_warn("mesh file: %s not found", filename.c_str()); return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("mtllib")) {
            // use threads? 
        } else if (line.starts_with("v ")) {
            glm::vec3 vertex;
            sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            t_vertices.push_back(vertex);
        } else if (line.starts_with("vn")) {
            glm::vec3 normal;
            sscanf(line.c_str(), "vn %f %f %f", &normal.x, &normal.y, &normal.z);
            t_normals.push_back(normal);
        } else if (line.starts_with("vt")) {
            glm::vec2 uv;
            sscanf(line.c_str(), "vt %f %f", &uv.x, &uv.y);
            t_uvs.push_back(uv);
        } else if (line.starts_with("f")) {
            std::regex pattern("([a-z]) (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+)");
            std::smatch match;
            if (!std::regex_match(line, match, pattern)) continue;
            for (int i = 0; i < 11; i++) {
                switch (i) {
                    case 2: {
                        vertices.push_back(t_vertices[std::stoi(match[2].str()) - 1]); break;
                    }
                    case 3: {
                        normals.push_back(t_normals[std::stoi(match[3].str()) - 1]); break;
                    }
                    case 4: {
                        uvs.push_back(t_uvs[std::stoi(match[4].str()) - 1]); break;
                    }
                    case 5: {
                        vertices.push_back(t_vertices[std::stoi(match[5].str()) - 1]); break;
                    }
                    case 6: {
                        normals.push_back(t_normals[std::stoi(match[6].str()) - 1]); break;
                    }
                    case 7: {
                        uvs.push_back(t_uvs[std::stoi(match[7].str()) - 1]); break;
                    }
                    case 8: {
                        vertices.push_back(t_vertices[std::stoi(match[8].str()) - 1]); break;
                    }
                    case 9: {
                        normals.push_back(t_normals[std::stoi(match[9].str()) - 1]); break;
                    }
                    case 10: {
                        uvs.push_back(t_uvs[std::stoi(match[10].str()) - 1]); break;
                    }
                    default: break;
                }
            }
        }
    }
}

entity::Material util::load_mesh_material(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        log_warn("mtl file: %s not found", filename.c_str()); return entity::Material();
    }

    entity::Material material;

    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("Ns")) {
            sscanf(line.c_str(), "Ns %f", &material.shininess);
        } else if (line.starts_with("Ka")) {
            sscanf(line.c_str(), "Ka %f %f %f", &material.ambient.x, &material.ambient.y, &material.ambient.z);
        } else if (line.starts_with("Kd")) {
            sscanf(line.c_str(), "Ka %f %f %f", &material.diffuse.x, &material.diffuse.y, &material.diffuse.z);
        } else if (line.starts_with("Ks")) {
            sscanf(line.c_str(), "Ka %f %f %f", &material.specular.x, &material.specular.y, &material.specular.z);
        } else if (line.starts_with("Ke")) {
            sscanf(line.c_str(), "Ke %f %f %f", &material.emissivity.x, &material.emissivity.y, &material.emissivity.z);
        } else if (line.starts_with("Ni")) {
            sscanf(line.c_str(), "Ni %f", &material.density);
        } else if (line.starts_with("d")) {
            sscanf(line.c_str(), "d %f", &material.transparency);
        } else if (line.starts_with("illum")) {
            sscanf(line.c_str(), "illum %d", &material.illumination);
        }
    }

    return material;
}

#pragma endregion Resource_Loader }