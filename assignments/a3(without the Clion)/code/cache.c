/*
 * cache.c
 */
#include "cache.h"
#include <stdlib.h>
#include <string.h>

/*
 * Initialize a new cache set with the given associativity and block
 * size.
 */
static void cache_set_init(cache_set_t *cache_set, unsigned int associativity,
                           size_t block_size) {
    int i;
    cache_set->cache_lines =
            (cache_line_t **) malloc(associativity * sizeof(cache_line_t *));

    for (i = 0; i < associativity; i++) {
        cache_set->cache_lines[i] =
                (cache_line_t *) malloc(sizeof(cache_line_t) + block_size);
        cache_set->cache_lines[i]->is_valid = 0;
    }
}

/*
 * Compute the shift and mask given the number of bytes/sets.
 */
static void get_shift_and_mask(int value, unsigned int *shift, uint64_t *mask,
                               int init_shift) {
    *mask = 0;
    *shift = init_shift;

    while (value > 1) {
        (*shift)++;
        value >>= 1;
        *mask = (*mask << 1) | 1;
    }
}

/*
 * Create a new cache that contains a total of num_blocks blocks, each
 * of which is block_size bytes long, with the given associativity,
 * and the given set of cache policies for replacement and write
 * operations.
 */
cache_t *cache_new(size_t num_blocks, size_t block_size,
                   unsigned int associativity, int policies) {
    int i;

    // Create cache_newthe cache and initialize constant fields.
    cache_t *cache = (cache_t *) malloc(sizeof(cache_t));
    cache->access_count = 0;
    cache->miss_count = 0;

    // Initialize size fields.
    cache->policies = policies;
    cache->block_size = block_size;
    cache->associativity = associativity;
    cache->num_sets = num_blocks / associativity;

    // Initialize shifts and masks.
    get_shift_and_mask(block_size, &cache->cache_index_shift,
                       &cache->block_offset_mask, 0);
    get_shift_and_mask(cache->num_sets, &cache->tag_shift,
                       &cache->cache_index_mask, cache->cache_index_shift);

    // Initialize cache sets.
    cache->sets = (cache_set_t *) malloc(cache->num_sets * sizeof(cache_set_t));
    for (i = 0; i < cache->num_sets; i++) {
        cache_set_init(&cache->sets[i], cache->associativity, cache->block_size);
    }

    // Done.
    return cache;
}

/*
 * Determine whether or not a cache line is valid for a given tag.
 */
static int cache_line_check_validity_and_tag(cache_line_t *cache_line,
                                             intptr_t tag) {
    return (cache_line->tag == tag && cache_line->is_valid );
    /* TO BE COMPLETED BY THE STUDENT */
    // return 0; // Added to remove warning; remove once function is implemented.
}

/*
 * Return long integer data from a cache line.
 */
static long cache_line_retrieve_data(cache_line_t *cache_line, size_t offset) {
    /* TO BE COMPLETED BY THE STUDENT */
     uintptr_t *ans = (uintptr_t *) (cache_line->data + offset);
     return *ans;

    // return 0; // Added to remove warning; remove once function is implemented.
}

/*
 * Move the cache lines inside a cache set so the cache line with the
 * given index is tagged as the most recently used one. The most
 * recently used cache line will be the 0'th one in the set, the
 * second most recently used cache line will be next, etc.  Cache
 * lines whose valid bit is 0 will occur after all cache lines whose
 * valid bit is 1.
 */
static cache_line_t *cache_line_make_mru(cache_set_t *cache_set,
                                         size_t line_index) {
    int i;
    cache_line_t *line = cache_set->cache_lines[line_index];

    for (i = line_index - 1; i >= 0; i--) {
        cache_set->cache_lines[i + 1] = cache_set->cache_lines[i];
    }

    cache_set->cache_lines[0] = line;
    return line;
}

/*
 * Retrieve a matching cache line from a set, if one exists.
 */
static cache_line_t *cache_set_find_matching_line(cache_t *cache,
                                                  cache_set_t *cache_set,
                                                  uintptr_t tag) {
    /* TO BE COMPLETED BY THE STUDENT */

    /* Don't forget to call cache_line_make_mru(cache_set, i) if you
     * find a matching cache line.
     */
    cache_line_t *ans = NULL;
    int n = cache->associativity;
    for (int i = 0; i < n; i++) {
        if (cache_line_check_validity_and_tag(cache_set->cache_lines[i], tag)) {
            //ans = cache_set->cache_lines[i];
//            return cache_line_make_mru(cache_set, i);
            return cache_set->cache_lines[i];
        }
    }
    return ans;
    // return NULL; // Added to remove warning; remove once function is
    // implemented.
}

/*
 * Function to find a cache line to use for new data. Uses either a
 * line not being used, or a suitable line to be replaced, based on
 * the cache's replacement policy.
 */
static cache_line_t *find_available_cache_line(cache_t *cache,
                                               cache_set_t *cache_set) {
    /* TO BE COMPLETED BY THE STUDENT */
    int ans = 0;
    int n = cache->associativity;
    for (int i = 0; i < n; i++) {
        if (!cache_set->cache_lines[i]->is_valid) {
            ans = i;
            break;
        } else {
            ans = n - 1;
        }
    }
//    return cache_line_make_mru(cache_set, ans);
    return cache_line_make_mru(cache_set, ans);
    /* Don't forget to call cache_line_make_mru(cache_set, i) once you
     * have selected the cache line to use.
     */
    // return NULL;  // Added to remove warning; remove once function is implemented.
}

/*
 * Add a block to a given cache set.
 */
static cache_line_t *cache_set_add(cache_t *cache, cache_set_t *cache_set,
                                   uintptr_t address, uintptr_t tag) {
    // First locate the cache line to use.
    cache_line_t *line = find_available_cache_line(cache, cache_set);

    // Now set it up.
    line->tag = tag;
    line->is_valid = 1;
    memcpy(line->data, (void *) (address & ~cache->block_offset_mask),
           cache->block_size);

    // And return it.
    return line;
}

/*
 * Read a single long integer from the cache.
 */
long cache_read(cache_t *cache, long *address) {
    cache_set_t *set;
    cache_line_t *lines;

    uintptr_t tag = (uintptr_t) address >> cache->tag_shift;
    uintptr_t set_index = ((uintptr_t) address >> cache->cache_index_shift) & cache->cache_index_mask;
    uintptr_t block_off = (uintptr_t) address & cache->block_offset_mask;


    set = &cache->sets[set_index];
    lines = cache_set_find_matching_line(cache, set, tag);
    cache->access_count++;

    if (lines == NULL) {
        cache->miss_count++;
        lines = cache_set_add(cache, set, (uintptr_t) address, tag);

    }
    return cache_line_retrieve_data(lines, block_off);



    /* TO BE COMPLETED BY THE STUDENT */
    //return 0;  // Added to remove warning; remove once function is implemented.
}

/*
 * Read a single integer from the cache.
 */
void cache_write(cache_t *cache, long *address, long value) {
    /* OPTIONAL FUNCTION, YOU ARE NOT REQUIRED TO IMPLEMENT IT */
}

/*
 * Return the number of cache misses since the cache was created.
 */
int cache_miss_count(cache_t *cache) { return cache->miss_count; }

/*
 * Return the number of cache accesses since the cache was created.
 */
int cache_access_count(cache_t *cache) { return cache->access_count; }

