#include <CL/opencl.hpp>
#include <iostream>
#include <string>

int opencl_setup() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform plat;
    for (auto &p : platforms) {
        std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
        std::cout << platver << '\n';
        if (platver.find("OpenCL 2.") != std::string::npos ||
            platver.find("OpenCL 3.") != std::string::npos) {
            plat = p;
            break;
        }
    }
    if (plat() == 0) {
        std::cout << "No OpenCL 2.0 or newer platform found.\n";
        return 1;
    }

    cl::Platform selected_plat = cl::Platform::setDefault(plat);
    if (selected_plat != plat) {
        std::cout << "Error setting default platform.\n";
        return 1;
    }

    std::vector<cl::Device> devices;
    selected_plat.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    cl::Device::setDefault(devices[0]);

    return 0;
}
