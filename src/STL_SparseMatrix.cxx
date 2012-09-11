#include <iostream>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>

#include "STL_SparseMatrix.h"

/** Construction Function **/
STL_SparseMatrix::STL_SparseMatrix(_IDX row_dim, _IDX col_dim)
{
	//printf("STL_SparseMatrix::STL_SparseMatrix(_IDX row_dim, _IDX col_dim)\n");
    assert(row_dim > -1 && col_dim > -1);
	_row_dim = row_dim;
	_col_dim = col_dim;
	rcv = new RCV;
	rptr = new _IDX[row_dim + 1];
	rlist = (_IDX *)0;
	clist = (_IDX *)0;
	vlist = (_ValueType *)0;
    update_rptr();
}

STL_SparseMatrix::STL_SparseMatrix(_IDX row_dim, _IDX col_dim, _IDX * row_list, _IDX * col_list, _ValueType * val_list, _IDX size)
{
	//printf("STL_SparseMatrix::STL_SparseMatrix(_IDX row_dim, _IDX col_dim, _IDX * row_list, _IDX * col_list, _ValueType * val_list, _IDX size)\n");
    assert(row_dim > -1 && col_dim > -1);
	_row_dim = row_dim;
	_col_dim = col_dim;
	rcv = new RCV;
	rptr = new _IDX[row_dim + 1];
	rlist = (_IDX *)0;
	clist = (_IDX *)0;
	vlist = (_ValueType *)0;
	add_elems(row_list, col_list, val_list, size);
}

STL_SparseMatrix::~STL_SparseMatrix()
{
	//printf("STL_SparseMatrix::~STL_SparseMatrix()\n");
	delete rcv;
	delete [] rptr;
	if (rlist != (_IDX *)0)
	{	
		//printf("Ready to delete rlist\n");
		delete [] rlist;
	}
	if (clist != (_IDX *)0)
	{	
		//printf("Ready to delete clist\n");
		delete [] clist;
	}
	if (vlist != (_ValueType *) 0)
	{	
		//printf("Ready to delete vlist\n");
		delete [] vlist;
	}
}

/** Private functions **/
void STL_SparseMatrix::update_rptr()
{
	//printf("STL_SparseMatrix::update_rptr()\n");
    _IDX * count = rptr;
	count[0] = 0;
	for (_IDX i = 1; i <= _row_dim; i ++)
		count[i] = 0;

	RCV::const_iterator it;
	_IDX rowid;

	for (it = rcv->begin(); it != rcv->end(); it ++)
	{
		const elemID& elemid = (*it).first;
		rowid = elemid.first;
		count[rowid+1] ++;
	}
	for (_IDX i = 1; i <= _row_dim; i ++)
		count[i] += count[i-1];
}

void STL_SparseMatrix::generate_rcvlist()
{
	//printf("STL_SparseMatrix::generate_rcvlist()\n");
    _IDX nnz = get_nnz();
    assert(nnz > -1);

	rlist = new _IDX[nnz];
	clist = new _IDX[nnz];
	vlist = new _ValueType[nnz];

	RCV::const_iterator it;
    int i;
	for (i = 0,it = rcv->begin(); it != rcv->end(); i ++,it ++)
	{
		const elemID& elemid = (*it).first;
		rlist[i] = elemid.first;
		clist[i] = elemid.second;
		vlist[i] = (*it).second;
    }
}

void STL_SparseMatrix::clear_rcvlist()
{
	//printf("STL_SparseMatrix::clear_rcvlist()\n");
	if (rlist != (_IDX *)0)
	{	
		//printf("Ready to delete rlist\n");
		delete rlist;
		rlist = (_IDX *)0;
	}
	if (clist != (_IDX *)0)
	{	
		//printf("Ready to delete clist\n");
		delete clist;
		clist = (_IDX *)0;
	}
	if (vlist != (_ValueType *) 0)
	{	
		//printf("Ready to delete vlist\n");
		delete vlist;
		vlist = (_ValueType *)0;
	}
}

