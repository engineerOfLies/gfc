#ifndef __GFC_MATRIX_H__
#define __GFC_MATRIX_H__

#include "gfc_vector.h"

/**
 * @note all matrices are assumed to be row-major ie:  the first index is row, the second is column
 */
typedef float GFC_Matrix4[4][4];
typedef float GFC_Matrix3[3][3];
typedef float GFC_Matrix2[2][2];

/**
 * @brief copy the contents of one matrix into another
 * @param d the destination matrix
 * @param s the source matrix
 */
void gfc_matrix2_copy(GFC_Matrix2 d,GFC_Matrix2 s);
void gfc_matrix3_copy(GFC_Matrix3 d,GFC_Matrix3 s);
void gfc_matrix4_copy(GFC_Matrix4 d,GFC_Matrix4 s);

/**
 * @brief set the matrix to an identity matrix
 * @param one the matrix to become an identity
 */
void gfc_matrix2_identity(GFC_Matrix2 one);
void gfc_matrix3_identity(GFC_Matrix3 one);
void gfc_matrix4_identity(GFC_Matrix4 one);

/**
 * @brief set the matrix to a zero matrix
 * @param zero the matrix to be set to zero
 */
void gfc_matrix2_zero(GFC_Matrix2 zero);
void gfc_matrix3_zero(GFC_Matrix3 zero);
void gfc_matrix4_zero(GFC_Matrix4 zero);

/**
 * @brief invert a matrix
 * @param out the resulting inverse matrix
 * @param in the matrix to invert
 * @return 1 on success, 0 if the matrix has no inverse.  On 0, out will not be changed
 */
Uint8 gfc_matrix2_invert(GFC_Matrix4 out, GFC_Matrix4 in);
Uint8 gfc_matrix3_invert(GFC_Matrix4 out, GFC_Matrix4 in);
Uint8 gfc_matrix4_invert(GFC_Matrix4 out, GFC_Matrix4 in);

/**
 * @brief create a translation matrix given the gfc_vector
 * @param out the output matrix, the contents of this matrix are overwritten
 * @param move the gfc_vector describing the translation
 */
void gfc_matrix3_make_translation(GFC_Matrix3 out,GFC_Vector2D move);
void gfc_matrix4_make_translation(GFC_Matrix4 out,GFC_Vector3D move);

/**
 * @brief translate a matrix by the supplied vector
 * @param out the output matrix
 * @param in the input matrix
 * @param move the vector to move it by
 */
void gfc_matrix3_translate(GFC_Matrix3 out,GFC_Matrix3 in, GFC_Vector2D move);
void gfc_matrix4_translate(GFC_Matrix4 out,GFC_Matrix4 in, GFC_Vector3D move);

/**
 * @brief setup a view matrix for a frustum centered at position, pointed at target, with up as the up direction
 * @note adapted from glm
 * @param out output matrix
 * @param position position of the "camera"
 * @param target location to look
 * @param up the direction considered "up"
 */
void gfc_matrix4_view(
    GFC_Matrix4  out,
    GFC_Vector3D position,
    GFC_Vector3D target,
    GFC_Vector3D up
);

/**
 * @brief send the information from a matrix to the log out
 * @note use the macro version to have file and linenumber automaticly passed in
 * @param mat the matrix to print
 */

#define gfc_matrix4_slog(mat) _gfc_matrix4_slog(__FILE__,__LINE__,mat)
#define gfc_matrix3_slog(mat) _gfc_matrix3_slog(__FILE__,__LINE__,mat)
#define gfc_matrix2_slog(mat) _gfc_matrix2_slog(__FILE__,__LINE__,mat)

void _gfc_matrix4_slog(const char *filename,Uint32 line,GFC_Matrix4 mat);
void _gfc_matrix3_slog(const char *filename,Uint32 line,GFC_Matrix3 mat);
void _gfc_matrix2_slog(const char *filename,Uint32 line,GFC_Matrix2 mat);

/**
 * @brief setup a perspective projection matrix
 * @note adapted from glm
 * @param out the output matrix
 * @param fov the field of view
 * @param aspect aspect ration (screen width / screen height)
 * @param near the near z plane
 * @param far the far z plane
 */
