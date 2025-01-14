#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "TH/generic/THStorage.cpp"
#else

#include <ATen/MapAllocator.h>
#include <c10/core/CPUAllocator.h>
#include <c10/util/irange.h>

#include <new>


THStorage* THStorage_(new)(void)
{
  return THStorage_new();
}

THStorage* THStorage_(newWithSize)(ptrdiff_t size)
{
  THStorage* storage = c10::make_intrusive<at::StorageImpl>(
                           c10::StorageImpl::use_byte_size_t(),
#ifdef THQUANTIZED
                           size * sizeof(quantized_t),
#else
                           size * sizeof(scalar_t),
#endif
                           c10::GetDefaultCPUAllocator(),
                           true)
                           .release();
  return storage;
}

THStorage* THStorage_(newWithAllocator)(ptrdiff_t size,
                                        at::Allocator *allocator)
{
  THStorage* storage = c10::make_intrusive<at::StorageImpl>(
                           c10::StorageImpl::use_byte_size_t(),
#ifdef THQUANTIZED
                           size * sizeof(quantized_t),
#else
                           size * sizeof(scalar_t),
#endif
                           allocator,
                           true)
                           .release();
  return storage;
}


THStorage* THStorage_(newWithMapping)(const char *filename, ptrdiff_t size, int flags)
{
  size_t actual_size = -1;
  THStorage* storage =
      c10::make_intrusive<at::StorageImpl>(
          c10::StorageImpl::use_byte_size_t(),
          size * sizeof(scalar_t),
          at::MapAllocator::makeDataPtr(
              filename, flags, size * sizeof(scalar_t), &actual_size),
          /* allocator */ nullptr,
          false)
          .release();

  if (size <= 0) {
    storage->set_nbytes(actual_size);
  }

  return storage;
}

void THStorage_(retain)(THStorage *storage)
{
  THStorage_retain(storage);
}

void THStorage_(free)(THStorage *storage)
{
  THStorage_free(storage);
}

THStorage* THStorage_(newWithDataAndAllocator)(at::DataPtr&& data, ptrdiff_t size,
                                               at::Allocator* allocator) {
  THStorage* storage = c10::make_intrusive<at::StorageImpl>(
                           c10::StorageImpl::use_byte_size_t(),
#ifdef THQUANTIZED
                           size * sizeof(quantized_t),
#else
                           size * sizeof(scalar_t),
#endif
                           std::move(data),
                           allocator,
                           allocator != nullptr)
                           .release();
  return storage;
}

void THStorage_(resizeBytes)(THStorage* storage, ptrdiff_t size_bytes) {
  return THStorage_resizeBytes(storage, size_bytes);
}

void THStorage_(fill)(THStorage *storage, scalar_t value)
{
  const auto type_meta = caffe2::TypeMeta::Make<scalar_t>();
  const size_t numel = storage->nbytes() / type_meta.itemsize();
  for (const auto i : c10::irange(numel)) {
    storage->data<scalar_t>()[i] = value;
  }
}

void THStorage_(set)(THStorage *self, ptrdiff_t idx, scalar_t value)
{
  const auto type_meta = caffe2::TypeMeta::Make<scalar_t>();
  const auto numel = static_cast<int64_t>(self->nbytes() / type_meta.itemsize());
  THArgCheck((idx >= 0) && (idx < numel), 2, "out of bounds");
  self->data<scalar_t>()[idx] = value;
}

scalar_t THStorage_(get)(const THStorage *self, ptrdiff_t idx)
{
  const auto type_meta = caffe2::TypeMeta::Make<scalar_t>();
  const auto numel = static_cast<int64_t>(self->nbytes() / type_meta.itemsize());
  THArgCheck((idx >= 0) && (idx < numel), 2, "out of bounds");
  return self->data<scalar_t>()[idx];
}

void THStorage_(swap)(THStorage *storage1, THStorage *storage2)
{
  std::swap(*storage1, *storage2);
}

#endif
