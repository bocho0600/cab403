OBJS := prac3g.o

# link
prac3e: $(OBJS)
		gcc $(OBJS) -o prac3g

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.c
		gcc -c $(CFLAGS) $*.c -o $*.o
		gcc -MM $(CFLAGS) $*.c > $*.d

# remove compilation products
clean:
		rm -f prac3g *.o *.d
