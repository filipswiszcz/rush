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

void util::load_mesh(const std::string &filename, std::vector<float> &vertices) {
    std::vector<glm::vec3> vertices_t, normals_t;
    std::vector<glm::vec2> uvs_t;

    std::vector<float> vertices_f;

    std::ifstream file(filename);
    if (!file) {
        log_warn("mesh file: %s not found", filename.c_str()); return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("v ")) {
            glm::vec3 vertex;
            sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            vertices_t.push_back(vertex);
        } else if (line.starts_with("vn")) {
            glm::vec3 normal;
            sscanf(line.c_str(), "vn %f %f %f", &normal.x, &normal.y, &normal.z);
            normals_t.push_back(normal);
        } else if (line.starts_with("vt")) {
            glm::vec2 uv;
            sscanf(line.c_str(), "vt %f %f", &uv.x, &uv.y);
            uvs_t.push_back(uv);
        } else if (line.starts_with("f")) {
            std::regex pattern("([a-z]) (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+)");
            std::smatch match;
            if (!std::regex_match(line, match, pattern)) continue;
            for (int i = 0; i < 10; i++) {
                switch (i) {
                    case 2: {
                        glm::vec3 *velem_a = &vertices_t[std::stoi(match[2].str()) - 1];
                        vertices.push_back(velem_a -> x);
                        vertices.push_back(velem_a -> y);
                        vertices.push_back(velem_a -> z);
                        break;
                    }
                    case 3: {
                        glm::vec2 *uelem_a = &uvs_t[std::stoi(match[3].str()) - 1];
                        vertices.push_back(uelem_a -> x);
                        vertices.push_back(uelem_a -> y);
                        break;
                    }
                    case 5: {
                        glm::vec3 *velem_b = &vertices_t[std::stoi(match[5].str()) - 1];
                        vertices.push_back(velem_b -> x);
                        vertices.push_back(velem_b -> y);
                        vertices.push_back(velem_b -> z);
                        break;
                    }
                    case 6: {
                        glm::vec2 *uelem_b = &uvs_t[std::stoi(match[6].str()) - 1];
                        vertices.push_back(uelem_b -> x);
                        vertices.push_back(uelem_b -> y);
                        break;
                    }
                    case 8: {
                        glm::vec3 *velem_c = &vertices_t[std::stoi(match[8].str()) - 1];
                        vertices.push_back(velem_c -> x);
                        vertices.push_back(velem_c -> y);
                        vertices.push_back(velem_c -> z);
                        break;
                    }
                    case 9: {
                        glm::vec2 *uelem_c = &uvs_t[std::stoi(match[9].str()) - 1];
                        vertices.push_back(uelem_c -> x);
                        vertices.push_back(uelem_c -> y);
                        break;
                    }
                    default: break;
                }
            }
        }
    }
}

#pragma endregion Resource_Loader }