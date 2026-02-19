#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <array>
#include <list>

#include "kdtree.h"
#include "point.h"

namespace py = pybind11;

// Bind Point class for specific dimensions
template <int N>
void bind_point(py::module& m, const std::string& name) {
    auto point_class =
        py::class_<Point<N>>(m, name.c_str())
            .def(py::init<int, const std::array<float, N>&>(), py::arg("id"), py::arg("coords"))
            .def("id", &Point<N>::id)
            .def("__getitem__",
                 [](const Point<N>& p, int axis) {
                     int idx = axis;
                     if (idx < 0) {
                         idx += N;  // Support Python-style negative indices
                     }
                     if (idx < 0 || idx >= N) {
                         throw py::index_error("Point index out of range");
                     }
                     return p[idx];
                 })
            .def_static("dimensions", &Point<N>::dimensions)
            .def("__repr__", [](const Point<N>& p) {
                std::string repr =
                    "Point" + std::to_string(N) + "D(id=" + std::to_string(p.id()) + ", coords=[";
                for (int i = 0; i < N; i++) {
                    if (i > 0)
                        repr += ", ";
                    repr += std::to_string(p[i]);
                }
                repr += "])";
                return repr;
            });

    // Bind dimension-specific accessors only when valid
    if (N >= 1) {
        point_class.def("x", &Point<N>::x);
    }
    if (N >= 2) {
        point_class.def("y", &Point<N>::y);
    }
    if (N >= 3) {
        point_class.def("z", &Point<N>::z);
    }
}

// Bind Kdtree class for specific dimensions
template <int N>
void bind_kdtree(py::module& m, const std::string& name) {
    py::class_<Kdtree<N>>(m, name.c_str())
        .def(py::init<const Point<N>&, int>(), py::arg("point"), py::arg("split_axis") = 0)
        .def("insert", &Kdtree<N>::insert, py::arg("point"))
        .def("get_nearby_points", &Kdtree<N>::get_nearby_points, py::arg("point"),
             py::arg("radius"));
}

PYBIND11_MODULE(pykdtree, m) {
    m.doc() = "Python bindings for N-dimensional k-d tree library";

    // Bind 2D types
    bind_point<2>(m, "Point2D");
    bind_kdtree<2>(m, "Kdtree2D");

    // Bind 3D types
    bind_point<3>(m, "Point3D");
    bind_kdtree<3>(m, "Kdtree3D");

    // Bind 4D types
    bind_point<4>(m, "Point4D");
    bind_kdtree<4>(m, "Kdtree4D");
}
