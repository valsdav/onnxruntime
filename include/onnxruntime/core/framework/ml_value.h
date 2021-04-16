// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include "core/common/common.h"
#include "core/common/exceptions.h"
#include "core/framework/allocator.h"
#include "core/framework/data_types.h"
#include "core/framework/tensor.h"

namespace onnxruntime {
class SparseTensor;
class TensorSeq;
}  // namespace onnxruntime

/**
   Represents both tensors and non-tensors.
*/
struct OrtValue {
 public:
  OrtValue() : data_(nullptr) {}
  ~OrtValue() {
    std::shared_ptr<void> d(nullptr);
    std::atomic_store(&data_, d);
  }
  OrtValue(const OrtValue& v) : type_(v.type_), fence_(v.fence_) {
    std::atomic_store(&data_, v.data_);
  }

  OrtValue(void* pData, onnxruntime::MLDataType type, onnxruntime::DeleteFunc deleter) {
    Init(pData, type, deleter);
  }

  void Init(void* pData, onnxruntime::MLDataType type, onnxruntime::DeleteFunc deleter) {
    std::shared_ptr<void> d(pData, deleter);
    std::atomic_store(&data_, d);
    type_ = type;
  }

  bool IsAllocated() const {
    auto d = std::atomic_load(&data_);
    return d && type_;
  }

  OrtValue& operator=(const OrtValue& v) {
    if (this != &v) {
      std::atomic_store(&data_, v.data_);
      type_ = v.type_;
      fence_ = v.fence_;
    }
    return *this;
  }

  OrtValue& operator=(OrtValue&& v) {
    if (this != &v) {
      std::atomic_store(&data_, v.data_);
      type_ = v.type_;
      fence_ = v.fence_;
    }
    return *this;
  }

  template <typename T>
  const T& Get() const {
    ORT_ENFORCE(onnxruntime::DataTypeImpl::GetType<T>() == type_, onnxruntime::DataTypeImpl::GetType<T>(), " != ", type_);
    auto d = std::atomic_load(&data_);
    return *static_cast<T*>(d.get());
  }

  template <typename T>
  T* GetMutable() {
    ORT_ENFORCE(onnxruntime::DataTypeImpl::GetType<T>() == type_, onnxruntime::DataTypeImpl::GetType<T>(), " != ", type_);
    auto d = std::atomic_load(&data_);
    return static_cast<T*>(d.get());
  }

  bool IsTensor() const noexcept {
    return (type_ != nullptr && type_->IsTensorType());
  }

  bool IsTensorSequence() const noexcept {
    return (type_ != nullptr && type_->IsTensorSequenceType());
  }

  bool IsSparseTensor() const noexcept {
    return (type_ != nullptr && type_->IsSparseTensorType());
  }

  onnxruntime::MLDataType Type() const {
    return type_;
  }

  onnxruntime::Fence_t Fence() const {
    return fence_.get();
  }

  void SetFence(onnxruntime::FencePtr fence) {
    fence_ = fence;
  }

  void ShareFenceWith(OrtValue& v) {
    fence_ = v.fence_;
  }

 private:
  std::shared_ptr<void> data_;
  onnxruntime::MLDataType type_{nullptr};
  onnxruntime::FencePtr fence_;
};

template <>
inline const onnxruntime::Tensor& OrtValue::Get<onnxruntime::Tensor>() const {
  ORT_ENFORCE(IsTensor(), "Trying to get a Tensor, but got: ", onnxruntime::DataTypeImpl::ToString(type_));
  auto d = std::atomic_load(&data_);
  return *static_cast<onnxruntime::Tensor*>(d.get());
}

template <>
inline onnxruntime::Tensor* OrtValue::GetMutable<onnxruntime::Tensor>() {
  ORT_ENFORCE(IsTensor(), "Trying to get a Tensor, but got: ", onnxruntime::DataTypeImpl::ToString(type_));
  auto d = std::atomic_load(&data_);
  return static_cast<onnxruntime::Tensor*>(d.get());
}

template <>
inline const onnxruntime::TensorSeq& OrtValue::Get<onnxruntime::TensorSeq>() const {
  ORT_ENFORCE(IsTensorSequence(), "Trying to get a TensorSeq, but got: ", onnxruntime::DataTypeImpl::ToString(type_));
  auto d = std::atomic_load(&data_);
  return *static_cast<onnxruntime::TensorSeq*>(d.get());
}

template <>
inline onnxruntime::TensorSeq* OrtValue::GetMutable<onnxruntime::TensorSeq>() {
  ORT_ENFORCE(IsTensorSequence(), "Trying to get a TensorSeq, but got: ", onnxruntime::DataTypeImpl::ToString(type_));
  auto d = std::atomic_load(&data_);
  return static_cast<onnxruntime::TensorSeq*>(d.get());
}

template <>
inline const onnxruntime::SparseTensor& OrtValue::Get<onnxruntime::SparseTensor>() const {
  ORT_ENFORCE(IsSparseTensor(), "Trying to get a SparseTensor, but got: ", onnxruntime::DataTypeImpl::ToString(type_));
  auto d = std::atomic_load(&data_);
  return *static_cast<onnxruntime::SparseTensor*>(d.get());
}

template <>
inline onnxruntime::SparseTensor* OrtValue::GetMutable<onnxruntime::SparseTensor>() {
  ORT_ENFORCE(IsSparseTensor(), "Trying to get a SparseTensor, but got: ", onnxruntime::DataTypeImpl::ToString(type_));
  auto d = std::atomic_load(&data_);
  return static_cast<onnxruntime::SparseTensor*>(d.get());
}

//TODO: remove the following line
#define MLValue OrtValue
