#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include "RL/arena.hpp"

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(uttt_engine, m){
    nb::class_<Arena>(m, "Arena")
        .def(nb::init<int>(), "size"_a)
        .def("step", &Arena::step)

        .def("get_input_buffer", [](Arena& self, int bufferIdx) {
            size_t shape[2] = { static_cast<size_t>(self.get_buffer_size()), 270 };
            return nb::ndarray<nb::numpy, uint8_t>(
                self.get_in_ptr(bufferIdx),
                2,       
            shape
            );
        }, "buffer_idx"_a)

        .def("get_output_buffer", [](Arena& self, int bufferIdx) {
            size_t shape[1] = { static_cast<size_t>(self.get_buffer_size()) };
            return nb::ndarray<nb::numpy, uint8_t>(
                self.get_out_ptr(bufferIdx),
                1,
                shape
            );
        }, "buffer_idx"_a)

        .def("get_reward_buffer", [](Arena& self, int bufferIdx) {
            size_t shape[1] = { static_cast<size_t>(self.get_buffer_size()) };
            return nb::ndarray<nb::numpy, float>(
                self.get_reward_ptr(bufferIdx),
                1,
                shape
            );
        }, "buffer_idx"_a);
}