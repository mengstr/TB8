PAL:=../tools/palbart

.PHONY: clean

all: clean $(PAL) tb8.bin

# Assemble the PDP8 sources
tb8.bin: tb8.pal
	$(PAL) -a -p -d -x $<


# Create the palbart assembler from source
$(PAL): ../tools/src/palbart.c
	$(CC) -Wall -Os -o $@ $<

        
#
clean:
	@rm -rf $(PAL) *~ *.bin *.err *.lst *.prm
