PROJECT_NAME = soar_web_server
SOURCE_PATH = ./soar
SOURCE = $(SOURCE_PATH)/components/network/*.cc $(SOURCE_PATH)/components/scheduling/*.cc $(SOURCE_PATH)/components/system/*.cc
LIBS = -lpthread 
CC = g++ -g
CFLAGS = -D__MULTI_THREAD_MEM_SHARE__
OBJS = main.o

all: $(PROJECT_NAME)

.PHONY : clean
clean:
	rm -f *.o $(PROJECT_NAME)

$(PROJECT_NAME):$(OBJS)
	$(CC) -o $(PROJECT_NAME) $(SOURCE) $(OBJS) $(LIBS)

%.o : %.cc
	$(CC) -c $< -o $@  $(CFLAGS) $(INCLUDE)