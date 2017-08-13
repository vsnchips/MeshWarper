#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <cstring>


#include "opengl.hpp"
#include "imgui.h"

#include "cgra/matrix.hpp"
#include "cgra/wavefront.hpp"

#include "ex1.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "math.h"


void Application::init() {
    // Load the shader program
    // The use of CGRA_SRCDIR "/path/to/shader" is so you don't
    // have to run the program from a specific folder.
    m_program = cgra::Program::load_program(
        CGRA_SRCDIR "/res/shaders/simple.vs.glsl",
        CGRA_SRCDIR "/res/shaders/simple.fs.glsl");

    // Create a view matrix that positions the camera
    // 10 units behind the object
    glm::mat4 viewMatrix(1);
    viewMatrix[3] = glm::vec4(0, 0, -10, 1);
    m_program.setViewMatrix(viewMatrix);

    xax = glm::vec3(1.,0.,0.);
    yax = glm::vec3(0.,1.,0.);
    zax = glm::vec3(0.,0.,1.);

    printf("\nxax: %f,%f,%f",xax.x,xax.y,xax.z);
    printf("\nyax: %f,%f,%f",yax.x,yax.y,yax.z);
    printf("\nzax: %f,%f,%f",zax.x,zax.y,zax.z);
    // Create the cube mesh
    createCube();
}

void Application::createCube() {
    /************************************************************
     * 2. Create a Mesh                                         *
     *                                                          *
     * Add the remaining triangles for the cube                 *
     ************************************************************/

    // Use the correct number of rows for the full
    // cube.
    cgra::Matrix<double> vertices(8, 3);
    cgra::Matrix<unsigned int> triangles(12, 3);

    vertices.setRow(0, {  1.0,  1.0,  1.0 });
    vertices.setRow(1, { -1.0, -1.0,  1.0 });
    vertices.setRow(2, {  1.0, -1.0,  1.0 });

    vertices.setRow(3, { -1.0, 1.0,  1.0 });
    vertices.setRow(4, { 1.0, 1.0,  -1.0 });
    vertices.setRow(5, { -1.0, -1.0,  -1.0 });

    vertices.setRow(6, { 1.0, -1.0,  -1.0 });
    vertices.setRow(7, { -1.0, 1.0,  -1.0 });

    // Remember to make sure that the order of the vertices
    // is counter-clockwise when looking at the front of the
    // triangle.
    triangles.setRow(0, { 0, 1, 2 });
    triangles.setRow(1, { 0, 3, 1 });
    triangles.setRow(2, { 4, 6, 5 });
    triangles.setRow(3, { 4, 5, 7 });
    triangles.setRow(4, { 0, 2, 6 });
    triangles.setRow(5, { 0, 6, 4 });
    triangles.setRow(6, { 3, 5, 1 });
    triangles.setRow(7, { 3, 7, 5 });
    triangles.setRow(8, { 0, 7, 3 });
    triangles.setRow(9, { 0, 4, 7 });
    triangles.setRow(10, { 2, 1, 5 });
    triangles.setRow(11, { 2, 5, 6 });

    m_mesh.setData(vertices, triangles);
}

void Application::loadObj(const char *filename) {
    cgra::Wavefront obj;
    // Wrap the loading in a try..catch block
    try {
        cgra::Wavefront obj = cgra::Wavefront::load(filename);
    } catch (std::exception e) {
        std::cerr << "Couldn't load file: '" << e.what() << "'" << std::endl;
        return;
    }

    /************************************************************
     * 2. Create a Mesh                                         *
     *                                                          *
     * Use the data in `obj` to create appropriately-sized      *
     * vertex and index matrices and fill them with data.       *
     *                                                          *
     * Assume that all the faces in the file are triangles.     *
     ************************************************************/

    // Replace these with appropriate values
    int numVertices  = obj.m_positions.size();
    int numTriangles = obj.m_faces.size();
    printf("\nVerts %u, Tris %u,\n", numVertices, numTriangles);

    cgra::Matrix<double> vertices(numVertices, 3);
    cgra::Matrix<unsigned int> triangles(numTriangles, 3);

    for (size_t i = 0; i < obj.m_positions.size(); i++) {
        // Add each position to the vertices matrix
        for(int i = 1; i <= numVertices;i++) vertices.setRow(i-1,{obj.position(i).x,obj.position(i).y,obj.position(i).z});
    }

    for (size_t i = 0; i < obj.m_faces.size(); i++) {
        // Add each triangle's indices to the triangles matrix
        // Remember that Wavefront files use indices that start at 1
        for(int i = 0; i < numTriangles;i++) triangles.setRow(i,{obj.m_faces[i].m_vertices[0].m_p-1,
                                                                 obj.m_faces[i].m_vertices[1].m_p-1,
                                                                 obj.m_faces[i].m_vertices[2].m_p-1});
    }

    m_mesh.setData(vertices, triangles);
}

