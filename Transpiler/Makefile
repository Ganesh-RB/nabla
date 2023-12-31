EXEC = nabla
BUILDDIR = Build
FLEX = flex
BISON = bison
CXX = clang++
CXXFLAGS = -std=c++17 -g
LIBSDIR = lib

OBJECTS = $(BUILDDIR)/grammar.tab.o $(BUILDDIR)/lex.yy.o $(BUILDDIR)/ast.o $(BUILDDIR)/traversal.o $(BUILDDIR)/sym.o 
.SILENT: ${LIBSDIR}

$(EXEC): $(OBJECTS) ${LIBSDIR}
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)

${LIBSDIR}:
	@mkdir -p ${LIBSDIR}
	$(MAKE) -C include -no-print-directory

grammar.tab.c grammar.tab.h : grammar.y
	$(BISON) -d grammar.y

lex.yy.c : lexer.l grammar.tab.h
	$(FLEX) lexer.l

$(BUILDDIR)/%.o: %.cpp %.h
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.c %.h
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.yy.c
	rm -f *.tab.c
	rm -f *.tab.h
	rm -f $(BUILDDIR)/*.o
	rm -f $(EXEC)
	rm -rf $(LIBSDIR)

graph: 
	dot -Tsvg computational_graph.dot -o output.svg
	xdg-open output.svg
