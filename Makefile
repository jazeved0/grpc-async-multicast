CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -pthread\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

BINDIR = ./bin
OBJDIR = ./bin
PROTODIR = ./protobuf
GENDIR = ./generated

vpath %.proto $(PROTODIR)
vpath %.cpp ./src

.PHONY: all clean format
.SECONDARY:

all: $(BINDIR)/client $(BINDIR)/server

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

$(BINDIR)/client: $(GENDIR)/demo.pb.o $(GENDIR)/demo.grpc.pb.o $(OBJDIR)/client.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(BINDIR)/server: $(GENDIR)/demo.pb.o $(GENDIR)/demo.grpc.pb.o $(OBJDIR)/server.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(GENDIR)/%.grpc.pb.cc: $(PROTODIR)/%.proto
	$(PROTOC) -I $(PROTODIR) --grpc_out=$(GENDIR) --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

$(GENDIR)/%.pb.cc: $(PROTODIR)/%.proto
	$(PROTOC) -I $(PROTODIR) --cpp_out=$(GENDIR) $<

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/client $(BINDIR)/server $(GENDIR)/*.pb.*

format:
	clang-format -i -style=LLVM *.cpp
