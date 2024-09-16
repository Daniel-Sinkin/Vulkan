import numpy as np
from pathlib import Path

def generate_sphere(radius, subdivisions, clockwise=False):
    vertices = []
    normals = []
    texcoords = []
    faces = []

    for i in range(subdivisions + 1):
        lat = np.pi * i / subdivisions
        v = i / subdivisions  # Texture coordinate for latitude

        for j in range(subdivisions + 1):
            lon = 2 * np.pi * j / subdivisions
            u = j / subdivisions  # Texture coordinate for longitude

            x = radius * np.sin(lat) * np.cos(lon)
            y = radius * np.sin(lat) * np.sin(lon)
            z = radius * np.cos(lat)

            vertices.append([x, y, z])
            normals.append([x / radius, y / radius, z / radius])
            texcoords.append([u, 1 - v])  # Flip v to match OpenGL convention

    # Generate faces (as triangles)
    for i in range(subdivisions):
        for j in range(subdivisions):
            v1 = i * (subdivisions + 1) + j
            v2 = v1 + 1
            v3 = (i + 1) * (subdivisions + 1) + j
            v4 = v3 + 1
            if clockwise:
                faces.append([v1, v3, v2])  # Triangle 1 (reversed order)
                faces.append([v2, v3, v4])  # Triangle 2 (reversed order)
            else:
                faces.append([v1, v2, v3])  # Triangle 1
                faces.append([v2, v4, v3])  # Triangle 2

    return np.array(vertices), np.array(normals), np.array(texcoords), np.array(faces)


def generate_torus(outer_radius, inner_radius, radial_subdivisions, tubular_subdivisions, clockwise=False):
    vertices = []
    normals = []
    texcoords = []
    faces = []

    for i in range(radial_subdivisions):
        theta = 2 * np.pi * i / radial_subdivisions
        cos_theta = np.cos(theta)
        sin_theta = np.sin(theta)
        
        for j in range(tubular_subdivisions):
            phi = 2 * np.pi * j / tubular_subdivisions
            cos_phi = np.cos(phi)
            sin_phi = np.sin(phi)

            x = (outer_radius + inner_radius * cos_phi) * cos_theta
            y = (outer_radius + inner_radius * cos_phi) * sin_theta
            z = inner_radius * sin_phi

            vertices.append([x, y, z])

            nx = cos_phi * cos_theta
            ny = cos_phi * sin_theta
            nz = sin_phi
            normals.append([nx, ny, nz])

            u = i / radial_subdivisions
            v = j / tubular_subdivisions
            texcoords.append([u, v])

    # Generate faces (as quads, split into two triangles)
    for i in range(radial_subdivisions):
        for j in range(tubular_subdivisions):
            v1 = i * tubular_subdivisions + j
            v2 = (v1 + 1) % tubular_subdivisions + i * tubular_subdivisions
            v3 = ((i + 1) % radial_subdivisions) * tubular_subdivisions + j
            v4 = (v3 + 1) % tubular_subdivisions + ((i + 1) % radial_subdivisions) * tubular_subdivisions

            if not clockwise:
                faces.append([v1, v3, v2])  # Triangle 1 (reversed order)
                faces.append([v2, v3, v4])  # Triangle 2 (reversed order)
            else:
                faces.append([v1, v2, v3])  # Triangle 1 (CCW winding)
                faces.append([v2, v4, v3])  # Triangle 2 (CCW winding)

    return np.array(vertices), np.array(normals), np.array(texcoords), np.array(faces)


def generate_tetrahedron(clockwise=False):
    vertices = np.array([
        [1, 1, 1],
        [-1, -1, 1],
        [-1, 1, -1],
        [1, -1, -1]
    ])

    texcoords = np.zeros((4, 2))

    # Faces of the tetrahedron
    faces = np.array([
        [0, 1, 2],
        [0, 3, 1],
        [0, 2, 3],
        [1, 3, 2]
    ])

    # Reverse winding order if clockwise is desired
    if not clockwise:
        faces = faces[:, [0, 2, 1]]

    # Calculate face normals
    normals = []
    for face in faces:
        v0 = vertices[face[0]]
        v1 = vertices[face[1]]
        v2 = vertices[face[2]]
        
        # Calculate two edge vectors
        edge1 = v1 - v0
        edge2 = v2 - v0
        
        # Compute the cross product to get the normal
        normal = np.cross(edge1, edge2)
        
        # Normalize the vector to unit length
        normal = normal / np.linalg.norm(normal)
        
        # Assign this normal to all three vertices of the face (flat shading)
        normals.append(normal)
        normals.append(normal)
        normals.append(normal)

    normals = np.array(normals).reshape(-1, 3)

    return vertices, normals, texcoords, faces


def save_obj(file_path, vertices, normals, texcoords, faces):
    with open(file_path, "w") as f:
        for vertex in vertices:
            f.write(f"v {vertex[0]} {vertex[1]} {vertex[2]}\n")
        for normal in normals:
            f.write(f"vn {normal[0]} {normal[1]} {normal[2]}\n")
        for texcoord in texcoords:
            f.write(f"vt {texcoord[0]} {texcoord[1]}\n")
        for face in faces:
            f.write(f"f {face[0]+1}/{face[0]+1}/{face[0]+1} {face[1]+1}/{face[1]+1}/{face[1]+1} {face[2]+1}/{face[2]+1}/{face[2]+1}\n")


if __name__ == "__main__":
    output_dir = Path("assets/models")
    output_dir.mkdir(parents=True, exist_ok=True)  # Ensure the directory exists

    # Sphere file path
    sphere_path = output_dir.joinpath("sphere.obj")
    if not sphere_path.exists():
        print(f"Generating sphere.obj...")
        radius = 1.0
        subdivisions = 20
        clockwise = False  # Ensure counterclockwise winding
        vertices, normals, texcoords, faces = generate_sphere(radius, subdivisions, clockwise)
        save_obj(sphere_path, vertices, normals, texcoords, faces)
        print(f"Sphere OBJ file saved as {sphere_path}")
    else:
        print(f"Sphere OBJ already exists, skipping generation.")

    # Torus file path
    torus_path = output_dir.joinpath("torus.obj")
    if not torus_path.exists():
        print(f"Generating torus.obj...")
        outer_radius = 1.0
        inner_radius = 0.3
        radial_subdivisions = 30
        tubular_subdivisions = 20
        clockwise = False  # Ensure counterclockwise winding
        vertices, normals, texcoords, faces = generate_torus(outer_radius, inner_radius, radial_subdivisions, tubular_subdivisions, clockwise)
        save_obj(torus_path, vertices, normals, texcoords, faces)
        print(f"Torus OBJ file saved as {torus_path}")
    else:
        print(f"Torus OBJ already exists, skipping generation.")

    # Tetrahedron file path
    tetrahedron_path = output_dir.joinpath("tetrahedron.obj")
    if not tetrahedron_path.exists():
        print(f"Generating tetrahedron.obj...")
        clockwise = False  # Ensure counterclockwise winding
        vertices, normals, texcoords, faces = generate_tetrahedron(clockwise)
        save_obj(tetrahedron_path, vertices, normals, texcoords, faces)
        print(f"Tetrahedron OBJ file saved as {tetrahedron_path}")
    else:
        print(f"Tetrahedron OBJ already exists, skipping generation.")