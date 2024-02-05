#include <stdlib.h>
#include <string.h>
#include <sgx_trts.h>
#include <algorithm>

//#include "config.h"
//#include "Enclave_t.h"
//#include "enclave_crypto.h"
//#include "math.h"
//#include "enclave_mh.h"
//#include "enclave_oa.h"
//#include "enclave_rhht.h"
//#include "enclave_cmtf.h"
#include "enclave_csk.h"
#include "fileio.h"
//#include "enclave_cms.h"
//#include "enclave_mcsk.h"
//#include "svd.h"
#include "util.h"

uint64_t uniq_access_count = 0;
struct csk* m_csk = NULL;
struct uniq* m_uniq = NULL;

static inline uint32_t calc_hash(uint64_t x, uint64_t a, uint64_t b)
{
	uint64_t result = a * x + b;
	result = (result & 0x7FFFFFFF) + (result >> 31);

	if(result >= 0x7FFFFFFF)
	{
		return (uint32_t) (result - 0x7FFFFFFF);
	}

	return (uint32_t) result;
}

void csk_init_param(uint32_t width, uint32_t depth)
{
	m_csk = (csk*) malloc(sizeof(csk));
	m_csk->width = width;
	m_csk->depth = depth;
	m_csk->width_minus_one = width - 1;
	m_csk->seeds = NULL;
	m_csk->s_thres = 200;
}

void uniq_init(uint32_t width, uint32_t* IDs)
{
	m_uniq = (uniq*) malloc(sizeof(uniq));
	m_uniq->width = width;
	m_uniq->IDs = IDs;
	m_uniq->buckets = (float*) malloc(width * sizeof(float));
	memset(m_uniq->buckets, 0, width * sizeof(float));
}

void uniq_free()
{
	free(m_uniq->buckets);
}

void csk_init_seeds()
{
	uint32_t d = m_csk->depth;
	m_csk->seeds = (uint64_t*) malloc(d * sizeof(uint64_t) << 2);

	for(size_t i = 0; i < d << 1; i++)
	{
		m_csk->seeds[(i << 1)] = my_sgx_rand();
		while(m_csk->seeds[(i << 1)] == 0)
		{
			m_csk->seeds[(i << 1)] = my_sgx_rand();
		}
		m_csk->seeds[(i << 1) + 1] = my_sgx_rand();
	}
	m_csk->seeds[0] = 344240983;
	m_csk->seeds[1] = 504526911;
	m_csk->seeds[2] = 101046549;
	m_csk->seeds[3] = 1644254098;
	m_csk->seeds[4] = 533510955;
	m_csk->seeds[5] = 689379691;
	m_csk->seeds[6] = 921762454;
	m_csk->seeds[7] = 826370884;
	m_csk->seeds[8] = 854722166;
	m_csk->seeds[9] = 1210521217;
	m_csk->seeds[10] = 1570464714;
	m_csk->seeds[11] = 1330533456;
	m_csk->seeds[12] = 1046745755;
	m_csk->seeds[13] = 961877355;
	m_csk->seeds[14] = 2046846889;
	m_csk->seeds[15] = 1251096658;
	m_csk->seeds[16] = 809429534;
	m_csk->seeds[17] = 1261839107;
	m_csk->seeds[18] = 689315085;
	m_csk->seeds[19] = 1369798213;
	m_csk->seeds[20] = 1885354250;
	m_csk->seeds[21] = 1610937831;
	m_csk->seeds[22] = 1307750239;
	m_csk->seeds[23] = 166508991;
	m_csk->seeds[24] = 1160608010;
	m_csk->seeds[25] = 1312485119;
	m_csk->seeds[26] = 343831418;
	m_csk->seeds[27] = 1678706705;
	m_csk->seeds[28] = 1624329680;
	m_csk->seeds[29] = 1604486465;
	m_csk->seeds[30] = 85023918;
	m_csk->seeds[31] = 20123961;
	/*m_csk->seeds[0] = 893117481;
	m_csk->seeds[1] = 2067202694;
	m_csk->seeds[2] = 1120264039;
	m_csk->seeds[3] = 1885735605;
	m_csk->seeds[4] = 2118300182;
	m_csk->seeds[5] = 285263636;
	m_csk->seeds[6] = 1533314557;
	m_csk->seeds[7] = 1787878813;
	m_csk->seeds[8] = 1644147734;
	m_csk->seeds[9] = 998645891;
	m_csk->seeds[10] = 1417960397;
	m_csk->seeds[11] = 870388511;
	m_csk->seeds[12] = 277374289;
	m_csk->seeds[13] = 44180178;
	m_csk->seeds[14] = 1967079161;
	m_csk->seeds[15] = 609737532;
	m_csk->seeds[16] = 797398574;
	m_csk->seeds[17] = 392757608;
	m_csk->seeds[18] = 725263426;
	m_csk->seeds[19] = 1597620;
	m_csk->seeds[20] = 1561660070;
	m_csk->seeds[21] = 1553465243;
	m_csk->seeds[22] = 1035132085;
	m_csk->seeds[23] = 1558440476;
	m_csk->seeds[24] = 364470766;
	m_csk->seeds[25] = 1821421032;
	m_csk->seeds[26] = 1636403805;
	m_csk->seeds[27] = 1873454512;
	m_csk->seeds[28] = 1139956153;
	m_csk->seeds[29] = 1766671330;
	m_csk->seeds[30] = 1489122362;
	m_csk->seeds[31] = 445248290;
	m_csk->seeds[0] = 720000562;
	m_csk->seeds[1] = 1991531570;
	m_csk->seeds[2] = 434130229;
	m_csk->seeds[3] = 236176094;
	m_csk->seeds[4] = 1410364956;
	m_csk->seeds[5] = 558832819;
	m_csk->seeds[6] = 1082144829;
	m_csk->seeds[7] = 1843615989;
	m_csk->seeds[8] = 1960050809;
	m_csk->seeds[9] = 1183824161;
	m_csk->seeds[10] = 781972793;
	m_csk->seeds[11] = 213752735;
	m_csk->seeds[12] = 1994798233;
	m_csk->seeds[13] = 538550749;
	m_csk->seeds[14] = 579807306;
	m_csk->seeds[15] = 818076076;
	m_csk->seeds[16] = 362290779;
	m_csk->seeds[17] = 1028418883;
	m_csk->seeds[18] = 1095440864;
	m_csk->seeds[19] = 1928205998;
	m_csk->seeds[20] = 1790571589;
	m_csk->seeds[21] = 1149718712;
	m_csk->seeds[22] = 293662817;
	m_csk->seeds[23] = 963238324;
	m_csk->seeds[24] = 904061715;
	m_csk->seeds[25] = 438398632;
	m_csk->seeds[26] = 253322470;
	m_csk->seeds[27] = 527602069;
	m_csk->seeds[28] = 1077411604;
	m_csk->seeds[29] = 1756214037;
	m_csk->seeds[30] = 982474161;
	m_csk->seeds[31] = 388067056;*/
}

