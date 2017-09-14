 
#include "lattice.hpp"
#include "glm/gtc/matrix_inverse.hpp"


Lattice::Lattice(){};

Lattice::Lattice( glm::vec3 min, glm::vec3 max, glm::vec3 res, cgra::Mesh & markmesh) 
	: m_min(min), m_max(max),m_resolution(res) {


// Calculate number of
// nodes needed.
int num = (res.x+2)*(res.y+2)*(res.z+2);
vao = cgra::Matrix<double> (num,3);

latProgram = cgra::Program::load_program(
        CGRA_SRCDIR "/res/shaders/simple.vs.glsl",
        //CGRA_SRCDIR "/res/shaders/lambert.fs.glsl");
        CGRA_SRCDIR "/res/shaders/simple.fs.glsl");

/*
printf("constructing endpoint node array with x: %f y %f z: %f\n", res.x+2, res.y+2, res.z+2 );
int ind = 0;
for (int i = 0; i < (int)res.x+2; i++) {
	float x = min.x+(i-1)*(max.x-min.x)/(res.x-1);
	for (int j = 0; j < (int)res.y+2; j++) {
		float y = min.y+(j-1)*(max.y-min.y)/(res.y-1);
		for (int k = 0; k < (int)res.z+2; k++) {
			// Create a LatticeNode
			// and add it to m_nodes
			LatticeNode node = LatticeNode(ind,glm::vec3(  x  ,  y  ,  min.z+(max.z-min.z)*(k-1)/(res.z-1)   ));
			m_end_nodes.push_back(node);
			ind++;
		} 
	} // Initial points set
}*/



printf("constructing mesh with x: %f y %f z: %f\n", res.x+2, res.y+2, res.z+2 );
int ind = 0;
for (int i = -1; i < (int)res.x+1; i++) {
	float x = min.x+i*(max.x-min.x)/(res.x-1);

	for (int j = -1; j < (int)res.y+1; j++) {
		float y = min.y+j*(max.y-min.y)/(res.y-1);

		for (int k = -1; k < (int)res.z+1; k++) {
			// Create a LatticeNode
			// and add it to m_nodes
			LatticeNode node = LatticeNode(ind,glm::vec3(  x  ,  y  ,  min.z+(max.z-min.z)*k/(res.z-1)   ));
			m_nodes.push_back(node); // m_nodes is a vector, needs no matrix dimensions.
			m_nodes.back().nodeMesh = &markmesh;
			m_nodes.back().isEnd = (i < 0 || i >= res.x || j < 0 || j >= res.y || k < 0 || k >= res.z);


			printf("setting mesh row %d %f %f %f \n", ind, (double)x , (double)y  ,  (double)(min.z+(max.z-min.z)*k/(res.z-1))  );
			vao.setRow(ind,{(double)x,(double)y,(double)(min.z+(max.z-min.z)*k/(res.z-1))});

			//printf("vao x: %f\n", vao.m_data[(k+j*res.z+i*res.y*res.z)*2]); 
			ind++;


		} 
	} // Initial points set
}

connectLines();

setMesh();
}

void Lattice::connectLines(){

// Connect lines
glm::vec3 res = m_resolution;

int linetotal = (res.x*res.y*(res.z-1) +
				 res.y*res.z*(res.x-1) +
				 res.z*res.x*(res.y-1) );
printf("\nlinetotal:%d\n", linetotal);

lineids = cgra::Matrix<unsigned int> (linetotal,2);

int ind = 1;
int lct = 0;
for (int i = 1; i < (int)res.x+1; i++) {
	float x = m_min.x+i*(m_max.x-m_min.x);
	for (int j = 1; j < (int)res.y+1; j++) {
		float y = m_min.y+i*(m_max.y-m_min.y);
		for (int k = 1; k < (int)res.z+1; k++) {
			if (k<res.z) 	{
				lineids.setRow(lct,{ind,ind+1});
				printf("setting line %d : %d %d\n", lct, ind, ind+1);

			//printf("lineids z: %d , %d\n", lineids.m_data[(k+j*res.z+i*res.y*res.z)*2], lineids.m_data[(k+j*res.z+i*res.y*res.z)*2+1]); 
			
				 lct++;
				}
			if (j<res.y) 	{
				lineids.setRow(lct,{ind,ind+(res.z+2)});
				//printf("setting line %d : %d %d\n", lct, ind, ind+1);
			//printf("lineids y: %d\n", lineids.m_data[(k+j*res.z+i*res.y*res.z)*2]); 
				 lct++;
				}
			if (i<res.x) 	{
				lineids.setRow(lct,{ind,ind+(res.y+2)*(res.z+2)});
			//	printf("setting line %d : %d %d\n", lct, ind, ind+1);
			//printf("lineids x: %d\n", lineids.m_data[(k+j*res.z+i*res.y*res.z)*2]); 
				 lct++;
				}
				ind++;
		} ind+=2;
	}	ind +=2;

 }

}
void Lattice::setMesh(){
//
	//latticeMesh.setData(verts,cgra::Matrix<unsigned int> foo);
	//cgra::Matrix<unsigned int> 
	latticeMesh.setData(vao,lineids);
	printf("mesh Set!\n");
}






