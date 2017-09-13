//#ifendif LATTICE_HPP

#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <cstring>

#include "opengl.hpp"
#include "imgui.h"

#include "cgra/mesh.hpp"
#include "cgra/shader.hpp"
#include "cgra/matrix.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "math.h"

class LatticeNode{

public:

	glm::vec3 p;
	int ojID;
	bool selected;

	LatticeNode();
	//~LatticeNode();
	LatticeNode(int k, glm::vec3(p));

	void draw();
	
};

class LatticeSpline{

public:

	std::vector<LatticeNode> children;
	LatticeSpline();
	~LatticeSpline();
	
};


class LatticePatch{

public:

	std::vector<LatticeSpline> children;
	LatticePatch();
	~LatticePatch();
	
};

class Lattice {

	std::vector<LatticeNode> m_nodes;
	cgra::Matrix<double> vao = cgra::Matrix<double>(0,3);
	cgra::Matrix<unsigned int> lineids = cgra::Matrix<unsigned int>(0,2);
	public:
	glm::vec3 m_min;
	glm::vec3 m_max;
	glm::vec3 m_resolution;
	cgra::Mesh latticeMesh;

    Lattice();

	Lattice(glm::vec3 min,glm::vec3 max, glm::vec3 res);
	
	LatticeNode &getNode(int x, int y, int z);

	void setMesh();

	std::vector<float> points;
	int xsegs, ysegs , zsegs;

	//cgra::mesh result(cgra:mesh before); 

	void click(float mouseX, float mouseY);


};

//#endif