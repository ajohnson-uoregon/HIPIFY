# clang -o hipify-gen hipify-gen.cpp CUDA2HIP_Runtime_API_types.cpp CUDA2HIP_Runtime_API_functions.cpp CUDA2HIP_Driver_API_types.cpp -fno-rtti -I. -I/home/alisterj/.local/include -L/home/alisterj/.local/lib -lLLVMSupport -lLLVMCore -std=c++14 -lstdc++ -lm -lpthread
#clang -o hipify-function-help hipify-function-help.cpp  -fno-rtti -I. -I/home/alisterj/.local/include -L/home/alisterj/.local/lib -lclang-cpp -lclangTooling -lclangBasic -lclangASTMatchers -lLLVMSupport -std=c++14 -lstdc++ -lm -lpthread
#./hipify-function-help /usr/local/cuda-10.1/include/cuda_runtime_api.h /usr/local/cuda-10.1/include/cuda_runtime.h /usr/local/cuda-10.1/include/driver_functions.h /usr/local/cuda-10.1/include/cuda_gl_interop.h /usr/local/cuda-10.1/include/cuda.h -- -I/usr/local/cuda-10.1/include -x cuda

rm hipify_runtime.cpp
echo $'#include "hip/hip_runtime.h"\n#include <vector>\n#include <string>\n\n' >> hipify_runtime.cpp
cat cuda_literals.cpp >> hipify_runtime.cpp
echo $'\n' >> hipify_runtime.cpp
cat kernel_launches.cpp >> hipify_runtime.cpp
echo $'\n' >> hipify_runtime.cpp
cat runtime_type_names.cpp >> hipify_runtime.cpp
echo $'\n' >> hipify_runtime.cpp
cat runtime_function_names.cpp >> hipify_runtime.cpp
