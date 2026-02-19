#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <array>
#include <list>

#include "kdtree.h"
#include "point.h"

namespace py = pybind11;

// Wrapper functions to handle Python interface
template <int N>
Point<N> make_point(int id, const std::array<float, N>& coords) {
    return Point<N>(id, coords);
}

template <int N>
Kdtree<N>* make_kdtree(const Point<N>& point, int split_axis = 0) {
    return new Kdtree<N>(point, split_axis);
}

// Bind Point class for specific dimensions
template <int N>
void bind_point(py::module& m, const std::string& name) {
    py::class_<Point<N>>(m, name.c_str())
        .def(py::init<int, const std::array<float, N>&>(), py::arg("id"), py::arg("coords"))
        .def("id", &Point<N>::id)
        .def("__getitem__", [](const Point<N>& p, int axis) { return p[axis]; })
        .def("x", &Point<N>::x)
        .def("y", &Point<N>::y)
        .def("z", &Point<N>::z)
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
