#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <gsl/gsl>

#include <experimental/coroutine>

#include <grpcpp/grpcpp.h>
#include <service.grpc.pb.h>

using namespace std;
using namespace std::experimental;
using namespace grpc;

auto make_service() -> unique_ptr<scheduler::Scheduler1::AsyncService>;

struct grpc_await_context_t {
  function<void(coroutine_handle<void>)> callback;

  bool await_ready() { return false; }
  void await_suspend(coroutine_handle<void> coro) { return callback(coro); }
  bool await_resume() { return static_cast<bool>(callback); }
  void set_false() { callback = nullptr; }
};
struct promise_grpc_service_routine_t : public grpc_await_context_t {
  auto initial_suspend() { return suspend_never{}; }
  auto final_suspend() { return suspend_never{}; }
  void unhandled_exception() { throw; }
  void return_value(grpc::Status status) {}

  auto get_return_object() { return this; }
  auto await_transform(function<void(coroutine_handle<void>)> &&callback)
      -> grpc_await_context_t & {
    this->callback = std::move(callback);
    return *this;
  }
};

struct grpc_service_routine_t : coroutine_handle<void> {
  using promise_type = promise_grpc_service_routine_t;

  grpc_service_routine_t(promise_type *p)
      : coroutine_handle<void>{
            coroutine_handle<promise_type>::from_promise(*p)} {}
};

auto completion(function<void(coroutine_handle<void>)> &&on_suspend) {
  return on_suspend;
}

auto serve_once_request(scheduler::Scheduler1::AsyncService &service,
                        gsl::not_null<grpc_impl::ServerCompletionQueue *> cq)
    -> grpc_service_routine_t {
  ServerContext ctx{};
  ServerAsyncResponseWriter<scheduler::Response1> responder{&ctx};

  scheduler::Request1 req{};

  co_await completion([&](coroutine_handle<void> coro) {
    service.RequestOnce(&ctx, &req, &responder, cq, cq, coro.address());
  });

  scheduler::Response1 res{};
  grpc::Status status = service.Once(&ctx, &req, &res);

  co_await completion([&](coroutine_handle<void> coro) {
    responder.Finish(res, status, coro.address());
  });
  co_return status;
}

auto serve_stream_request(scheduler::Scheduler1::AsyncService &service,
                          gsl::not_null<grpc_impl::ServerCompletionQueue *> cq)
    -> grpc_service_routine_t {
  ServerContext ctx{};
  ServerAsyncWriter<scheduler::Response1> responder{&ctx};

  scheduler::Request1 req{};

  co_await completion([&](coroutine_handle<void> coro) {
    service.RequestStream(&ctx, &req, &responder, //
                          cq, cq, coro.address());
  });

  scheduler::Response1 res{};
  grpc::Status status = service.Once(&ctx, &req, &res);

  for (auto i = 0u; i < 1u; ++i)
    co_await completion([&](coroutine_handle<void> coro) {
      responder.Write(res, coro.address());
    });

  co_await completion([&](coroutine_handle<void> coro) {
    responder.Finish(status, coro.address());
  });
  co_return status;
}

void serve_with_queue(grpc::Server &server, grpc::CompletionQueue &cq,
                      size_t repeat) {
  void *user_data = nullptr;
  bool ok = false;

  // dispatch and continue serve
  while (repeat--) {
    if (cq.Next(&user_data, &ok) == false)
      FAIL("completion queue: next failed");

    using promise_t = promise_grpc_service_routine_t;
    auto task = coroutine_handle<promise_t>::from_address(user_data);
    if (ok == false)
      task.promise().set_false();

    if (task && task.done() == false)
      task.resume();
  }
}

TEST_CASE("completion queue registration", "[grpc]") {
  ServerBuilder builder{};
  builder.AddListeningPort("0.0.0.0:30444", grpc::InsecureServerCredentials());

  SECTION("no registration") {
    // no server object
  }
  SECTION("RegisterService") {
    scheduler::Scheduler1::AsyncService service{};
    REQUIRE_NOTHROW(builder.RegisterService(&service));
  }

  auto cq = builder.AddCompletionQueue();
  REQUIRE(cq);
  REQUIRE_NOTHROW(cq->Shutdown());
}

