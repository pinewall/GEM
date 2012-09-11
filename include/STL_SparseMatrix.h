#ifndef STL_SPARSEMATRIX_H
#define STL_SPARSEMATRIX_H

#include <vector>
#include <map>
#include "types.h"

typedef std::pair<_IDX,_IDX> elemID;

struct elemIDcomp
{
	bool operator()(const elemID & lhs, const elemID & rhs) const
	{
		if (lhs.first != rhs.first)
			return (lhs.first < rhs.first);
		else
			return (lhs.second < rhs.second);
	}
};

typedef std::map<elemID,_ValueType,elemIDcomp> RCV; 
typedef std::vector<_ValueType> Vec;

class STL_SparseMatrix
{
	protected:
		_IDX _row_dim;
		_IDX _col_dim;
		RCV * rcv;
		rowPtr * rptr;
		rowList * rlist;
		colList * clist;
		valList * vlist;
		
	public:
		STL_SparseMatrix(_IDX row_dim, _IDX col_dim);
		STL_SparseMatrix(_IDX row_dim, _IDX col_dim, _IDX * row_list, _IDX * col_list, _ValueType * val_list, _IDX size);
		~STL_SparseMatrix();
		
		_IDX		get_row_dim()	{	return _row_dim;	}
		_IDX		get_col_dim()	{	return _col_dim;	}
		_IDX		get_nnz()		{	return rcv->size();	}
		_IDX    *	get_rptr()		{	return rptr;		}
		RCV     *	get_rcv()		{	return rcv;			}
		_IDX    *	get_rlist()		{	return rlist;		}
		_IDX    *	get_clist()		{	return clist;		}
		_ValueType *get_vlist()		{	return vlist;		}

		/* absolutely public */
		void add_elem(_IDX r, _IDX c, _ValueType v);
		void add_elems(_IDX * r, _IDX * c, _ValueType * v, _IDX size);
		//void del_elem(_IDX r, _IDX c, _ValueType v);
		void print_elems();
		
		/* more private */
		void add_elem_no_update(_IDX r, _IDX c, _ValueType v);
		void update_rptr();
		void generate_rcvlist();
		void clear_rcvlist();
		
		/* Matrix & Vector operations */
		static STL_SparseMatrix * mat_transpose(STL_SparseMatrix * mat);
		static STL_SparseMatrix * mat_add(STL_SparseMatrix * mat1, STL_SparseMatrix * mat2);
		static STL_SparseMatrix * mat_sub(STL_SparseMatrix * mat1, STL_SparseMatrix * mat2);
		static STL_SparseMatrix * mat_mul(STL_SparseMatrix * mat1, STL_SparseMatrix * mat2);
		static Vec * mat_vec_mul(STL_SparseMatrix * mat, Vec * vec);
		static Vec * vec_nadd(int argcnt,...);
		static Vec * vec_subdiv(Vec * vec1, Vec * vec2);
		//static Vec * vec_dotmul(Vec * vec1, Vec * vec2);
		//static Vec * vec_dotdiv(Vec * vec1, Vec * vec2);
		//static Vec * vec_sub(Vec * vec1, Vec * vec2);
		//static _ValueType  vec_dot(Vec * vec1, Vec * vec2);
		//static _ValueType vec_norm2(Vec * vec);
		static _ValueType vec_rmsd(Vec * v);
		
		static short elemID_compare(const elemID& id1, const elemID& id2);
};
#endif
