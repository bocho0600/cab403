OBJS := prac3f.o

# link
prac3f: $(OBJS)
		gcc $(OBJS) -o prac3f

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.c
		gcc -c $(CFLAGS) $*.c -o $*.o
		gcc -MM $(CFLAGS) $*.c > $*.d

# remove compilation products
clean:
		rm -f prac3f *.o *.d
