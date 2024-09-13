import numpy as np
from pathlib import Path

def generate_sphere_mesh(radius, subdivisions, positions):
    vertices = []
    normals = []
    texcoords = []
    faces = []
    vertex_offset = 0

    for position in positions:
        for i in range(subdivisions + 1):
            lat = np.pi * i / subdivisions
            v = i / subdivisions  # Texture coordinate for latitude
            for j in range(subdivisions + 1):
                lon = 2 * np.pi * j / subdivisions
                u = j / subdivisions  # Texture coordinate for longitude
                x = radius * np.sin(lat) * np.cos(lon) + position[0]
                y = radius * np.sin(lat) * np.sin(lon) + position[1]
                z = radius * np.cos(lat) + position[2]
                vertices.append([x, y, z])
                normals.append([(x - position[0]) / radius, (y - position[1]) / radius, (z - position[2]) / radius])
                texcoords.append([u, 1 - v])  # Flip v to match OpenGL convention

        # Generate faces (as triangles) with correct winding
        for i in range(subdivisions):
            for j in range(subdivisions):
                v1 = vertex_offset + i * (subdivisions + 1) + j
                v2 = v1 + 1
                v3 = vertex_offset + (i + 1) * (subdivisions + 1) + j
                v4 = v3 + 1
                faces.append([v1, v3, v2])  # Triangle 1 (corrected winding)
                faces.append([v2, v3, v4])  # Triangle 2 (corrected winding)

        vertex_offset += (subdivisions + 1) * (subdivisions + 1)

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
            # OBJ indices are 1-based, so we need to add 1 to all indices
            f.write(f"f {face[0]+1}/{face[0]+1}/{face[0]+1} {face[1]+1}/{face[1]+1}/{face[1]+1} {face[2]+1}/{face[2]+1}/{face[2]+1}\n")

def generate_grid_positions(spacing):
    positions = []
    grid_size = 3  # Hardcoded grid size of 3x3x3
    offset = (grid_size - 1) * spacing / 2  # To center the grid around the origin

    for x in range(grid_size):
        for y in range(grid_size):
            for z in range(grid_size):
                # Only include positions on the outer shell of the 3x3x3 grid
                if x in [0, 2] or y in [0, 2] or z in [0, 2]:
                    position = [
                        x * spacing - offset,
                        y * spacing - offset,
                        z * spacing - offset
                    ]
                    positions.append(position)

    return positions

if __name__ == "__main__":
    radius = 1.0
    subdivisions = 30  # Number of subdivisions for the sphere
    spacing = radius * 4.0  # Increased spacing between spheres
    
    output_dir = Path("assets/models")
    output_dir.mkdir(parents=True, exist_ok=True)  # Ensure the directory exists
    output_path = output_dir.joinpath(f"sphere_{subdivisions}.obj")
    
    # Generate only the outer positions of the 3x3x3 grid (8 positions)
    positions = generate_grid_positions(spacing)
    
    vertices, normals, texcoords, faces = generate_sphere_mesh(radius, subdivisions, positions)
    
    save_obj(output_path, vertices, normals, texcoords, faces)
    print(f"OBJ file with spheres in the outer shell of a 3x3x3 grid saved as {output_path}")