#include <rerun.hpp>
#include <vector>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/Surface_mesh_parameterization/IO/File_off.h>
#include <CGAL/Surface_mesh_parameterization/parameterize.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/bounding_box.h>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_2                                Point_2;
typedef Kernel::Point_3                                Point_3;
typedef CGAL::Surface_mesh<Kernel::Point_3>            SurfaceMesh;
typedef boost::graph_traits<SurfaceMesh>::vertex_descriptor   vertex_descriptor;
typedef boost::graph_traits<SurfaceMesh>::halfedge_descriptor halfedge_descriptor;
typedef boost::graph_traits<SurfaceMesh>::face_descriptor     face_descriptor;

namespace PMP = CGAL::Polygon_mesh_processing;
namespace SMP = CGAL::Surface_mesh_parameterization;

 int main() {

    SurfaceMesh sm;

    if (!PMP::IO::read_polygon_mesh("D:/data/ply/bunny.ply", sm)) {
        std::cerr << "Error: Invalid input." << std::endl;
        return EXIT_FAILURE;
    }
    halfedge_descriptor bhd    = CGAL::Polygon_mesh_processing::longest_border(sm).first;
    typedef SurfaceMesh::Property_map<vertex_descriptor, Point_2> UV_pmap;
    UV_pmap uv_map = sm.add_property_map<vertex_descriptor, Point_2>("h:uv").first;

    SMP::parameterize(sm, bhd, uv_map);
    std::ofstream out("D:/result.off");
    SMP::IO::output_uvmap_to_off(sm, bhd, uv_map, out);


    auto bbox = CGAL::bounding_box(sm.points().begin(), sm.points().end());
    auto max  = bbox.max();
    auto min  = bbox.min();
    auto center = (max - min)/2;
    for (auto& p : sm.points()) {
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

     for (vertex_descriptor v : CGAL::vertices(sm)) {
        rerun::Position3D p = {
            (float)sm.point(v).x(), (float)sm.point(v).y(), (float)sm.point(v).z()};
        vertices.push_back(p);
        vertex_colors.push_back({255, 255, 0});
     }

     for (face_descriptor f : CGAL::faces(sm)) {
        CGAL::Vertex_around_face_iterator<SurfaceMesh> vbegin, vend;
        for (boost::tie(vbegin, vend) = CGAL::vertices_around_face(CGAL::halfedge(f, sm), sm);
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


     for (halfedge_descriptor hf : CGAL::halfedges(sm)) {
        vertex_descriptor source = CGAL::source(hf, sm);
        vertex_descriptor target = CGAL::target(hf, sm);

        auto sourcePoint = sm.point(source);
        auto targetPoint = sm.point(target);
        auto v = targetPoint - sourcePoint;
        origins.push_back({(float)sourcePoint.x(), (float)sourcePoint.y(), (float)sourcePoint.z()});
        vectors.push_back({(float)v.x(), (float)v.y(), (float)v.z()});
        colors.push_back({0, 0, 255});
     }


     rec.log("arrows",
             rerun::Arrows3D::from_vectors(vectors).with_origins(origins).with_colors(colors));
 }