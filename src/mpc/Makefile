ROOT_DIR=../..
SUFFIX=mpc

include $(ROOT_DIR)/include/Makefile.config
include $(ROOT_DIR)/include/Makefile.common

OBJS = dag_rewriter_mpc.o obligation.o environment.o state_translator.o

all: $(addprefix $(OBJ_DIR)/, $(OBJS)) .setup
