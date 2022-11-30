#ifndef __GFC_MATRIX_H__
#define __GFC_MATRIX_H__

#include "gfc_vector.h"

typedef float Matrix4[4][4];
typedef float Matrix3[3][3];
typedef float Matrix2[2][2];


typedef struct
{
    Matrix4 model;
    Matrix4 view;
    Matrix4 proj;
}UniformBufferObject;

/**
 * @brief copy the contents of one matrix into another
 * @param d the destination matrix
 * @param s the source matrix
 */
void gfc_matrix_copy(
    Matrix4 d,
    Matrix4 s
  );

/**
 * @brief set the matrix to an identity matrix
 * @param one the matrix to become an identity
 */
void gfc_matrix_identity(Matrix4 one);

/**
 * @brief set the matrix to a zero matrix
 * @param zero the matrix to be set to zero
 */
void gfc_matrix_zero(Matrix4 zero);

/**
 * @brief invert a matrix
 * @param out the resulting inverse matrix
 * @param in the matrix to invert
 * @return 1 on success, 0 if the matrix has no inverse.  On 0, out will not be changed
 */
Uint8 gfc_matrix4_invert(Matrix4 out, Matrix4 in);

/**
 * @brief create a translation matrix given the vector
 * @param out the output matrix, the contents of this matrix are overwritten
 * @param move the vector describing the translation
 */
void gfc_matrix_make_translation(
    Matrix4 out,
    Vector3D move
);

void gfc_matrix_translate(
    Matrix4 out,
    Vector3D move
);

/**
 * @brief setup a view matrix for a frustum centered at position, pointed at target, with up as the up direction
 * @note adapted from glm
 * @param out output matrix
 * @param position position of the "camera"
 * @param target location to look
 * @param up the direction considered "up"
 */
void gfc_matrix_view(
    Matrix4  out,
    Vector3D position,
    Vector3D target,
    Vector3D up
);

/**
 * @brief send the information from a matrix to the log out
 * @param mat the matrix to print
 */
void gfc_matrix4_slog(Matrix4 mat);
void gfc_matrix3_slog(Matrix3 mat);
void gfc_matrix2_slog(Matrix2 mat);

/**
 * @brief setup a perspective projection matrix
 * @note adapted from glm
 * @param out the output matrix
 * @param fov the field of view
 * @param aspect aspect ration (screen width / screen height)
 * @param near the near z plane
 * @param far the far z plane
 */
void gfc_matrix_perspective(
    Matrix4     out,
    float      fov,
    float      aspect,
    float      near,
    float      far
);

/**
 * @brief map screen coordinates into 3d world coordinate
 * @note use this twice, with z value 0 and z value 1 to create a trace segment to determine what
 *       is beneath the screen position
 * @param in the screen coordinates to map
 * @param view the view matrix of the scene
 * @param proj the projection matrix of the scene
 * @param viewport screen extent (x,y)
 * @return 3d scene coordinates of the screen coordinates
 */
Vector3D gfc_unproject(Vector3D in,Matrix4 view, Matrix4 proj,Vector2D viewport);

/**
 * @brief multiply the two input matrices together and save the result into out
 * @note operation is out = a * b, so most of the time a is changed by b
 * @note result = childOp * parentOp
 * @param out the output matrix
 * @param a one multiplicand matrix
 * @param b another multiplicand matrix
 */
void gfc_matrix_multiply(
    Matrix4 out,
    Matrix4 a,
    Matrix4 b
  );

/**
 * @brief multiply a vector by the matrix, v * M
 * @param out a pointer to the vector that will hold the result
 * @param mat input matrix to multiply by
 * @param vec input matrix to multiply by
 */
void gfc_matrix_v_multiply_M(
    Vector4D * out,
    Matrix4    mat,
    Vector4D   vec
);

/**
 * @brief multiply a matrix by a vector, M * v
 * @param out a pointer to the vector that will hold the result
 * @param mat input matrix to multiply by
 * @param vec input matrix to multiply by
 */
void gfc_matrix_M_multiply_v(
  Vector4D * out,
  Matrix4    mat,
  Vector4D   vec
);


/**
 * @brief create a transformation matrix from the three basic operation vectors
 * @param out the output matrix
 * @param translation the movement produced from the matrix
 * @param rotation the amount to rotation with this matrix about the x, y, and z axis.
 * @param scale the amount to scale with this matrix.  remember (1,1,1) is no change.  (0,0,0) is destroyed
 */
void gfc_matrix4_from_vectors(
    Matrix4 out,
    Vector3D translation,
    Vector3D rotation,
    Vector3D scale);

/**
 * @brief extract translation,rotation, and scale from a matrix
 * @param in the matrix to extract from
 * @param translation if provided, this will be populated with the translation information
 * @param rotation if provided, this will be populated with the rotation information
 * @param scale if provided, this will be populated with the scale information
 */
void gfc_matrix4_to_vectors(
    Matrix4 in,
    Vector3D *translation,
    Vector3D *rotation,
    Vector3D *scale);

/**
 * @brief multiply a matrix by the scalar value
 * @param out the output matrix
 * @param m1 input matrix to multiply by
 * @param s input scalar value to multiply by
 */
void gfc_matrix_multiply_scalar(Matrix4 out,Matrix4 m1,float s);

/**
 * @brief rotate a matrix around the cardinal axis by the provided amount
 * @param out the result of the rotation
 * @param m the matrix to rotate
 * @param theta the amount to rotate by
 */
void gfc_matrix_rotate_x(
    Matrix4     out,
    Matrix4     m,
    float       theta
);
void gfc_matrix_rotate_y(
    Matrix4     out,
    Matrix4     m,
    float       theta
);
void gfc_matrix_rotate_z(
    Matrix4     out,
    Matrix4     m,
    float       theta
);

/**
 * @brief sequentially apply the cardinal axis rotations about the x, y, and z axis accordingly
 * @param out the result of the rotation
 * @param m the matrix to rotate
 * @param v the amount to rotate by in the x, y, and z axis
 */
void gfc_matrix_rotate_by_vector(
    Matrix4     out,
    Matrix4     m,
    Vector3D    v
);

/**
 * @brief multiply a matrix around a specific axis
 * @param out the output matrix
 * @param in  the input matrix
 * @param degree the amount, in radians, to rotate by
 * @param axis the axis about which to rotate
 */
void gfc_matrix_rotate(
    Matrix4     out,
    Matrix4     in,
    float       degree,
    Vector3D    axis
);

/**
 * @brief scale a matrix based on the input scale vector
 * @param out the matrix to be scaled.  it is changed by this function
 * @param scale the amount to scale it by
 */
void gfc_matrix_scale(
    Matrix4 out,
    Vector3D scale
);


#endif
