#ifndef __CL_IDMA_H__
#define __CL_IDMA_H__

#include "archi/dma/idma_v2.h"
#include "archi/chips/pulp/memory_map.h"

// Some definitions for iDMA

#define IDMA_EVENT 8 // all iDMA tx_cplt events are broadcast
#define IDMA_ID_COUNTER_WIDTH 32
#define IDMA_ID_MASK 0xffffffff

typedef enum {
    IDMA_PROT_AXI = 0, // AXI protocol: L2 memory
    IDMA_PROT_OBI = 1, // OBI protocol: L1 memory
    IDMA_PROT_INIT = 4 // INIT protocol: /dev/null (write to here and the stream disappears, read from here and get all-zeros)
} idma_prot_t;

typedef unsigned int dma_ext_t;

typedef struct {
  unsigned int src;
  unsigned int dst;
  unsigned int size;
  unsigned int length;
  unsigned int src_stride_2d;
  unsigned int dst_stride_2d;
  unsigned int src_stride_3d;
  unsigned int dst_stride_3d;
  unsigned int num_reps_2d;
  unsigned int num_reps_3d;
  unsigned int tid;
} DMA_copy;

#define IDMA_DEFAULT_CONFIG 0x0
#define IDMA_DEFAULT_CONFIG_L1TOL2 (IDMA_DEFAULT_CONFIG | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_SRC_PROTOCOL_OFFSET) | (IDMA_PROT_AXI << IDMA_REG32_3D_CONF_DST_PROTOCOL_OFFSET))
#define IDMA_DEFAULT_CONFIG_L2TOL1 (IDMA_DEFAULT_CONFIG | (IDMA_PROT_AXI << IDMA_REG32_3D_CONF_SRC_PROTOCOL_OFFSET) | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_DST_PROTOCOL_OFFSET))
#define IDMA_DEFAULT_CONFIG_L1TOL1 (IDMA_DEFAULT_CONFIG | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_SRC_PROTOCOL_OFFSET) | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_DST_PROTOCOL_OFFSET))

#define IDMA_DEFAULT_CONFIG_2D 0x400
#define IDMA_DEFAULT_CONFIG_L1TOL2_2D (IDMA_DEFAULT_CONFIG_2D | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_SRC_PROTOCOL_OFFSET) | (IDMA_PROT_AXI << IDMA_REG32_3D_CONF_DST_PROTOCOL_OFFSET))
#define IDMA_DEFAULT_CONFIG_L2TOL1_2D (IDMA_DEFAULT_CONFIG_2D | (IDMA_PROT_AXI << IDMA_REG32_3D_CONF_SRC_PROTOCOL_OFFSET) | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_DST_PROTOCOL_OFFSET))
#define IDMA_DEFAULT_CONFIG_L1TOL1_2D (IDMA_DEFAULT_CONFIG_2D | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_SRC_PROTOCOL_OFFSET) | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_DST_PROTOCOL_OFFSET))

#define IDMA_DEFAULT_CONFIG_3D 0x800
#define IDMA_DEFAULT_CONFIG_L1TOL2_3D (IDMA_DEFAULT_CONFIG_3D | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_SRC_PROTOCOL_OFFSET) | (IDMA_PROT_AXI << IDMA_REG32_3D_CONF_DST_PROTOCOL_OFFSET))
#define IDMA_DEFAULT_CONFIG_L2TOL1_3D (IDMA_DEFAULT_CONFIG_3D | (IDMA_PROT_AXI << IDMA_REG32_3D_CONF_SRC_PROTOCOL_OFFSET) | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_DST_PROTOCOL_OFFSET))
#define IDMA_DEFAULT_CONFIG_L1TOL1_3D (IDMA_DEFAULT_CONFIG_3D | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_SRC_PROTOCOL_OFFSET) | (IDMA_PROT_OBI << IDMA_REG32_3D_CONF_DST_PROTOCOL_OFFSET))

// Configurations

// 1D Transfers
/** Memory transfer with event-based completion.
 *
 \param   src  Address in the memory where to store the data. There is no restriction on memory alignment.
 \param   dst  Address in the memory where to load the data. There is no restriction on memory alignment.
 \param   size Number of bytes to be transfered. The only restriction is that this size must fit 16 bits, i.e. must be inferior to 65536.
 \return       The identifier of the transfer. This can be used with plp_dma_wait to wait for the completion of this transfer.
*/

static inline int pulp_cl_idma_L1ToL2(DMA_copy transfer);
static inline int pulp_cl_idma_L2ToL1(DMA_copy transfer);
static inline int pulp_cl_idma_L1ToL1(DMA_copy transfer);

