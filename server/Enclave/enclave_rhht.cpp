#include "stdlib.h"
#include "string.h"
#include "enclave_rhht.h"
#include <sgx_trts.h>
#include <algorithm>

struct rhht_table* rhht_snp_table = NULL;
struct rhht_table_pcc* rhht_snp_table_pcc = NULL;
struct elem_pcc* stats_table = NULL;

// Allocate memory for SNP hash table
void allocate_table(uint32_t capacity)
{
	// Allocate memory for the top hash_table structure
	rhht_snp_table = (struct rhht_table*) malloc(sizeof(struct rhht_table));

	// Initialization
	rhht_snp_table->num_elems = 0;
	rhht_snp_table->capacity = capacity;
	rhht_snp_table->resize_threshold = (capacity * LOAD_FACTOR_PERCENT) / 100;

	// Allocate memory for the actual element buffer 
	rhht_snp_table->buffer = (struct elem*) malloc(capacity * sizeof(struct elem));

	// Mark all elements as unused
	for(uint32_t i = 0; i < capacity; i++)
	{
		rhht_snp_table->buffer[i].key = 0;
	}
}

void deallocate_table()
{
	free(rhht_snp_table->buffer);
	free(rhht_snp_table);
}

void allocate_stats_table(uint32_t capacity, uint8_t num_pc)
{
	sort_mh_by_key();
	stats_table = (struct elem_pcc*) malloc(capacity * sizeof(struct elem_pcc));
	for(uint32_t i = 0; i < capacity; i++)
	{
		mh->mh_array_f[i].val = 0; // dotprod
		stats_table[i].ssqg = 0;
		stats_table[i].sx = 0;
		stats_table[i].pc_projections[0] = 0;
		stats_table[i].pc_projections[1] = 0;
	}
}

void allocate_table_pcc(uint32_t capacity, uint8_t num_pc)
{
}

void deallocate_table_pcc()
{
	uint32_t capacity = rhht_snp_table_pcc->capacity;
	for(uint32_t i = 0; i < capacity; i++)
	{
		if(rhht_snp_table_pcc->buffer[i].pc_projections != NULL)
		{
			free(rhht_snp_table_pcc->buffer[i].pc_projections);
		}
	}
	free(rhht_snp_table_pcc->buffer);
	free(rhht_snp_table_pcc);
}

void construct(uint32_t index, uint32_t key, uint16_t case_count, uint16_t control_count)
{
	rhht_snp_table->buffer[index].key = key;
	rhht_snp_table->buffer[index].case_count = case_count;
	rhht_snp_table->buffer[index].control_count = control_count;
}

void construct_pcc(uint32_t index)
{
}

void set_index(uint32_t index, uint32_t key_, float sg, float d, float sc, float* pr)
{
}

inline uint32_t probe_distance(uint32_t key, uint32_t slot_index)
{
	uint32_t hash = key & ((rhht_snp_table->capacity) - 1);
	return (slot_index + rhht_snp_table->capacity - hash);
}

inline uint32_t probe_distance_pcc(uint32_t key, uint32_t slot_index)
{
	uint32_t hash = key & ((rhht_snp_table_pcc->capacity) - 1);
	return (slot_index + rhht_snp_table_pcc->capacity - hash);
}

inline void insert_helper(uint32_t hash, uint32_t key, uint16_t case_count, uint16_t control_count)
{
	uint32_t pos = hash;
	uint32_t dist = 0;
	for(;;)
	{
		if((rhht_snp_table->buffer[pos].key & ((rhht_snp_table->capacity) - 1)) == 0)
		{
			construct(pos, key, case_count, control_count);
			return;
		}

		uint32_t existing_elem_probe_dist = probe_distance(rhht_snp_table->buffer[pos].key, pos);
		if(existing_elem_probe_dist < dist)
		{
			uint32_t temp_key = rhht_snp_table->buffer[pos].key;
			uint16_t temp_case_count = rhht_snp_table->buffer[pos].case_count;
			uint16_t temp_control_count = rhht_snp_table->buffer[pos].control_count;

			rhht_snp_table->buffer[pos].key = key;
			rhht_snp_table->buffer[pos].case_count = case_count;
			rhht_snp_table->buffer[pos].control_count = control_count;

			key = temp_key;
			case_count = temp_case_count;
			control_count = temp_control_count;

			dist = existing_elem_probe_dist;
		}

		pos = pos + 1;
		dist = dist + 1;
	}
}

inline void insert_helper_pcc(uint32_t hash, uint32_t key)
{
}

int32_t find_pcc(uint32_t key)
{
	mh_node_f node;
	node.key = key;
	mh_node_f* p = stlpmtx_std::lower_bound(mh->mh_array_f, mh->mh_array_f + mh->max_heap_size, node, cmp_mh_key);
	if (p == mh->mh_array_f + mh->max_heap_size || p->key != key) { return -1; }
	else { return p - mh->mh_array_f; }
}

// Expand the hash table if the number of elements exceed the resize threshold
void grow()
{
	struct elem* old_elems = rhht_snp_table->buffer;
	uint32_t old_capacity = rhht_snp_table->capacity;
	uint32_t new_capacity = old_capacity * 2;

	allocate_table(new_capacity);

	for(uint32_t i = 0; i < old_capacity; i++)
	{
		struct elem e = old_elems[i];
		uint32_t key = e.key;
		uint32_t hash = key & (new_capacity - 1);
		if(key != 0)
		{
			insert_helper(hash, e.key, e.case_count, e.control_count);
		}
	}
	free(old_elems);
}

void grow_pcc()
{
}

void insert(uint32_t key, uint8_t allele_type, uint8_t patient_status)
{
	rhht_snp_table->num_elems = rhht_snp_table->num_elems + 1;
	if(rhht_snp_table->num_elems >= rhht_snp_table->resize_threshold)
	{
		grow();
	}

	uint32_t hash = key & ((rhht_snp_table->capacity) - 1);
	if(patient_status == 0)
	{
		insert_helper(hash, key, 0, (uint32_t) allele_type);
	}
	else
	{
		insert_helper(hash, key, (uint32_t) allele_type, 0);
	}
}

void insert_pcc(uint32_t key)
{
	rhht_snp_table_pcc->num_elems = rhht_snp_table_pcc->num_elems + 1;
	if(rhht_snp_table_pcc->num_elems >= rhht_snp_table_pcc->resize_threshold)
	{
		grow_pcc();
	}

	uint32_t hash = key & ((rhht_snp_table_pcc->capacity) - 1);
	insert_helper_pcc(hash, key);
}

int32_t lookup_index(uint32_t key)
{
	uint32_t hash = key & ((rhht_snp_table->capacity) - 1);
	uint32_t pos = hash;
	uint32_t dist = 0;
	for(;;)
	{
		uint32_t curr_hash = rhht_snp_table->buffer[pos].key & ((rhht_snp_table->capacity) - 1);
		if(curr_hash == 0)
		{
			return -1;
		}
		else if(dist > probe_distance(rhht_snp_table->buffer[pos].key, pos))
		{
			return -1;
		}
		else if(curr_hash == hash && rhht_snp_table->buffer[pos].key == key)
		{
			return pos;
		}

		pos = pos + 1;
		dist = dist + 1;
	}
}

int32_t find(uint32_t key)
{
	return lookup_index(key);
}