void STL_SparseMatrix::add_elem_no_update(_IDX r, _IDX c, _ValueType v)
{
	//printf("STL_SparseMatrix::add_elem_no_update(_IDX r, _IDX c, _ValueType v)\n");
    assert(r < _row_dim && c < _col_dim);
    assert(r > -1 && c > -1);
	rcv->insert(std::pair<elemID,_ValueType>(elemID(r,c),v));
}

/** Public functions **/
void STL_SparseMatrix::add_elem(_IDX r, _IDX c, _ValueType v)
{
	//printf("STL_SparseMatrix::add_elem(_IDX r, _IDX c, _ValueType v)\n");
    assert(r < _row_dim && c < _col_dim);
    assert(r > -1 && c > -1);
	rcv->insert(std::pair<elemID,_ValueType>(elemID(r,c),v));
	update_rptr();
}

void STL_SparseMatrix::add_elems(_IDX * r, _IDX * c, _ValueType * v, _IDX size)
{
	//printf("STL_SparseMatrix::add_elems(_IDX * r, _IDX * c, _ValueType * v, _IDX size)\n");
	_IDX i;
	for (i = 0; i < size; i ++)
    {
        assert(r[i] < _row_dim && c[i] < _col_dim);
        assert(r[i] > -1 && c[i] > -1);
	    rcv->insert(std::pair<elemID,_ValueType>(elemID(r[i],c[i]),v[i]));
    }
	update_rptr();
}

void STL_SparseMatrix::print_elems()
{
	//printf("STL_SparseMatrix::print_elems()\n");
	_IDX i,size = get_nnz();
	RCV::const_iterator it;

	printf("\nDimension: [%d x %d]\n", get_row_dim(), get_col_dim());
	printf("Triple Groups: [%d]\n", size);
	for (it = rcv->begin(); it != rcv->end(); it ++)
	{
		const elemID& elemid = (*it).first;
		//printf("<%4d,%4d,\t%lf>\n", elemid.first, elemid.second, (*it).second);
        std::cout << "<" << elemid.first << "\t" << elemid.second << "\t" << (*it).second << std::endl;
	}
	printf("Row Ptr\n");
	for (i = 0; i <= _row_dim; i ++)
		printf("%d\n", rptr[i]);
	printf("\n");
}

/** Static Functions **/

/* Matrix-Matrix Part */
	// transpose
STL_SparseMatrix * STL_SparseMatrix::mat_transpose(STL_SparseMatrix * mat)
{
	//printf("STL_SparseMatrix::mat_transpose(STL_SparseMatrix * mat)\n");
	_IDX mat_row_dim = mat->get_row_dim();
	_IDX mat_col_dim = mat->get_col_dim();
	_IDX trans_row_dim = mat_col_dim;
	_IDX trans_col_dim = mat_row_dim;

    mat->generate_rcvlist();

    _IDX * mat_row_list = mat->get_rlist();
    _IDX * mat_col_list = mat->get_clist();
    _ValueType * mat_val_list = mat->get_vlist();
    int mat_nnz = mat->get_nnz();

	STL_SparseMatrix * trans = new STL_SparseMatrix(trans_row_dim, trans_col_dim, mat_col_list, mat_row_list, mat_val_list, mat_nnz);

    mat->clear_rcvlist();
	return trans;
}
	// add