// 2D Transfers
/** 2-dimensional transfer with event-based completion.
 *
  \param   src    Address in the memory where to store the data. There is no restriction on memory alignment.
  \param   dst    Address in the memory where to load the data. There is no restriction on memory alignment.
  \param   size   Number of bytes to be transfered. The only restriction is that this size must fit 16 bits, i.e. must be inferior to 65536.
  \param   src_stride 2D stride, which is the number of bytes which are added to the beginning of the current line to switch to the next one. Must fit 16 bits, i.e. must be inferior to 65536.
  \param   dst_stride 2D stride, which is the number of bytes which are added to the beginning of the current line to switch to the next one. Must fit 16 bits, i.e. must be inferior to 65536.
  \param   num_reps   Number of 1D transfers that comprise the 2D transfer.

  \return         The identifier of the transfer. This can be used with plp_dma_wait to wait for the completion of this transfer.
  */

static inline int pulp_cl_idma_L1ToL2_2d(DMA_copy transfer);
static inline int pulp_cl_idma_L2ToL1_2d(DMA_copy transfer);
static inline int pulp_cl_idma_L1ToL1_2d(DMA_copy transfer);

// 3D Transfers
/** 3-dimensional transfer with event-based completion.
 *
  \param   src            Address in the memory where to store the data. There is no restriction on memory alignment.
   \param   dst           Address in the memory where to load the data. There is no restriction on memory alignment.
   \param   size          Number of bytes to be transfered. The only restriction is that this size must fit 16 bits, i.e. must be inferior to 65536.
   \param   src_stride    2D stride, which is the number of bytes which are added to the beginning of the current line to switch to the next one. Must fit 16 bits, i.e. must be inferior to 65536.
   \param   dst_stride    2D stride, which is the number of bytes which are added to the beginning of the current line to switch to the next one. Must fit 16 bits, i.e. must be inferior to 65536.
   \param   num_reps      Number of 1D transfers that comprise the 2D transfer.
   \param   src_stride_3d Stride between 2D pages in the source memory.
   \param   dst_stride_3d Stride between 2D pages in the destination memory.
   \param   num_reps_3d   Number of 2D pages to be transfered.

  \return         The identifier of the transfer. This can be used with plp_dma_wait to wait for the completion of this transfer.
  */

static inline int pulp_cl_idma_L1ToL2_3d(DMA_copy transfer);
static inline int pulp_cl_idma_L2ToL1_3d(DMA_copy transfer);
static inline int pulp_cl_idma_L1ToL1_3d(DMA_copy transfer);

/** DMA barrier.
 * This blocks the core until no transfer is on-going in the DMA.
 * Careful: these only wait for transfers towards L1
 */

static inline void plp_cl_dma_barrier_toL1();

/** DMA barrier.
 * This blocks the core until no transfer is on-going in the DMA.
 * Careful: these only wait for transfers towards L2
 */

static inline void plp_cl_dma_barrier_toL2();

/** DMA wait.
  * This blocks the core until the specified transfer is finished.
  *
  \param   counter  The counter ID identifying the transfer. This has been returned from an enqueued transfer (e.g. plp_dma_l2ToL1_2d)
 */

static inline void plp_cl_dma_wait_toL1(unsigned int dma_tx_id);
static inline void plp_cl_dma_wait_toL2(unsigned int dma_tx_id);
//

/** Return the DMA status for a transfer towards L1 memory.
 *
  \return             DMA status. 1 means there are still on-going transfers, 0 means nothing is on-going.
  */

static inline unsigned int plp_cl_dma_status_toL1();

/** Return the DMA status for a transfer towards L2 memory.
 *
  \return             DMA status. 1 means there are still on-going transfers, 0 means nothing is on-going.
  */

static inline unsigned int plp_cl_dma_status_toL2();

#if ARCHI_HAS_DMA_DEMUX
#define DMA_DEMUX_ADDR ARCHI_IDMA_DEMUX_ADDR
#endif
#define DMA_ADDR ARCHI_IDMA_EXT_ADDR

