/* Copyright (c) 2014-2017, ARM Limited and Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "Common.h"
#include "Mathematics.h"
#include "Matrix.h"

#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>

namespace MaliSDK
{
    /* [Define bias matrix] */
    /* Bias matrix. */
    const float Matrix::biasArray[16] =
    {
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f,
    };
    /* [Define bias matrix] */

    /* Identity matrix. */
    const float Matrix::identityArray[16] =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    Matrix Matrix::biasMatrix     = Matrix(biasArray);
    Matrix Matrix::identityMatrix = Matrix(identityArray);

    Matrix::Matrix(const float* array)
    {
        memcpy(elements, array, 16 * sizeof(float));
    }

    Matrix::Matrix(void)
    {
    }

    float& Matrix::operator[] (unsigned element)
    {
        ASSERT(element <= 15, "Invalid matrix element index.");

        return elements[element];
    }

    Matrix Matrix::operator* (Matrix right)
    {
        return multiply(this, &right);
    }

    Matrix& Matrix::operator= (const Matrix &another)
    {
        if (this != &another)
        {
            memcpy(this->elements, another.elements, 16 * sizeof(float));
        }
        
        return *this;
    }

    Matrix Matrix::createRotationX(float angleInDegrees)
    {
        float  angleInRadians = degreesToRadians(angleInDegrees);
        Matrix result         = identityMatrix;

        result.elements[5]  =  cos(angleInRadians);
        result.elements[9]  = -sin(angleInRadians);
        result.elements[6]  =  sin(angleInRadians);
        result.elements[10] =  cos(angleInRadians);

        return result;
    }

    Matrix Matrix::createRotationY(float angleInDegrees)
    {
        float  angleInRadians = degreesToRadians(angleInDegrees);
        Matrix result         = identityMatrix;

        result.elements[0]  =  cos(angleInRadians);
        result.elements[8]  =  sin(angleInRadians);
        result.elements[2]  = -sin(angleInRadians);
        result.elements[10] =  cos(angleInRadians);

        return result;
    }

    Matrix Matrix::createRotationZ(float angleInDegrees)
    {
        float  angleInRarians = degreesToRadians(angleInDegrees);
        Matrix result         = identityMatrix;

        result.elements[0] =  cos(angleInRarians);
        result.elements[4] = -sin(angleInRarians);
        result.elements[1] =  sin(angleInRarians);
        result.elements[5] =  cos(angleInRarians);

        return result;
    }

    Matrix Matrix::createScaling(float x, float y, float z)
    {
        Matrix result = identityMatrix;

        result.elements[0]  = x;
        result.elements[5]  = y;
        result.elements[10] = z;

        return result;
    }

    Matrix Matrix::createTranslation(float x, float y, float z)
    {
        Matrix result = identityMatrix;

        result.elements[12] = x;
        result.elements[13] = y;
        result.elements[14] = z;

        return result;
    }

    float* Matrix::getAsArray(void)
    {
        return elements;
    }

    float Matrix::matrixDeterminant(float *matrix)
    {
        float result = 0.0f;

        result = matrix[0] * (matrix[4] * matrix[8] - matrix[7] * matrix[5]);
        result -= matrix[3] * (matrix[1] * matrix[8] - matrix[7] * matrix[2]);
        result += matrix[6] * (matrix[1] * matrix[5] - matrix[4] * matrix[2]);

        return result;
    }

    float Matrix::matrixDeterminant(Matrix *matrix)
    {
        float matrix3x3[9];
        float determinant3x3 = 0.0f;
        float result = 0.0f;

        /* Remove (i, j) (1, 1) to form new 3x3 matrix. */
        matrix3x3[0] = matrix->elements[5];
        matrix3x3[1] = matrix->elements[6];
        matrix3x3[2] = matrix->elements[7];
        matrix3x3[3] = matrix->elements[9];
        matrix3x3[4] = matrix->elements[10];
        matrix3x3[5] = matrix->elements[11];
        matrix3x3[6] = matrix->elements[13];
        matrix3x3[7] = matrix->elements[14];
        matrix3x3[8] = matrix->elements[15];
        determinant3x3 = matrixDeterminant(matrix3x3);
        result += matrix->elements[0] * determinant3x3;

        /* Remove (i, j) (1, 2) to form new 3x3 matrix. */
        matrix3x3[0] = matrix->elements[1];
        matrix3x3[1] = matrix->elements[2];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[9];
        matrix3x3[4] = matrix->elements[10];
        matrix3x3[5] = matrix->elements[11];
        matrix3x3[6] = matrix->elements[13];
        matrix3x3[7] = matrix->elements[14];
        matrix3x3[8] = matrix->elements[15];
        determinant3x3 = matrixDeterminant(matrix3x3);
        result -= matrix->elements[4] * determinant3x3;

        /* Remove (i, j) (1, 3) to form new 3x3 matrix. */
        matrix3x3[0] = matrix->elements[1];
        matrix3x3[1] = matrix->elements[2];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[5];
        matrix3x3[4] = matrix->elements[6];
        matrix3x3[5] = matrix->elements[7];
        matrix3x3[6] = matrix->elements[13];
        matrix3x3[7] = matrix->elements[14];
        matrix3x3[8] = matrix->elements[15];
        determinant3x3 = matrixDeterminant(matrix3x3);
        result += matrix->elements[8] * determinant3x3;

        /* Remove (i, j) (1, 4) to form new 3x3 matrix. */
        matrix3x3[0] = matrix->elements[1];
        matrix3x3[1] = matrix->elements[2];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[5];
        matrix3x3[4] = matrix->elements[6];
        matrix3x3[5] = matrix->elements[7];
        matrix3x3[6] = matrix->elements[9];
        matrix3x3[7] = matrix->elements[10];
        matrix3x3[8] = matrix->elements[11];
        determinant3x3 = matrixDeterminant(matrix3x3);
        result -= matrix->elements[12] * determinant3x3;

        return result;
    }

    Matrix Matrix::matrixInvert(Matrix *matrix)
    {
        Matrix result;
        float matrix3x3[9];

        /* Find the cofactor of each element. */
        /* Element (i, j) (1, 1) */
        matrix3x3[0] = matrix->elements[5];
        matrix3x3[1] = matrix->elements[6];
        matrix3x3[2] = matrix->elements[7];
        matrix3x3[3] = matrix->elements[9];
        matrix3x3[4] = matrix->elements[10];
        matrix3x3[5] = matrix->elements[11];
        matrix3x3[6] = matrix->elements[13];
        matrix3x3[7] = matrix->elements[14];
        matrix3x3[8] = matrix->elements[15];
        result.elements[0] = matrixDeterminant(matrix3x3);

        /* Element (i, j) (1, 2) */
        matrix3x3[0] = matrix->elements[1];
        matrix3x3[1] = matrix->elements[2];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[9];
        matrix3x3[4] = matrix->elements[10];
        matrix3x3[5] = matrix->elements[11];
        matrix3x3[6] = matrix->elements[13];
        matrix3x3[7] = matrix->elements[14];
        matrix3x3[8] = matrix->elements[15];
        result.elements[4] = -matrixDeterminant(matrix3x3);

        /* Element (i, j) (1, 3) */
        matrix3x3[0] = matrix->elements[1];
        matrix3x3[1] = matrix->elements[2];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[5];
        matrix3x3[4] = matrix->elements[6];
        matrix3x3[5] = matrix->elements[7];
        matrix3x3[6] = matrix->elements[13];
        matrix3x3[7] = matrix->elements[14];
        matrix3x3[8] = matrix->elements[15];
        result.elements[8] = matrixDeterminant(matrix3x3);

        /* Element (i, j) (1, 4) */
        matrix3x3[0] = matrix->elements[1];
        matrix3x3[1] = matrix->elements[2];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[5];
        matrix3x3[4] = matrix->elements[6];
        matrix3x3[5] = matrix->elements[7];
        matrix3x3[6] = matrix->elements[9];
        matrix3x3[7] = matrix->elements[10];
        matrix3x3[8] = matrix->elements[11];
        result.elements[12] = -matrixDeterminant(matrix3x3);

        /* Element (i, j) (2, 1) */
        matrix3x3[0] = matrix->elements[4];
        matrix3x3[1] = matrix->elements[6];
        matrix3x3[2] = matrix->elements[7];
        matrix3x3[3] = matrix->elements[8];
        matrix3x3[4] = matrix->elements[10];
        matrix3x3[5] = matrix->elements[11];
        matrix3x3[6] = matrix->elements[12];
        matrix3x3[7] = matrix->elements[14];
        matrix3x3[8] = matrix->elements[15];
        result.elements[1] = -matrixDeterminant(matrix3x3);

        /* Element (i, j) (2, 2) */
        matrix3x3[0] = matrix->elements[0];
        matrix3x3[1] = matrix->elements[2];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[8];
        matrix3x3[4] = matrix->elements[10];
        matrix3x3[5] = matrix->elements[11];
        matrix3x3[6] = matrix->elements[12];
        matrix3x3[7] = matrix->elements[14];
        matrix3x3[8] = matrix->elements[15];
        result.elements[5] = matrixDeterminant(matrix3x3);

        /* Element (i, j) (2, 3) */
        matrix3x3[0] = matrix->elements[0];
        matrix3x3[1] = matrix->elements[2];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[4];
        matrix3x3[4] = matrix->elements[6];
        matrix3x3[5] = matrix->elements[7];
        matrix3x3[6] = matrix->elements[12];
        matrix3x3[7] = matrix->elements[14];
        matrix3x3[8] = matrix->elements[15];
        result.elements[9] = -matrixDeterminant(matrix3x3);

        /* Element (i, j) (2, 4) */
        matrix3x3[0] = matrix->elements[0];
        matrix3x3[1] = matrix->elements[2];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[4];
        matrix3x3[4] = matrix->elements[6];
        matrix3x3[5] = matrix->elements[7];
        matrix3x3[6] = matrix->elements[8];
        matrix3x3[7] = matrix->elements[10];
        matrix3x3[8] = matrix->elements[11];
        result.elements[13] = matrixDeterminant(matrix3x3);

        /* Element (i, j) (3, 1) */
        matrix3x3[0] = matrix->elements[4];
        matrix3x3[1] = matrix->elements[5];
        matrix3x3[2] = matrix->elements[7];
        matrix3x3[3] = matrix->elements[8];
        matrix3x3[4] = matrix->elements[9];
        matrix3x3[5] = matrix->elements[11];
        matrix3x3[6] = matrix->elements[12];
        matrix3x3[7] = matrix->elements[13];
        matrix3x3[8] = matrix->elements[15];
        result.elements[2] = matrixDeterminant(matrix3x3);

        /* Element (i, j) (3, 2) */
        matrix3x3[0] = matrix->elements[0];
        matrix3x3[1] = matrix->elements[1];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[8];
        matrix3x3[4] = matrix->elements[9];
        matrix3x3[5] = matrix->elements[11];
        matrix3x3[6] = matrix->elements[12];
        matrix3x3[7] = matrix->elements[13];
        matrix3x3[8] = matrix->elements[15];
        result.elements[6] = -matrixDeterminant(matrix3x3);

        /* Element (i, j) (3, 3) */
        matrix3x3[0] = matrix->elements[0];
        matrix3x3[1] = matrix->elements[1];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[4];
        matrix3x3[4] = matrix->elements[5];
        matrix3x3[5] = matrix->elements[7];
        matrix3x3[6] = matrix->elements[12];
        matrix3x3[7] = matrix->elements[13];
        matrix3x3[8] = matrix->elements[15];
        result.elements[10] = matrixDeterminant(matrix3x3);

        /* Element (i, j) (3, 4) */
        matrix3x3[0] = matrix->elements[0];
        matrix3x3[1] = matrix->elements[1];
        matrix3x3[2] = matrix->elements[3];
        matrix3x3[3] = matrix->elements[4];
        matrix3x3[4] = matrix->elements[5];
        matrix3x3[5] = matrix->elements[7];
        matrix3x3[6] = matrix->elements[8];
        matrix3x3[7] = matrix->elements[9];
        matrix3x3[8] = matrix->elements[11];
        result.elements[14] = -matrixDeterminant(matrix3x3);

        /* Element (i, j) (4, 1) */
        matrix3x3[0] = matrix->elements[4];
        matrix3x3[1] = matrix->elements[5];
        matrix3x3[2] = matrix->elements[6];
        matrix3x3[3] = matrix->elements[8];
        matrix3x3[4] = matrix->elements[9];
        matrix3x3[5] = matrix->elements[10];
        matrix3x3[6] = matrix->elements[12];
        matrix3x3[7] = matrix->elements[13];
        matrix3x3[8] = matrix->elements[14];
        result.elements[3] = -matrixDeterminant(matrix3x3);

        /* Element (i, j) (4, 2) */
        matrix3x3[0] = matrix->elements[0];
        matrix3x3[1] = matrix->elements[1];
        matrix3x3[2] = matrix->elements[2];
        matrix3x3[3] = matrix->elements[8];
        matrix3x3[4] = matrix->elements[9];
        matrix3x3[5] = matrix->elements[10];
        matrix3x3[6] = matrix->elements[12];
        matrix3x3[7] = matrix->elements[13];
        matrix3x3[8] = matrix->elements[14];
        result.elements[7] = matrixDeterminant(matrix3x3);

        /* Element (i, j) (4, 3) */
        matrix3x3[0] = matrix->elements[0];
        matrix3x3[1] = matrix->elements[1];
        matrix3x3[2] = matrix->elements[2];
        matrix3x3[3] = matrix->elements[4];
        matrix3x3[4] = matrix->elements[5];
        matrix3x3[5] = matrix->elements[6];
        matrix3x3[6] = matrix->elements[12];
        matrix3x3[7] = matrix->elements[13];
        matrix3x3[8] = matrix->elements[14];
        result.elements[11] = -matrixDeterminant(matrix3x3);

        /* Element (i, j) (4, 4) */
        matrix3x3[0] = matrix->elements[0];
        matrix3x3[1] = matrix->elements[1];
        matrix3x3[2] = matrix->elements[2];
        matrix3x3[3] = matrix->elements[4];
        matrix3x3[4] = matrix->elements[5];
        matrix3x3[5] = matrix->elements[6];
        matrix3x3[6] = matrix->elements[8];
        matrix3x3[7] = matrix->elements[9];
        matrix3x3[8] = matrix->elements[10];
        result.elements[15] = matrixDeterminant(matrix3x3);

        /* The adjoint is the transpose of the cofactor matrix. */
        matrixTranspose(&result);

        /* The inverse is the adjoint divided by the determinant. */
        result = matrixScale(&result, 1.0f / matrixDeterminant(matrix));

        return result;
    }

    Matrix Matrix::matrixLookAt(Vec3f eye, Vec3f center, Vec3f up)
    {
        Vec3f  xVector;
        Vec3f  yVector;
        Vec3f  zVector = { center.x - eye.x, center.y - eye.y, center.z - eye.z };
        Matrix result  = identityMatrix;

        zVector.normalize();

        xVector = Vec3f::cross(zVector, up);
        xVector.normalize();

        yVector = Vec3f::cross(xVector, zVector);

        /*
        * The final lookAt should look like:
        *
        * lookAt[] = {xVector.x,         yVector.x,         -zVector.x,         0.0f,
        *             xVector.y,         yVector.y,         -zVector.y,         0.0f,
        *             xVector.z,         yVector.z,         -zVector.z,         0.0f,
        *             dot(xVector, eye), dot(yVector, eye),  dot(zVector, eye), 1.0f };
        */

        result[0] =  xVector.x;
        result[1] =  yVector.x;
        result[2] = -zVector.x;

        result[4] =  xVector.y;
        result[5] =  yVector.y;
        result[6] = -zVector.y;

        result[8]  =  xVector.z;
        result[9]  =  yVector.z;
        result[10] = -zVector.z;

        result[12] = Vec3f::dot(xVector, eye);
        result[13] = Vec3f::dot(yVector, eye);
        result[14] = Vec3f::dot(zVector, eye);

        return result;
    }

    Matrix Matrix::matrixPerspective(float FOV, float ratio, float zNear, float zFar)
    {
        Matrix result = identityMatrix;

        FOV = 1.0f / tan(FOV * 0.5f);

        result.elements[0]  = FOV / ratio;
        result.elements[5]  = FOV;
        result.elements[10] = -(zFar + zNear) / (zFar - zNear);
        result.elements[11] = -1.0f;
        result.elements[14] = (-2.0f * zFar * zNear) / (zFar - zNear);
        result.elements[15] = 0.0f;

        return result;
    }

    Matrix Matrix::matrixScale(Matrix *matrix, float scale)
    {
        Matrix result;

        for (int allElements = 0; allElements < 16; allElements++)
        {
            result.elements[allElements] = matrix->elements[allElements] * scale;
        }

        return result;
    }

    void Matrix::matrixTranspose(Matrix *matrix)
    {
        float temp;

        temp = matrix->elements[1];
        matrix->elements[1] = matrix->elements[4];
        matrix->elements[4] = temp;

        temp = matrix->elements[2];
        matrix->elements[2] = matrix->elements[8];
        matrix->elements[8] = temp;

        temp = matrix->elements[3];
        matrix->elements[3] = matrix->elements[12];
        matrix->elements[12] = temp;

        temp = matrix->elements[6];
        matrix->elements[6] = matrix->elements[9];
        matrix->elements[9] = temp;

        temp = matrix->elements[7];
        matrix->elements[7] = matrix->elements[13];
        matrix->elements[13] = temp;

        temp = matrix->elements[11];
        matrix->elements[11] = matrix->elements[14];
        matrix->elements[14] = temp;
    }

    Matrix Matrix::multiply(Matrix *left, Matrix *right)
    {
        Matrix result;

        for (int row = 0; row < 4; row++)
        {
            for (int column = 0; column < 4; column++)
            {
                /* result.elements[row * 4 + column]  = left->elements[0 + row * 4] * right->elements[column + 0 * 4];
                 * result.elements[row * 4 + column] += left->elements[1 + row * 4] * right->elements[column + 1 * 4];
                 * result.elements[row * 4 + column] += left->elements[2 + row * 4] * right->elements[column + 2 * 4];
                 * result.elements[row * 4 + column] += left->elements[3 + row * 4] * right->elements[column + 3 * 4];*/
                float accumulator = 0.0f;

                for (int allElements = 0; allElements < 4; allElements++)
                {
                    accumulator += left->elements[allElements * 4 + row] * right->elements[column * 4 + allElements];
                }

                result.elements[column * 4 + row] = accumulator;
            }
        }

        return result;
    }

    Vec4f Matrix::vertexTransform(Vec4f *vertex, Matrix *matrix)
    {
        Vec4f result;

        result.x  = vertex->x * matrix->elements[0];
        result.x += vertex->y * matrix->elements[4];
        result.x += vertex->z * matrix->elements[8];
        result.x += vertex->w * matrix->elements[12];

        result.y  = vertex->x * matrix->elements[1];
        result.y += vertex->y * matrix->elements[5];
        result.y += vertex->z * matrix->elements[9];
        result.y += vertex->w * matrix->elements[13];

        result.z  = vertex->x * matrix->elements[2];
        result.z += vertex->y * matrix->elements[6];
        result.z += vertex->z * matrix->elements[10];
        result.z += vertex->w * matrix->elements[14];

        result.w  = vertex->x * matrix->elements[3];
        result.w += vertex->y * matrix->elements[7];
        result.w += vertex->z * matrix->elements[11];
        result.w += vertex->w * matrix->elements[15];

        return result;
    }
}