STL_SparseMatrix * STL_SparseMatrix::mat_add(STL_SparseMatrix * mat1, STL_SparseMatrix * mat2)
{
	//printf("STL_SparseMatrix::mat_add(STL_SparseMatrix * mat1, STL_SparseMatrix * mat2)\n");
	assert(mat1->get_row_dim() == mat2->get_row_dim());
	assert(mat1->get_col_dim() == mat2->get_col_dim());

	_IDX maxsize = mat1->get_nnz() + mat2->get_nnz();
	_IDX * mat_row_list = new _IDX[maxsize];
	_IDX * mat_col_list = new _IDX[maxsize];
	_ValueType * mat_val_list = new _ValueType[maxsize];

	RCV::const_iterator it1, it2;
	_IDX realsize;
	elemID id1, id2;
	_ValueType v1,v2;

	it1 = mat1->rcv->begin();
	it2 = mat2->rcv->begin();
	realsize = 0;
	while(it1 != mat1->rcv->end() || it2 != mat2->rcv->end())
	{
		id1 = (*it1).first;
		id2 = (*it2).first;
		v1 = (*it1).second;
		v2 = (*it2).second;
		if (elemID_compare(id1,id2) == -1)
		{
			mat_row_list[realsize]=id1.first;
			mat_col_list[realsize]=id1.second;
			mat_val_list[realsize]=v1;
			it1 ++;
		}
		else if (elemID_compare(id1,id2) == 1)
		{
			mat_row_list[realsize]=id2.first;
			mat_col_list[realsize]=id2.second;
			mat_val_list[realsize]=v2;
			it2 ++;
		}
		else
		{
			mat_row_list[realsize]=id2.first;
			mat_col_list[realsize]=id2.second;
			mat_val_list[realsize]=v1+v2;
			it1 ++;
			it2 ++;
		}
		realsize ++;
	}
	STL_SparseMatrix * addres = new STL_SparseMatrix(mat1->get_row_dim(), mat1->get_col_dim(), mat_row_list, mat_col_list, mat_val_list, realsize);
	delete [] mat_row_list;
	delete [] mat_col_list;
	delete [] mat_val_list;
	return addres;
}
	// sub
STL_SparseMatrix * STL_SparseMatrix::mat_sub(STL_SparseMatrix * mat1, STL_SparseMatrix * mat2)
{
	//printf("STL_SparseMatrix::mat_sub(STL_SparseMatrix * mat1, STL_SparseMatrix * mat2)\n");
	assert(mat1->get_row_dim() == mat2->get_row_dim());
	assert(mat1->get_col_dim() == mat2->get_col_dim());

	_IDX maxsize = mat1->get_nnz() + mat2->get_nnz();
	_IDX * mat_row_list = new _IDX[maxsize];
	_IDX * mat_col_list = new _IDX[maxsize];
	_ValueType * mat_val_list = new _ValueType[maxsize];

	RCV::const_iterator it1, it2;
	_IDX realsize;
	elemID id1, id2;
	_ValueType v1,v2;

	it1 = mat1->rcv->begin();
	it2 = mat2->rcv->begin();
    realsize = 0;
	while(it1 != mat1->rcv->end() || it2 != mat2->rcv->end())
	{
		id1 = (*it1).first;
		id2 = (*it2).first;
		v1 = (*it1).second;
		v2 = (*it2).second;
		if (elemID_compare(id1,id2) == -1)
		{
			mat_row_list[realsize]=id1.first;
			mat_col_list[realsize]=id1.second;
			mat_val_list[realsize]=v1;
			it1 ++;
		}
		else if (elemID_compare(id1,id2) == 1)
		{
			mat_row_list[realsize]=id2.first;
			mat_col_list[realsize]=id2.second;
			mat_val_list[realsize]=-v2;
			it2 ++;
		}
		else
		{
			mat_row_list[realsize]=id2.first;
			mat_col_list[realsize]=id2.second;
			mat_val_list[realsize]=v1-v2;
			it1 ++;
			it2 ++;
		}
        realsize ++;
	}
	STL_SparseMatrix * subres = new STL_SparseMatrix(mat1->get_row_dim(), mat1->get_col_dim(), mat_row_list, mat_col_list, mat_val_list, realsize);
	delete [] mat_row_list;
	delete [] mat_col_list;
	delete [] mat_val_list;
	return subres;
}
	// mul
