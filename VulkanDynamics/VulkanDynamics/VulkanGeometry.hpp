#ifndef __VK_GEOMETRY_HPP__
#define __VK_GEOMETRY_HPP__


namespace VkApplication {

	void MainVulkApplication::loadModel() {

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

		/*
		//DEFAULT
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }
		*/

		
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str(), 0, true)) {
			throw std::runtime_error(warn + err);
		}
		int numberOfPoints = 0;
		//FEL LORD
		for (const auto& shape : shapes) {

			for (int i = 0; i < shape.mesh.indices.size(); i += 3 ) {

				Vertex v1, v2, v3;

				v1.pos = { attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 0], attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 1], attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 2] };
				v2.pos = { attrib.vertices[3 * shape.mesh.indices[i+1].vertex_index + 0], attrib.vertices[3 * shape.mesh.indices[i+1].vertex_index + 1], attrib.vertices[3 * shape.mesh.indices[i+1].vertex_index + 2] };
				v3.pos = { attrib.vertices[3 * shape.mesh.indices[i+2].vertex_index + 0], attrib.vertices[3 * shape.mesh.indices[i+2].vertex_index + 1], attrib.vertices[3 * shape.mesh.indices[i+2].vertex_index + 2] };

				v1.color = { 1.0f, 1.0f, 1.0f };
				v2.color = { 1.0f, 1.0f, 1.0f };
				v3.color = { 1.0f, 1.0f, 1.0f };

				glm::vec3 triNormal = glm::normalize ( glm::cross((v2.pos - v1.pos), (v3.pos - v1.pos)) );

				v1.vertexNormal = v2.vertexNormal = v3.vertexNormal = triNormal;

				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);

				indices.push_back(numberOfPoints++);
				indices.push_back(numberOfPoints++);
				indices.push_back(numberOfPoints++);
			}
		}

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, GROUND_PATH.c_str(), 0, true)) {
			throw std::runtime_error(warn + err);
		}

		//GROUND
		for (const auto& shape : shapes) {

			for (int i = 0; i < shape.mesh.indices.size(); i += 3) {

				Vertex v1, v2, v3;

				v1.pos = { attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 0], attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 1], attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 2] };
				v2.pos = { attrib.vertices[3 * shape.mesh.indices[i + 1].vertex_index + 0], attrib.vertices[3 * shape.mesh.indices[i + 1].vertex_index + 1], attrib.vertices[3 * shape.mesh.indices[i + 1].vertex_index + 2] };
				v3.pos = { attrib.vertices[3 * shape.mesh.indices[i + 2].vertex_index + 0], attrib.vertices[3 * shape.mesh.indices[i + 2].vertex_index + 1], attrib.vertices[3 * shape.mesh.indices[i + 2].vertex_index + 2] };

				v1.color = { 1.0f, 1.0f, 1.0f };
				v2.color = { 1.0f, 1.0f, 1.0f };
				v3.color = { 1.0f, 1.0f, 1.0f };

				glm::vec3 triNormal = glm::normalize(glm::cross((v2.pos - v1.pos), (v3.pos - v1.pos)));

				v1.vertexNormal = v2.vertexNormal = v3.vertexNormal = triNormal;

				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);

				indices.push_back(numberOfPoints++);
				indices.push_back(numberOfPoints++);
				indices.push_back(numberOfPoints++);
			}
		}
		
		/*
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, VASE_PATH.c_str(), 0, true)) {
			throw std::runtime_error(warn + err);
		}

		//uniqueVertices.clear();
		numberOfPoints = 0;

		for (const auto& shape : shapes) {

			for (int i = 0; i < shape.mesh.indices.size(); i += 3) {

				Vertex v1, v2, v3;

				v1.pos = { attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 0], attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 1], attrib.vertices[3 * shape.mesh.indices[i].vertex_index + 2] };
				v2.pos = { attrib.vertices[3 * shape.mesh.indices[i + 1].vertex_index + 0], attrib.vertices[3 * shape.mesh.indices[i + 1].vertex_index + 1], attrib.vertices[3 * shape.mesh.indices[i + 1].vertex_index + 2] };
				v3.pos = { attrib.vertices[3 * shape.mesh.indices[i + 2].vertex_index + 0], attrib.vertices[3 * shape.mesh.indices[i + 2].vertex_index + 1], attrib.vertices[3 * shape.mesh.indices[i + 2].vertex_index + 2] };

				v1.color = { 1.0f, 1.0f, 1.0f };
				v2.color = { 1.0f, 1.0f, 1.0f };
				v3.color = { 1.0f, 1.0f, 1.0f };

				glm::vec3 triNormal = glm::normalize(glm::cross((v2.pos - v1.pos), (v3.pos - v1.pos)));

				v1.vertexNormal = v2.vertexNormal = v3.vertexNormal = triNormal;

				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);

				indices.push_back(numberOfPoints++);
				indices.push_back(numberOfPoints++);
				indices.push_back(numberOfPoints++);
			}
		}
		*/
	}
}
#endif