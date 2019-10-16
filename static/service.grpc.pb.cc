// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: service.proto

#include "service.pb.h"
#include "service.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace plugins {

static const char* Executor1_method_names[] = {
  "/plugins.Executor1/Invoke1",
  "/plugins.Executor1/Invoke2",
  "/plugins.Executor1/Invoke3",
  "/plugins.Executor1/Invoke4",
};

std::unique_ptr< Executor1::Stub> Executor1::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Executor1::Stub> stub(new Executor1::Stub(channel));
  return stub;
}

Executor1::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Invoke1_(Executor1_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Invoke2_(Executor1_method_names[1], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_Invoke3_(Executor1_method_names[2], ::grpc::internal::RpcMethod::CLIENT_STREAMING, channel)
  , rpcmethod_Invoke4_(Executor1_method_names[3], ::grpc::internal::RpcMethod::BIDI_STREAMING, channel)
  {}

::grpc::Status Executor1::Stub::Invoke1(::grpc::ClientContext* context, const ::plugins::Request1& request, ::plugins::Response1* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Invoke1_, context, request, response);
}

void Executor1::Stub::experimental_async::Invoke1(::grpc::ClientContext* context, const ::plugins::Request1* request, ::plugins::Response1* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Invoke1_, context, request, response, std::move(f));
}

void Executor1::Stub::experimental_async::Invoke1(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::plugins::Response1* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Invoke1_, context, request, response, std::move(f));
}

void Executor1::Stub::experimental_async::Invoke1(::grpc::ClientContext* context, const ::plugins::Request1* request, ::plugins::Response1* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Invoke1_, context, request, response, reactor);
}

