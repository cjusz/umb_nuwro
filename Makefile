#DEBUGON = -g

BINDIR = bin
OBJDIR = obj
LIBDIR = lib
STATICLIBDIR = lib/static

CXXFLAGS = `${ROOTSYS}/bin/root-config --cflags` -fPIC -g -O0 $(DEBUGON) -Isrc  -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -Wno-unused-but-set-variable -Wno-reorder
LDFLAGS = -Wl,--no-as-needed `${ROOTSYS}/bin/root-config --libs` -lPythia6 -lEG -lEGPythia6 -lGeom -lMinuit
CXX = g++
CC = g++

TARGETS = nuwro \
					kaskada \
					myroot \
					glue \
					nuwro2neut \
					nuwro2nuance \
					nuwro2rootracker\
					dumpParams \
					test_beam_rf \
					test_makehist \
					test_nucleus \
					test_beam \
					fsi \
					niwg \
					ladek_topologies \
					test_nuwro \
					mb_nce_run \
					ganalysis \
					test_reweight_CCQE_rootracker

TARGETS := $(addprefix $(BINDIR)/, $(TARGETS) )

SF_OBJS=$(patsubst src/%, $(OBJDIR)/%, $(patsubst %.cc,%.o,$(wildcard src/sf/*.cc)))
DIS_SRCSRAW=$(wildcard src/dis/*.cc)
DIS_TOREMOVE=$(addprefix src/dis/, reshadr.cc resevent.cc resweight.cc disweight.cc dishadr.cc)
DIS_SRCS=$(filter-out $(DIS_TOREMOVE),$(DIS_SRCSRAW))
DIS_OBJS=$(patsubst src/%, $(OBJDIR)/%, $(patsubst %.cc,%.o,$(DIS_SRCS)))
RW_OBJS=$(patsubst src/%, $(OBJDIR)/%, $(patsubst %.cc,%.o,$(wildcard src/reweight/*.cc)))
EVENT_SRCS = $(addprefix src/, \
												event1.cc \
												event1dict.cc \
												generatormt.cc \
												particle.cc \
												pauli.cc \
												cohevent2.cc \
												cohdynamics2.cc \
												qelevent1.cc \
												mecdynamics.cc \
												mecevent.cc \
												mecevent_tem.cc \
												mecevent_Nieves.cc\
												mecdynamics2.cc \
												mecevent2.cc \
												qel_sigma.cc \
												kinsolver.cc \
												kinematics.cc \
												pdg.cc \
												target_mixer.cc \
												nucleus.cc \
												sfevent.cc \
												ff.cc \
												dirs.cc \
												rpa_2013.cc \
												nucleus_data.cc \
												isotopes.cc \
												elements.cc \
												beam.cc \
												nd280stats.cc \
												beamHist.cc \
												coh.cc \
												fsi.cc \
												pitab.cc \
												scatter.cc \
												kaskada7.cc \
												Interaction.cc \
												nuwro.cc )
EVENT_OBJS = $(patsubst src/%, $(OBJDIR)/%, $(patsubst %.cc,%.o,$(EVENT_SRCS)))

LIBS=$(STATICLIBDIR)/libevent.a $(STATICLIBDIR)/libsf.a $(STATICLIBDIR)/libdis.a $(STATICLIBDIR)/libreweight.a
SHLIBS=$(LIBDIR)/libevent.so $(LIBDIR)/libsf.so $(LIBDIR)/libdis.so $(LIBDIR)/libreweight.so

all: $(TARGETS) $(LIBS) $(SHLIBS)

$(STATICLIBDIR)/libevent.a: $(EVENT_OBJS)
	@mkdir -p $(@D)
	ar -rcs $@ $^
$(STATICLIBDIR)/libsf.a: $(SF_OBJS)
	@mkdir -p $(@D)
	ar -rcs $@ $^
$(STATICLIBDIR)/libdis.a: $(DIS_OBJS)
	@mkdir -p $(@D)
	ar -rcs $@ $^
$(STATICLIBDIR)/libreweight.a: $(RW_OBJS)
	@mkdir -p $(@D)
	ar -rcs $@ $^

$(LIBDIR)/libevent.so: $(EVENT_OBJS)
	@mkdir -p $(@D)
	$(CXX) -shared $(LDFLAGS) $^ -o $@
$(LIBDIR)/libsf.so: $(SF_OBJS)
	@mkdir -p $(@D)
	$(CXX) -shared $(LDFLAGS) $^ -o $@
$(LIBDIR)/libdis.so: $(DIS_OBJS)
	@mkdir -p $(@D)
	$(CXX) -shared $(LDFLAGS) $^ -o $@
$(LIBDIR)/libreweight.so: $(RW_OBJS)
	@mkdir -p $(@D)
	$(CXX) -shared $(LDFLAGS) $^ -o $@

$(BINDIR)/nuwro: $(OBJDIR)/main.o $(STATICLIBDIR)/libevent.a $(STATICLIBDIR)/libsf.a $(STATICLIBDIR)/libdis.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent -lsf -ldis $(LDFLAGS)

$(BINDIR)/myroot: $(OBJDIR)/myroot.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/glue: $(OBJDIR)/glue.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/nuwro2neut: $(OBJDIR)/nuwro2neut.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/nuwro2nuance: $(OBJDIR)/nuwro2nuance.o $(STATICLIBDIR)/libevent.a
		 @mkdir -p $(@D)
		 $(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/nuwro2rootracker: $(OBJDIR)/nuwro2rootracker.o $(STATICLIBDIR)/libevent.a $(STATICLIBDIR)/libreweight.a
		 @mkdir -p $(@D)
		 $(CXX) $< -o $@ -L$(STATICLIBDIR) -lreweight -levent $(LDFLAGS)

$(BINDIR)/test_reweight_CCQE_rootracker: $(OBJDIR)/test_reweight_CCQE_rootracker.o $(STATICLIBDIR)/libreweight.a $(STATICLIBDIR)/libevent.a src/reweight/RooTrackerEvent.h
		 @mkdir -p $(@D)
		 $(CXX) $< -o $@ -L$(STATICLIBDIR) -lreweight -levent $(LDFLAGS)

$(BINDIR)/kaskada: $(OBJDIR)/kaskada.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

FSIOBJS = $(addprefix $(OBJDIR)/, mplots.o plots.o calculations.o simulations.o vivisection.o)
$(BINDIR)/fsi: $(OBJDIR)/fsi_main.o $(FSIOBJS) $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< $(FSIOBJS) -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

MBNCEOBJ = $(addprefix $(OBJDIR)/, mb_nce.o mb_nce_fit.o)
$(BINDIR)/mb_nce_run: $(OBJDIR)/mb_nce_run.o $(MBNCEOBJ) $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< $(MBNCEOBJ) -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

NIWGOBJ = $(addprefix $(OBJDIR)/, calculations.o niwg_ccqe.o niwg_tech.o niwg_ccpi.o)
$(BINDIR)/niwg: $(OBJDIR)/niwg.o $(NIWGOBJ) $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< $(NIWGOBJ) -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/ladek_topologies: $(OBJDIR)/ladek_topologies.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/test_nuwro: $(OBJDIR)/test.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/ganalysis: $(OBJDIR)/ganalysis.o $(STATICLIBDIR)/libevent.a $(STATICLIBDIR)/libsf.a $(STATICLIBDIR)/libdis.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent -lsf -ldis $(LDFLAGS)

$(BINDIR)/dumpParams: $(OBJDIR)/dumpParams.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/test_nucleus: $(OBJDIR)/test_nucleus.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/test_beam: $(OBJDIR)/test_beam.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/test_beam_rf: $(OBJDIR)/test_beam_rf.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/test_makehist: $(OBJDIR)/test_makehist.o $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

$(BINDIR)/test_balancer: $(OBJDIR)/test_balancer.cc $(STATICLIBDIR)/libevent.a
		@mkdir -p $(@D)
		$(CXX) $< -o $@ -L$(STATICLIBDIR) -levent $(LDFLAGS)

clean:
	@rm -f *.o *.d src/event1dict.cc src/reweight/*.{o,d} src/dis/*.{o,d} src/sf/*.{o,d} src/*.{o,d}\

distclean: clean
	@rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR) *.o *.d src/event1dict.{h,cc,o,d} src/reweight/*.{o,d} src/dis/*.{o,d} src/sf/*.{o,d} src/*.{o,d}\

src/event1dict.h src/event1dict.cc: src/params_all.h src/params.h src/qel_sigma.h src/event1.h src/event1LinkDef.h
		cd src; ${ROOTSYS}/bin/rootcint -f event1dict.cc -c event1.h event1LinkDef.h;

src/params_all.h:  src/params.xml src/params.h src/params.sed Makefile
		@xmllint --dtdvalid src/params.dtd src/params.xml --noout
		@echo "#define PARAMS_ALL()\\">src/params_all.h
		@sed -f src/params.sed src/params.xml >> src/params_all.h
		@echo "" >> src/params_all.h

$(OBJDIR)/%.o $(OBJDIR)/%.d: src/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.d: %.cc
	@echo Making dependencies for $<
	@$(SHELL) -ec '$(CC) -MM -MT "$@ $<" $(CXXFLAGS) $< \
	| sed s/.cc\:/.o:/ > $@;\
	[ -s $@ ] || rm -f $@'

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
 -include $(patsubst %.cc,%.d,$(wildcard  src/dis/*.cc src/sf/*.cc src/*.cc src/gui/*.cc)) src/event1dict.d
endif
endif
