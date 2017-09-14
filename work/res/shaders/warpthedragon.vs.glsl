#version 150 core

#define MAX_MESHARRAYSIZE 3072
#define MAX_SPLINESIZE 20

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;

uniform int uTechID;

uniform int xres, yres, zres;

uniform float xsize,ysize,zsize;

uniform float latticeVerts[MAX_MESHARRAYSIZE];


in vec3 vertPosition;
in vec3 vertNormal;

out vec3 fragPosition;
out vec3 fragNormal;



vec3 interpolate(float t,vec3 spline[MAX_SPLINESIZE], int res){
	
	if (uTechID == 0) { //Interpolate

		vec3 p0 = spline[int(floor(t*float(res)))];
		vec3 p1 = spline[int(ceil(max(t,0.999)*float(res)))];

		float tsub = t*(float(res)-floor(t*float(res)));

		return tsub*p0+(1-tsub)*p1;
		}
	else if(uTechID == 2){ //Bezier


	}else return vec3(0);

}


		/*for(int i = 0; i < xres-1; i++){
			for(int j = 0; j < yres-1; j++){
				for(int k = 0; k < zres-1; k++){
					if 
				}
			}	
		}
	}  */


vec3 pointfromVolume(vec3 t, float latticeArray[MAX_MESHARRAYSIZE], int xr, int yr, int zr) {

	vec3 mySpline[MAX_SPLINESIZE];
	for (int i = 0; i <  yr; i++){}

	return vec3(1);
}
vec3 pointfromPatch(vec2 t, vec3 patch[MAX_SPLINESIZE*MAX_SPLINESIZE], int yr, int zr){

	vec3 mySpline[MAX_SPLINESIZE];
	
	for (int i = 0; i <  yr; i++){
	vec3 zSpline[MAX_SPLINESIZE];
		for (int j = 0; j < zr; j++) {
			zSpline[j] = patch[i*zr+j];
		}// build a spline for this patchpoint
	
	mySpline[i] = interpolate(t.y,zSpline,zr);
	}//build a patch spline

	return interpolate(t.x, mySpline,yr);
}


void main() {

	vec3 sample = vertPosition;

	//vec3 dimension = vec(xsize,ysize,zsize);
	vec3 dimension = vec3(4.0,3.0,2.0);

	vec3 testSpline[MAX_SPLINESIZE];
	testSpline[0]=vec3(-1,0.,0.5),
	testSpline[1]=vec3(-0.5,0.5,0.5);
	testSpline[2]=vec3(0.5,1.,-0.5);
	testSpline[3]=vec3(1.,0.,0.5);
						

	vec3 testWiggle = interpolate(sample.x/5.0,testSpline,4);
	testWiggle*=dimension;

	vec3 volumePosition = pointfromVolume(vertPosition,latticeVerts,xres,yres,zres);

	vec3 newPosition = vertPosition * volumePosition;

	newPosition += testWiggle;

	// Standard projworldmodel transformations;
    gl_Position = projectionMat * viewMat * modelMat * vec4(newPosition, 1.0);

    vec4 pos = viewMat * modelMat * vec4(vertPosition, 1.0);
    fragPosition = vec3(pos) / pos.w;

    mat3 normalMat = transpose(inverse(mat3(viewMat * modelMat)));
    fragNormal = normalMat * vertNormal;
}
