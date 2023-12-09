#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "sample.h"
#include "toolkit.h"

namespace py = pybind11;

PYBIND11_MODULE(pysampletoolkit, m) {
    py::class_<sample_luban::SampleLubanToolKit>(m, "SampleLubanToolKit")
        .def(py::init<const std::string &,const std::string &,const std::string &>())
        .def("process_sample", &sample_luban::SampleLubanToolKit::process_sample)
        .def("__repr__", [](const sample_luban::SampleLubanToolKit &t) -> std::string {
            return "<pyluban.SampleLubanToolKit>";
        });
    py::class_<sample_luban::SampleToolkit>(m, "SampleToolKit")
        .def(py::init<const std::string &>())
        .def("process_item_featrue", &sample_luban::SampleToolkit::process_item_featrue)
        .def("__repr__", [](const sample_luban::SampleToolkit &t) -> std::string {
            return "<pyluban.SampleToolKit>";
        });
}

 