void csk_init(uint32_t width, uint32_t depth)
{
	csk_init_param(width, depth);
	
	m_csk->sketch = (int16_t**) malloc(depth * sizeof(int16_t*));
	m_csk->sketchf = NULL;
	for(size_t i = 0; i < depth; i++)
	{
		m_csk->sketch[i] = (int16_t*) malloc(width * sizeof(int16_t));
		memset(m_csk->sketch[i], 0, width * sizeof(int16_t));
	}

	csk_init_seeds();
}

void csk_init_f(uint32_t width, uint32_t depth)
{
	csk_init_param(width, depth);
	
	m_csk->sketch = NULL;
	m_csk->sketchf = (float**) malloc(depth * sizeof(float*));
	for(size_t i = 0; i < depth; i++)
	{
		m_csk->sketchf[i] = (float*) malloc(width * sizeof(float));
		memset(m_csk->sketchf[i], 0, width * sizeof(float));
	}

	csk_init_seeds();
}

void csk_free()
{
	if(m_csk->seeds != NULL)
	{
		free(m_csk->seeds);
	}

	if(m_csk->sketch != NULL)
	{
		for(size_t i = 0; i < m_csk->depth; i++)
		{
			free(m_csk->sketch[i]);
		}
		free(m_csk->sketch);
	}

	if(m_csk->sketchf != NULL)
	{
		for(size_t i = 0; i < m_csk->depth; i++)
		{
			free(m_csk->sketchf[i]);
		}
		free(m_csk->sketchf);
	}

	free(m_csk);
}

void csk_setsth(int new_threshold)
{
	m_csk->s_thres = new_threshold;
}

void csk_update_var(uint64_t item, int16_t count)
{
	uint32_t hash;
	uint32_t pos;
	int16_t count_;

	for(size_t i = 0; i < m_csk->depth; i++)
	{
		hash = calc_hash(item, m_csk->seeds[i << 1], m_csk->seeds[(i << 1) + 1]);
//		hash = cal_hash(item, m_csk->seeds[i << 1], m_csk->seeds[(i << 1) + 1]);
		pos = hash & m_csk->width_minus_one;

		uint32_t temp = (i + m_csk->depth) << 1;
		hash = calc_hash(item, m_csk->seeds[temp], m_csk->seeds[temp + 1]);
//		hash = calc_hash(item, m_csk->seeds[(i + m_csk->depth) << 1], m_csk->seeds[((i + m_csk->depth) << 1) + 1]);
//		hash = cal_hash(item, m_csk->seeds[(i + m_csk->depth) << 1], m_csk->seeds[((i + m_csk->depth) << 1) + 1]);
		count_ = (((hash & 0x1) == 0) ? -1 : 1) * count;

		if(m_csk->sketch[i][pos] >= HASH_MAX_16 && count_ > 0)
		{
			continue;
		}
		if(m_csk->sketch[i][pos] <= HASH_MIN_16 && count_ < 0)
		{
			continue;
		}
		m_csk->sketch[i][pos] = m_csk->sketch[i][pos] + count_;
	}
}

