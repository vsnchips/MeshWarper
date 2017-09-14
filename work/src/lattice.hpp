//#ifendif LATTICE_HPP

#pragma once

#define MAX_LATTICE_VS_ARRAYSIZE 1000

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
	glm::vec3 worldp;
	int ojID;
	int techID;
	bool selected;

	bool isEnd;

	LatticeNode();
	//~LatticeNode();
	LatticeNode(int k, glm::vec3(p));

	cgra::Mesh * nodeMesh;

	glm::mat4 myTransform;

	void draw();
    void setID(int id);

    void move(glm::vec2 dydx, glm::mat4 rotationmat, float m_scale, float depth);
		
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
	
	public:

	cgra::Matrix<double> vao = cgra::Matrix<double>(0,3);
	cgra::Matrix<unsigned int> lineids = cgra::Matrix<unsigned int>(0,2);

	std::vector<LatticeNode> m_nodes;
	//std::vector<LatticeNode> m_end_nodes;
	float VSArray[MAX_LATTICE_VS_ARRAYSIZE];

	glm::vec3 m_min;
	glm::vec3 m_max;
	glm::vec3 m_resolution;
	cgra::Mesh latticeMesh;

	cgra::Program latProgram;

	int techID;

    Lattice();

	Lattice(glm::vec3 min,glm::vec3 max, glm::vec3 res, cgra::Mesh &);
	
	LatticeNode &getNode(int x, int y, int z);
	LatticeNode &getEndNode(int x, int y, int z);
    LatticeNode &getByID(int id);

	void setMesh();

	std::vector<float> points;
	int xsegs, ysegs , zsegs;

	//cgra::mesh result(cgra:mesh before); 

	void makeVSArray();
	void connectLines();
	void click(float mouseX, float mouseY);

	void draw(cgra::Program,glm::mat4,glm::mat4,glm::mat4,float);
	void drawForPick(cgra::Program,glm::mat4,glm::mat4,glm::mat4,float);

	void setTechnique(int tech,cgra::Program m_program);

	cgra::Mesh transformMesh(cgra::Mesh inmesh);


};

//#endif