#if defined(__riscv__) && !defined(RV_ISA_RV32) && !defined(__LLVM__)
#define IDMA_WRITE(value, offset) __builtin_pulp_OffsetedWrite((value), (int *)DMA_ADDR, (offset))
#define IDMA_READ(offset) __builtin_pulp_OffsetedRead((int *)DMA_ADDR, (offset))
#ifdef ARCHI_HAS_DMA_DEMUX
#define DMA_CL_WRITE(value, offset) __builtin_pulp_OffsetedWrite((value), (int *)DMA_DEMUX_ADDR, (offset))
#define DMA_CL_READ(offset) __builtin_pulp_OffsetedRead((int *)DMA_DEMUX_ADDR, (offset))
#endif
#else
#define IDMA_WRITE(value, offset) pulp_write32(DMA_ADDR + (offset), (value))
#define IDMA_READ(offset) pulp_read32(DMA_ADDR + (offset))
#define DMA_CL_WRITE(value, offset) IDMA_WRITE(value, offset)
#define DMA_CL_READ(offset) IDMA_READ(offset)
#ifdef ARCHI_HAS_DMA_DEMUX
#define DMA_CL_WRITE(value, offset) pulp_write32(DMA_DEMUX_ADDR + (offset), (value))
#define DMA_CL_READ(offset) pulp_read32(DMA_DEMUX_ADDR + (offset))
#endif
#endif

// if we don't have the peripheral demux, the cluster write/read functions are equal to the regular versions
#ifndef ARCHI_HAS_DMA_DEMUX
#define DMA_CL_WRITE(value, offset) IDMA_WRITE(value, offset)
#define DMA_CL_READ(offset) IDMA_READ(offset)
#endif


// 1D TRANSFERS

static inline int pulp_cl_idma_L1ToL2(DMA_copy transfer) {
  printf ("Transfer to configure: dst: 0x%8x | src: 0x%8x | size: 0x%8x \n", transfer.dst, transfer.src, transfer. size);
  unsigned int dma_tx_id;
  unsigned int cfg = IDMA_DEFAULT_CONFIG_L1TOL2;
  DMA_CL_WRITE(transfer.src, IDMA_REG32_3D_SRC_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst, IDMA_REG32_3D_DST_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.size, IDMA_REG32_3D_LENGTH_LOW_REG_OFFSET);
  DMA_CL_WRITE(cfg, IDMA_REG32_3D_CONF_REG_OFFSET);

  asm volatile("" : : : "memory");
  // Launch TX
  dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_NEXT_ID_0_REG_OFFSET);

  return dma_tx_id;
}
static inline int pulp_cl_idma_L2ToL1(DMA_copy transfer) {
  unsigned int dma_tx_id;
  unsigned int cfg = IDMA_DEFAULT_CONFIG_L2TOL1;
  DMA_CL_WRITE(transfer.src, IDMA_REG32_3D_SRC_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst, IDMA_REG32_3D_DST_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.size, IDMA_REG32_3D_LENGTH_LOW_REG_OFFSET);
  DMA_CL_WRITE(cfg, IDMA_REG32_3D_CONF_REG_OFFSET);

  asm volatile("" : : : "memory");
  // Launch TX
  dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_NEXT_ID_1_REG_OFFSET);

  return dma_tx_id;
}

static inline int pulp_cl_idma_L1ToL1(DMA_copy transfer) {
  unsigned int dma_tx_id;
  unsigned int cfg = IDMA_DEFAULT_CONFIG_L1TOL1;
  DMA_CL_WRITE(transfer.src, IDMA_REG32_3D_SRC_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst, IDMA_REG32_3D_DST_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.size, IDMA_REG32_3D_LENGTH_LOW_REG_OFFSET);
  DMA_CL_WRITE(cfg, IDMA_REG32_3D_CONF_REG_OFFSET);

  asm volatile("" : : : "memory");
  // Launch TX
  dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_NEXT_ID_1_REG_OFFSET);

  return dma_tx_id;
}

// 2D TRANSFERS

static inline int pulp_cl_idma_L1ToL2_2d(DMA_copy transfer) {
  unsigned int dma_tx_id;
  unsigned int cfg = IDMA_DEFAULT_CONFIG_L1TOL2_2D;
  DMA_CL_WRITE(transfer.src, IDMA_REG32_3D_SRC_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst, IDMA_REG32_3D_DST_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.size, IDMA_REG32_3D_LENGTH_LOW_REG_OFFSET);
  DMA_CL_WRITE(cfg, IDMA_REG32_3D_CONF_REG_OFFSET);
  DMA_CL_WRITE(transfer.src_stride_2d, IDMA_REG32_3D_SRC_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst_stride_2d, IDMA_REG32_3D_DST_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.num_reps_2d, IDMA_REG32_3D_REPS_2_LOW_REG_OFFSET);

  asm volatile("" : : : "memory");
  dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_NEXT_ID_0_REG_OFFSET);
  return dma_tx_id;
}

