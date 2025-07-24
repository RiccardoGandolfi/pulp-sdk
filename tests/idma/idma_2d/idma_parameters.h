typedef struct {
  unsigned int size;
  unsigned int length;
  unsigned int src_stride;
  unsigned int dst_stride;
} TransferParameters;

TransferParameters transfer_params[] = {
{85, 2, 9, 9},
{107, 5, 14, 7},
{128, 10, 14, 20},
{61, 4, 7, 8},
{35, 7, 12, 8},
{17, 7, 12, 15},
{120, 7, 10, 14},
{20, 3, 11, 5},
};

