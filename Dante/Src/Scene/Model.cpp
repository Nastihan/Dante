#include "Pch.h"
#include "Scene/Model.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_USE_CPP20
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "../ThirdParty/tinygltf/tiny_gltf.h"

namespace Dante::Scene
{
	Model::Model(std::string path)
	{
		std::string warning{};
		std::string error{};

		tinygltf::TinyGLTF context;
		tinygltf::Model model;

		if (!context.LoadASCIIFromFile(&model, &error, &warning, path))
		{
			if (!error.empty())
			{
				OutputDebugStringA(error.c_str());
			}

			if (!warning.empty())
			{
				OutputDebugStringA(warning.c_str());
			}
		}

		std::vector<Vertex> vertices;
		tinygltf::Scene& scene = model.scenes[model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); i++)
		{
			tinygltf::Node& node = model.nodes[scene.nodes[i]];
			if (node.mesh < 0 || node.mesh > model.meshes.size()) continue;

			tinygltf::Mesh& mesh = model.meshes[node.mesh];
			for (size_t i = 0; i < mesh.primitives.size(); i++)
			{
				tinygltf::Primitive primitive = mesh.primitives[i];
				tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

			}

		}


	}


}