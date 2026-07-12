#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include "RL/arena.hpp"

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(uttt_engine, m){
    nb::class_<Arena>(m, "Arena")
        .def("__init__", [](Arena* footprint, int size, 
                            nb::ndarray<int8_t, nb::ndim<2>> states0, 
                            nb::ndarray<int8_t, nb::ndim<2>> states1,
                            nb::ndarray<int8_t, nb::ndim<1>> actions0, 
                            nb::ndarray<int8_t, nb::ndim<1>> actions1) {
            
            int8_t* s0 = (int8_t*)states0.data();
            int8_t* s1 = (int8_t*)states1.data();
            int8_t* a0 = (int8_t*)actions0.data();
            int8_t* a1 = (int8_t*)actions1.data();

            new (footprint) Arena(size, s0, s1, a0, a1);
        })
        .def("reset", &Arena::reset)
        .def("step", &Arena::step, nb::call_guard<nb::gil_scoped_release>())
        .def("flush_trajectory", &Arena::flush_trajectory)
        .def("sample_trajectory",[](Arena* footprint, int count,
                                    nb::ndarray<int8_t, nb::ndim<2>> states, 
                                    nb::ndarray<int8_t, nb::ndim<1>> actions,
                                    nb::ndarray<nb::ndim<1>, nb::c_contig> values){
            int8_t* s = (int8_t*)states.data();
            int8_t* a = (int8_t*)actions.data();
            _Float16* v = (_Float16*)values.data();
            
            footprint->sample_trajectory(count, s, a, v);
        });
}