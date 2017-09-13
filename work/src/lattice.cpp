
#include "lattice.hpp"


Lattice::Lattice(){};

Lattice::Lattice(
glm::vec3 min,
glm::vec3 max,
glm::vec3 res)
: m_min(min), m_max(max),
m_resolution(res) {


// Calculate number of
// nodes needed.
int num = res.x*res.y*res.z;
vao = cgra::Matrix<double> (num,3);
int linetotal = (res.x*res.y*(res.z-1) +
				 res.y*res.z*(res.x-1) +
				 res.z*res.x*(res.y-1) );
printf("\nlinetotal:%d\n", linetotal);

lineids = cgra::Matrix<unsigned int> (linetotal,2);

//Fill the vao

printf("constructing mesh with x: %f y %f z: %f\n", res.x, res.y, res.z );
int ind = 0;
for (int i = 0; i < (int)res.x; i++) {
	float x = min.x+i*(max.x-min.x)/res.x;
	for (int j = 0; j < (int)res.y; j++) {
		float y = min.y+j*(max.y-min.y)/res.y;
		for (int k = 0; k < (int)res.z; k++) {
			// Create a LatticeNode
			// and add it to m_nodes
			LatticeNode node = LatticeNode(k,glm::vec3(  x  ,  y  ,  min.z+(max.z-min.z)*k/res.z   ));
			m_nodes.push_back(node);

			printf("setting mesh row %f %f %f \n",   (double)x , (double)y  ,  (double)(min.z+(max.z-min.z)*k/res.z)  );
			vao.setRow(ind,{(double)x,(double)y,(double)(min.z+(max.z-min.z)*k/res.z)});

			printf("vao x: %f\n", vao.m_data[(k+j*res.z+i*res.y*res.z)*2]); 
			ind++;
		} 
	} // Initial points set
}


// Connect lines

ind = 0;
int lct = 0;
for (int i = 0; i < (int)res.x; i++) {
	float x = min.x+i*(max.x-min.x);
	for (int j = 0; j < (int)res.y; j++) {
		float y = min.y+i*(max.y-min.y);
		for (int k = 0; k < (int)res.z; k++) {
			if (k<res.z-1) 	{
				lineids.setRow(lct,{ind,ind+1});
				printf("setting line %d : %d %d\n", lct, ind, ind+1);

			printf("lineids z: %d , %d\n", lineids.m_data[(k+j*res.z+i*res.y*res.z)*2], lineids.m_data[(k+j*res.z+i*res.y*res.z)*2+1]); 
			
				 lct++;
				}
			if (j<res.y-1) 	{
				lineids.setRow(lct,{ind,ind+res.z});
				printf("setting line %d : %d %d\n", lct, ind, ind+1);
			printf("lineids y: %d\n", lineids.m_data[(k+j*res.z+i*res.y*res.z)*2]); 
				 lct++;
				}
			if (i<res.x-1) 	{
				lineids.setRow(lct,{ind,ind+res.y*res.z});
				printf("setting line %d : %d %d\n", lct, ind, ind+1);
			printf("lineids x: %d\n", lineids.m_data[(k+j*res.z+i*res.y*res.z)*2]); 
				 lct++;
				}
				ind++;
		} 
	}

 }

setMesh();
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

LatticeNode::LatticeNode(int k, glm::vec3 np){
	ojID = k;
	p = np;
}