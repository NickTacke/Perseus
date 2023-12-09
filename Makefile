# Set the compiler variables
CXX=g++ -std=c++17
CXXFLAGS+=-Wall -lX11 -lXi
LDFLAGS+=-L${CURDIR}/bin
OBJDIR=obj
OBJS=$(addprefix ${OBJDIR}/,$(subst .cpp,.o,${CXXFILES}))

# Get all the .h and .cpp files
HFILES:=$(shell find src/ -type f -iname *.h -print)
CXXFILES:=$(shell find src/ -type f -iname *.cpp -print)

# Make /bin/Perseus and let the user know of other commands
all: ${CURDIR}/bin/Perseus
	@echo "-[ Use \"make dev\" to keep debug symbols ]-"
	@echo "-[ Use \"make clean\" to remove object files ]-"

# Add more flags if being ran as dev mode
dev: CXXFLAGS+= -g -DDEBUG_CERR
dev: ${CURDIR}/bin/Perseus

# Remove all the object files in the obj/ folder
.PHONY: clean
clean:
	rm -r ${OBJDIR}

# Compile the .cpp files and output their object files to the OBJDIR
${CURDIR}/bin/Perseus: $(OBJS)
	${CXX} -o ${CURDIR}/bin/Perseus $(OBJS) ${LDFLAGS} ${CXXFLAGS}

# Compile the binary from the object files
${OBJDIR}/%.o: %.cpp $(HFILES)
	@mkdir -p $$(dirname $@)
	$(CXX) -c -o $@ $< ${LDFLAGS} $(CXXFLAGS)
