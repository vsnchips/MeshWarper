#version 150 core
#pragma glsl
#pragma target 3.0

#define MAX_MESHARRAYSIZE 1000
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

uniform float uCatRomLooseNess = 0.2;

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
}

vec3 cubicBezier( float t,vec3 p0, vec3 p1, vec3 p2, vec3 p3){
	return pow(1-t,3)*p0 + 3*pow(1-t,2)*pow(t,1)*p1 + 3*pow(1-t,1)*pow(t,2)*p2 + pow(t,3)*p3;
}
vec3 deCasteljau(float t, vec3 spline[MAX_SPLINESIZE], int res)
	  {
		//ITERATIVE DE-CASTLEJAU

		int i = res -1;
		while (i >0){
			for (int j=0; j<=i; j++){
				spline[j] = mix (spline[j],spline[j+1],t);

		} i--;
	  }

		return spline[0];

	}


vec3 cubicHermite (float t,vec3 p0, vec3 p1, vec3 p2, vec3 p3, float loose){

	vec3 herm[MAX_SPLINESIZE];
		herm[0]= p1;
		herm[1]= p1 + loose*(p2-p0);
		herm[2]=p2 + loose*(p1-p3);
		herm[3]=p2;
	//return cubicBezier(

	return deCasteljau(t, herm,4);
}
//CatMul Rom
vec3 CatMullRom(float t,vec3 spline[MAX_SPLINESIZE], int segments){
		int id = int(floor(t*segments)); // pick a segment
		float tsub = (segments)*mod(t,1./(segments));
		return cubicHermite( tsub, spline[id],spline[id+1],spline[id+2],spline[id+3], uCatRomLooseNess);

}



vec3 spline(float t,vec3 spline[MAX_SPLINESIZE], int res){
	
	if (uTechID == 2 )
	{
		return CatMullRom(t, spline, res -3);
	}

	if (uTechID == 1)// Bezier;
		return deCasteljau(t,spline,res);


	//Linear Interpolate

		int id = int(floor(t*(res-1)));
		
		vec3 p0 = spline[id];
		vec3 p1 = spline[id+1];
		

		float tsub = (res-1)*mod(t,1./(res-1));
		 //return tsub*p1+(1.-tsub)*p0; 
		return mix (p0,p1,tsub);

}


vec3 pointfromVolume(vec3 t, int xr, int yr, int zr) {

	vec3 volumeSpline[MAX_SPLINESIZE];
	
	int s = 0;
	if (uTechID==2)
	{ s=-1; xr+=2; yr+=2; zr+=2;}
	
	for (int i = s; i < xr+s; i++){         // X AXIS
		
		vec3 patchSpline[MAX_SPLINESIZE];
			for (int j = s; j < yr+s; j++){

				vec3 atomicSpline[MAX_SPLINESIZE];
				for (int k = s; k < zr+s; k++) {
					atomicSpline[k-s] = getVecfromArray(i,j,k);
				}// build a spline for this patchpoint
	
				patchSpline[j-s] = spline(t.z,atomicSpline,zr);  // Z axis lerp
		}//build a patch spline

		volumeSpline[i-s] = spline(t.y, patchSpline,yr);   // Y axis lerp

	}// build a volumetric spline;

	return spline(t.x,volumeSpline,xr);

	}

void main() {

	vec3 span = meshmax-meshorigin;

	vec3 sample = (vertPosition-meshorigin)/span;

	vec3 volumePosition = pointfromVolume(sample,xres,yres,zres);

	vec3 finalPos = vertPosition + latticeVerts[41];

	/// Splines all the way down

	// Standard projworldmodel transformations;
    gl_Position = projectionMat * viewMat * modelMat * vec4(volumePosition, 1);

    vec4 pos = viewMat * modelMat * vec4(vertPosition, 1.0);
    fragPosition = vec3(pos) / pos.w;

    mat3 normalMat = transpose(inverse(mat3(viewMat * modelMat)));
    fragNormal = normalMat * vertNormal;
}