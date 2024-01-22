SRC := $(wildcard src/*.c)												 	#定义源文件(.c)
OBJ := $(patsubst src/%.c,object/%.o,$(SRC)) 		 #定义目标文件(.o)
EXEC := bin/client_start													 #定义可执行文件
CFLAGS := -Wall -pthread -Iinclude								  #编译选项

#默认目标-链接规则
all:show $(OBJ)
	@echo "链接"
	gcc $(OBJ) -o $(EXEC) $(CFLAGS)

#显示源文件
show:
	@echo "识别源文件:"
	@echo $(SRC)
	@echo "****************************"

#编译规则
$(OBJ): object/%.o:src/%.c
	@echo "编译:" $<
	gcc -c $< -o $@ $(CFLAGS)
	@echo "****************************"

#清理目标文件
clean:
	rm $(OBJ)