CLONE:=git clone -q --depth=1 
PAL:=tools/palbart
CHK:=tools/chkoverlap
EMU:=tools/pdp8emu
CORE:=tools/pt8exam
REPORT:=tools/palreport.sh
UDP:=tools/tb8udp

RUNTIME:=5

.PHONY: run report clean distclean goldenclean test

all: tb8.bin

# Assemble the PDP8 source into a .bin file
tb8.bin: tb8.pal tools
	$(PAL) -a -p -n -d -x -t 8 $<
	$(CHK) -b $@
	$(CHK) -b -m -s $@ > tb8.map
	$(CORE) -w tb8.bin


#
report:
	$(REPORT) < tb8.lst | tee tb8.rpt

#
run: tb8.bin
	(sleep 1; cat tb8.test1.cmd | nc -u 127.0.0.1 2288) & ($(EMU) tb8.bin.core; killall nc)


#
test: tb8.bin $(addsuffix .lst.tmp, $(basename $(wildcard tests/*.bas)))  $(addsuffix .run.tmp, $(basename $(wildcard tests/*.bas)))

tests/%.lst.tmp : tests/%.bas
	@echo Testing LIST for $<
	@$(eval ID=$(shell $(EMU) tb8.bin.core > golden.tmp & echo $$!))
	@sleep 0.3
	@printf "c\n"  			|$(UDP)
	@printf "s200p\n" 		|$(UDP)
	@printf "g\n" 			|$(UDP)
	@$(UDP) < $<
	@printf "\n"  			|$(UDP)
	@printf "LIST\n" 		|$(UDP)
	@sleep 2
	@kill -hup $(ID) >/dev/null 2>/dev/null || true
	@sed -n '/>LIST/,/^[[:space:]]*$$/p' golden.tmp | sed -e '$$a\' > $@
	@diff -a $@ $(addsuffix .golden, $(basename $@))
	@rm $@

tests/%.run.tmp : tests/%.bas
	@echo Testing RUN for $<
	@$(eval ID=$(shell $(EMU) tb8.bin.core > golden.tmp & echo $$!))
	@sleep 0.3
	@printf "c\n"  			|$(UDP)
	@printf "s200p\n" 		|$(UDP)
	@printf "g\n" 			|$(UDP)
	@$(UDP) < $<
	@sleep 0.3
	@printf "RUN\n" 		|$(UDP)
	@sleep 0.3
	@if [ -f '$(addsuffix .inp, $(basename $<))' ]; then $(UDP) 1 < $(addsuffix .inp, $(basename $<));\
	else sleep $(RUNTIME); fi
	@sleep 0.3
	@kill -hup $(ID) >/dev/null 2>/dev/null || true
	@sed -n '/>RUN/,//p' golden.tmp | sed -e '$$a\' > $@
	@diff -a $@ $(addsuffix .golden, $(basename $@))
	@rm $@

#
golden: $(addsuffix .lst.golden, $(basename $(wildcard tests/*.bas))) $(addsuffix .run.golden, $(basename $(wildcard tests/*.bas)))

tests/%.lst.golden : tests/%.bas
	@echo Making golden LIST for $<
	@$(eval ID=$(shell $(EMU) tb8.bin.core > golden.tmp & echo $$!))
	@sleep 0.3
	@printf "c\n"  			|$(UDP)
	@printf "s200p\n" 		|$(UDP)
	@printf "g\n" 			|$(UDP)
	@$(UDP) < $<
	@printf "\n"  			|$(UDP)
	@printf "LIST\n" 		|$(UDP)
	@sleep 2
	@kill -hup $(ID) >/dev/null 2>/dev/null || true
	@sed -n '/>LIST/,/^[[:space:]]*$$/p' golden.tmp | sed -e '$$a\' > $@

tests/%.run.golden : tests/%.bas
	@echo Making golden RUN for $<
	@$(eval ID=$(shell $(EMU) tb8.bin.core > golden.tmp & echo $$!))
	@sleep 0.3
	@printf "c\n"  			|$(UDP)
	@printf "s200p\n" 		|$(UDP)
	@printf "g\n" 			|$(UDP)
	@$(UDP) < $<
	@sleep 0.3
	@printf "RUN\n" 		|$(UDP)
	@sleep 0.3
	@if [ -f '$(addsuffix .inp, $(basename $<))' ]; then $(UDP) 1 < $(addsuffix .inp, $(basename $<));\
	else sleep $(RUNTIME); fi
	@sleep 0.3
	@kill -hup $(ID) >/dev/null 2>/dev/null || true
	@sed -n '/>RUN/,//p' golden.tmp | sed -e '$$a\' > $@


#
tools:
	mkdir -p tools

	$(CLONE) https://github.com/SmallRoomLabs/tb8udp.git tools/tb8udp-src
	@cd tools/tb8udp-src; make; cp tb8udp ..; cd ..; rm -rf tb8udp-src
	@echo ""

	$(CLONE) https://github.com/SmallRoomLabs/pt8exam.git tools/pt8exam-src
	@cd tools/pt8exam-src; make; cp pt8exam ..; cd ..; rm -rf pt8exam-src
	@echo ""

	$(CLONE) https://github.com/SmallRoomLabs/palbart.git tools/palbart-src
	@cd tools/palbart-src; make; cp palbart ..; cd ..; rm -rf palbart-src
	@echo ""

	$(CLONE) https://github.com/SmallRoomLabs/chkoverlap.git tools/chkoverlap-src
	@cd tools/chkoverlap-src; make; cp chkoverlap ..; cd ..; rm -rf chkoverlap-src
	@echo ""

	$(CLONE) https://github.com/SmallRoomLabs/pdp8emu.git tools/pdp8emu-src
	@cd tools/pdp8emu-src; make; cp pdp8emu ..; cd ..; rm -rf pdp8emu-src
	@echo ""

	$(CLONE) https://github.com/SmallRoomLabs/palreport.git tools/palreport-src
	@cd tools/palreport-src; cp palreport.sh ..; cd ..; rm -rf palreport-src
	@echo ""
	

#
clean:
	rm -rf *~ *.tmp *.bin *.err *.lst *.prm *.rim *.map *.core *.rpt tests/*~ tests/*.tmp

#
distclean:
	make clean
	rm -rf tools/

#
goldenclean: 
	rm -f tests/*.golden