void Application::drawScene() {
    // Calculate the aspect ratio of the viewport;
    // width / height
    float aspectRatio = m_viewportSize.x / m_viewportSize.y;
    // Calculate the projection matrix with a field-of-view of 45 degrees
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

    // Set the projection matrix
    m_program.setProjectionMatrix(projectionMatrix);

    glm::mat4 modelTransform(1.0f);

    /************************************************************
     * 3. Manual Transforms                                     *
     *                                                          *
     * Use `m_translation`, `m_scale`, and `m_rotationMatrix`   *
     * to create the  `modelTransform` matrix.                  *
     * The following glm functions will be useful:              *
     *    `glm::translate`                                      *
     *    `glm::scale`                                          *
     ************************************************************/

    static bool show_app_manualtransforms = false;

   // ImGui::Begin("Manual Transforms",&show_app_manualtransforms,ImVec2(250,200),1.f,0);
    ImGui::Begin("Manual Transforms",&show_app_manualtransforms,ImVec2(250,200));
    //ImGui::Begin("Manual Transforms");

    ImGui::SliderFloat3("Translate",&m_translation[0],-10.0f,10.0f, "%.5f",1.5f);
    ImGui::SliderFloat("Scale",&m_scale,-5.0f,5.0f, "%.5f", 1.5f);
    if(ImGui::SliderFloat3("Rotate",&polarrotation[0],-M_PI,M_PI, "%.5f", 1.0f)){
        // User's spun the globe
        // Find the resulting matrix!

        //1. Transform the Z/north pole, X/west, and Y/celestial vectors via the input lat/long TODO: How do i get this into a mat3x3?
        zax = glm::rotate(
                    (glm::rotate(glm::vec3(0.,0.,1.),polarrotation.x,glm::vec3(0.,1.,0.))) // tilt it on Y over to X to latitude
                    ,polarrotation.y, glm::vec3(0.,0.,1.));  // spin it on true Zorth to longtitude

       /* yax = glm::rotate(
                    glm::vec3(0.,1.,0.) // tilt it on Y over to X to latitude
                    ,polarrotation.y, glm::vec3(0.,0.,1.));  // spin it on true Zorth to longtitude

        xax = glm::rotate(
                    (glm::rotate(glm::vec3(1.,0.,0.),polarrotation.x,glm::vec3(0.,1.,0.))) // tilt it on Y over to X to latitude
                    ,polarrotation.y, glm::vec3(0.,0.,1.));  // spin it on true Zorth to longtitude

        */

        //2.Find the normal and angle between Zorth and the new Z, and apply the same rotation to Xwest and YCelestial
        glm::vec3 tnorm = glm::cross(glm::vec3(0.,0.,1.),zax);
        yax = glm::rotate(glm::vec3(0.,1.,0.),polarrotation.x,tnorm);
        xax = glm::rotate(glm::vec3(1.,0.,0.),polarrotation.x,tnorm);


        //3. Rotate X and Y around the tilted Z pole/
        yax = glm::rotate(yax, polarrotation.z, zax);
        xax = glm::rotate(xax, polarrotation.z, zax);

    };
    ImGui::End();


    glm::mat4 basevecs = glm::mat4(glm::vec4(xax,0),glm::vec4(yax,0),glm::vec4(zax,0),glm::vec4(0.f,0.f,0.f,1.f));
    //m_rotationMatrix = glm::rotate(glm::mat4(1.0),manrotate.z,zax);
    m_rotationMatrix = glm::rotate(basevecs,manrotate.z,zax);
    m_rotationMatrix = glm::rotate(m_rotationMatrix,manrotate.y,yax);
    m_rotationMatrix = glm::rotate(m_rotationMatrix,manrotate.x,xax);

    modelTransform *= glm::translate(glm::mat4(),m_translation);
    modelTransform *= glm::scale(modelTransform,glm::vec3(m_scale));

    modelTransform *= m_rotationMatrix;


    m_program.setModelMatrix(modelTransform);

    // Draw the mesh
    m_mesh.draw();
}

