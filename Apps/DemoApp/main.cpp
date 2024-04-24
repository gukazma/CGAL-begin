#include <rerun.hpp>
 #include <vector>


#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
typedef CGAL::Simple_cartesian<double> K;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;
typedef Mesh::Vertex_index             vertex_descriptor;
typedef Mesh::Face_index               face_descriptor;

 int main() {
    //Mesh m;
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

     std::vector<rerun::Position3D> vertex_positions = {
         {0.0f, 1.0f, 0.0f},
         {1.0f, 0.0f, 0.0f},
         {0.0f, 0.0f, 0.0f},
     };
     const rerun::Color vertex_colors[3] = {
         {255, 255, 0},
         {255, 255, 0},
         {255, 255, 0},
     };
     const std::vector<uint32_t> indices = {2, 1, 0};

     rec.log(
         "triangle",
         rerun::Mesh3D(vertex_positions)
             .with_vertex_normals({{0.0, 0.0, 1.0}})
             .with_vertex_colors(vertex_colors)
             .with_mesh_properties(rerun::components::MeshProperties::from_triangle_indices(indices))
     );

     rerun::LineStrip3D linestrip({
         {0.0f, 1.0f, 0.0f},
         {1.0f, 0.0f, 0.0f},
         {0.0f, 0.0f, 0.0f},
         {0.0f, 1.0f, 0.0f},
     });

     rec.log("segments", rerun::LineStrips3D(linestrip));
 }