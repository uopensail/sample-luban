#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


#include "sample.h"
#include "toolkit.h"

namespace py = pybind11;

PYBIND11_MODULE(pysampletoolkit, m) {
    py::class_<sample_luban::SampleLubanToolKit>(m, "SampleLubanToolKit")
        .def(py::init<const std::string &,const std::string &>())
        .def("sample_feature", &sample_luban::SampleLubanToolKit::sample_feature)
        .def("process_sample", py::overload_cast<sample_luban::PoolGetter*, luban::SharedFeaturesPtr, const std::string&, const std::string&>(&sample_luban::SampleLubanToolKit::process_sample))
        .def("process_sample", py::overload_cast<sample_luban::PoolGetter*, luban::SharedFeaturesPtr, luban::SharedFeaturesPtr, const std::string&>(&sample_luban::SampleLubanToolKit::process_sample))
        .def("process_user", &sample_luban::SampleLubanToolKit::process_user)
        .def("__repr__", [](const sample_luban::SampleLubanToolKit &t) -> std::string {
            return "<pysampletoolkit.SampleLubanToolKit>";
        });
    py::class_<sample_luban::Pool,std::shared_ptr<sample_luban::Pool>>(m, "Pool")
        .def(py::init<const std::string&>())
        .def("get", &sample_luban::Pool::get)
        .def("__repr__", [](const sample_luban::Pool &t) -> std::string {
            return "<pysampletoolkit.Pool>";
        });
    py::class_<sample_luban::PoolGetter>(m, "PoolGetter")
        .def(py::init<const std::vector<std::string>&>())
        .def(py::init<const std::vector<std::shared_ptr<sample_luban::Pool>>&>())
        .def("get", &sample_luban::PoolGetter::get)
        .def("__repr__", [](const sample_luban::PoolGetter &t) -> std::string {
            return "<pysampletoolkit.PoolGetter>";
        });
    py::class_<sample_luban::SampleToolKit>(m, "SampleToolKit")
        .def(py::init<const std::vector<std::string> &, const std::string &,const std::string &>())
        .def("process_sample", &sample_luban::SampleToolKit::process_sample_files)
        .def("__repr__", [](const sample_luban::SampleToolKit &t) -> std::string {
            return "<pysampletoolkit.SampleToolKit>";
        });
}

 