void Application::doGUI() {
    ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Shapes");

    /************************************************************
     *                                                          *
     * 2. Load an .obj file                                     *
     *                                                          *
     * Add an input for a filename.                             *
     *                                                          *
     ************************************************************
     *                                                          *
     * 3. Manual Transforms                                     *
     *                                                          *
     * Create inputs for controlling translation, scale and     *
     * rotation.                                                *
     *                                                          *
     ************************************************************
     *                                                          *
     * 5. Add a checkbox for rendering the object in wireframe  *
     *  mode.                                                   *
     ************************************************************/
    static bool wireframe;
    if(ImGui::Button("WireFrame ?")) {
        wireframe = !wireframe;
        m_mesh.setDrawWireframe(wireframe);
}

    static char dragon[] = "../../meshes/stanford_dragon/dragon.obj";

    static char path[256];

    if (std::strlen(path)<1) std::strcpy(path,dragon);

    if (ImGui::InputText("Load *.OBJ",path, 256)){

        FILE * file = fopen(path, "r");
        if( file == NULL ){
            printf("Impossible to open the file !\n");
            //return false;
        }else {
            printf("Loading %s \n", path);

            //loadObj(path);   // The CGRA wavefront loader isnt working for me

            // So I wrote this one:

            std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
            std::vector< double > temp_vertices;
            std::vector< double > temp_uvs;
            std::vector< double > temp_normals;

            while( 1 ){

                char lineHeader[128];
                // read the first word of the line
                int res = fscanf(file, "%s", lineHeader);
                if (res == EOF)
                    break;

            if ( strcmp( lineHeader, "v" ) == 0 ){
                float vertexx;
                float vertexy;
                float vertexz;
                fscanf(file, "%f %f %f\n", &vertexx, &vertexy, &vertexz );
                temp_vertices.push_back(vertexx);
                temp_vertices.push_back(vertexy);
                temp_vertices.push_back(vertexz);
            }else if ( strcmp( lineHeader, "vt" ) == 0 ){
                float uvx;
                float uvy;
                fscanf(file, "%f %f\n", &uvx, &uvy );
                temp_uvs.push_back(uvx);
                temp_uvs.push_back(uvy);
            }else if ( strcmp( lineHeader, "vn" ) == 0 ){
                float normalx;
                float normaly;
                float normalz;
                fscanf(file, "%f %f %f\n", &normalx, &normaly, &normalz );
                temp_normals.push_back(normalx);
                temp_normals.push_back(normaly);
                temp_normals.push_back(normalz);

            }else if ( strcmp( lineHeader, "f" ) == 0 ){
                std::string vertex1, vertex2, vertex3;
                unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                //int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                int matches = fscanf(file, "%u %u %u\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
                if (matches != 3){
                    printf("Cant read this .obj");
                   // return false;
                }
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                uvIndices    .push_back(uvIndex[0]);
                uvIndices    .push_back(uvIndex[1]);
                uvIndices    .push_back(uvIndex[2]);
                normalIndices.push_back(normalIndex[0]);
                normalIndices.push_back(normalIndex[1]);
                normalIndices.push_back(normalIndex[2]);
            }


        }

            cgra::Matrix<double> verts(temp_vertices.size()/3,3);
            for(unsigned int i =0; i < temp_vertices.size()/3; i++) verts.setRow(i,{temp_vertices[i*3],temp_vertices[i*3+1],temp_vertices[i*3+2]});

            cgra::Matrix<unsigned int> faces(vertexIndices.size()/3,3);
            for(unsigned int i=0; i< vertexIndices.size()/3;i++) faces.setRow(i,{vertexIndices[i*3]-1,vertexIndices[i*3+1]-1,vertexIndices[i*3+2]-1});


            int numVertices  = temp_vertices.size()/3;
            int numTriangles = vertexIndices.size()/3;
            printf("Verts %u, Tris %u,\n", numVertices, numTriangles);

     m_mesh.setData(verts,faces);

     } //endif loading
    }//endif textinput


    //debugging stuff:

    ImGui::Text("tricount%d : " , m_mesh.m_indices.size()/3);
    ImGui::Text("vertcount%d : " , m_mesh.m_vertices.size());

    ImGui::End();
}


// Input Handlers

void Application::onMouseButton(int button, int action, int) {
    if (button >=0 && button < 3) {
        // Set the 'down' state for the appropriate mouse button
        m_mouseButtonDown[button] = action == GLFW_PRESS;
    }
}

