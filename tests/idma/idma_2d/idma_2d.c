#include "idma_2d.h"

#define NB_TRANSFERS 7
#define TOT_SIZE 8 * CORE_SPACE
#define NB_TASKS 1

int glob_errors = 0;
uint32_t l1_addr[8] = {0};
uint32_t l2_addr[8] = {0};
uint32_t l1_dst_addr[8] = {0};

void print_transfer (TransferParameters transfer) {
    PRINTF ("Core[%d] => Transfer Parameters | Size: %d | Length: %d | Src_stride_2d: %d | Dst_stride_2d: %d \n", 
            pi_core_id(), transfer.size,
            transfer.length, transfer.src_stride, transfer.dst_stride);
}

int idma_2D (TransferParameters transfer, int core_id, int ext2loc, int loc2loc) {
    volatile uint8_t *src_ptr, *dst_ptr;

    int error = 0;
    int offset_2d;

    uint32_t src_stride = transfer.src_stride;
    uint32_t dst_stride = transfer.dst_stride;
    uint32_t size = transfer.size;
    uint32_t length = transfer.length;
    uint32_t num_reps = size/length;

    if (loc2loc == 1) {
        // L1 to L1 transfer
        src_ptr = (uint8_t*) l1_addr[core_id];
        dst_ptr = (uint8_t*) l1_dst_addr[core_id];
    } else if (ext2loc == 1) {
        // L2 to L1 transfer
        src_ptr = (uint8_t*) l2_addr[core_id];
        dst_ptr = (uint8_t*) l1_addr[core_id];
    } else {
        // L1 to L2 transfer
        src_ptr = (uint8_t*) l1_addr[core_id];
        dst_ptr = (uint8_t*) l2_addr[core_id];
    }

    // Fill source region with test data
    for (int q = 0; q < num_reps; q++) {
        offset_2d = q * src_stride;
        for (int i = 0; i < length; i++) {
            src_ptr[i+offset_2d] = (uint8_t)(i & 0xFF);
        }
    }

    if (loc2loc == 1) {
        plp_cl_dma_wait_toL1(pulp_cl_idma_L1ToL1_2d((unsigned int)src_ptr, (unsigned int)dst_ptr, length, src_stride, dst_stride, num_reps));
    } if (ext2loc == 1) {
        plp_cl_dma_wait_toL1(pulp_cl_idma_L2ToL1_2d((unsigned int)src_ptr, (unsigned int)dst_ptr, length, src_stride, dst_stride, num_reps));
    } else {
        plp_cl_dma_wait_toL2(pulp_cl_idma_L1ToL2_2d((unsigned int)src_ptr, (unsigned int)dst_ptr, length, src_stride, dst_stride, num_reps));
    }

    // Check the results
    
    for (unsigned int rep = 0; rep < num_reps; rep++) {
        unsigned int src_offset = rep * src_stride;
        unsigned int dst_offset = rep * dst_stride;
        for (unsigned int i = 0; i < length; i++) {
            uint8_t expected = src_ptr[src_offset + i];
            uint8_t actual = dst_ptr[dst_offset + i];

            if (expected != actual) {
                error++;
                if (core_id == 0) {
                    PRINTF ("ERROR: expected[%d] @%8x = %8x vs actual[%d] @%8x = %8x \n", src_offset + i, &src_ptr[src_offset + i], 
                            expected, dst_offset+i, &dst_ptr[dst_offset + i], actual);
                }
            }

        }
    }
    
    return error;
}

void idma_task() {
    PRINTF ("Core[%d] has entered idma_task \n", pi_core_id());
    
    for (int k = 0; k < NB_TRANSFERS; k ++) {
        print_transfer(idma_presets[k]);
        // L1 -> L2
        glob_errors += idma_2D(idma_presets[k], pi_core_id(), 0, 0);
        // L2 -> L1
        glob_errors += idma_2D(idma_presets[k], pi_core_id(), 1, 0);
        // L1 -> L1
        glob_errors += idma_2D(idma_presets[k], pi_core_id(), 0, 1);
    }
}

void allocate_mem_to_cores () {
    int core_id = pi_core_id();

    // Pre-allocate TOT_SIZE = 8 * CORE_SPACE: then we split this window to assign
    // each core its available space for iDMA transfers

    if (core_id == 0) {
        l1_addr[0]     = (uint32_t) pi_l1_malloc(0, TOT_SIZE);
        l1_dst_addr[0] = (uint32_t) pi_l1_malloc(0, TOT_SIZE);
        l2_addr[0]     = (uint32_t) pi_l2_malloc(TOT_SIZE);
    }
    pi_cl_team_barrier();
    // The following rt_team_barrier is needed so that
    // no core can assign its address range until the mallocs are executed

    l1_addr[core_id] = l1_addr[0] + core_id * CORE_SPACE;
    l1_dst_addr[core_id] = l1_dst_addr[0] + core_id * CORE_SPACE;
    l2_addr[core_id] = l2_addr[0] + core_id * CORE_SPACE;

    PRINTF ("Core %d: l1_addr = %8x \n", core_id, l1_addr[core_id]);
    PRINTF ("Core %d: l2_addr = %8x \n", core_id, l2_addr[core_id]);
    PRINTF ("Core %d: l1_dst_addr = %8x \n", core_id, l1_dst_addr[core_id]);
    // The following rt_team_barrier is needed so that
    // no core can start executing until all address ranges have been assigned
    pi_cl_team_barrier();
}

void free_allocated_memory () {
    pi_cl_team_barrier();
    // Only Core 0 takes care of freeing the allocated memory, since it's the one
    // allocated it at the beginning of the test
    if (pi_core_id() == 0) {
        pi_l1_free(0, l1_addr, TOT_SIZE);
        pi_l1_free(0, l1_dst_addr, TOT_SIZE);
        pi_l2_free(l2_addr, TOT_SIZE);
    }
}

static void pe_entry(void *arg)
{
    int *errors = (int *)arg;
    allocate_mem_to_cores();
#ifdef MULTI_CORE_P
    idma_task();
#elif MULTI_CORE_S
    pi_cl_team_critical_enter();
    idma_task();
    pi_cl_team_critical_exit();
#else
    if (pi_core_id() == 0) {
        idma_task();
    }
#endif
    free_allocated_memory();
}

static int check_fork()
{
  unsigned int var = 0;
  pi_cl_team_fork(pi_cl_team_nb_cores(), pe_entry, (void *)&var);
  return (1<<pi_cl_team_nb_cores()) - 1 != var;
}

static void cluster_entry(void *arg)
{
    check_fork((int) pi_cl_team_nb_cores());
}

static int test_task_sync()
{
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;
    struct pi_cluster_task task;
    int errors = 0;

    pi_cluster_conf_init(&conf);
    conf.id = 0;

    pi_open_from_conf(&cluster_dev, &conf);
    
    pi_cluster_open(&cluster_dev);

    pi_cluster_task(&task, &cluster_entry, NULL);
    pi_cluster_send_task_to_cl(&cluster_dev, &task);
    pi_cluster_close(&cluster_dev);
    errors = glob_errors;
    return errors;
}

int test_entry()
{
  int errors = 0;

  PRINTF("Starting test\n");

  errors += test_task_sync();

  if (errors)
    PRINTF("Test failure\n");
  else
    PRINTF("Test success\n");

  return errors;
}

void test_kickoff(void *arg)
{
  int ret = test_entry();
  pmsis_exit(ret);
}


int main () {
    return pmsis_kickoff((void *)test_kickoff);
}