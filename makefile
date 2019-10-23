cc = gcc

CFLAG = -std=c89 -DCONFIG_STD_C89
LINK  = -lm
INC   = -I ./src/ -I
obj   = image_util.o image_bmp.o data_util.o conv2d.o std_conv2d.o \
	activation.o data_layer.o pad.o pool.o debug_log.o list.o

lenet: lenet.c $(obj)
	$(cc) -o lenet lenet.c $(obj) $(INC) $(CFLAG) $(LINK)

image_util.o: src/image_util.h src/image_util.c
	$(cc) -c src/image_util.c $(INC) $(CFLAG)
image_bmp.o: src/image_bmp.h src/image_bmp.c
	$(cc) -c src/image_bmp.c $(INC) $(CFLAG)
data_util.o: src/data_util.h src/data_util.c
	$(cc) -c src/data_util.c $(INC) $(CFLAG)
conv2d.o: src/conv2d.h src/conv2d.c
	$(cc) -c src/conv2d.c $(INC) $(CFLAG)
pad.o: src/pad.h src/pad.c
	$(cc) -c src/pad.c $(INC) $(CFLAG)
pool.o: src/pool.h src/pool.c
	$(cc) -c src/pool.c $(INC) $(CFLAG)
activation.o: src/activation.h src/activation.c
	$(cc) -c src/activation.c $(INC) $(CFLAG)
std_conv2d.o: src/std_conv2d.h src/std_conv2d.c
	$(cc) -c src/std_conv2d.c $(INC) $(CFLAG)
data_layer.o: src/data_layer.h src/data_layer.c
	$(cc) -c src/data_layer.c $(INC) $(CFLAG)
debug_log.o: src/debug_log.h src/debug_log.c
	$(cc) -c src/debug_log.c $(INC) $(CFLAG)
list.o: src/list.h src/list.c
	$(cc) -c src/list.c $(INC) $(CFLAG)


clean:
	find . -name "*.o"  | xargs rm -f && rm lenet