void gfc_matrix4_perspective(
    GFC_Matrix4     out,
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
GFC_Vector3D gfc_unproject(GFC_Vector3D in,GFC_Matrix4 view, GFC_Matrix4 proj,GFC_Vector2D viewport);

/**
 * @brief multiply the two input matrices together and save the result into out
 * @note operation is out = a * b, so most of the time a is changed by b
 * @note result = childOp * parentOp
 * @param out the output matrix
 * @param a one multiplicand matrix
 * @param b another multiplicand matrix
 */
void gfc_matrix2_multiply(
    GFC_Matrix2 out,
    GFC_Matrix2 a,
    GFC_Matrix2 b
  );
void gfc_matrix3_multiply(
    GFC_Matrix3 out,
    GFC_Matrix3 a,
    GFC_Matrix3 b
  );
void gfc_matrix4_multiply(
    GFC_Matrix4 out,
    GFC_Matrix4 a,
    GFC_Matrix4 b
  );

/**
 * @brief multiply a gfc_vector by the matrix, v * M
 * @param out a pointer to the gfc_vector that will hold the result
 * @param mat input matrix to multiply by
 * @param vec input matrix to multiply by
 */
void gfc_matrix2_v_multiply(GFC_Vector2D *out,GFC_Vector2D vec,GFC_Matrix2 mat);
void gfc_matrix3_v_multiply(GFC_Vector3D *out,GFC_Vector3D vec,GFC_Matrix3 mat);
void gfc_matrix4_v_multiply(GFC_Vector4D *out,GFC_Vector4D vec,GFC_Matrix4 mat);

/**
 * @brief multiply a matrix by a gfc_vector, M * v
 * @param out a pointer to the gfc_vector that will hold the result
 * @param mat input matrix to multiply by
 * @param vec input matrix to multiply by
 */
void gfc_matrix2_multiply_v(GFC_Vector2D * out,GFC_Matrix2 mat,GFC_Vector2D vec);
void gfc_matrix3_multiply_v(GFC_Vector3D * out,GFC_Matrix3 mat,GFC_Vector3D vec);
void gfc_matrix4_multiply_v(GFC_Vector4D * out,GFC_Matrix4 mat,GFC_Vector4D vec);


/**
 * @brief create a transformation matrix from the three basic operation gfc_vectors
 * @param out the output matrix
 * @param translation the movement produced from the matrix
 * @param rotation the amount to rotation with this matrix about the x, y, and z axis.
 * @param scale the amount to scale with this matrix.  remember (1,1,1) is no change.  (0,0,0) is destroyed
 */
void gfc_matrix4_from_vectors(
    GFC_Matrix4 out,
    GFC_Vector3D translation,
    GFC_Vector3D rotation,
    GFC_Vector3D scale);

/**
 * @brief create a matrix from the thee basic operation gfc_vectors, where the rotation is defined by a quaternion
 * @param translation the movement produced from the matrix
 * @param quaternion the amount to rotation the matrix with a quaternion
 * @param scale the amount to scale with this matrix.  remember (1,1,1) is no change.  (0,0,0) is destroyed
 */
void gfc_matrix4_from_vectors_q(
    GFC_Matrix4 out,
    GFC_Vector3D translation,
    GFC_Vector4D quaternion,
    GFC_Vector3D scale);

/**
 * @brief extract translation,rotation, and scale from a matrix
 * @param in the matrix to extract from
 * @param translation if provided, this will be populated with the translation information
 * @param rotation if provided, this will be populated with the rotation information
 * @param scale if provided, this will be populated with the scale information
 */
void gfc_matrix4_to_vectors(
    GFC_Matrix4 in,
    GFC_Vector3D *translation,
    GFC_Vector3D *rotation,
    GFC_Vector3D *scale);

/**
 * @brief get a quaternion matrix from quaternion gfc_vector
 * @param out the resulting matrix
 * @param q the quaternion gfc_vector
 */
void gfc_matrix4_from_quaternion(
    GFC_Matrix4     out,
    GFC_Vector4D    q);

/**
 * @brief multiply a matrix by the scalar value
 * @param out the output matr it is changed by this functionix
 * @param m1 input matrix to multiply by
 * @param s input scalar value to multiply by
 */
void gfc_matrix2_multiply_scalar(GFC_Matrix2 out,GFC_Matrix2 m1,float s);
void gfc_matrix3_multiply_scalar(GFC_Matrix3 out,GFC_Matrix3 m1,float s);
void gfc_matrix4_multiply_scalar(GFC_Matrix4 out,GFC_Matrix4 m1,float s);

/**
 * @brief rotate a matrix around the cardinal axis by the provided amount
 * @param out the result of the rotation
 * @param m the matrix to rotate
 * @param theta the amount to rotate by
 */

void gfc_matrix4_rotate_x(GFC_Matrix4 out,GFC_Matrix4 m,float theta);
void gfc_matrix4_rotate_y(GFC_Matrix4 out,GFC_Matrix4 m,float theta);
void gfc_matrix4_rotate_z(GFC_Matrix4 out,GFC_Matrix4 m,float theta);

/**
 * @brief sequentially apply the cardinal axis rotations about the x, y, and z axis accordingly
 * @param out the result of the rotation
 * @param m the matrix to rotate
 * @param v the amount to rotate by in the x, y, and z axis
 */
void gfc_matrix4_rotate_by_vector(
    GFC_Matrix4     out,
    GFC_Matrix4     m,
    GFC_Vector3D    v
);

/**
 * @brief multiply a matrix around a specific axis
 * @param out the output matrix
 * @param in  the input matrix
 * @param radians the amount, in radians, to rotate by
 * @param axis the axis about which to rotate
 */
void gfc_matrix4_rotate(GFC_Matrix4 out,GFC_Matrix4 in,float radians,GFC_Vector3D axis);

/**
 * @brief scale a matrix based on the input scale gfc_vector
 * @param out the matrix to be scaled.
 * @param in the matrix to be scaled.  
 * @param scale the amount to scale it by
 */
void gfc_matrix4_scale(GFC_Matrix4 out,GFC_Matrix4 in,GFC_Vector3D scale);


#endif
