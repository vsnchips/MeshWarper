#version 150 core

#define MAX_MESHARRAYSIZE 3072
#define MAX_SPLINESIZE 20

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;

uniform vec3 meshorigin;

uniform int uTechID;

uniform int xres, yres, zres;

uniform float xsize,ysize,zsize;

uniform float latticeVerts[MAX_MESHARRAYSIZE];


in vec3 vertPosition;
in vec3 vertNormal;

out vec3 fragPosition;
out vec3 fragNormal;



vec3 interpolate(float t,vec3 spline[MAX_SPLINESIZE], int res){
	
	if (uTechID == 0) { //Linear Interpolate

		//int id = int(floor(min(t,0.999)*float(res-1.0)));
		//int id = int(floor(t));
		int id = int(floor(t*res));
		/*
		vec3 p0 = spline[id];
		vec3 p1 = spline[id+1];*/
		vec3 p0,p1;
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

		//float tsub = (t*float(res))-float(floor(t*float(res)));
		float tsub = (res-1)*mod(t,1./(res-1));
		//return vec3(tsub);
		 return tsub*p1+(1.-tsub)*p0; //+ vec3(sin(tsub*10));
		 //return vec3(mod(t*4,2));
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

	vec3 meshorigin = vec3(-3.5,0,-.1);
	vec3 sample = vertPosition-meshorigin;

	//vec3 dimension = vec(xsize,ysize,zsize);
	vec3 dimension = vec3(4.0,3.0,2.0);

	vec3 testSpline[MAX_SPLINESIZE];
	testSpline[0]=vec3(-3,0.,0.8);
	testSpline[1]=vec3(-2.5,0,0.8);
	testSpline[2]=vec3(4.5,-1.,-.1);
	testSpline[3]=vec3(6.,0.,0.5);
						

	vec3 testWiggle = interpolate(float(sample.x)/6,testSpline,4);
	//testWiggle*=dimension;
	testWiggle*=1;

	//vec3 volumePosition = pointfromVolume(vertPosition,latticeVerts,xres,yres,zres);

	vec3 newPosition = vertPosition; //* volumePosition;
	newPosition.x = testWiggle.x;

	//newPosition.yz += testWiggle.yz;// + vec3(sin(gl_Position*5.));
	//newPosition.x = testWiggle.x;


	// Standard projworldmodel transformations;
    gl_Position = projectionMat * viewMat * modelMat * vec4(newPosition, 1.0);

    vec4 pos = viewMat * modelMat * vec4(vertPosition, 1.0);
    fragPosition = vec3(pos) / pos.w;// + vec3(sin(gl_Position*5.));

    mat3 normalMat = transpose(inverse(mat3(viewMat * modelMat)));
    fragNormal = normalMat * vertNormal;
}
