#pragma once

#include <ATen/Error.h>
#include <ATen/ScalarType.h>

#include <cstddef>
#include <iosfwd>
#include <string>

namespace at {
/// Represents a a compute device on which a tensor is located. A device is
/// uniquely identified by a type, which specifies the type of machine it is
/// (e.g. CPU or CUDA GPU), and a device index or ordinal, which identifies the
/// specific compute device when there is more than one of a certain type. The
/// device index is optional, and in its defaulted state represents (abstractly)
/// "the current device". Further, there are two constraints on the value of the
/// device index, if one is explicitly stored:
/// 1. A negative index represents the current device, a non-negative index
/// represents a specific, concrete device,
/// 2. When the device type is CPU, the device index must be zero.
struct Device {
  /// The possible values of the device *type*.
  enum class Type { CPU, CUDA };

  /// Converts a `Backend` to a `Device::Type` if possible.
  static Type backend_to_type(Backend backend) {
    switch (backend) {
      case kCPU:
      case kSparseCPU:
        return Type::CPU;
      case kCUDA:
      case kSparseCUDA:
        return Type::CUDA;
      default:
        AT_ERROR(
            "Invalid backend ", toString(backend), " for Device construction");
    }
  }

  /// Constructs a new `Device` from a `Type` and an optional device index.
  /* implicit */ Device(Type type, int32_t index = -1)
      : type_(type), index_(index) {
    AT_CHECK(
        index == -1 || index >= 0,
        "Device index must be -1 or non-negative, got ",
        index);
    AT_CHECK(
        !is_cpu() || index <= 0,
        "CPU device index must be -1 or zero, got ",
        index);
  }

  /// Constructs a `Device` from a string description, for convenience.
  /// The string supplied must follow the following schema:
  /// `(cpu|cuda):[<device-index>]`
  /// where `cpu:` or `cuda:` specifies the device type, and
  /// `<device-index>` optionally specifies a device index.
  /* implicit */ Device(const std::string& device_string);

  /// Constructs a new `Device` from a `Backend` (which is converted to a
  /// `Type`, if possible) and an optional device index.
  /* implicit */ Device(Backend backend, int32_t index = -1)
      : Device(backend_to_type(backend), index) {}

  /// Returns true if the type and index of this `Device` matches that of
  /// `other`.
  bool operator==(const Device& other) const noexcept {
    return this->type_ == other.type_ && this->index_ == other.index_;
  }

  /// Returns true if the type or index of this `Device` differs from that of
  /// `other`.
  bool operator!=(const Device& other) const noexcept {
    return !(*this == other);
  }

  /// Sets the device index.
  void set_index(int32_t index) {
    index_ = index;
  }

  /// Returns the type of device this is.
  Type type() const noexcept {
    return type_;
  }

  /// Returns the optional index.
  const int32_t& index() const noexcept {
    return index_;
  }

  /// Returns true if the device has a non-default index.
  bool has_index() const noexcept {
    return index_ != -1;
  }

  /// Return true if the device is of CUDA type.
  bool is_cuda() const noexcept {
    return type_ == Type::CUDA;
  }

  /// Return true if the device is of CPU type.
  bool is_cpu() const noexcept {
    return type_ == Type::CPU;
  }

 private:
  Type type_;
  int32_t index_ = -1;
};
} // namespace at

std::ostream& operator<<(std::ostream& stream, at::Device::Type type);
std::ostream& operator<<(std::ostream& stream, const at::Device& device);
