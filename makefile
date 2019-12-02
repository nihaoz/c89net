# Using clang for higher performance
# cc = clang
# Have fun with Tiny C Compiler, disable Openmp and some other 
# optional functions, since tcc doesn't support these features 
# cc = tcc
cc = gcc

# Openmp support
OMP   = -DENABLE_OPENMP -fopenmp

# Option for c89, may degrade performance
C89F  = # -std=c89 -DCONFIG_STD_C89

LINK  = -lm
INC   = -I ./src/

DEBUGF = # -g -fsanitize=address -fno-omit-frame-pointer

OPTM_A = -march=native
OPTM_O = -Os
OPTM   = $(OPTM_A) $(OPTM_O)

# Optional functions
OPTFN  = -DSET_GCC_BUILTIN_VEX

# Resource limitation
RESLIM = # -DLESS_RESOURCE

# Testing features

TESTFN = -DENABLE_MEMMGR

CFLAG = $(C89F) $(TESTFN) $(OPTM) $(OMP) $(DEBUGF)

G_FN_INC  = -I ./src/optimization_simd/gcc_builtin_vector/
G_FN_CTRL = ./src/global_function_config.h ./src/global_function_config.c

obj = image_util.o image_bmp.o data_util.o data_types.o array_ops.o memmgr.o \
	conv2d.o spatial_conv2d.o depthwise_conv2d.o activation.o upsample.o \
	pointwise_conv2d.o data_layer.o pool.o fmap_ops.o fullyconn.o list.o \
	pad.o normalization.o debug_log.o global_function_config.o           \
	vxsf_fullyconn.o 

DEMO  = memmgr_test
CNN   = lenet tinynet
ALL   = $(DEMO) $(CNN)

all: $(ALL)

demo: $(DEMO)

cnn: $(CNN)

# CNNs
lenet: cnn/lenet/lenet.c $(obj)
	$(cc) -o $(@) cnn/lenet/lenet.c $(obj) $(INC) $(CFLAG) $(LINK)
tinynet: cnn/tinynet/tinynet.c $(obj)
	$(cc) -o $(@) cnn/tinynet/tinynet.c $(obj) $(INC) $(CFLAG) $(LINK)

# Simple demo
matmul_test: demo/matmul_test.c $(obj)
	$(cc) -o $(@) demo/matmul_test.c $(obj) $(INC) $(CFLAG) $(LINK)

memmgr_test: demo/memmgr_test.c $(obj)
	$(cc) -o $(@) demo/memmgr_test.c $(obj) $(INC) $(CFLAG) $(LINK)

list.o: src/list.h src/list.c
	$(cc) -c src/list.c $(INC) $(CFLAG) -Wno-unused-result
image_util.o: src/image_util.h src/image_util.c
	$(cc) -c src/image_util.c $(INC) $(CFLAG)
image_bmp.o: src/image_bmp.h src/image_bmp.c
	$(cc) -c src/image_bmp.c $(INC) $(CFLAG) -Wno-unused-result
data_types.o: src/data_types.h src/data_types.c
	$(cc) -c src/data_types.c $(INC) $(CFLAG)
data_util.o: src/data_util.h src/data_util.c
	$(cc) -c src/data_util.c $(INC) $(CFLAG)
array_ops.o: src/array_ops.h src/array_ops.c
	$(cc) -c src/array_ops.c $(INC) $(CFLAG)
fmap_ops.o: src/fmap_ops.h src/fmap_ops.c
	$(cc) -c src/fmap_ops.c $(INC) $(CFLAG)
conv2d.o: src/conv2d.h src/conv2d.c
	$(cc) -c src/conv2d.c $(INC) $(CFLAG)
pad.o: src/pad.h src/pad.c
	$(cc) -c src/pad.c $(INC) $(CFLAG)
pool.o: src/pool.h src/pool.c
	$(cc) -c src/pool.c $(INC) $(CFLAG)
upsample.o: src/upsample.h src/upsample.c
	$(cc) -c src/upsample.c $(INC) $(CFLAG)
activation.o: src/activation.h src/activation.c
	$(cc) -c src/activation.c $(INC) $(CFLAG)
normalization.o: src/normalization.h src/normalization.c
	$(cc) -c src/normalization.c $(INC) $(CFLAG)
spatial_conv2d.o: src/spatial_conv2d.h src/spatial_conv2d.c
	$(cc) -c src/spatial_conv2d.c $(INC) $(CFLAG)
depthwise_conv2d.o: src/depthwise_conv2d.h src/depthwise_conv2d.c
	$(cc) -c src/depthwise_conv2d.c $(INC) $(CFLAG)
pointwise_conv2d.o: src/pointwise_conv2d.h src/pointwise_conv2d.c
	$(cc) -c src/pointwise_conv2d.c $(INC) $(CFLAG)
fullyconn.o: src/fullyconn.h src/fullyconn.c
	$(cc) -c src/fullyconn.c $(INC) $(CFLAG)
data_layer.o: src/data_layer.h src/data_layer.c
	$(cc) -c src/data_layer.c $(INC) $(CFLAG)
memmgr.o: src/memmgr.h src/memmgr.c
	$(cc) -c src/memmgr.c $(INC) $(CFLAG)
debug_log.o: src/debug_log.h src/debug_log.c
	$(cc) -c src/debug_log.c $(INC) $(CFLAG)

# Global functions config
global_function_config.o: $(G_FN_CTRL)
	$(cc) -c src/global_function_config.c $(INC) $(G_FN_INC) $(CFLAG) $(OPTFN)

# Optional functions
vxsf_fullyconn.o: src/optimization_simd/gcc_builtin_vector/vxsf_fullyconn.h \
		src/optimization_simd/gcc_builtin_vector/vxsf_fullyconn.c
	$(cc) -c src/optimization_simd/gcc_builtin_vector/vxsf_fullyconn.c \
		$(INC) $(CFLAG) $(OPTFN)

clean:
	find . -name "*.o"  | xargs rm -f && rm $(ALL)