LatticeNode &Lattice::getNode(
int x, int y, int z) {
// Find the matching node and
// return it
if ( x>m_resolution.x || y>m_resolution.y || z>m_resolution.z ) printf("\n Lattice node array out of bounds - will now segfault");;
return m_nodes[x*m_resolution.y*m_resolution.z+y+m_resolution.z+z];

}

LatticeNode &Lattice::getEndNode(
int x, int y, int z) {
// Find the matching node and
// return it
if ( x>m_resolution.x || y>m_resolution.y || z>m_resolution.z ) printf("\n Lattice node array out of bounds - will now segfault");;
return m_nodes[x*m_resolution.y*m_resolution.z+y+m_resolution.z+z];

}

LatticeNode &Lattice::getByID(int id){
	return m_nodes[id];
}

LatticeNode::LatticeNode(int k, glm::vec3 np){
	ojID = k;
	p = np;
}

void LatticeNode::setID(int id){
        ojID=id;
        nodeMesh->setID(id);
}

void Lattice::makeVSArray(){
	int mx = (techID!=2)?m_resolution.x:m_resolution.x+2;
	int my = (techID!=2)?m_resolution.y:m_resolution.y+2;
	int mz = (techID!=2)?m_resolution.z:m_resolution.z+2; 
	if (techID==2){
		for (int i=0; i < mx; i++){
			for (int j=0; j < my; j++){
				for (int k=0; k < mz; k++){
						if (k<1) { 
						VSArray[3*i*mz*my + j*mz + k] = getNode(i,j,k).p.x;
					}
						VSArray[3*i*mz*my + j*mz + k] = getNode(i,j,k).p.x;
						VSArray[3*i*mz*my + j*mz + k +1] = getNode(i,j,k).p.y;
						VSArray[3*i*mz*my + j*mz + k +2] = getNode(i,j,k).p.z;
						}
				   

				}
			}
		}else{
		for (int i=1; i < mx+1; i++){
			for (int j=1; j < my+1; j++){
				for (int k=1; k < mz+1; k++){
					if (techID=2)
						VSArray[3*i*mz*my + j*mz + k] = getNode(i,j,k).p.x;
						VSArray[3*i*mz*my + j*mz + k +1] = getNode(i,j,k).p.y;
						VSArray[3*i*mz*my + j*mz + k +2] = getNode(i,j,k).p.z;
					}
				}
			}
		}
}


    void LatticeNode::move(glm::vec2 dydx, glm::mat4 rotationmat, float m_scale, float depth){

    	printf("depth %f\n", depth);

    	dydx.x/=m_scale;
    	dydx.y/=m_scale;

    	dydx.x*=depth;
    	dydx.y*=depth;

    	//glm::vec4 tvec = glm::vec4(dydx.x,dydx.y,0,1);
    	//tvec *= glm::inverse(rotationmat);

//    	glm::mat4 rotationTranslationMatrix = glm::translate(glm::inverse(rotationmat), glm::vec3(dydx.x,dydx.y,0));
    	glm::vec4 transvec(dydx.x,-dydx.y,0,1);

    	transvec = glm::inverse(rotationmat) * transvec;

        printf("move object by x %fy %fz %f\n", transvec.x,transvec.y,transvec.z);

    	//glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1), glm::vec3(dydx.x,-dydx.y,0));

    	glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1), glm::vec3(transvec.x,transvec.y,transvec.z));

    	//printf("tvec x %f y %f z %f\n", tvec.x,tvec.y,tvec.z);

    	//p = glm::vec3(p4.x,p4.y,p4.z);
    	glm::vec4 p4 = glm::vec4(p.x,p.y,p.z,1);
    	//p4 = TranslationMatrix * glm::inverse(rotationmat) * p4;
    	p4 = TranslationMatrix * p4;

    	p = glm::vec3(p4.x,p4.y,p4.z);
        printf("move object by x %fy %fz %f\n", TranslationMatrix[3][0], TranslationMatrix[3][1], TranslationMatrix[3][2]);

    }

