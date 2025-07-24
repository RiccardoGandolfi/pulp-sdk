#include "idma_1d_multicore.h"

#define NB_TRANSFERS 13
#define TOT_SIZE 8 * CORE_SPACE
#define NB_TASKS 1

int glob_errors = 0;
uint32_t l1_addr[8] = {0};
uint32_t l2_addr[8] = {0};

int idma_1D (uint32_t size, int core_id) {
    int error = 0;
    volatile uint8_t *src_ptr, *dst_ptr;

    // L1 to L2 transfer
    src_ptr = (uint8_t*) l1_addr[core_id];
    dst_ptr = (uint8_t*) l2_addr[core_id];

    PRINTF ("CORE[%d]: l1_addr: 0x%8x | l2_addr: 0x%8x \n", core_id, l1_addr[core_id], l2_addr[core_id]);

    for (int i = 0; i < size; i++) {
        src_ptr[i] = (uint8_t)(i & 0xFF);
    }

    plp_cl_dma_wait_toL2(pulp_cl_idma_L1ToL2((unsigned int) src_ptr, (unsigned int) dst_ptr, size));

    // Check the results

    for (int i=0; i < size; i++) {
        uint8_t expected = src_ptr[i]; 
        uint8_t actual   = dst_ptr[i];

        if (expected != actual) {
            error++;
            if (core_id == 0) {
                PRINTF ("Error: expected @%8x = %8x vs actual @%8x = %8x \n", expected, &src_ptr[i], actual, &dst_ptr[i]);
            }
        }
    }

    return error;
}


void allocate_mem_to_cores () {
    int core_id = pi_core_id();

    // Pre-allocate TOT_SIZE = 8 * CORE_SPACE: then we split this window to assign
    // each core its available space for iDMA transfers
    // pi_l1_malloc starts allocating from 0x10004008 in L1
    // pi_l2_malloc starts allocating from 0x1c000a60 in L2

    if (core_id == 0) {
        l1_addr[0]     = (uint32_t) pi_l1_malloc(0, TOT_SIZE);
        l2_addr[0]     = (uint32_t) pi_l2_malloc(TOT_SIZE);
    }

    // The following rt_team_barrier is needed so that
    // no core can assign its address range until the mallocs are executed

    for (int i=1; i < pi_cl_team_nb_cores(); i++) {
        l1_addr[i]     = l1_addr[0] + i * CORE_SPACE;
        l2_addr[i]     = l2_addr[0] + i * CORE_SPACE;

        PRINTF ("Core %d: l1_addr = %8x \n", i, l1_addr[i]);
        PRINTF ("Core %d: l2_addr = %8x \n", i, l2_addr[i]);
    }
    // The following rt_team_barrier is needed so that
    // no core can start executing until all address ranges have been assigned

}

void free_allocated_memory () {
    // Only Core 0 takes care of freeing the allocated memory, since it's the one
    // allocated it at the beginning of the test
    if (pi_core_id() == 0) {
        pi_l1_free(0, l1_addr, TOT_SIZE);
        pi_l2_free(l2_addr, TOT_SIZE);
    }
}

void idma_task() {
    PRINTF ("Core[%d] has entered idma_task \n", pi_core_id());
    
    for (int k = 0; k < NB_TRANSFERS; k ++) {
        PRINTF ("Transfer %d with size %d \n", k, idma_presets[k]);
        glob_errors += idma_1D(idma_presets[k], pi_core_id());
    }
}

static void pe_entry(void *arg)
{
  int *errors = (int *)arg;
  pi_cl_team_critical_enter();
  idma_task();
  pi_cl_team_critical_exit();
}

static int check_fork()
{
  unsigned int var = 0;
  PRINTF ("Before cl team fork \n");
  allocate_mem_to_cores();
  pi_cl_team_fork(pi_cl_team_nb_cores(), pe_entry, (void *)&var);
  free_allocated_memory();
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