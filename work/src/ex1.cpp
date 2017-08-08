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
    unsigned int numVertices  = 0;
    unsigned int numTriangles = 0;

    cgra::Matrix<double> vertices(numVertices, 3);
    cgra::Matrix<unsigned int> triangles(numTriangles, 3);

    for (size_t i = 0; i < obj.m_positions.size(); i++) {
        // Add each position to the vertices matrix
    }

    for (size_t i = 0; i < obj.m_faces.size(); i++) {
        // Add each triangle's indices to the triangles matrix
        // Remember that Wavefront files use indices that start at 1
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

    static glm::vec3 manrotate;

   // ImGui::Begin("Manual Transforms",&show_app_manualtransforms,ImVec2(250,200),1.f,0);
    ImGui::Begin("Manual Transforms",&show_app_manualtransforms,ImVec2(250,200));
    //ImGui::Begin("Manual Transforms");

    ImGui::SliderFloat3("Translate",&m_translation[0],-10.0f,10.0f, "%.5f",1.5f);
    ImGui::SliderFloat("Scale",&m_scale,-5.0f,5.0f, "%.5f", 1.5f);
    ImGui::SliderFloat3("Rotate",&manrotate[0],-M_PI,M_PI, "%.5f", 1.0f);
    ImGui::End();


    glm::mat4 rot4 = glm::rotate(glm::mat4(1.0),manrotate.z,glm::vec3(0,0,1));
    rot4 = glm::rotate(rot4,manrotate.y,glm::vec3(0,1,0));
    rot4 = glm::rotate(rot4,manrotate.x,glm::vec3(1,0,0));



    modelTransform *= glm::translate(glm::mat4(),m_translation);
    modelTransform *= glm::scale(modelTransform,glm::vec3(m_scale));

    modelTransform *= rot4;


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

            /*cgra::Wavefront loadmesh;

            try {
                loadmesh = cgra::Wavefront::load(path);
            } catch (std::exception e) {
                printf( "\nLoading failed : %s \n", e.what());
            }*/

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
                    printf("File can't be read by our simple parser : ( Try exporting with other options\n");
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
            m_mesh.setData(verts,faces);


     } //endif loading
    }//endif textinput

    // Example for rotation, use glm to create a a rotation
    // matrix from this vector


    /*static glm::vec3 rotation(0, 0, 0);
    if (ImGui::InputFloat3("Rotation", &rotation[0])) {
        // This block is executed if the input changes
    }*/

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

    glm::mat4 rotmat =  glm::rotate(glm::mat4(),60.0f, glm::vec3( mousePositionDelta.x,mousePositionDelta.y,0.5));
    //m_rotationMatrix *= rotmat;


    if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_LEFT]) {

    } else if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_MIDDLE]) {

    } else if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_RIGHT]) {

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