void Lattice::draw(cgra::Program m_program,glm::mat4 modTransform,glm::mat4 rotMat, glm::mat4 modelTrans,float m_scale  ){

    GLuint loc = glGetUniformLocation(
    m_program.glName(), "gColor");
    glUniform1i(loc,-1);

	for(int i = 0; i <m_nodes.size(); i++){

		if (!m_nodes[i].isEnd && techID!= 2){
    glm::mat4 markerTransform(1.0f);


		glm::vec3 p = m_nodes[i].p;
		glm::vec4 tp = glm::vec4(p.x*m_scale,p.y*m_scale,p.z*m_scale,0);
	
		glm::mat4 nodeTransform(1.0f);

	markerTransform *= modelTrans;
	markerTransform *= glm::scale(markerTransform,glm::vec3(0.3));

		tp = rotMat * tp;
		nodeTransform *= glm::translate(glm::mat4(20),glm::vec3(tp.x,tp.y,tp.z));
		nodeTransform *= glm::scale(markerTransform,glm::vec3(0.3));

		nodeTransform*=modTransform;
   		m_program.setModelMatrix(nodeTransform);

/*Color Picking
    //int colorID = m_nodes[i].ojID;
   		int colorID = i;
    glUniform1i(loc, colorID);
    */

    	m_nodes[i].draw();
    	}
    }

    latProgram.use();
    latticeMesh.draw(GL_LINES);
}

void Lattice::drawForPick(cgra::Program m_program,glm::mat4 modTransform,glm::mat4 rotMat, glm::mat4 modelTrans,float m_scale  ){


     GLuint loc = glGetUniformLocation(
    m_program.glName(), "gColor");
    glUniform1i(loc,-1);

	for(int i = 0; i <m_nodes.size(); i++){

		if (!m_nodes[i].isEnd){
    glm::mat4 markerTransform(1.0f);


		glm::vec3 p = m_nodes[i].p;
		glm::vec4 tp = glm::vec4(p.x*m_scale,p.y*m_scale,p.z*m_scale,0);
	
		glm::mat4 nodeTransform(1.0f);

	markerTransform *= modelTrans;
	markerTransform *= glm::scale(markerTransform,glm::vec3(0.3));

		tp = rotMat * tp;
		nodeTransform *= glm::translate(glm::mat4(20),glm::vec3(tp.x,tp.y,tp.z));
		nodeTransform *= glm::scale(markerTransform,glm::vec3(0.3));

		nodeTransform*=modTransform;
   		m_program.setModelMatrix(nodeTransform);

//Color Picking
    int colorID = m_nodes[i].ojID;
    GLuint loc = glGetUniformLocation(
    m_program.glName(), "gColor");
    glUniform1i(loc, colorID);


    	m_nodes[i].draw();
    }

    glUniform1i(loc,-1);
  }
}

void LatticeNode::draw(){
	nodeMesh->draw(GL_TRIANGLES);
	return;
}

void Lattice::setTechnique(int tech,cgra::Program m_program){
	techID = tech;
    GLuint loc = glGetUniformLocation(
    m_program.glName(), "uTechID");
    glUniform1i(loc,techID);
}

cgra::Mesh Lattice::transformMesh(cgra::Mesh inmesh){
	return cgra::Mesh();
}