#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

namespace our
{

#define ATTRIB_LOC_POSITION 0
#define ATTRIB_LOC_COLOR 1
#define ATTRIB_LOC_TEXCOORD 2
#define ATTRIB_LOC_NORMAL 3

    class Mesh
    {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements
        GLsizei elementCount;

    public:
        float minX, maxX;
        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &elements)
        {
            // TODO: (Req 1) Write this function
            //  remember to store the number of elements in "elementCount" since you will need it for drawing
            minX = 10000000.0;
            maxX = -10000000.0;
            // get the minium x and max x of the mesh
            for (auto v : vertices)
            {
                if (v.position.x < minX)
                    minX = v.position.x;
                if (v.position.x > maxX)
                    maxX = v.position.x;
            }
            // for drawing
            //  locations, use the constants:  ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            elementCount = (GLsizei)elements.size();
            glGenVertexArrays(1, &VAO);
            // now we need to define how the vertix array should read the data from the buffer
            glBindVertexArray(VAO);

            glGenBuffers(1, &VBO);
            // if i need to deal with this buffer i should bind it at first
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // to send the data GL_ARRAY_BUFFER, vertices size ,size of each vertex,usage GL_STATIC_DRAW i will not change this data
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), (void *)vertices.data(), GL_STATIC_DRAW);
            // now we need to get the data from the buffer and put it in the position

            // who u will send to = positionLoc, how many component will u send in time=3 xyz , what type of components ,
            // normalized=no , stride=sizeof(Vertex), pointer=0
            // that's why we dont write stride=0
            // the pointer "offcet" here come from the begining of the array for positions
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, sizeof(Vertex), (void *)0);

            // size = 4, since color in vertex is of type vec4
            // here we normalize because color 0 to 255 we need to be 0 to 1
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void *)offsetof(Vertex, color));

            // size = 2, since tex_coord is a 2D vec
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, false, sizeof(Vertex), (void *)offsetof(Vertex, tex_coord));

            // size = 3, since normal is of type vec3
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);
            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, false, sizeof(Vertex), (void *)offsetof(Vertex, normal));
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount * sizeof(GL_UNSIGNED_INT), (void *)elements.data(), GL_STATIC_DRAW);
        }

        // this function should render the mesh
        void draw()
        {
            // TODO: (Req 1) Write this function
            glBindVertexArray(VAO);
            /*
           1- modeSpecifies what kind of primitives to render. Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY and GL_PATCHES are accepted.
           2- countSpecifies the number of elements to be rendered.
           3- typeSpecifies the type of the values in indices. Must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.
           4- indicesSpecifies a pointer to the location where the indices are stored.
            */
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, (void *)0);
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh()
        {
            // TODO: (Req 1) Write this function
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &VBO);
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}