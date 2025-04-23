#pragma once
#include "Mesh.h"
#include <memory>

class MeshPrimitive
{

public:


	static std::shared_ptr<Mesh> Createplane(float w,float h);
	static std::shared_ptr<Mesh> CreateBox(float w, float h, float d);
	static std::shared_ptr<Mesh> CreateRing(float outerRadius, float innerRadius, uint32_t divide);
	static std::shared_ptr<Mesh> CreateCylinder(float outerRadius, float innerRadius, uint32_t divide, float height);
	//static std::shared_ptr<Mesh> CreateCylinder(float outerRadius, float innerRadius, uint32_t divide);


};

