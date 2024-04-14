#include <stdlib.h>
#include <string.h>
#include <sgx_trts.h>
#include <algorithm>
#include "math.h"
#include "util.h"
#include "enclave_mcsk.h"

struct mcsk* m_mcsk = NULL;
uint32_t PCA_SNP_count = 0;
uint32_t* PCA_SNPs = NULL;

void pca_init(uint32_t width, uint32_t* IDs)
{
        PCA_SNP_count = width;
        PCA_SNPs = IDs;
}

void mcsk_init_seeds()
{
	m_mcsk->seeds = (uint64_t*) malloc(4 * sizeof(uint64_t));
	for(size_t i = 0; i < 2; i++)
	{
		m_mcsk->seeds[(i << 1)] = my_sgx_rand();
		while (m_mcsk->seeds[(i << 1)] == 0)
		{
			m_mcsk->seeds[(i << 1)] = my_sgx_rand();
		}
		m_mcsk->seeds[(i << 1) + 1] = my_sgx_rand();
	}
}

void mcsk_init(uint32_t width, uint32_t num_pc, float eps) 
{
	m_mcsk = (mcsk*) malloc(sizeof(mcsk));

	m_mcsk->m = width;
	m_mcsk->k = num_pc;
	m_mcsk->epsilon = eps;
	m_mcsk->depth = PCA_SNP_count;
	uint32_t depth = m_mcsk->depth;
	m_mcsk->depth_minus_one = depth - 1;

	m_mcsk->msketchf = (float**) malloc(depth * sizeof(float*));

	for(size_t i = 0; i < depth; i++)
	{
		// Last column: for storing the row mean
		m_mcsk->msketchf[i] = (float*) malloc((width + 1) * sizeof(float));

		// Non-standard way of initialization
		// Proper way is to set each entry 0.0 separately
		memset(m_mcsk->msketchf[i], 0, (width + 1) * sizeof(float));
	}

	mcsk_init_seeds();
}

void mcsk_quick_init(uint32_t width, uint32_t depth) 
{
	m_mcsk->m = width;
	m_mcsk->k = 2;
	m_mcsk->epsilon = sqrt(5.0 / depth);
	m_mcsk->depth = depth;
	m_mcsk->depth_minus_one = depth - 1;

	m_mcsk->msketchf = (float**) malloc(depth * sizeof(float*));
	for (size_t i = 0; i < depth; i++) 
	{
		// Last column: for storing the row mean
		m_mcsk->msketchf[i] = (float*) malloc((width + 1) * sizeof(float));
		// Non-standard way of initialization
		// Proper way is to set each entry 0.0 separately
		memset(m_mcsk->msketchf[i], 0, (width + 1) * sizeof(float));
	}

	mcsk_init_seeds();
}

void mcsk_setk(uint32_t num_pc) 
{
	m_mcsk->k = num_pc;
	m_mcsk->epsilon = sqrt(m_mcsk->k * 1.0 / m_mcsk->depth);
}

void mcsk_free() 
{
	for (size_t i = 0; i < m_mcsk->depth; i++)
	{
		if (m_mcsk->msketchf[i] != NULL)
		{
			free(m_mcsk->msketchf[i]);
		}
	}
	if (m_mcsk->msketchf != NULL)
	{
		free(m_mcsk->msketchf);
	}

	if (m_mcsk->seeds != NULL)
	{
		free(m_mcsk->seeds);
	}
}

uint32_t pca_snps_find(uint32_t ID)
{
	uint32_t* p = stlpmtx_std::lower_bound(PCA_SNPs, PCA_SNPs + PCA_SNP_count, ID);
	if (p == PCA_SNPs + PCA_SNP_count || *p != ID) { return PCA_SNP_count; }
	else { return p - PCA_SNPs; }
}

void mcsk_update_var(uint64_t item, uint32_t col, float count) 
{
	uint32_t row = pca_snps_find(item);
	float count_ = count;
	if (row >= PCA_SNP_count) return;
	m_mcsk->msketchf[row][col] = m_mcsk->msketchf[row][col] + count_;
	m_mcsk->msketchf[row][m_mcsk->m] = m_mcsk->msketchf[row][m_mcsk->m] + count_;
}

void mcsk_mean_centering() 
{
	uint32_t d = m_mcsk->depth, w = m_mcsk->m;
	for (uint32_t row = 0; row < d; row++) 
	{ 
		for (uint32_t col = 0; col < w; col++)
			m_mcsk->msketchf[row][col] -= (m_mcsk->msketchf[row][w] * (1.0 / m_mcsk->m));
	}
}

float** getmsk() 
{
	return m_mcsk->msketchf;
}