STL_SparseMatrix * STL_SparseMatrix::mat_mul(STL_SparseMatrix * mat1, STL_SparseMatrix * mat2)
{
	//printf("STL_SparseMatrix::mat_mul(STL_SparseMatrix * mat1, STL_SparseMatrix * mat2)\n");
	assert(mat1->get_col_dim() == mat2->get_row_dim());
	STL_SparseMatrix * mulres = new STL_SparseMatrix(mat1->get_row_dim(),mat2->get_col_dim());
	int row,col,i,j;
    int nnz;                                            // current nnz of row of mat1
	_ValueType tmpsum;
	rowPtr * p1 = mat1->get_rptr();
	rowPtr * p2 = mat2->get_rptr();
	mat1->generate_rcvlist();
	mat2->generate_rcvlist();
	colList * clist_1 = mat1->clist;
	colList * clist_2 = mat2->clist;
	valList * vlist_1 = mat1->vlist;
	valList * vlist_2 = mat2->vlist;

    int nnz_max_1 = mat1->get_col_dim() * 0.01;         // max nnz of each row of mat1
    int nnz_max_2 = mat2->get_col_dim() * 0.01;         // max nnz of each row of mat2
    nnz_max_1 = (nnz_max_1 < 256) ? 256 : nnz_max_1;
    nnz_max_2 = (nnz_max_2 < 256) ? 256 : nnz_max_2;
    assert(nnz_max_1 < 10000);
    assert(nnz_max_2 < 10000);
    int nnz_max = nnz_max_1 * nnz_max_2;

    //int ** curr_col = new int* [nnz];
    //int ** edge_col = new int* [nnz];
    int col_start_id, col_end_id;                             // just for mat2
    int col_min, col_max;                               // just for mat2
    _ValueType * buf = new _ValueType[nnz_max];
    int nnzidx_1, nnzidx_2;                             // to locate element in storage view
    int nnzcol_1, nnzrow_2;                             // to locate element in math view


    // do jobs for each line of mat1 and whole mat2
    for (row = 0; row < mat1->get_row_dim(); row ++)
    {
        // nnz of current row
        nnz = p1[row+1] - p1[row];
        if (nnz == 0)
            continue;

        // reset possible results of current row
        memset(buf, 0, sizeof(_ValueType) * nnz_max);

        // prepare nnz col ptr for each nnz row of mat2
        col_min = mat2->get_col_dim();
        col_max = -1;
        for (j = 0; j < nnz; j ++)
        {
            nnzidx_1 = p1[row] + j;             // index of nnz col of mat1 in storage view
            nnzcol_1 = clist_1[nnzidx_1];       // index of nnz col of mat1 in math view
            nnzrow_2 = nnzcol_1;                // index of nnz row of mat2 in math view
            //nnzidx_2 = p2[nnzrow_2];            // index of nnz row of mat2 in storage view
            //curr_col[j] = &clist_2[nnzidx_2];   // pointer to index of nnz col of nnz row of mat2 in storage view 
            //nnzidx_2 = p2[nnzrow_2 + 1];
            //edge_col[j] = &clist_2[nnzidx_2];
            if (p2[nnzrow_2] != p2[nnzrow_2 + 1])       // means that nnz exists in current row
            {
                if (clist_2[p2[nnzrow_2]] < col_min)
                    col_min = clist_2[p2[nnzrow_2]];
                if (clist_2[p2[nnzrow_2 + 1] - 1] > col_max)
                    col_max = clist_2[p2[nnzrow_2 + 1] - 1];
            }
        }
    
        //printf("row: %d\ncol_min: %d\ncol_max: %d\n", row, col_min, col_max);

        // start nnz calculation of mat1*mat2
        for (j = 0; j < nnz; j ++)
        {
            nnzidx_1 = p1[row] + j;             // index of nnz col of mat1 in storage view
            nnzcol_1 = clist_1[nnzidx_1];       // index of nnz col of mat1 in math view
            nnzrow_2 = nnzcol_1;                // index of nnz row of mat2 in math view

            col_start_id = p2[nnzrow_2];
            col_end_id = p2[nnzrow_2+1];
            for (i = col_start_id; i < col_end_id; i ++)
            {
                buf[clist_2[i]- col_min] += vlist_1[nnzidx_1] * vlist_2[i];
                //printf("buf[%d] = %lf\n", clist_2[i]- col_min, buf[clist_2[i]- col_min]);
            }
        }

        // ensure nnz and dump
        for (col = col_min; col <= col_max; col ++)
        {
           if (fabs(buf[col - col_min]) > 1e-30)
           {
               //printf("add (%d,%d,%lf)\n", row,col,buf[col-col_min]);
               mulres->add_elem_no_update(row,col,buf[col-col_min]);           
           }
        }
    }
    mulres->update_rptr();
    delete [] buf;
	return mulres;
}