static inline int pulp_cl_idma_L2ToL1_2d(DMA_copy transfer) {
  unsigned int dma_tx_id;
  unsigned int cfg = IDMA_DEFAULT_CONFIG_L2TOL1_2D;
  DMA_CL_WRITE(transfer.src, IDMA_REG32_3D_SRC_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst, IDMA_REG32_3D_DST_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.size, IDMA_REG32_3D_LENGTH_LOW_REG_OFFSET);
  DMA_CL_WRITE(cfg, IDMA_REG32_3D_CONF_REG_OFFSET);
  DMA_CL_WRITE(transfer.src_stride_2d, IDMA_REG32_3D_SRC_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst_stride_2d, IDMA_REG32_3D_DST_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.num_reps_2d, IDMA_REG32_3D_REPS_2_LOW_REG_OFFSET);

  asm volatile("" : : : "memory");
  dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_NEXT_ID_1_REG_OFFSET);
  return dma_tx_id;
}

static inline int pulp_cl_idma_L1ToL1_2d(DMA_copy transfer) {
  unsigned int dma_tx_id;
  unsigned int cfg = IDMA_DEFAULT_CONFIG_L1TOL1_2D;
  DMA_CL_WRITE(transfer.src, IDMA_REG32_3D_SRC_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst, IDMA_REG32_3D_DST_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.size, IDMA_REG32_3D_LENGTH_LOW_REG_OFFSET);
  DMA_CL_WRITE(cfg, IDMA_REG32_3D_CONF_REG_OFFSET);
  DMA_CL_WRITE(transfer.src_stride_2d, IDMA_REG32_3D_SRC_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst_stride_2d, IDMA_REG32_3D_DST_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.num_reps_2d, IDMA_REG32_3D_REPS_2_LOW_REG_OFFSET);

  asm volatile("" : : : "memory");
  dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_NEXT_ID_1_REG_OFFSET);
  return dma_tx_id;
}

// 3D TRANSFERS

static inline int pulp_cl_idma_L1ToL2_3d(DMA_copy transfer) {
  unsigned int dma_tx_id;
  unsigned int cfg = IDMA_DEFAULT_CONFIG_L1TOL2_3D;
  DMA_CL_WRITE(transfer.src, IDMA_REG32_3D_SRC_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst, IDMA_REG32_3D_DST_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.size, IDMA_REG32_3D_LENGTH_LOW_REG_OFFSET);
  DMA_CL_WRITE(cfg, IDMA_REG32_3D_CONF_REG_OFFSET);
  DMA_CL_WRITE(transfer.src_stride_2d, IDMA_REG32_3D_SRC_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst_stride_2d, IDMA_REG32_3D_DST_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.src_stride_3d, IDMA_REG32_3D_SRC_STRIDE_3_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst_stride_3d, IDMA_REG32_3D_DST_STRIDE_3_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.num_reps_2d, IDMA_REG32_3D_REPS_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.num_reps_3d, IDMA_REG32_3D_REPS_3_LOW_REG_OFFSET);

  asm volatile("" : : : "memory");
  dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_NEXT_ID_0_REG_OFFSET);
  return dma_tx_id;
}

static inline int pulp_cl_idma_L2ToL1_3d(DMA_copy transfer) {
  unsigned int dma_tx_id;
  unsigned int cfg = IDMA_DEFAULT_CONFIG_L2TOL1_3D;
  DMA_CL_WRITE(transfer.src, IDMA_REG32_3D_SRC_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst, IDMA_REG32_3D_DST_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.size, IDMA_REG32_3D_LENGTH_LOW_REG_OFFSET);
  DMA_CL_WRITE(cfg, IDMA_REG32_3D_CONF_REG_OFFSET);
  DMA_CL_WRITE(transfer.src_stride_2d, IDMA_REG32_3D_SRC_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst_stride_2d, IDMA_REG32_3D_DST_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.num_reps_2d, IDMA_REG32_3D_REPS_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.src_stride_3d, IDMA_REG32_3D_SRC_STRIDE_3_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst_stride_3d, IDMA_REG32_3D_DST_STRIDE_3_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.num_reps_3d, IDMA_REG32_3D_REPS_3_LOW_REG_OFFSET);

  asm volatile("" : : : "memory");
  dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_NEXT_ID_1_REG_OFFSET);
  return dma_tx_id;
}

