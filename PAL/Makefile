PAL:=tools/palbart
CHK:=tools/chkoverlap
EMU:=tools/pdp8emu
CORE:=tools/pt8exam
REPORT:=tools/palreport.sh

.PHONY: run tools clean distclean

all: tb8.bin

# Assemble the PDP8 source into a .bin file
tb8.bin: tb8.pal
	$(PAL) -a -p -n -d -x -t 8 $<
	$(CHK) -b $@
	$(CHK) -b -m -s $@ > tb8.map
	$(CORE) -w tb8.bin
	$(REPORT) < tb8.lst | tee tb8.rpt

#
run: tb8.bin
	(sleep 1; cat tb8.test1.cmd | nc -u 127.0.0.1 2288) & ($(EMU) tb8.bin.core; killall nc)

#
tools:
	mkdir -p tools
	#
	git clone --depth=1 https://github.com/SmallRoomLabs/pt8exam.git tools/pt8exam-src
	cd tools/pt8exam-src; make; cp pt8exam ..; cd ..; rm -rf pt8exam-src
	#
	git clone --depth=1 https://github.com/SmallRoomLabs/palbart.git tools/palbart-src
	cd tools/palbart-src; make; cp palbart ..; cd ..; rm -rf palbart-src
	#
	git clone --depth=1 https://github.com/SmallRoomLabs/chkoverlap.git tools/chkoverlap-src
	cd tools/chkoverlap-src; make; cp chkoverlap ..; cd ..; rm -rf chkoverlap-src
	#
	git clone --depth=1 https://github.com/SmallRoomLabs/pdp8emu.git tools/pdp8emu-src
	cd tools/pdp8emu-src; make; cp pdp8emu ..; cd ..; rm -rf pdp8emu-src
	#
	git clone --depth=1 https://github.com/SmallRoomLabs/palreport.git tools/palreport-src
	cd tools/palreport-src; make; cp palreport.sh ..; cd ..; rm -rf palreport-src
	
	#
clean:
	rm -rf *~ *.bin *.err *.lst *.prm *.rim *.map *.core *.rpt

distclean:
	make clean
	rm -rf tools/