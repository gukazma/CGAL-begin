#include <rerun.hpp>
#include <vector>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/bounding_box.h>

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;
typedef Mesh::Vertex_index             vertex_descriptor;
typedef boost::graph_traits<Mesh>::face_descriptor   face_descriptor;
typedef boost::graph_traits<Mesh>::halfedge_descriptor halfedge_descriptor;
typedef boost::graph_traits<Mesh>::vertex_descriptor vertex_descriptor;

namespace PMP = CGAL::Polygon_mesh_processing;
 int main() {

    Mesh m;

    if (!PMP::IO::read_polygon_mesh("D:/data/ply/bunny.ply", m)) {
        std::cerr << "Error: Invalid input." << std::endl;
        return EXIT_FAILURE;
    }

    auto bbox = CGAL::bounding_box(m.points().begin(), m.points().end());
    auto max  = bbox.max();
    auto min  = bbox.min();
    auto center = (max - min)/2;
    for (auto& p : m.points()) {
        p -= center;
    }
    //// Add the points as vertices
    //vertex_descriptor u = m.add_vertex(K::Point_3(0, 1, 0));
    //vertex_descriptor v = m.add_vertex(K::Point_3(0, 0, 0));
    //vertex_descriptor w = m.add_vertex(K::Point_3(1, 1, 0));
    //vertex_descriptor x = m.add_vertex(K::Point_3(1, 0, 0));
    //m.add_face(u, v, w);
    //face_descriptor f = m.add_face(u, v, x);
    //if (f == Mesh::null_face()) {
    //    std::cerr << "The face could not be added because of an orientation error." << std::endl;
    //    f = m.add_face(u, x, v);
    //    assert(f != Mesh::null_face());
    //}
     const auto rec = rerun::RecordingStream("rerun_example_mesh3d_indexed");
     rec.spawn().exit_on_failure();

     std::vector<rerun::Position3D> vertices;
     std::vector<rerun::Color>      vertex_colors;
     std::vector<uint32_t>    indices;

     for (vertex_descriptor v : CGAL::vertices(m)) {
        rerun::Position3D p = {(float)m.point(v).x(), (float)m.point(v).y(), (float)m.point(v).z()};
        vertices.push_back(p);
        vertex_colors.push_back({255, 255, 0});
     }

     for (face_descriptor f : CGAL::faces(m)) {
        CGAL::Vertex_around_face_iterator<Mesh> vbegin, vend;
        for (boost::tie(vbegin, vend) = CGAL::vertices_around_face(CGAL::halfedge(f, m), m);
             vbegin != vend;
             ++vbegin) {
            indices.push_back(*vbegin);
        }
     }
     rec.log(
         "triangle",
             rerun::Mesh3D(vertices)
             .with_vertex_normals({{0.0, 0.0, 1.0}})
             .with_vertex_colors(vertex_colors)
             .with_mesh_properties(rerun::components::MeshProperties::from_triangle_indices(indices))
     );
     std::vector<rerun::Position3D> origins;
     std::vector<rerun::Vector3D>   vectors;
     std::vector<rerun::Color>      colors;


     for (halfedge_descriptor hf : CGAL::halfedges(m)) {
        vertex_descriptor source = CGAL::source(hf, m);
        vertex_descriptor target = CGAL::target(hf, m);

        auto sourcePoint = m.point(source);
        auto targetPoint = m.point(target);
        auto v = targetPoint - sourcePoint;
        origins.push_back({(float)sourcePoint.x(), (float)sourcePoint.y(), (float)sourcePoint.z()});
        vectors.push_back({(float)v.x(), (float)v.y(), (float)v.z()});
        colors.push_back({0, 0, 255});
     }


     rec.log("arrows",
             rerun::Arrows3D::from_vectors(vectors).with_origins(origins).with_colors(colors));
 }