static inline int pulp_cl_idma_L1ToL1_3d(DMA_copy transfer) {
  unsigned int dma_tx_id;
  unsigned int cfg = IDMA_DEFAULT_CONFIG_L1TOL1_3D;
  DMA_CL_WRITE(transfer.src, IDMA_REG32_3D_SRC_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst, IDMA_REG32_3D_DST_ADDR_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.size, IDMA_REG32_3D_LENGTH_LOW_REG_OFFSET);
  DMA_CL_WRITE(cfg, IDMA_REG32_3D_CONF_REG_OFFSET);
  DMA_CL_WRITE(transfer.src_stride_2d, IDMA_REG32_3D_SRC_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst_stride_2d, IDMA_REG32_3D_DST_STRIDE_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.src_stride_3d, IDMA_REG32_3D_SRC_STRIDE_3_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.dst_stride_3d, IDMA_REG32_3D_DST_STRIDE_3_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.num_reps_2d, IDMA_REG32_3D_REPS_2_LOW_REG_OFFSET);
  DMA_CL_WRITE(transfer.num_reps_3d, IDMA_REG32_3D_REPS_3_LOW_REG_OFFSET);

  asm volatile("" : : : "memory");
  dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_NEXT_ID_1_REG_OFFSET);
  return dma_tx_id;
}

// READ TRANSFER ID FUNCTIONS

static inline unsigned int pulp_cl_idma_get_id_to_L1() {
  unsigned int dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_DONE_ID_1_REG_OFFSET);
  return dma_tx_id;
}

static inline unsigned int pulp_cl_idma_get_id_to_L2() {
  unsigned int dma_tx_id = DMA_CL_READ(IDMA_REG32_3D_DONE_ID_0_REG_OFFSET);
  return dma_tx_id;
}

// BARRIER FUNCTIONS

static inline unsigned int pulp_cl_idma_tx_cplt_toL1(unsigned int dma_tx_id) {
  unsigned int done_id = DMA_CL_READ(IDMA_REG32_3D_DONE_ID_1_REG_OFFSET);
  unsigned int my_id = dma_tx_id & IDMA_ID_MASK;
  if (done_id >> (IDMA_ID_COUNTER_WIDTH-1) == my_id >> (IDMA_ID_COUNTER_WIDTH-1)) {
    return my_id <= done_id;
  } else {
    return ((done_id & ((IDMA_ID_MASK - (1<<(IDMA_ID_COUNTER_WIDTH-1))) < (1<<(IDMA_ID_COUNTER_WIDTH-2)))));
  }
}

static inline unsigned int pulp_cl_idma_tx_cplt_toL2(unsigned int dma_tx_id) {
  unsigned int done_id = DMA_CL_READ(IDMA_REG32_3D_DONE_ID_0_REG_OFFSET);
  unsigned int my_id = dma_tx_id & IDMA_ID_MASK;
  if (done_id >> (IDMA_ID_COUNTER_WIDTH-1) == my_id >> (IDMA_ID_COUNTER_WIDTH-1)) {
    return my_id <= done_id;
  } else {
    return ((done_id & ((IDMA_ID_MASK - (1<<(IDMA_ID_COUNTER_WIDTH-1))) < (1<<(IDMA_ID_COUNTER_WIDTH-2)))));
  }
}

static inline void plp_cl_dma_wait_toL1(unsigned int dma_tx_id) {
  while(!pulp_cl_idma_tx_cplt_toL1(dma_tx_id)) {
    eu_evt_maskWaitAndClr(1 << IDMA_EVENT);
  }
  return;
}

static inline void plp_cl_dma_barrier_toL1() {
  while(plp_cl_dma_status_toL1()) {
    eu_evt_maskWaitAndClr(1 << IDMA_EVENT);
  }
}

static inline void plp_cl_dma_wait_toL2(unsigned int dma_tx_id) {
  while(!pulp_cl_idma_tx_cplt_toL2(dma_tx_id)) {
    eu_evt_maskWaitAndClr(1 << IDMA_EVENT);
  }
  return;
}

static inline void plp_cl_dma_barrier_toL2() {
  while(plp_cl_dma_status_toL2()) {
    eu_evt_maskWaitAndClr(1 << IDMA_EVENT);
  }
}

// STATUS FUNCTIONS

static inline unsigned int plp_cl_dma_status_toL1() {
  return DMA_CL_READ(IDMA_REG32_3D_STATUS_1_REG_OFFSET);
}

static inline unsigned int plp_cl_dma_status_toL2() {
  return DMA_CL_READ(IDMA_REG32_3D_STATUS_0_REG_OFFSET);
}

#endif