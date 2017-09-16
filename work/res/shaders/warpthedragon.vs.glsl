#version 150 core
#pragma glsl
#pragma target 3.0

#define MAX_MESHARRAYSIZE 960
#define MAX_SPLINESIZE 20

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;

uniform vec3 meshorigin;
uniform vec3 meshmax;

uniform vec3 latticeVerts[MAX_MESHARRAYSIZE]; 
//uniform float latticeYVerts[MAX_MESHARRAYSIZE];
//uniform float latticeZVerts[MAX_MESHARRAYSIZE];

uniform int uTechID;

uniform int xres;
uniform int yres;
uniform int zres;


in vec3 vertPosition;
in vec3 vertNormal;

out vec3 fragPosition;
out vec3 fragNormal;

vec3 getVecfromArray(int x, int y, int z){
	int i = (z+1)+ (zres+2)*(y+1) + (zres+2)*(yres+2)*(x+1); 
	return latticeVerts[i];
				//latticeYVerts[i],				latticeZVerts[i]);


}

vec3 interpolate(float t,vec3 spline[MAX_SPLINESIZE], int res){
	
	//if (uTechID == 0) { //Linear Interpolate

		//int id = int(floor(min(t,0.999)*float(res-1.0)));
		//int id = int(floor(t));
		int id = int(floor(t*(res-1)));
		
		vec3 p0 = spline[id];
		vec3 p1 = spline[id+1];
		
		/*if (t<0.5){
			 p0 = spline[0];
			 p1 = spline[1];
		}else {
			 p0 = spline[1];
			 p1 = spline[2];
		}/*else{
			 p0 = spline[2];
			 p1 = spline[3];
		}*/

		//float tsub = (t*float(res))-float(floor(t*float(res)));
		float tsub = (res-1)*mod(t,1./(res-1));
		//return vec3(tsub);
		 return tsub*p1+(1.-tsub)*p0; //+ vec3(sin(tsub*10));
		 //return vec3(mod(t*4,2));
		//}
	//else if(uTechID == 2){ //Bezier


	//}else return vec3(0);

}


		/*for(int i = 0; i < xres-1; i++){
			for(int j = 0; j < yres-1; j++){
				for(int k = 0; k < zres-1; k++){
					if 
				}
			}	
		}
	}  */


vec3 pointfromVolume(vec3 t, int xr, int yr, int zr) {

	vec3 volumeSpline[MAX_SPLINESIZE];
	
	for (int i = 0; i < xr; i++){         // X AXIS
		
		vec3 patchSpline[MAX_SPLINESIZE];
			for (int j = 0; j < yr; j++){

				vec3 atomicSpline[MAX_SPLINESIZE];
				for (int k = 0; k < zr; k++) {
					atomicSpline[k] = getVecfromArray(i,j,k);
				}// build a spline for this patchpoint
	
				patchSpline[j] = interpolate(t.z,atomicSpline,zr);  // Z axis lerp
		}//build a patch spline

		volumeSpline[i] = interpolate(t.y, patchSpline,yr);   // Y axis lerp

	}// get a patchvec3 mySpline[MAX_SPLINESIZE];

	return interpolate(t.x,volumeSpline,xr);

	}

void main() {

	vec3 span = meshmax-meshorigin;
	vec3 sample = 0.99*(vertPosition-meshorigin)/span;
	//sample = (vertPosition-meshorigin)/15;

	//vec3 dimension = vec(xsize,ysize,zsize);
	//vec3 dimension = vec3(4.0,3.0,2.0);

	vec3 volumePosition = pointfromVolume(sample,xres,yres,zres);

	vec3 finalPos = vertPosition + latticeVerts[41];

	/// Splines all the way down, behbeh


	//newPosition.yz += testWiggle.yz;// + vec3(sin(gl_Position*5.));
	//newPosition.x = testWiggle.x;


	// Standard projworldmodel transformations;
    gl_Position = projectionMat * viewMat * modelMat * vec4(volumePosition, 1);

    vec4 pos = viewMat * modelMat * vec4(vertPosition, 1.0);
    fragPosition = vec3(pos) / pos.w;

    mat3 normalMat = transpose(inverse(mat3(viewMat * modelMat)));
    fragNormal = normalMat * vertNormal;
}

	//vec3 volumePosition = pointfromVolume(vertPosition,latticeVerts,xres,yres,zres);