TEST_CASE("completion queue shutdown", "[grpc]") {
  ServerBuilder builder{};
  builder.AddListeningPort("0.0.0.0:30445", grpc::InsecureServerCredentials());

  auto service = make_service();
  REQUIRE_NOTHROW(builder.RegisterService(service.get()));

  auto cq = builder.AddCompletionQueue();
  REQUIRE(cq);

  auto launcher = builder.BuildAndStart();
  REQUIRE(launcher);
  REQUIRE_NOTHROW(launcher->Shutdown(), launcher->Wait());
  REQUIRE_NOTHROW(cq->Shutdown());

  // we have to drain the queue. see https://github.com/grpc/grpc/issues/8610
  {
    void *user_data;
    bool ok;
    REQUIRE_FALSE(cq->Next(&user_data, &ok));
  }
}

TEST_CASE("server method call", "[grpc][impl]") {
  ServerBuilder builder{};
  builder.AddListeningPort("0.0.0.0:30443", grpc::InsecureServerCredentials());

  auto service = make_service();
  REQUIRE_NOTHROW(builder.RegisterService(service.get()));

  auto cq = builder.AddCompletionQueue();
  auto launcher = builder.BuildAndStart();
  // completion queue is referencing grpc server.
  // so their life cycle must be managed precisely
  auto on_return_1 = gsl::finally([queue = cq.get()]() {
    REQUIRE_NOTHROW(queue->Shutdown());
    // ensure the drain. see https://github.com/grpc/grpc/issues/8610
    void *user_data;
    bool ok;
    REQUIRE_FALSE(queue->Next(&user_data, &ok));
  });
  auto on_return_2 = gsl::finally([s = launcher.get()]() {
    // ...
    REQUIRE_NOTHROW(s->Shutdown());
    REQUIRE_NOTHROW(s->Wait());
  });

  auto creds = grpc::InsecureChannelCredentials();
  auto chan = grpc::CreateChannel("127.0.0.1:30443", creds);

  SECTION("client requests 'Once'") {
    serve_once_request(*service, cq.get());

    // will make one request ...
    [chan, queue = cq.get()]() -> grpc_service_routine_t {
      auto stub = scheduler::Scheduler1::NewStub(chan);

      ClientContext ctx{};
      scheduler::Request1 req{};
      auto rpc = stub->AsyncOnce(&ctx, req, queue);

      scheduler::Response1 res{};
      grpc::Status status{};

      co_await completion([&, rpc = rpc.get()](coroutine_handle<void> coro) {
        rpc->Finish(&res, &status, coro.address());
      });
      co_return status;
    }();
    // continue until all coroutines are done
    REQUIRE_NOTHROW(serve_with_queue(*launcher, *cq, 3));
  }

  SECTION("client requests 'Stream'") {
    serve_stream_request(*service, cq.get());

    // will make one request ...
    [chan, queue = cq.get()]() -> grpc_service_routine_t {
      auto stub = scheduler::Scheduler1::NewStub(chan);

      ClientContext ctx{};
      scheduler::Request1 req{};
      auto rpc = unique_ptr<grpc::ClientAsyncReader<scheduler::Response1>>{};

      co_await completion([&](coroutine_handle<void> coro) {
        rpc = stub->AsyncStream(&ctx, req, queue, coro.address());
      });

      scheduler::Response1 res{};
      auto reader = [&, rpc = rpc.get()](coroutine_handle<void> coro) {
        rpc->Read(&res, coro.address());
      };

      for (bool ok = co_await completion(reader);
           ok; // false if completion queue notifies false
           ok = co_await completion(reader)) {
        // ... on send ...
      }

      grpc::Status status{};
      co_await[&, rpc = rpc.get()](coroutine_handle<void> coro) {
        rpc->Finish(&status, coro.address());
      };
      co_return status;
    }();
    // continue until all coroutines are done
    REQUIRE_NOTHROW(serve_with_queue(*launcher, *cq, 7));
  }
}

class AsyncServiceImpl : public scheduler::Scheduler1::AsyncService {
  grpc::Status status = grpc::Status{grpc::StatusCode::UNIMPLEMENTED, ""};

  grpc::Status                       //
  Once(grpc::ServerContext *context, //
       const scheduler::Request1 *request,
       scheduler::Response1 *response) override {

    return status;
  }

  grpc::Status
  Stream(grpc::ServerContext *context, //
         const scheduler::Request1 *request,
         grpc::ServerWriter<scheduler::Response1> *writer) override {
    return status;
  }
};

auto make_service() -> unique_ptr<scheduler::Scheduler1::AsyncService> {
  return make_unique<AsyncServiceImpl>();
}