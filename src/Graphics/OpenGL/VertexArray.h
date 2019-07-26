#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "../../ngine.h"
#include "Buffer.h"

namespace NerdThings::Ngine::Graphics::OpenGL {
    /*
     * OpenGL Types
     */
    enum GLType {
        /*
         * Unsigned char
         */
        TYPE_UNSIGNED_BYTE = 0x1401,

        /*
         * Unsigned int
         */
        TYPE_UNSIGNED_INT = 0x1405,

        /*
         * Float
         */
        TYPE_FLOAT = 0x1406
    };

    /*
     * A vertex buffer element
     */
    struct GLVertexBufferElement {
        GLType Type;
        unsigned int Count;
        unsigned int Normalized;
        unsigned int Location;

        static unsigned int GetSizeOf(GLType type_);
    };

    /*
     * Vertex buffer layout.
     * This provides information about shader bindings.
     * Not used by the GL batcher, but useful for custom implementations.
     */
    class GLVertexBufferLayout {
        /*
         * Layout elements
         */
        std::vector<GLVertexBufferElement> _Elements;

        /*
         * Vertex data stride
         */
        unsigned int _Stride;
    public:
        /*
         * Create a new layout
         */
        GLVertexBufferLayout() = default;

        /*
         * Reset the layout
         */
        void Clear();

        /*
         * Push an element onto the layout.
         * TODO: Template this instead???
         */
        void Push(GLType type_, unsigned int count_, unsigned int location_);

        /*
         * Get all of the elements
         */
        std::vector<GLVertexBufferElement> GetElements() const;

        /*
         * Get the layout stride
         */
        unsigned int GetStride() const;
    };

    /*
     * A vertex array object
     */
    struct GLVertexArray {
        /*
         * OpenGL ID
         */
        unsigned int ID;

        /*
         * Create a vertex array
         */
        GLVertexArray();

        /*
         * Destruct and delete vertex array
         */
        ~GLVertexArray();

        /*
         * Add a buffer with a layout
         */
        void AddBuffer(std::shared_ptr<GLBuffer> vertexBuffer_, const GLVertexBufferLayout &layout_);

        /*
         * Binds the vertex array
         */
        void Bind();
    };
}

#endif //VERTEXARRAY_H
