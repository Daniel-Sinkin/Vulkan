import numpy as np
from pathlib import Path


def generate_sphere(radius, subdivisions):
    vertices = []
    normals = []
    faces = []

    for i in range(subdivisions + 1):
        lat = np.pi * i / subdivisions
        for j in range(subdivisions + 1):
            lon = 2 * np.pi * j / subdivisions
            x = radius * np.sin(lat) * np.cos(lon)
            y = radius * np.sin(lat) * np.sin(lon)
            z = radius * np.cos(lat)
            vertices.append([x, y, z])
            normals.append([x / radius, y / radius, z / radius])

    # Generate faces
    for i in range(subdivisions):
        for j in range(subdivisions):
            v1 = i * (subdivisions + 1) + j + 1
            v2 = v1 + 1
            v3 = (i + 1) * (subdivisions + 1) + j + 1
            v4 = v3 + 1
            faces.append([v1, v2, v4, v3])

    return np.array(vertices), np.array(normals), np.array(faces)


def save_obj(file_path, vertices, normals, faces):
    with open(file_path, "w") as f:
        for vertex in vertices:
            f.write(f"v {vertex[0]} {vertex[1]} {vertex[2]}\n")
        for normal in normals:
            f.write(f"vn {normal[0]} {normal[1]} {normal[2]}\n")
        for face in faces:
            # OBJ indices are 1-based, so we need to add 1 to all indices
            f.write(f"f {face[0]}//{face[0]} {face[1]}//{face[1]} {face[2]}//{face[2]} {face[3]}//{face[3]}\n")


if __name__ == "__main__":
    radius = 1.0
    subdivisions = 20  # Increase for smoother sphere

    vertices, normals, faces = generate_sphere(radius, subdivisions)

    output_path = Path("sphere.obj")
    save_obj(output_path, vertices, normals, faces)

    print(f"OBJ file saved as {output_path}")