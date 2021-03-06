#include <torch/csrc/WindowsTorchApiMacro.h>
#include <ATen/core/ivalue.h>
#include <torch/csrc/jit/pickler.h>


namespace torch {
namespace jit {

/// Save a `torch::IValue` in a format compatible with Python's `pickle` module
///
/// If present, `tensor_table` is a pointer to a table in which tensors that
/// are contained within `ivalue` are stored, and the bytes returned by the
/// pickler will only include references to these tensors in the table. This can
/// be used to keep the binary blob size small.
/// If not provided, tensors are stored in the same byte stream as the pickle
/// data, similar to `torch.save()` in eager Python.
///
/// Pickled values can be loaded in Python and C++:
/// \rst
/// .. code-block:: cpp
///
///  torch::IValue float_value(2.3);
///
///  // TODO: when tensors are stored in the pickle, delete this
///  std::vector<at::Tensor> tensor_table;
///  auto data = torch::jit::pickle(float_value, &tensor_table);
///
///  std::vector<torch::IValue> ivalues =
///      torch::jit::unpickle(data.data(), data.size());
///
/// .. code-block:: python
///
///   values = torch.load('data.pkl')
///   print(values)
///
/// \endrst
TORCH_API std::vector<char> pickle(
    const IValue& ivalue,
    std::vector<at::Tensor>* tensor_table = nullptr);

/// Pickle an IValue by calling a function to handle writing the data.
///
/// `writer` is a function that takes in a pointer to a chunk of memory and its
/// size and consumes it.
///
/// See `jit::pickle` for more details.
TORCH_API void pickle(
    std::function<void(const char* data_start, size_t data_len)> writer,
    const IValue& ivalue,
    std::vector<at::Tensor>* tensor_table = nullptr);

/// `reader` is a function that takes in a size to read from some pickled
/// binary. `reader` should remember where it last read.
///
/// `bounds_checker` is a function that returns `true` if the reader can read
/// more data, and `false` if it cannot (i.e. if a stream has hit its end of
/// file)
///
/// See `torch::pickle` for details.
TORCH_API IValue unpickle(
    std::function<const char*(size_t)> reader,
    std::function<bool()> bounds_chcker,
    const std::vector<at::Tensor>* tensor_table = nullptr,
    ClassResolver class_resolver = nullptr);

/// Decode a chunk of memory containing pickled data into its `torch::IValue`s.
///
/// If any `torch::IValue`s in the pickled data are `Object`s, then a
/// `class_resolver` function must be provided.
///
/// See `torch::pickle` for details.
TORCH_API IValue unpickle(
    const char* data,
    size_t size,
    const std::vector<at::Tensor>* tensor_table = nullptr,
    ClassResolver class_resolver = nullptr);

} // namespace jit
} // namespace torch
