/*
 *
*/


#include "BLI_utildefines.h"
#include "BLI_math.h"

#include "DNA_mesh_types.h"

#include "BKE_editmesh.h"

#include "bmesh.h"

#include "mesh_dimensions.h"
#include "mechanical_utils.h"


void tag_vertexs_on_coplanar_faces(BMesh *bm, float *point, float* dir){
	BMFace *f;
	BMVert *eve;
	BMIter iter, viter;
	float p[3],vec[3];

	BM_ITER_MESH (f, &iter, bm, BM_FACES_OF_MESH) {
		if (parallel_v3u_v3u(dir,f->no)) {
			// Coplanar?
			int ok =1;
			BM_ITER_ELEM (eve, &viter, f, BM_VERTS_OF_FACE) {
				sub_v3_v3v3(vec, point, eve->co);
				normalize_v3(vec);
				project_v3_v3v3(p,vec,dir);
				if (len_squared_v3(p) > DIM_CONSTRAINT_PRECISION) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				BM_ITER_ELEM (eve, &viter, f, BM_VERTS_OF_FACE) {
					BM_elem_flag_enable(eve, BM_ELEM_TAG);
				}
			}
		}
	}
}

void tag_vertexs_affected_by_dimension (BMesh *bm, BMDim *edm)
{
	BMIter iter;
	BMVert* eve;

	// Tag all elements to be affected by change
	BM_ITER_MESH (eve, &iter, bm, BM_VERTS_OF_MESH) {
		if (BM_elem_flag_test(eve, BM_ELEM_SELECT)) {
			BM_elem_flag_enable(eve, BM_ELEM_TAG);
		} else {
			// Disable as default
			BM_elem_flag_disable(eve, BM_ELEM_TAG);
		}
	}

	// Tag vertexs of dimension
	for (int i=0;i<edm->totverts;i++) {
		// Tag elements
		BM_elem_flag_enable(edm->v[i], BM_ELEM_TAG);
	}
}

bool parallel_v3_v3(float *v1, float *v2) {
	return fabs(fabs(dot_v3v3(v1,v2)) - len_v3(v1)*len_v3(v2)) < DIM_CONSTRAINT_PRECISION;
}

bool parallel_v3u_v3u(float *v1, float *v2) {
	return fabs(fabs(dot_v3v3(v1,v2)) - 1) < DIM_CONSTRAINT_PRECISION;
}

bool perpendicular_v3_v3(float *v1, float *v2) {
	return fabs(dot_v3v3(v1,v2)) < DIM_CONSTRAINT_PRECISION;
}
/**
 * @brief point_on_axis
 * @param c reference point
 * @param a axis
 * @param p point
 * @return true if the point is on axis
 */
bool point_on_axis (float *c, float*a, float *p) {
	float v1[3],v2[3];
	add_v3_v3v3(v1,c,a);
	sub_v3_v3v3(v2,p,c);
	return fabs(dot_v3v3(v1,v2)) < DIM_CONSTRAINT_PRECISION;
}


/**
 * @brief point_dist_to_plane
 * @param c reference point (on plane)
 * @param a axis
 * @param p point
 * @return distance perp to plane
 */
float point_dist_to_plane (float *c, float *a, float *p){
	float r[3],m[3];
	sub_v3_v3v3(r,p,c);
	project_v3_v3v3(m,r,a);
	return len_v3(m);
}

/**
 * @brief point_dist_to_plane
 * @param c reference point (on plane)
 * @param a axis
 * @param p point
 * @return true if distance to plane is 0
 */
bool point_on_plane (float *c, float *a, float *p) {
	return fabs(point_dist_to_plane(c,a,p))<DIM_CONSTRAINT_PRECISION;
}


void v_perpendicular_to_axis(float *r, float *c, float *p, float *a) {
	float m[3];
	sub_v3_v3v3(r,p,c);
	project_v3_v3v3(m,r,a);
	sub_v3_v3(r,m);
}