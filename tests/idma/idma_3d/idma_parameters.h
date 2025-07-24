typedef struct {
  unsigned int size;
  unsigned int length;
  unsigned int src_stride_2d;
  unsigned int dst_stride_2d;
  unsigned int src_stride_3d;
  unsigned int dst_stride_3d;
  unsigned int num_reps_3d;
} TransferParameters;

TransferParameters transfer_params[] = {
{127, 3, 7, 11, 5, 11, 3},
{43, 4, 7, 11, 14, 8, 1},
{83, 2, 12, 9, 12, 11, 5},
{136, 8, 13, 12, 16, 18, 3},
{42, 7, 12, 11, 9, 16, 2},
};

