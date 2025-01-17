#ifndef THC_GENERIC_FILE
#define THC_GENERIC_FILE "THC/generic/THCStorage.cpp"
#else

#include <c10/util/intrusive_ptr.h>
#include <c10/util/typeid.h>


void THCStorage_(set)(THCState *state, THCStorage *self, ptrdiff_t index, scalar_t value)
{
  THArgCheck(
      (index >= 0) && (index < static_cast<int64_t>(self->nbytes() / sizeof(scalar_t))),
      2,
      "index out of bounds");
  cudaStream_t stream = c10::cuda::getCurrentCUDAStream();
  at::cuda::memcpy_and_sync(self->data<scalar_t>() + index, &value, sizeof(scalar_t),
                              cudaMemcpyHostToDevice,
                              stream);
}

scalar_t THCStorage_(get)(THCState *state, const THCStorage *self, ptrdiff_t index)
{
  THArgCheck(
      (index >= 0) && (index < static_cast<int64_t>(self->nbytes() / sizeof(scalar_t))),
      2,
      "index out of bounds");
  scalar_t value;
  cudaStream_t stream = c10::cuda::getCurrentCUDAStream();
  at::cuda::memcpy_and_sync(&value, self->data<scalar_t>() + index, sizeof(scalar_t),
                                  cudaMemcpyDeviceToHost, stream);
  return value;
}

THCStorage* THCStorage_(new)(THCState *state)
{
  THStorage* storage = c10::make_intrusive<at::StorageImpl>(
                           c10::StorageImpl::use_byte_size_t(),
                           0,
                           c10::cuda::CUDACachingAllocator::get(),
                           true)
                           .release();
  return storage;
}

THCStorage* THCStorage_(newWithSize)(THCState *state, ptrdiff_t size)
{
  THStorage* storage = c10::make_intrusive<at::StorageImpl>(
                           c10::StorageImpl::use_byte_size_t(),
                           size * sizeof(scalar_t),
                           c10::cuda::CUDACachingAllocator::get(),
                           true)
                           .release();
  return storage;
}

THCStorage* THCStorage_(newWithMapping)(THCState *state, const char *fileName, ptrdiff_t size, int isShared)
{
  THError("not available yet for THCStorage");
  return NULL;
}

THCStorage* THCStorage_(newWithDataAndAllocator)(
    THCState* state,
    at::DataPtr&& data,
    ptrdiff_t size,
    at::Allocator* allocator) {
  THStorage* storage = c10::make_intrusive<at::StorageImpl>(
                           c10::StorageImpl::use_byte_size_t(),
                           size * sizeof(scalar_t),
                           std::move(data),
                           allocator,
                           allocator != nullptr)
                           .release();
  return storage;
}

void THCStorage_(retain)(THCState *state, THCStorage *self)
{
  THStorage_retain(self);
}

void THCStorage_(free)(THCState *state, THCStorage *self)
{
  THStorage_free(self);
}
#endif
