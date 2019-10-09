
protoc --version

proto_file="marionette.proto"
target_dir="$(pwd)/marionette"

mkdir -p $target_dir

# Proto3 definitions
cp $proto_file  $target_dir/$proto_file

# Version/Timestamp
protoc --version > $target_dir/timestamp
date >> $target_dir/timestamp

#  Go
protoc $proto_file --go_out=$target_dir \
         --plugin=./tools/protoc-gen-go
protoc $proto_file --go_out=plugins=grpc:$target_dir \
         --plugin=./tools/protoc-gen-go

#  C++
protoc $proto_file --cpp_out=$target_dir
protoc $proto_file --grpc_out=$target_dir \
         --plugin=protoc-gen-grpc=./tools/grpc_cpp_plugin

#  Python
protoc $proto_file --python_out=$target_dir
protoc $proto_file --grpc_out=$target_dir \
         --plugin=protoc-gen-grpc=./tools/grpc_python_plugin

#  Node.JS
protoc $proto_file --grpc_out=$target_dir  \
         --plugin=protoc-gen-grpc=./tools/grpc_node_plugin

#  C#
protoc $proto_file --csharp_out=$target_dir
protoc $proto_file --grpc_out=$target_dir \
         --plugin=protoc-gen-grpc=./tools/grpc_csharp_plugin