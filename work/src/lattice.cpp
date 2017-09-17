 
#include "lattice.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/integer.hpp"


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
        CGRA_SRCDIR "/res/shaders/lambert.fs.glsl");

glm::mat4 viewMatrix(1);
viewMatrix[3] = glm::vec4(0, 0, -1, 1);
latProgram.setViewMatrix(viewMatrix);



span = glm::vec3(max.x-min.x,max.y-min.y,max.z-min.z);

handleSize = 0.5*glm::length(span)/(glm::max(glm::max(m_resolution.x,m_resolution.y),m_resolution.z)+10);

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


			printf("setting mesh row %d %f %f %f \n", ind,   x  ,  y  ,  min.z+(max.z-min.z)*k/(res.z-1)   );
			vao.setRow(ind,{  x  ,  y  ,  min.z+(max.z-min.z)*k/(res.z-1)  });

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

int linetotal = ((res.x+2)*(res.y+2)*(res.z+1) +
				 (res.y+2)*(res.z+2)*(res.x+1) +
				 (res.z+2)*(res.x+2)*(res.y+1) );
printf("\nlinetotal:%d\n", linetotal);

lineids = cgra::Matrix<unsigned int> (linetotal,2);



int ind = 0;
int lct = 0;
for (int i = 0; i < (int)res.x+2; i++) {
	float x = m_min.x+i*(m_max.x-m_min.x);
	for (int j = 0; j < (int)res.y+2; j++) {
		float y = m_min.y+i*(m_max.y-m_min.y);
		for (int k = 0; k < (int)res.z+2; k++) {
			if((k>0 && j>0 && i>0) && (k<=res.z && j<=res.y && i<=res.x)){

				if (k<res.z){
				lineids.setRow(lct,{ind,ind+1});
				//printf("setting line %d : %d %d\n", lct, ind, ind+1);
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
			}
				ind++;
		 //ind+=2;
	  }
	}	//ind +=2;

 }

}
void Lattice::setMesh(){

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

int Lattice::getFullSize(){
	return (m_resolution.x+2)*(m_resolution.y+2)*(m_resolution.z+2);
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

void Lattice::VSArraytoUniform(cgra::Program program){

	static int tick = 0;
	tick++;
 	VSArray[0] += 0.3*sin(tick*0.01);

	//GLfloat floatArray[3072] ;
	for (int i = 0; i < (m_resolution.x+2) * 
						(m_resolution.y+2) *
						(m_resolution.z+2); i++)
	{
		VSArray[i*3] = getByID(i).p.x;
 		VSArray[i*3+1] = getByID(i).p.y;
 		VSArray[i*3+2] = getByID(i).p.z;	
	}

	printf("latticeNode.p 41: %f %f %f\n", getByID(41).p.x,
									 getByID(41).p.y,
									 getByID(41).p.z);

	printf("vao 42: %f %f %f\n", *vao[42,0],
									 *vao[42,1],
									 *vao[42,2]);
	printf("vsarray 42: %f %f %f\n", VSArray[42*3],
									 VSArray[42*3+1],
									 VSArray[42*3+2]);

	//Set uniforms
	int location = glGetUniformLocation(program.glName(), "latticeVerts");
	glUniform3fv(location, MAX_LATTICE_VS_ARRAYSIZE, &VSArray[0]);
	location = glGetUniformLocation(program.glName(), "xres");
	glUniform1i(location, m_resolution.x);
	location = glGetUniformLocation(program.glName(), "yres");
	glUniform1i(location, m_resolution.y);
	location = glGetUniformLocation(program.glName(), "zres");
	glUniform1i(location, m_resolution.z);

	location = glGetUniformLocation(program.glName(), "meshorigin");
	glUniform3fv(location, 1, (GLfloat*)&m_min);

	location = glGetUniformLocation(program.glName(), "meshmax");
	glUniform3fv(location,1, (GLfloat*)&m_max);

}

    void LatticeNode::move(glm::vec2 dydx, glm::mat4 rotationmat, float m_scale, float depth){

    	printf("depth %f\n", depth);

    	dydx.x/=m_scale;
    	dydx.y/=m_scale;

    	dydx.x*=depth*3;
    	dydx.y*=depth*3;

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

void Lattice::draw(cgra::Program useProgram,glm::mat4 modTransform,glm::mat4 rotMat, glm::mat4 modelTrans,float m_scale  ){

    useProgram.use();
    GLuint loc = glGetUniformLocation(useProgram.glName(), "gColor");

    GLfloat idColor[4];
    
    idColor[0] = 255;
    idColor[1] = 255;
    idColor[2] = 255;
    idColor[3] = 1.0;
    glUniform4fv(loc, 1, idColor);

	for(int i = 0; i <m_nodes.size(); i++){

		if (!m_nodes[i].isEnd||showEnds)
		{

		glm::vec3 p = m_nodes[i].p;
		glm::vec4 tp = glm::vec4(p.x,p.y,p.z,1.);		//Transform pipeline
	
		glm::mat4 nodeTransform(1.0f);
    	glm::mat4 markerTransform(1.0f);

		nodeTransform*=modTransform;
		nodeTransform *= glm::translate(glm::mat4(1.),glm::vec3(tp.x,tp.y,tp.z));
		nodeTransform *= glm::scale(glm::mat4(1.),glm::vec3(handleSize));

   		useProgram.setModelMatrix(nodeTransform);

    	m_nodes[i].draw();
    	}
    }

   	latProgram.setModelMatrix(modTransform);
    latticeMesh.draw(GL_LINES);
}

void Lattice::drawForPick(cgra::Program useProgram,glm::mat4 modTransform,glm::mat4 rotMat, glm::mat4 modelTrans,float m_scale  ){

     useProgram.use();
    GLuint loc = glGetUniformLocation(
    useProgram.glName(), "gColor");
    glUniform1i(loc,-1);


   	GLfloat idColor[4];
	for(int i = 0; i <m_nodes.size(); i++){

		if (!m_nodes[i].isEnd||showEnds)
		{

		glm::vec3 p = m_nodes[i].p;							//Transform pipeline
		glm::vec4 tp = glm::vec4(p.x,p.y,p.z,1.);
	
		glm::mat4 nodeTransform(1.0f);
    	glm::mat4 markerTransform(1.0f);

		nodeTransform*=modTransform;
		nodeTransform *= glm::translate(glm::mat4(1.),glm::vec3(tp.x,tp.y,tp.z));
		nodeTransform *= glm::scale(glm::mat4(1.),glm::vec3(handleSize));

   		useProgram.setModelMatrix(nodeTransform);

//Color Picking
	idColor[0] = glm::mod(float(m_nodes[i].ojID),float(256));
	idColor[1] = floor(glm::mod(float(m_nodes[i].ojID),float(256*256))/256.0f);
    idColor[2] = m_nodes[i].ojID/float(256*256);
    idColor[3] = 1.0;
    GLuint loc = glGetUniformLocation(
    useProgram.glName(), "gColor");
    glUniform4fv(loc, 1, idColor);


    	m_nodes[i].draw();
    }

    idColor[0] = 255;
    idColor[1] = 255;
    idColor[2] = 255;
    idColor[3] = 1.0;
    glUniform4fv(loc, 1, idColor);
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

cgra::Mesh Lattice::makeWarpMesh( cgra::Mesh &refMesh){

	warpMesh = cgra::Mesh(refMesh);
int vcount = refMesh.m_vertices.size();

	cgra::Matrix<double> newVertices(vcount,3);
	printf("refmesh %i\n", vcount);

	glm::vec3 grab = refMesh.m_vertices[vcount/2].m_position;
	printf("median vertex %f %f %f\n", grab.x,grab.y,grab.z);
	for (int i = 0 ; i < refMesh.m_vertices.size(); i++){
		grab = refMesh.m_vertices[i].m_position;
		//grab = pointfromVolume(grab, m_resolution.x, m_resolution.y, m_resolution.z, *(this));

		newVertices.setRow(i, {grab.x, grab.y, grab.z});

	}

	// copy more indices, again.. (Why??????)
	cgra::Matrix<unsigned int> moreIndices(refMesh.m_indices.size()/3,3);
	for (int i = 0 ; i < moreIndices.numRows(); i++){
		moreIndices.setRow(i,{  refMesh.m_indices[i*3],
								refMesh.m_indices[i*3+1],
								refMesh.m_indices[i*3+2] });
	}

	warpMesh.setData(newVertices,moreIndices);

	return warpMesh;
}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
///////////////////////////warper////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////



glm::vec3 interpolate(float t,glm::vec3 spline[], int res, int uTechID){
	
	if (uTechID == 0) { //Linear Interpolate

		//int id = int(floor(min(t,0.999)*float(res-1.0)));
		//int id = int(floor(t));
		int id = int(floor(t*res));
		/*
		vec3 p0 = spline[id];
		vec3 p1 = spline[id+1];*/
		glm::vec3 p0,p1;
		if (t<0.33333){
			 p0 = spline[0];
			 p1 = spline[1];
		}else if (t<0.6666){
			 p0 = spline[1];
			 p1 = spline[2];
		}else{
			 p0 = spline[2];
			 p1 = spline[3];
		}

//		float tsub = (res-1)*glm::mod(t,1./(res-1));
		float tsub = t * (res-1) - glm::floor(t * (res-1));//

		//return vec3(tsub);
		return glm::vec3(tsub*p1.x+(1.-tsub)*p0.x, //+ vec3(sin(tsub*10));
		 					tsub*p1.y+(1.-tsub)*p0.y,
		 					tsub*p1.z+(1.-tsub)*p0.z);
		 //return vec3(mod(t*4,2));
		}
	else if(uTechID == 2){ //Bezier


	}

    return glm::vec3(0);

}


		/*for(int i = 0; i < xres-1; i++){
			for(int j = 0; j < yres-1; j++){
				for(int k = 0; k < zres-1; k++){s
					if 
				}
			}	
		}
	}  */


glm::vec3 pointfromVolume(glm::vec3 t, int xres, int yres, int zres, Lattice & someLattice) {

	glm::vec3 volumeSpline[MAX_SPLINESIZE];
	
	for (int i = 0; i <  xres; i++){         // X AXIS
		
		glm::vec3 patchSpline[MAX_SPLINESIZE];
			for (int j = 0; j <  yres; j++){

				glm::vec3 atomicSpline[MAX_SPLINESIZE];
				for (int k = 0; k < zres; k++) {
					atomicSpline[k] = someLattice.getNode(i,j,k).p;
				}// build a spline for this patchpoint
	
				patchSpline[i] = interpolate(t.y,atomicSpline,zres,0);  // Z axis lerp
		}//build a patch spline

		volumeSpline[i] = interpolate(t.x, patchSpline,yres,0);   // Y axis lerp

	}// get a patchvec3 mySpline[MAX_SPLINESIZE];

	return interpolate(t.x,volumeSpline,xres,0);

	printf("volpoint!\n");
	}

glm::vec3 pointfromPatch(glm::vec2 t, glm::vec3 patch[MAX_SPLINESIZE*MAX_SPLINESIZE], int yr, int zr){

	glm::vec3 mySpline[MAX_SPLINESIZE];
	
	for (int i = 0; i <  yr; i++){
	glm::vec3 zSpline[MAX_SPLINESIZE];
		for (int j = 0; j < zr; j++) {
			zSpline[j] = patch[i*zr+j];
		}// build a spline for this patchpoint
	
	mySpline[i] = interpolate(t.y,zSpline,zr,0);
	}//build a patch spline

	return interpolate(t.x, mySpline,yr,0);
}































