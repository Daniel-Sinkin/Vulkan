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
            v1 = i * (subdivisions + 1) + j + 1
            v2 = v1 + 1
            v3 = (i + 1) * (subdivisions + 1) + j + 1
            v4 = v3 + 1
            # Create two triangles per quad
            if clockwise:
                faces.append([v1, v3, v2])  # Triangle 1 (reversed order)
                faces.append([v2, v3, v4])  # Triangle 2 (reversed order)
            else:
                faces.append([v1, v2, v3])  # Triangle 1
                faces.append([v2, v4, v3])  # Triangle 2

    return np.array(vertices), np.array(normals), np.array(texcoords), np.array(faces)


def save_obj(file_path, vertices, normals, texcoords, faces):
    with open(file_path, "w") as f:
        for vertex in vertices:
            f.write(f"v {vertex[0]} {vertex[1]} {vertex[2]}\n")
        for normal in normals:
            f.write(f"vn {normal[0]} {normal[1]} {normal[2]}\n")
        for texcoord in texcoords:
            f.write(f"vt {texcoord[0]} {texcoord[1]}\n")
        for face in faces:
            # OBJ indices are 1-based, so we need to add 1 to all indices
            f.write(f"f {face[0]}/{face[0]}/{face[0]} {face[1]}/{face[1]}/{face[1]} {face[2]}/{face[2]}/{face[2]}\n")


if __name__ == "__main__":
    radius = 1.0
    subdivisions = 20  # Increase for smoother sphere
    clockwise = True # Default to counterclockwise winding

    vertices, normals, texcoords, faces = generate_sphere(radius, subdivisions, clockwise)

    # Create the path with the number of subdivisions in the filename
    output_dir = Path("assets/models")
    output_dir.mkdir(parents=True, exist_ok=True)  # Ensure the directory exists
    output_path = output_dir.joinpath(f"sphere_{subdivisions}.obj")

    save_obj(output_path, vertices, normals, texcoords, faces)

    print(f"OBJ file saved as {output_path}")