/* Matrix-Vector Part */
Vec * STL_SparseMatrix::mat_vec_mul(STL_SparseMatrix * mat, Vec * vec)
{
	//printf("STL_SparseMatrix::mat_vec_mul(STL_SparseMatrix * mat, Vec * vec)\n");
	assert(mat->get_col_dim() == vec->size());
	Vec * mv = new Vec;
	mv->reserve(mat->get_row_dim());

	_IDX i;
	for (i = 0; i < mat->get_row_dim(); i ++)
		mv->push_back(0.0);

	RCV::const_iterator it;
	for (it = mat->rcv->begin(); it != mat->rcv->end(); it ++)
	{
		const elemID& elemid = (*it).first;
		(*mv)[elemid.first] += (*vec)[elemid.second] * (*it).second;
	}
	return mv;
}

/* Vector Part */
Vec * STL_SparseMatrix::vec_nadd(int argcnt,...)
{
	std::vector<Vec *> veclist;
	va_list ap;
	va_start(ap, argcnt);
	while(argcnt-- > 0)
		veclist.push_back(va_arg(ap,Vec *));
	va_end(ap);
	size_t lsize = veclist.size();
	//printf("veclist.size = %ld\n", lsize);
	size_t vsize = veclist[0]->size();
	for (size_t i = 1; i < lsize; i ++)
	  assert(vsize == veclist[i]->size());
	Vec * nadd = new Vec;
	nadd->reserve(vsize);
	for (size_t i = 0; i < vsize; i ++)
	{
		_ValueType sum = 0;
		for (size_t j = 0; j < lsize; j ++)
			sum += (*(veclist[j]))[i];
		nadd->push_back(sum);
	}
	return nadd;
}

Vec * STL_SparseMatrix::vec_subdiv(Vec * vec1, Vec * vec2)
{
	assert(vec1->size() == vec2->size());
	size_t size = vec1->size();
	Vec * sub = new Vec;
	sub->reserve(size);
	for (size_t i = 0; i < size; i ++)
	{
		if (fabs((*vec2)[i]) < 1e-30)
		{
			if (fabs((*vec1)[i]) < 1e-30)
			  sub->push_back(0.0);
			else
			  sub->push_back(8888.8888);
		}
		else
			sub->push_back((*vec1)[i] - (*vec2)[i]);
	}
	return sub;
}

_ValueType STL_SparseMatrix::vec_rmsd(Vec * v)
{
	size_t size = v->size();
	_ValueType rmsd = 0.0;
	for (size_t i = 0; i < size; i ++)
	{
		rmsd += (*v)[i] * (*v)[i];
	}
	rmsd = sqrt(rmsd / size);
	return rmsd;
}

/* Other */
short STL_SparseMatrix::elemID_compare(const elemID& id1, const elemID& id2)
{
	if (id1.first < id2.first)
		return -1;
	else if (id1.first > id2.first)
		return 1;
	else
	{
		if (id1.second < id2.second)
			return -1;
		else if (id1.second > id2.second)
			return 1;
	}
	return 0;
}
