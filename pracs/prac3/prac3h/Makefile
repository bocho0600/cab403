OBJS := prac3h.o

# link
prac3h: $(OBJS)
		gcc $(OBJS) -o prac3h

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.c
		gcc -c $(CFLAGS) $*.c -o $*.o
		gcc -MM $(CFLAGS) $*.c > $*.d

# remove compilation products
clean:
		rm -f prac3h *.o *.d