void Executor1::Stub::experimental_async::Invoke1(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::plugins::Response1* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Invoke1_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::plugins::Response1>* Executor1::Stub::AsyncInvoke1Raw(::grpc::ClientContext* context, const ::plugins::Request1& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::plugins::Response1>::Create(channel_.get(), cq, rpcmethod_Invoke1_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::plugins::Response1>* Executor1::Stub::PrepareAsyncInvoke1Raw(::grpc::ClientContext* context, const ::plugins::Request1& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::plugins::Response1>::Create(channel_.get(), cq, rpcmethod_Invoke1_, context, request, false);
}

::grpc::ClientReader< ::plugins::Response1>* Executor1::Stub::Invoke2Raw(::grpc::ClientContext* context, const ::plugins::Request1& request) {
  return ::grpc_impl::internal::ClientReaderFactory< ::plugins::Response1>::Create(channel_.get(), rpcmethod_Invoke2_, context, request);
}

void Executor1::Stub::experimental_async::Invoke2(::grpc::ClientContext* context, ::plugins::Request1* request, ::grpc::experimental::ClientReadReactor< ::plugins::Response1>* reactor) {
  ::grpc_impl::internal::ClientCallbackReaderFactory< ::plugins::Response1>::Create(stub_->channel_.get(), stub_->rpcmethod_Invoke2_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::plugins::Response1>* Executor1::Stub::AsyncInvoke2Raw(::grpc::ClientContext* context, const ::plugins::Request1& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::plugins::Response1>::Create(channel_.get(), cq, rpcmethod_Invoke2_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::plugins::Response1>* Executor1::Stub::PrepareAsyncInvoke2Raw(::grpc::ClientContext* context, const ::plugins::Request1& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::plugins::Response1>::Create(channel_.get(), cq, rpcmethod_Invoke2_, context, request, false, nullptr);
}

::grpc::ClientWriter< ::plugins::Request1>* Executor1::Stub::Invoke3Raw(::grpc::ClientContext* context, ::plugins::Response1* response) {
  return ::grpc_impl::internal::ClientWriterFactory< ::plugins::Request1>::Create(channel_.get(), rpcmethod_Invoke3_, context, response);
}

void Executor1::Stub::experimental_async::Invoke3(::grpc::ClientContext* context, ::plugins::Response1* response, ::grpc::experimental::ClientWriteReactor< ::plugins::Request1>* reactor) {
  ::grpc_impl::internal::ClientCallbackWriterFactory< ::plugins::Request1>::Create(stub_->channel_.get(), stub_->rpcmethod_Invoke3_, context, response, reactor);
}

::grpc::ClientAsyncWriter< ::plugins::Request1>* Executor1::Stub::AsyncInvoke3Raw(::grpc::ClientContext* context, ::plugins::Response1* response, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc_impl::internal::ClientAsyncWriterFactory< ::plugins::Request1>::Create(channel_.get(), cq, rpcmethod_Invoke3_, context, response, true, tag);
}

::grpc::ClientAsyncWriter< ::plugins::Request1>* Executor1::Stub::PrepareAsyncInvoke3Raw(::grpc::ClientContext* context, ::plugins::Response1* response, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncWriterFactory< ::plugins::Request1>::Create(channel_.get(), cq, rpcmethod_Invoke3_, context, response, false, nullptr);
}

::grpc::ClientReaderWriter< ::plugins::Request1, ::plugins::Response1>* Executor1::Stub::Invoke4Raw(::grpc::ClientContext* context) {
  return ::grpc_impl::internal::ClientReaderWriterFactory< ::plugins::Request1, ::plugins::Response1>::Create(channel_.get(), rpcmethod_Invoke4_, context);
}

void Executor1::Stub::experimental_async::Invoke4(::grpc::ClientContext* context, ::grpc::experimental::ClientBidiReactor< ::plugins::Request1,::plugins::Response1>* reactor) {
  ::grpc_impl::internal::ClientCallbackReaderWriterFactory< ::plugins::Request1,::plugins::Response1>::Create(stub_->channel_.get(), stub_->rpcmethod_Invoke4_, context, reactor);
}

::grpc::ClientAsyncReaderWriter< ::plugins::Request1, ::plugins::Response1>* Executor1::Stub::AsyncInvoke4Raw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc_impl::internal::ClientAsyncReaderWriterFactory< ::plugins::Request1, ::plugins::Response1>::Create(channel_.get(), cq, rpcmethod_Invoke4_, context, true, tag);
}

::grpc::ClientAsyncReaderWriter< ::plugins::Request1, ::plugins::Response1>* Executor1::Stub::PrepareAsyncInvoke4Raw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncReaderWriterFactory< ::plugins::Request1, ::plugins::Response1>::Create(channel_.get(), cq, rpcmethod_Invoke4_, context, false, nullptr);
}

Executor1::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Executor1_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Executor1::Service, ::plugins::Request1, ::plugins::Response1>(
          std::mem_fn(&Executor1::Service::Invoke1), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Executor1_method_names[1],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< Executor1::Service, ::plugins::Request1, ::plugins::Response1>(
          std::mem_fn(&Executor1::Service::Invoke2), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Executor1_method_names[2],
      ::grpc::internal::RpcMethod::CLIENT_STREAMING,
      new ::grpc::internal::ClientStreamingHandler< Executor1::Service, ::plugins::Request1, ::plugins::Response1>(
          std::mem_fn(&Executor1::Service::Invoke3), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Executor1_method_names[3],
      ::grpc::internal::RpcMethod::BIDI_STREAMING,
      new ::grpc::internal::BidiStreamingHandler< Executor1::Service, ::plugins::Request1, ::plugins::Response1>(
          std::mem_fn(&Executor1::Service::Invoke4), this)));
}

Executor1::Service::~Service() {
}

::grpc::Status Executor1::Service::Invoke1(::grpc::ServerContext* context, const ::plugins::Request1* request, ::plugins::Response1* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Executor1::Service::Invoke2(::grpc::ServerContext* context, const ::plugins::Request1* request, ::grpc::ServerWriter< ::plugins::Response1>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Executor1::Service::Invoke3(::grpc::ServerContext* context, ::grpc::ServerReader< ::plugins::Request1>* reader, ::plugins::Response1* response) {
  (void) context;
  (void) reader;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Executor1::Service::Invoke4(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::plugins::Response1, ::plugins::Request1>* stream) {
  (void) context;
  (void) stream;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace plugins

