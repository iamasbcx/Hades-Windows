// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: hades_win.proto
#ifndef GRPC_hades_5fwin_2eproto__INCLUDED
#define GRPC_hades_5fwin_2eproto__INCLUDED

#include "hades_win.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/client_context.h>
#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace proto {

class Transfer final {
 public:
  static constexpr char const* service_full_name() {
    return "proto.Transfer";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::proto::RawData, ::proto::Command>> Transfer(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::proto::RawData, ::proto::Command>>(TransferRaw(context));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::proto::RawData, ::proto::Command>> AsyncTransfer(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::proto::RawData, ::proto::Command>>(AsyncTransferRaw(context, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::proto::RawData, ::proto::Command>> PrepareAsyncTransfer(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::proto::RawData, ::proto::Command>>(PrepareAsyncTransferRaw(context, cq));
    }
    class async_interface {
     public:
      virtual ~async_interface() {}
      virtual void Transfer(::grpc::ClientContext* context, ::grpc::ClientBidiReactor< ::proto::RawData,::proto::Command>* reactor) = 0;
    };
    typedef class async_interface experimental_async_interface;
    virtual class async_interface* async() { return nullptr; }
    class async_interface* experimental_async() { return async(); }
   private:
    virtual ::grpc::ClientReaderWriterInterface< ::proto::RawData, ::proto::Command>* TransferRaw(::grpc::ClientContext* context) = 0;
    virtual ::grpc::ClientAsyncReaderWriterInterface< ::proto::RawData, ::proto::Command>* AsyncTransferRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncReaderWriterInterface< ::proto::RawData, ::proto::Command>* PrepareAsyncTransferRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());
    std::unique_ptr< ::grpc::ClientReaderWriter< ::proto::RawData, ::proto::Command>> Transfer(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriter< ::proto::RawData, ::proto::Command>>(TransferRaw(context));
    }
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::proto::RawData, ::proto::Command>> AsyncTransfer(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::proto::RawData, ::proto::Command>>(AsyncTransferRaw(context, cq, tag));
    }
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::proto::RawData, ::proto::Command>> PrepareAsyncTransfer(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::proto::RawData, ::proto::Command>>(PrepareAsyncTransferRaw(context, cq));
    }
    class async final :
      public StubInterface::async_interface {
     public:
      void Transfer(::grpc::ClientContext* context, ::grpc::ClientBidiReactor< ::proto::RawData,::proto::Command>* reactor) override;
     private:
      friend class Stub;
      explicit async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class async* async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class async async_stub_{this};
    ::grpc::ClientReaderWriter< ::proto::RawData, ::proto::Command>* TransferRaw(::grpc::ClientContext* context) override;
    ::grpc::ClientAsyncReaderWriter< ::proto::RawData, ::proto::Command>* AsyncTransferRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncReaderWriter< ::proto::RawData, ::proto::Command>* PrepareAsyncTransferRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_Transfer_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status Transfer(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::proto::Command, ::proto::RawData>* stream);
  };
  template <class BaseClass>
  class WithAsyncMethod_Transfer : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithAsyncMethod_Transfer() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_Transfer() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Transfer(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::proto::Command, ::proto::RawData>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestTransfer(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::proto::Command, ::proto::RawData>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncBidiStreaming(0, context, stream, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_Transfer<Service > AsyncService;
  template <class BaseClass>
  class WithCallbackMethod_Transfer : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithCallbackMethod_Transfer() {
      ::grpc::Service::MarkMethodCallback(0,
          new ::grpc::internal::CallbackBidiHandler< ::proto::RawData, ::proto::Command>(
            [this](
                   ::grpc::CallbackServerContext* context) { return this->Transfer(context); }));
    }
    ~WithCallbackMethod_Transfer() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Transfer(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::proto::Command, ::proto::RawData>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerBidiReactor< ::proto::RawData, ::proto::Command>* Transfer(
      ::grpc::CallbackServerContext* /*context*/)
      { return nullptr; }
  };
  typedef WithCallbackMethod_Transfer<Service > CallbackService;
  typedef CallbackService ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_Transfer : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithGenericMethod_Transfer() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_Transfer() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Transfer(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::proto::Command, ::proto::RawData>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_Transfer : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawMethod_Transfer() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_Transfer() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Transfer(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::proto::Command, ::proto::RawData>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestTransfer(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncBidiStreaming(0, context, stream, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawCallbackMethod_Transfer : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service* /*service*/) {}
   public:
    WithRawCallbackMethod_Transfer() {
      ::grpc::Service::MarkMethodRawCallback(0,
          new ::grpc::internal::CallbackBidiHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
            [this](
                   ::grpc::CallbackServerContext* context) { return this->Transfer(context); }));
    }
    ~WithRawCallbackMethod_Transfer() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Transfer(::grpc::ServerContext* /*context*/, ::grpc::ServerReaderWriter< ::proto::Command, ::proto::RawData>* /*stream*/)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::ServerBidiReactor< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* Transfer(
      ::grpc::CallbackServerContext* /*context*/)
      { return nullptr; }
  };
  typedef Service StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef Service StreamedService;
};

}  // namespace proto


#endif  // GRPC_hades_5fwin_2eproto__INCLUDED