uint32_t uniq_bucket_find(uint32_t ID)
{
	uint32_t* p = stlpmtx_std::lower_bound(m_uniq->IDs, m_uniq->IDs + m_uniq->width, ID);
	if (p == m_uniq->IDs + m_uniq->width || *p != ID) { return m_uniq->width; }
	else { return p - m_uniq->IDs; }
}


/*
 * printf:
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}

void csk_update_var_f(uint64_t item, float count)
{
	uint32_t hash;
	uint32_t pos;
	
	uint32_t uniq_pos = uniq_bucket_find(item);
	//if (item == 100665936) { printf("Before change: %.6f\n", m_uniq->buckets[uniq_pos]); }
	if (uniq_pos < m_uniq->width) {
		uniq_access_count += 1;
		m_uniq->buckets[uniq_pos] += count;
		if (m_uniq->IDs[uniq_pos] != item)
		{ printf("Problem! %u %u %u\n", uniq_pos, m_uniq->IDs[uniq_pos], item); }
		//if (item == 100665936) { printf("After change: %.6f\n", m_uniq->buckets[uniq_pos]); }
	}
	else for(size_t i = 0; i < m_csk->depth; i++)
	{
		hash = cal_hash(item, m_csk->seeds[i << 1], m_csk->seeds[(i << 1) + 1]);
		pos = hash & m_csk->width_minus_one;

		hash = cal_hash(item, m_csk->seeds[(i + m_csk->depth) << 1], m_csk->seeds[((i + m_csk->depth) << 1) + 1]);
		if((hash & 0x1) == 0)
		{
			m_csk->sketchf[i][pos] = m_csk->sketchf[i][pos] - count;
		}
		else
		{
			m_csk->sketchf[i][pos] = m_csk->sketchf[i][pos] + count;
		}
	}
}



/***** Test function *****/
void csk_update_var_row(uint64_t item, int16_t count, size_t row)
{
	uint32_t hash;

	hash = cal_hash(item, m_csk->seeds[row << 1], m_csk->seeds[(row << 1) + 1]);
	uint32_t pos = hash & m_csk->width_minus_one;
	hash = cal_hash(item, m_csk->seeds[(row + m_csk->depth) << 1], m_csk->seeds[((row + m_csk->depth) << 1) + 1]);
	int16_t count_ = (((hash & 0x1) == 0) ? -1 : 1) * count;

	if(m_csk->sketch[row][pos] >= HASH_MAX_16 && count_ > 0)
	{
		return;
	}

	if(m_csk->sketch[row][pos] <= HASH_MIN_16 && count_ < 0)
	{
		return;
	}

	m_csk->sketch[row][pos] = m_csk->sketch[row][pos] + count_;
}

void csk_update_var_row_f(uint64_t item, float count, size_t row)
{
	uint32_t hash;

	hash = cal_hash(item, m_csk->seeds[row << 1], m_csk->seeds[(row << 1) + 1]);
	uint32_t pos = hash & m_csk->width_minus_one;
	hash = cal_hash(item, m_csk->seeds[(row + m_csk->depth) << 1], m_csk->seeds[((row + m_csk->depth) << 1) + 1]);

	if((hash & 0x1) == 0)
	{
		m_csk->sketchf[row][pos] = m_csk->sketchf[row][pos] - count;
	}
	else
	{
		m_csk->sketchf[row][pos] = m_csk->sketchf[row][pos] + count;
	}
}
/***** END: Test function *****/

int16_t csk_query_median_odd(uint64_t item)
{
	int16_t* values;
	int16_t median;
	uint32_t hash;
	uint32_t pos;
	int32_t sign;

	values = (int16_t*) malloc(m_csk->depth * sizeof(int16_t));

	for(size_t i = 0; i < m_csk->depth; i++)
	{
		hash = cal_hash(item, m_csk->seeds[i << 1], m_csk->seeds[(i << 1) + 1]);
		pos = hash & m_csk->width_minus_one;
		hash = cal_hash(item, m_csk->seeds[(i + m_csk->depth) << 1], m_csk->seeds[((i + m_csk->depth) << 1) + 1]);
		sign = ((hash & 0x1) == 0) ? -1 : 1;
		values[i] = m_csk->sketch[i][pos] * sign;
	}

	// Sort values
	qsort(values, m_csk->depth, sizeof(int16_t), cmpfunc_int16);

	// Get median of values
	median = values[m_csk->depth / 2];

	// Free memory
	free(values);

	return median;
}

