src = $(wildcard *.c)        
obj = $(patsubst %.c, %.o, $(src))  
cflags += -pthread -Wall -g
ldflags += -pthread -lstdc++
target=force_controller
all:$(target)

$(target):$(obj)
	gcc $(ldflags) $(^) -o $(@)   

%.o:%.c
	gcc -c $(cflags) $(^) -o $(@)
	
.PHONY:clean all
clean:
	-rm -rf $(target) $(obj)

 