void Application::onCursorPos(double xpos, double ypos) {
    /**********************************************************
     * 4. Interactive Transforms                              *
     *                                                        *
     * Change `translation`, `scale` and `rotation` based on  *
     * `mousePositionDelta`.                                  *
     **********************************************************/

    // Make a vec2 with the current mouse position
    glm::vec2 currentMousePosition(xpos, ypos);

    // Get the difference from the previous mouse position
    glm::vec2 mousePositionDelta = currentMousePosition - m_mousePosition;

  //  glm::mat4 rotmat =  glm::rotate(glm::mat4(),60.0f, glm::vec3( mousePositionDelta.x,mousePositionDelta.y,0.5));
    //m_rotationMatrix *= rotmat;

    static bool click=false;
    static glm::vec3 sxa, sya, sza;
    static glm::vec2 fclick;

    if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_LEFT]) {
        //glm::vec3 axis = glm::inverse(m_rotationMatrix) * glm::vec3(0,1,0);
        static int width, height;
        glfwGetWindowSize(m_window, &width, &height);
        if (!click) {
            printf("Clicked\n");
            click = true;


            fclick = glm::vec2(-1+2*currentMousePosition.x/width,-1+2*currentMousePosition.y/height);
            printf("currentMouse: %lf, %lf",fclick.x,fclick.y);
            sxa = xax;
            sya = yax;
            sza = zax;
            printf("sxa : %lf,%lf,%lf",sxa.x,sxa.y,sxa.z);
        }

        glm::vec2 nowpos = glm::vec2(-1+2*currentMousePosition.x/width,-1+2*currentMousePosition.y/height);
        glm::vec4 apA = glm::vec4(fclick.x,-fclick.y,glm::cos(glm::asin(glm::min(glm::length(fclick),1.0f))),1.);
        glm::vec4 apB = glm::vec4(nowpos.x,-nowpos.y,glm::cos(glm::asin(glm::min(glm::length(nowpos),1.0f))),1.);


        printf("apA : %lf,%lf,%lf,%lf",apA.x,apA.y,apA.z,apA.w);
        printf("apB : %lf,%lf,%lf,%lf",apB.x,apB.y,apB.z,apB.w);

        //apA=glm::normalize(apA);
        //apB=glm::normalize(apB);

        glm::vec3 apA3 = glm::vec3(apA.x,apA.y,apA.z);
        glm::vec3 apB3 = glm::vec3(apB.x,apB.y,apB.z);
        //glm::mat4 arc = glm::rotate(glm::mat4(1.0),(glm::acos(glm::dot(apA,apB))),glm::cross(apA3,apB3));
        float t = glm::acos(glm::dot(glm::normalize(apA3),glm::normalize(apB3)));

        printf("\nt: %f", t);
        glm::vec3 n = glm::cross(apA3,apB3);
        printf("\nxax: %f,%f,%f",n.x,n.y,n.z);
        xax= glm::rotate(sxa,t,n);
        yax= glm::rotate(sya,t,n);
        zax= glm::rotate(sza,t,n);

        printf("\nxax: %f,%f,%f",xax.x,xax.y,xax.z);
        printf("\nyax: %f,%f,%f",yax.x,yax.y,yax.z);
        printf("\nzax: %f,%f,%f",zax.x,zax.y,zax.z);

        //Transform modified by arcball twiddling
        //To it as polar coords:

        //1.Get Latitude and Longtitude
        polarrotation.x = glm::acos(glm::dot(glm::vec3(0.,0.,1.),zax)); // -pi<Latitude<=pi ;

        polarrotation.y = glm::atan(zax.y,zax.x);
        //if (zax.y < 0.f) polarrotation.y = polarrotation.y + M_PI;  // Longtitude
        //polarrotation.y = polarrotation.y + ((zax.y < 0.f )? M_PI : 0);  // Longtitude

        //2. Get the normal of current and reference Z basevecs
        glm::vec3 tiltnorm = glm::cross(zax,glm::vec3(0.,0.,1.));
        //glm::mat4 untilt = glm::rotate(glm::vec3(1.),polarrotation.x,tiltnorm);
        //glm::vec3 uprightX = xax * glm::rotate(vec3(1.),polarrotation.x,tiltnorm);
        glm::vec3 uprightX = glm::rotate(xax,polarrotation.x,tiltnorm);
        glm::vec3 uprightY = glm::rotate(yax,polarrotation.x,tiltnorm);
        //glm::vec3 uprightY = yax * untilt;

        // Get the Z angle
        polarrotation.z = glm::acos(glm::dot(uprightX,glm::vec3(1.0f,0.f,0.f)));
        //polarrotation.z -= uprightX.y<0? M_PI : 0.f;

    }else click = false;

     if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_MIDDLE]) {
        m_scale += glm::min(m_scale,0.1f)*0.1*mousePositionDelta.y;

    }if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_RIGHT]) {
         //
    }



    // Update the mouse position to the current one
    m_mousePosition = currentMousePosition;
}

void Application::onKey(int key, int scancode, int action, int mods) {
    // `(void)foo` suppresses unused variable warnings
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
}

void Application::onScroll(double xoffset, double yoffset) {
    // `(void)foo` suppresses unused variable warnings
    (void)xoffset;
    (void)yoffset;
}