int16_t csk_query_median_even(uint64_t item)
{
	int16_t* values;
	int16_t median;
	uint32_t hash;
	uint32_t pos;
	int32_t sign;

	values = (int16_t*) malloc(m_csk->depth * sizeof(int16_t));

	for(size_t i = 0; i < m_csk->depth; i++)
	{
		hash = cal_hash(item, m_csk->seeds[i << 1], m_csk->seeds[(i << 1) + 1]);
		pos = hash & m_csk->width_minus_one;
		hash = cal_hash(item, m_csk->seeds[(i + m_csk->depth) << 1], m_csk->seeds[((i + m_csk->depth) << 1) + 1]);
		sign = ((hash & 0x1) == 0) ? -1 : 1;
		values[i] = m_csk->sketch[i][pos] * sign;
	}

	// Sort values
	qsort(values, m_csk->depth, sizeof(int16_t), cmpfunc_int16);

	// Get median of values
	if(values[m_csk->depth / 2] < -(m_csk->s_thres))
	{
		median = values[m_csk->depth / 2 - 1];
	}
	else if(values[m_csk->depth / 2 - 1] > m_csk->s_thres)
	{
		median = values[m_csk->depth / 2];
	}
	else
	{
		median = (values[m_csk->depth / 2 - 1] + values[m_csk->depth / 2]) / 2;
	}

	// Free memory
	free(values);

	return median;
}

float csk_query_median_odd_f(uint64_t item)
{
	uint32_t uniq_pos = uniq_bucket_find(item);
        if (uniq_pos < m_uniq->width) { return m_uniq->buckets[uniq_pos]; }

	float* values;
	float median;
	uint32_t hash;
	uint32_t pos;
	//int sign;

	values = (float*) malloc(m_csk->depth * sizeof(float));

	for(size_t i = 0; i < m_csk->depth; i++)
	{
		hash = cal_hash(item, m_csk->seeds[i << 1], m_csk->seeds[(i << 1) + 1]);
		pos = hash & m_csk->width_minus_one;
		hash = cal_hash(item, m_csk->seeds[(i + m_csk->depth) << 1], m_csk->seeds[((i + m_csk->depth) << 1) + 1]);
		//sign = ((hash & 0x1) == 0) ? -1 : 1;
		if((hash & 0x1) == 0)
		{
			values[i] = -m_csk->sketchf[i][pos];
		}
		else
		{
			values[i] = m_csk->sketchf[i][pos];
		}
		//values[i] = m_csk->sketchf[i][pos] * sign;
	}

	// Sort values
	qsort(values, m_csk->depth, sizeof(float), cmpfunc_float);

	// Get median of values
	median = values[m_csk->depth / 2];

	// Free memory
	free(values);

	return median;
}

float csk_query_median_even_f(uint64_t item)
{
	uint32_t uniq_pos = uniq_bucket_find(item);
        if (uniq_pos < m_uniq->width) { return m_uniq->buckets[uniq_pos]; }

	float* values;
	float median;
	uint32_t hash;
	uint32_t pos;
	//int sign;

	values = (float*) malloc(m_csk->depth * sizeof(float));

	for(size_t i = 0; i < m_csk->depth; i++)
	{
		hash = cal_hash(item, m_csk->seeds[i << 1], m_csk->seeds[(i << 1) + 1]);
		pos = hash & m_csk->width_minus_one;
		hash = cal_hash(item, m_csk->seeds[(i + m_csk->depth) << 1], m_csk->seeds[((i + m_csk->depth) << 1) + 1]);
		//sign = ((hash & 0x1) == 0) ? -1 : 1;
		//values[i] = m_csk->sketch32[i][pos] * sign;
		if((hash & 0x1) == 0)
		{
			values[i] = -m_csk->sketchf[i][pos];
		}
		else
		{
			values[i] = m_csk->sketchf[i][pos];
		}
	}

	// Sort values
	qsort(values, m_csk->depth, sizeof(float), cmpfunc_float);

	// Get median of values
	if(values[m_csk->depth / 2] + m_csk->s_thres < 0.0)
	{
		median = values[m_csk->depth / 2 - 1];
		printf("Huge negative value\n");
	}
	else if(values[m_csk->depth / 2 - 1] - m_csk->s_thres > 0.0)
	{
		median = values[m_csk->depth / 2];
		printf("Huge positive value\n");
	}
	else
	{
		median = (values[m_csk->depth / 2 - 1] + values[m_csk->depth / 2]) / 2;
	}

	// Free memory
	free(values);

	return median;
}
